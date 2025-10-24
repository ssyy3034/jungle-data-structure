#include <stdio.h>
#include <string.h>
#include "csapp.h" // CSAPP 실습을 위해 저자가 제공하는 유틸리티 함수 및 헤더 모음

/* Recommended max cache and object sizes */
// 캐시 구현 시 사용할 최대 크기 상수들 (이번 단계에서는 사용되지 않음)
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void doit(int browser_fd);
void *worker_routine(void *arg);

typedef struct cache_block {
    char url[MAXLINE];
    char content[MAX_OBJECT_SIZE];
    int size;
    struct cache_block *next;
    struct cache_block *prev;
} cache_block_t;

pthread_mutex_t cache_lock;

/* cache state */
cache_block_t *cache_head = NULL; /* most-recently used at head */
int cache_total_size = 0;

void cache_init(void);
void cache_init() { // 캐시 초기화 구현 (정상 위치)
    pthread_mutex_init(&cache_lock, NULL); // mutex 초기화
    cache_head = NULL; // 리스트 초기화
    cache_total_size = 0; // 크기 초기화
}

/* Find a cached block by url. If found, move it to head (MRU) and return it. */ // 캐시 조회 및 MRU 갱신
cache_block_t *cache_find(const char *url) {
    pthread_mutex_lock(&cache_lock); // 락 획득
    cache_block_t *p = cache_head; // 탐색 포인터
    while (p) { // 리스트 전체 탐색
        if (strcmp(p->url, url) == 0) { // 키 비교
            /* move to head if not already */
            if (p != cache_head) {
                /* unlink p */
                if (p->prev) p->prev->next = p->next; // 이전 노드 연결 조정
                if (p->next) p->next->prev = p->prev; // 다음 노드 연결 조정
                /* insert at head */
                p->next = cache_head; // 현재 head를 p 다음으로
                if (cache_head) cache_head->prev = p; // 이전 포인터 설정
                p->prev = NULL; // p의 이전은 없음
                cache_head = p; // p를 head로 설정
            }
            pthread_mutex_unlock(&cache_lock); // 락 해제
            return p; // 캐시된 블록 반환
        }
        p = p->next; // 다음 노드로 이동
    }
    pthread_mutex_unlock(&cache_lock); // 락 해제
    return NULL; // 발견 못함
}

/* Evict least-recently-used blocks until at least `needed` bytes are free */ // LRU 축출
void cache_evict_if_needed(int needed) {
    while (cache_total_size + needed > MAX_CACHE_SIZE) { // 필요 용량 확보될 때까지
        /* evict tail */
        cache_block_t *p = cache_head; // head에서 시작
        if (!p) break; // 비어있으면 중단
        /* find tail */
        while (p->next) p = p->next; // 리스트 끝(least-recently used) 찾기
        /* unlink tail */
        if (p->prev) p->prev->next = NULL; // 꼬리 제거
        else cache_head = NULL; // 하나만 있으면 head도 NULL
        cache_total_size -= p->size; // 캐시 총량 감소
        free(p); // 메모리 해제
    }
}

/* Store a response in cache (simple LRU insert at head) */ // 캐시 저장 (head에 삽입)
void cache_store(const char *url, const char *buf, int size) {
    if (size > MAX_OBJECT_SIZE) return; /* too big to cache */ // 개체가 너무 크면 저장 안 함
    pthread_mutex_lock(&cache_lock); // 락 획득
    cache_evict_if_needed(size); // 필요 시 축출
    cache_block_t *b = malloc(sizeof(cache_block_t)); // 새 블록 할당
    if (!b) { pthread_mutex_unlock(&cache_lock); return; } // 할당 실패 처리
    strncpy(b->url, url, MAXLINE-1); // URL 복사
    b->url[MAXLINE-1] = '\0'; // 널 종료 보장
    memcpy(b->content, buf, size); // 내용 복사
    b->size = size; // 크기 설정
    /* insert at head */
    b->prev = NULL; // 이전 없음
    b->next = cache_head; // 기존 head를 다음으로
    if (cache_head) cache_head->prev = b; // 기존 head의 이전 설정
    cache_head = b; // 새 head 설정
    cache_total_size += size; // 총량 업데이트
    pthread_mutex_unlock(&cache_lock); // 락 해제
}



    /* You won't lose style points for including this long line in your code */ // 예제의 user-agent 상수
    static const char *user_agent_hdr =
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 " // user-agent 헤더 문자열
        "Firefox/10.0.3\r\n"; // user-agent 끝내기


    /**
     * @brief 프록시 서버의 메인 함수, 프로그램의 시작점입니다.
     * * @param argc 명령줄 인자의 개수
     * @param argv 명령줄 인자 배열. argv[1]에 프록시가 리스닝할 포트 번호가 전달됩니다.
     */
    int main(int argc, char **argv) // 프로그램 진입점
    {
      int listenfd, connfd;                 // 리스닝 소켓과 연결된 소켓의 파일 디스크립터
      char hostname[MAXLINE], port[MAXLINE]; // 연결된 클라이언트의 호스트 이름과 포트를 저장할 버퍼
      socklen_t clientlen;                  // 클라이언트 주소 구조체의 크기
      struct sockaddr_storage clientaddr;   // 다양한 주소 체계를 지원하는 범용 클라이언트 주소 구조체

      /* 명령줄 인자 개수 확인 */ // 인자 검사
      if (argc != 2)
      {
        fprintf(stderr, "usage: %s <port>\n", argv[0]); // 사용법 출력
        exit(1);                                        // 비정상 종료
      }

      // 지정된 포트 번호(argv[1])로 리스닝 소켓을 열고, 그 디스크립터를 listenfd에 저장
      listenfd = Open_listenfd(argv[1]); // 리스닝 소켓 오픈
        cache_init(); /* 캐시 초기화 */ // 캐시 초기화 호출

      // 서버의 메인 루프: 클라이언트의 연결 요청을 무한히 기다림
      while (1)
      {
        clientlen = sizeof(clientaddr); // accept()를 호출하기 전, 주소 구조체의 크기를 초기화
    
        // 클라이언트의 연결 요청을 수락하고, 통신을 위한 새로운 소켓(connfd) 생성
        // 이 함수는 새로운 연결이 들어올 때까지 여기서 멈춰(blocking) 있습니다.
        int *connfd = malloc(sizeof(int)); // 동적 할당하여 스레드로 전달
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 연결 수락
    
        pthread_t tid; // 스레드 ID
        Pthread_create(&tid, NULL, (void *)worker_routine, (void *)connfd);  // 워커 스레드 생성
        // 새로운 스레드를 생성하여 각 클라이언트 연결을 독립적으로 처리
      }
      return 0; // (실제로는 무한 루프 때문에 이 줄은 실행되지 않음)
    }


    /**
     * @brief 단일 HTTP 트랜잭션(요청/응답)을 처리하는 함수
     * @param browser_fd 브라우저(클라이언트)와 연결된 파일 디스크립터
     */
    /* main() 함수 위나 다른 파일에 새로 작성 */
    void *worker_routine(void *arg) 
    {
        // 1. connfdp를 받아서 실제 connfd 값 추출
        int connfd = *((int *)arg); // 전달받은 포인터에서 fd 추출
    
        // 2. connfdp 메모리 해제 (free)
        free(arg); // 전달용 메모리 해제

        // 3. 스레드 분리 (detach) - main이 join으로 기다릴 필요 없음
        pthread_detach(pthread_self()); // 스레드 detach

        // 4. 실제 요청 처리
        doit(connfd); // 요청 처리 함수 호출

        // 5. 연결 종료 (close)
        Close(connfd); // 연결 닫기

        return NULL; // 스레드 종료
    }
    void doit(int browser_fd) // 단일 요청 처리 구현
    {
      char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE]; // 요청 관련 버퍼
      char hostname[MAXLINE], path[MAXLINE]; // 서버 호스트와 경로 버퍼
      char port_str[MAXLINE]; // 포트 문자열 버퍼
      rio_t rio_to_browser; // 브라우저와의 통신을 위한 RIO 버퍼 구조체

      /* 1. 브라우저가 보낸 요청 읽기 및 파싱 */ // 요청 읽기
      // browser_fd와 rio_to_browser 버퍼를 연결(associate)
      Rio_readinitb(&rio_to_browser, browser_fd); // RIO 초기화
  
      // rio_to_browser 버퍼를 통해 브라우저로부터 요청 라인(첫 줄)을 읽음
      if(Rio_readlineb(&rio_to_browser, buf, MAXLINE) <= 0) {
          return; // 읽기 실패 또는 빈 요청이면 함수 종료
      }
      printf("Request line: %s", buf); // 디버깅을 위해 수신한 요청 라인 출력
  
      // 요청 라인을 공백 기준으로 method, uri, version으로 파싱하여 각 변수에 저장
      sscanf(buf, "%s %s %s", method, uri, version); // 요청 라인 파싱
  
      // 이 프록시는 GET 메소드만 처리하므로, 다른 메소드이면 에러 처리
      if (strcasecmp(method, "GET")) {
          // clienterror(browser_fd, ...); // (TINY의 에러 함수를 가져와 사용하면 좋음)
          return; // GET이 아니면 종료
      }
      /* Robust URI parsing: extract hostname, port (optional), and path */
      /* Expected forms:
         http://host[:port]/path
         http://host
      */
      char *hostbegin, *hostend, *pathbegin, *portbegin; // 파싱용 포인터
      hostbegin = strstr(uri, "//"); // 스킴 이후 위치 찾기
      if (hostbegin) hostbegin += 2; else hostbegin = uri; // // 이후를 호스트 시작으로
      /* find path begin */
      pathbegin = strchr(hostbegin, '/'); // 경로 시작 위치 검색
      if (pathbegin) {
          strncpy(path, pathbegin, MAXLINE-1); // 경로 복사
          path[MAXLINE-1] = '\0'; // 널 종료
          hostend = pathbegin; // 호스트 끝은 경로 시작 전
      } else {
          strcpy(path, "/"); // 경로가 없으면 루트
          hostend = hostbegin + strlen(hostbegin); // 호스트 끝은 문자열 끝
      }
      /* check for port */
      portbegin = strchr(hostbegin, ':'); // 포트 구분자 찾기
      if (portbegin && portbegin < hostend) {
          /* hostname is hostbegin..portbegin-1 */
          int hlen = portbegin - hostbegin; // 호스트 길이
          strncpy(hostname, hostbegin, hlen); // 호스트 복사
          hostname[hlen] = '\0'; // 널 종료
          /* port is after ':' up to hostend */
          int plen = hostend - (portbegin + 1); // 포트 길이
          strncpy(port_str, portbegin + 1, plen); // 포트 복사
          port_str[plen] = '\0'; // 널 종료
      } else {
          /* no port */
          int hlen = hostend - hostbegin; // 호스트 길이
          strncpy(hostname, hostbegin, hlen); // 호스트 복사
          hostname[hlen] = '\0'; // 널 종료
          strcpy(port_str, "80"); // 기본 포트 80 사용
      }

      /* 2. 최종 서버로 전송할 새로운 HTTP 요청 메시지 생성 */
      char request_buf[MAXLINE]; // 요청 메시지를 담을 버퍼
  
      /* If URL is cached, serve directly from cache */
      cache_block_t *cached = cache_find(uri); // 캐시 조회
      if (cached) {
          Rio_writen(browser_fd, cached->content, cached->size); // 캐시에서 바로 응답
          return; // 처리 종료
      }
  
      // 요청 라인 생성: "GET [경로] HTTP/1.0" 형식
      snprintf(request_buf, MAXLINE, "GET %s HTTP/1.0\r\n", path); // 요청 라인 작성

      // 브라우저에서 온 헤더들을 읽어 필터링하여 요청에 추가
      int has_host = 0; // Host 헤더 존재 여부 플래그
      Rio_readlineb(&rio_to_browser, buf, MAXLINE); // 첫 헤더 라인 읽기
      while (strcmp(buf, "\r\n") != 0) { // 빈 줄(헤더 종료) 전까지
          // 헤더 이름 추출 (대소문자 구별 없이 비교)
          if (!strncasecmp(buf, "Host:", 5)) {
              has_host = 1; // Host 헤더가 있음을 표시
              snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "%s", buf); // Host 전달
          } else if (!strncasecmp(buf, "Connection:", 11) || !strncasecmp(buf, "Proxy-Connection:", 17) || !strncasecmp(buf, "User-Agent:", 11)) {
              // 이 헤더들은 우리가 직접 설정하므로 무시
          } else {
              // 기타 헤더는 그대로 전달
              snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "%s", buf); // 기타 헤더 추가
          }
          Rio_readlineb(&rio_to_browser, buf, MAXLINE); // 다음 헤더 라인 읽기
      }

      if (!has_host) {
          snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "Host: %s\r\n", hostname); // Host 없으면 추가
      }

      // 표준 User-Agent과 연결 관련 헤더를 강제로 설정
      snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "%s", user_agent_hdr); // user-agent 설정
      snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "Connection: close\r\n"); // 연결 닫기 설정
      snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "Proxy-Connection: close\r\n"); // 프록시 연결 닫기 설정

      // 헤더 끝
      snprintf(request_buf + strlen(request_buf), MAXLINE - strlen(request_buf), "\r\n"); // 헤더 종료 표시

      /* 3. 최종 목적지 서버에 연결 (프록시가 '클라이언트'처럼 행동하는 부분) */
      // 파싱한 호스트 이름과 포트 번호로 최종 서버에 TCP 연결을 시도
      int server_fd = Open_clientfd(hostname, port_str); // 서버에 연결
      if (server_fd < 0) {
          // 연결 실패 시 로그를 남기고 함수 종료
          printf("Connection failed to %s:%s\n", hostname, port_str); // 연결 실패 로그
          return; // 종료
      }

      rio_t rio_to_server; // 최종 서버와의 통신을 위한 RIO 버퍼 구조체
      Rio_readinitb(&rio_to_server, server_fd); // server_fd와 rio_to_server 버퍼를 연결
  
      /* 4. 생성한 요청 메시지를 최종 서버로 전송 */
      Rio_writen(server_fd, request_buf, strlen(request_buf)); // 서버로 요청 전송

      /* 5. 최종 서버의 응답을 브라우저로 그대로 중계(Relay) */
      ssize_t n; // 바이트 수 리턴용
      char object_buf[MAX_OBJECT_SIZE]; // 캐시 저장용 임시 버퍼
      int object_size = 0; // 임시 버퍼에 쌓인 크기
      int cacheable = 1; /* assume cacheable until proven otherwise */ // 캐시 가능성 플래그

      while ((n = Rio_readnb(&rio_to_server, buf, MAXLINE)) > 0) { // 서버 응답을 읽음
          Rio_writen(browser_fd, buf, n); // 브라우저로 그대로 전송
          /* accumulate into object buffer if small enough */
          if (object_size + n <= MAX_OBJECT_SIZE) { // 크기가 허용되면 누적
              memcpy(object_buf + object_size, buf, n); // 데이터 복사
              object_size += n; // 크기 증가
          } else {
              cacheable = 0; /* too big to cache */ // 너무 커서 캐시 불가
          }
      }

      /* store in cache if still cacheable */
      if (cacheable && object_size > 0) {
          cache_store(uri, object_buf, object_size); // 캐시에 저장 시도
      }
      /* 6. 최종 서버와의 연결을 닫음 */
      Close(server_fd); // 서버 연결 닫기
    }