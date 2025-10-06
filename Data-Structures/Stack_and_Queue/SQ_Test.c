// flash_ftl_sim.c
// Build: gcc -std=c11 -O2 flash_ftl_sim.c -o flash && ./flash
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define PAGE_SIZE          16            // 데모라서 작게 (바이트)
#define PAGES_PER_BLOCK    8
#define BLOCKS             4             // 총 블록 개수 (작게)
#define TOTAL_PAGES        (BLOCKS * PAGES_PER_BLOCK)

// 논리 주소 공간(=사용자에게 보이는 페이지 수)
#define LOGICAL_PAGES      12            // 일부 over-provisioning 느낌

// 물리 주소
typedef struct {
    int block;
    int page;
    bool valid; // 매핑 유효 여부
} PhysAddr;

// 페이지 메타/데이터
typedef struct {
    char data[PAGE_SIZE];
    bool is_free;     // 아직 안 쓴 페이지(모두 1이라고 가정)
    bool is_valid;    // 유효 데이터인지(무효화되면 false)
    int  lpn;         // 이 페이지에 담긴 논리 페이지 번호(디버그용)
} Page;

// 블록
typedef struct {
    Page pages[PAGES_PER_BLOCK];
    int wear_count;   // erase할 때마다 증가 (웨어레벨링 감각)
} Block;

// 전체 플래시
typedef struct {
    Block blocks[BLOCKS];
    int   wp_block;    // 현재 쓰기 포인터(블록)
    int   wp_page;     // 현재 쓰기 포인터(페이지)
} Flash;

static Flash gFlash;

// L2P 매핑 테이블: 논리 페이지 -> 물리 주소
static PhysAddr L2P[LOGICAL_PAGES];

// 유틸
static void init_flash(void) {
    memset(&gFlash, 0, sizeof(gFlash));
    for (int b = 0; b < BLOCKS; ++b) {
        for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
            gFlash.blocks[b].pages[p].is_free  = true;
            gFlash.blocks[b].pages[p].is_valid = false;
            gFlash.blocks[b].pages[p].lpn      = -1;
            memset(gFlash.blocks[b].pages[p].data, 0xFF, PAGE_SIZE); // 1로 채워져있다고 가정
        }
    }
    for (int i = 0; i < LOGICAL_PAGES; ++i) {
        L2P[i].block = -1;
        L2P[i].page  = -1;
        L2P[i].valid = false;
    }
    gFlash.wp_block = 0;
    gFlash.wp_page  = 0;
}

static bool advance_wp(void) {
    // 다음 free page로 이동
    for (int b = 0; b < BLOCKS; ++b) {
        for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
            if (gFlash.blocks[b].pages[p].is_free) {
                gFlash.wp_block = b;
                gFlash.wp_page  = p;
                return true;
            }
        }
    }
    return false; // free page 없음 → GC 필요
}

static void print_state(const char* tag) {
    printf("\n=== STATE: %s ===\n", tag);
    for (int b = 0; b < BLOCKS; ++b) {
        printf("Block %d (wear=%d):\n", b, gFlash.blocks[b].wear_count);
        for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
            Page* pg = &gFlash.blocks[b].pages[p];
            printf("  [%d,%d] free=%d valid=%d lpn=%2d data=\"",
                   b, p, pg->is_free, pg->is_valid, pg->lpn);
            for (int i = 0; i < PAGE_SIZE; ++i) {
                char c = pg->data[i];
                if (c == (char)0xFF) c = '.'; // 지워진 셀 가독성
                putchar(c);
            }
            printf("\"\n");
        }
    }
    printf("L2P: ");
    for (int i = 0; i < LOGICAL_PAGES; ++i) {
        if (L2P[i].valid)
            printf("(%d->%d,%d) ", i, L2P[i].block, L2P[i].page);
        else
            printf("(%d->--) ", i);
    }
    printf("\nWP: (%d,%d)\n", gFlash.wp_block, gFlash.wp_page);
}

// 물리 페이지에 쓰기(빈 페이지만 가능)
static bool phys_write(int b, int p, int lpn, const char* src) {
    Page* pg = &gFlash.blocks[b].pages[p];
    if (!pg->is_free) return false; // 덮어쓰기 금지!
    memset(pg->data, 0xFF, PAGE_SIZE);
    // 데이터 길이 제한해서 써줌 (데모)
    size_t len = strnlen(src, PAGE_SIZE);
    memcpy(pg->data, src, len);
    pg->is_free  = false;
    pg->is_valid = true;
    pg->lpn      = lpn;
    return true;
}

static void invalidate(PhysAddr pa) {
    if (pa.block < 0) return;
    Page* pg = &gFlash.blocks[pa.block].pages[pa.page];
    if (pg->is_valid) pg->is_valid = false; // 논리 삭제(무효화)
}

// 블록 전체 erase
static void erase_block(int b) {
    Block* blk = &gFlash.blocks[b];
    for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
        blk->pages[p].is_free  = true;
        blk->pages[p].is_valid = false;
        blk->pages[p].lpn      = -1;
        memset(blk->pages[p].data, 0xFF, PAGE_SIZE);
    }
    blk->wear_count++;
}

// 가장 "무효 페이지 많은" 블록을 GC 희생양으로 선택
static int pick_victim_block(void) {
    int best = -1, best_inv = -1;
    for (int b = 0; b < BLOCKS; ++b) {
        int inv = 0, used = 0;
        for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
            if (!gFlash.blocks[b].pages[p].is_free) {
                used++;
                if (!gFlash.blocks[b].pages[p].is_valid) inv++;
            }
        }
        if (used == 0) continue; // 텅 비면 굳이 희생양 X
        if (inv > best_inv) {
            best_inv = inv;
            best = b;
        }
    }
    return best;
}

// GC: 희생 블록의 유효 페이지만 새 free 페이지들로 복사, 매핑 갱신, 희생 블록 erase
static void gc(void) {
    int victim = pick_victim_block();
    if (victim < 0) return;

    // 1) 유효 페이지들을 임시로 복사
    for (int p = 0; p < PAGES_PER_BLOCK; ++p) {
        Page* vpg = &gFlash.blocks[victim].pages[p];
        if (!vpg->is_free && vpg->is_valid) {
            // free 페이지 확보
            if (!advance_wp()) {
                // 이론상 여기 오면 free 없음 → 순환 의존. 데모에서는 BLOCKS/OP 충분히 잡자.
                fprintf(stderr, "GC failed: no free page\n");
                return;
            }
            int nb = gFlash.wp_block, np = gFlash.wp_page;
            // 새 위치에 쓰기
            phys_write(nb, np, vpg->lpn, vpg->data);
            // L2P 매핑 갱신(새 위치)
            L2P[vpg->lpn].block = nb;
            L2P[vpg->lpn].page  = np;
            L2P[vpg->lpn].valid = true;
            // 이전 물리 페이지는 곧 지워질 블록이라 신경 X
        }
    }
    // 2) 희생 블록 erase
    erase_block(victim);
    // 3) WP를 다음 free로
    advance_wp();
}

// FTL API: 쓰기(삽입/갱신)
static void ftl_write(int lpn, const char* data) {
    assert(lpn >= 0 && lpn < LOGICAL_PAGES);
    // 덮어쓰기 불가 → out-of-place
    // 1) free page 필요
    if (!advance_wp()) {
        gc();
        bool ok = advance_wp();
        if (!ok) { fprintf(stderr, "No space even after GC\n"); return; }
    }
    int b = gFlash.wp_block, p = gFlash.wp_page;
    // 2) 새 페이지에 쓰고
    bool ok = phys_write(b, p, lpn, data);
    if (!ok) { fprintf(stderr, "phys_write failed (not free)\n"); return; }
    // 3) 예전 매핑 무효화
    if (L2P[lpn].valid) invalidate(L2P[lpn]);
    // 4) L2P 갱신
    L2P[lpn].block = b;
    L2P[lpn].page  = p;
    L2P[lpn].valid = true;
    // 5) 다음 free 탐색은 다음 호출 시 advance_wp에서
}

// FTL API: 읽기
static bool ftl_read(int lpn, char* out) {
    assert(lpn >= 0 && lpn < LOGICAL_PAGES);
    if (!L2P[lpn].valid) return false;
    Page* pg = &gFlash.blocks[L2P[lpn].block].pages[L2P[lpn].page];
    memcpy(out, pg->data, PAGE_SIZE);
    return true;
}

// FTL API: 삭제(논리 삭제 = 무효화)
static void ftl_delete(int lpn) {
    assert(lpn >= 0 && lpn < LOGICAL_PAGES);
    if (L2P[lpn].valid) {
        invalidate(L2P[lpn]);
        L2P[lpn].valid = false;
        L2P[lpn].block = -1;
        L2P[lpn].page  = -1;
    }
}

int main(void) {
    init_flash();
    print_state("init");

    // 삽입(쓰기)
    ftl_write(0, "user:kim");     // LPN 0
    ftl_write(1, "user:park");    // LPN 1
    ftl_write(2, "msg:hello");    // LPN 2
    ftl_write(3, "note:abc");     // LPN 3
    print_state("after 4 writes");

    // 갱신(out-of-place): LPN 1을 새 데이터로 덮어쓰기 시도 → 새 페이지에 기록, 이전 페이지 무효화
    ftl_write(1, "user:park*v2");
    print_state("update LPN1");

    // 삭제(논리): LPN 2 삭제 → 페이지는 남아있지만 invalid
    ftl_delete(2);
    print_state("delete LPN2");

    // 공간 압박을 만들어 GC 트리거
    for (int i = 4; i < 11; ++i) {
        char buf[PAGE_SIZE]; snprintf(buf, sizeof(buf), "x%02d", i);
        ftl_write(i, buf);
    }
    print_state("after more writes (may trigger GC)");

    // 읽기 예시
    char out[PAGE_SIZE+1]; memset(out, 0, sizeof(out));
    if (ftl_read(1, out)) printf("\nREAD LPN1 -> \"%s\"\n", out);
    else printf("\nREAD LPN1 -> <not found>\n");

    // 마지막으로 수동 GC 한 번 더 (상황 관찰용)
    gc();
    print_state("after manual GC");

    return 0;
}
