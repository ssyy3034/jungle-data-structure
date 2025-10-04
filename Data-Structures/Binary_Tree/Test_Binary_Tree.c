// test_binary_trees_allinone.c
// Build: gcc -std=c11 -O2 test_binary_trees_allinone.c -o bt_tests

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>  // dup, dup2, close
#include <fcntl.h>   // open

/* ==========================
   Binary Tree Node
   ========================== */
typedef struct _btnode {
    int item;
    struct _btnode *left;
    struct _btnode *right;
} BTNode;

/* ==========================
   Q1~Q8 함수 시그니처
   ========================== */
int  identical(BTNode *tree1, BTNode *tree2);
int  maxHeight(BTNode *root);
int  countOneChildNodes(BTNode *root);
int  sumOfOddNodes(BTNode *root);
void mirrorTree(BTNode *node);
void printSmallerValues(BTNode *node, int m);
int  smallestValue(BTNode *node);         // TODO: 동하가 구현
int  hasGreatGrandchild(BTNode *node);    // TODO: 동하가 구현

/* ==========================
   테스트 유틸 (트리 생성/해제/도우미)
   ========================== */
static BTNode* newNode(int v){ BTNode*n=malloc(sizeof(BTNode)); n->item=v; n->left=n->right=NULL; return n; }
static void freeTree(BTNode *r){ if(!r) return; freeTree(r->left); freeTree(r->right); free(r); }
static void inorderToBuf(BTNode *r, char *buf, size_t cap){
    if(!r) return;
    inorderToBuf(r->left, buf, cap);
    char tmp[32]; snprintf(tmp,sizeof(tmp),"%d ", r->item);
    strncat(buf, tmp, cap - strlen(buf) - 1);
    inorderToBuf(r->right, buf, cap);
}

/* stdout 캡처 (printSmallerValues, hasGreatGrandchild 검증용) — dup/dup2 기반 */
static int saved_stdout_fd = -1;
static char cap_path[FILENAME_MAX] = "capture_stdout.txt";

static void begin_capture(void){
    fflush(stdout);
    saved_stdout_fd = dup(fileno(stdout));                // stdout FD 백업
    int fd = open(cap_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) { perror("open capture"); exit(1); }
    if (dup2(fd, fileno(stdout)) == -1) { perror("dup2 to stdout"); exit(1); }
    close(fd);
}

static void end_capture(void){
    fflush(stdout);
    if (saved_stdout_fd != -1) {
        if (dup2(saved_stdout_fd, fileno(stdout)) == -1) { perror("dup2 restore"); exit(1); }
        close(saved_stdout_fd);
        saved_stdout_fd = -1;
    }
}

static void read_capture(char *out, size_t cap){
    FILE *f=fopen(cap_path,"r"); if(!f){ out[0]='\0'; return; }
    size_t n=fread(out,1,cap-1,f); out[n]='\0'; fclose(f);
}

/* ==========================
   PDF 예시 트리 빌더
   ========================== */
static void build_identical_example(BTNode **t1, BTNode **t2){
    BTNode *a=newNode(5);
    a->left=newNode(3); a->right=newNode(7);
    a->left->left=newNode(1); a->left->right=newNode(2);
    a->right->left=newNode(4); a->right->right=newNode(8);

    BTNode *b=newNode(5);
    b->left=newNode(3); b->right=newNode(7);
    b->left->left=newNode(1); b->left->right=newNode(2);
    b->right->left=newNode(4); b->right->right=newNode(8);

    *t1=a; *t2=b;
}

static BTNode* build_maxHeight_example(){
    BTNode *r=newNode(4);
    r->left=newNode(2); r->right=newNode(6);
    r->left->left=newNode(1); r->left->right=newNode(3);
    r->right->left=newNode(5); r->right->right=newNode(7);
    return r;
}

static BTNode* build_oneChild_example(){
    BTNode *r=newNode(50);
    r->left=newNode(20); r->right=newNode(60);
    r->left->left=newNode(10);
    r->left->right=newNode(30);
    r->left->right->left=newNode(55);
    r->right->right=newNode(80);
    return r;
}

static BTNode* build_sumOdd_example(){
    BTNode *r=newNode(50);
    r->left=newNode(40); r->right=newNode(60);
    r->left->left=newNode(11); r->left->right=newNode(35);
    r->right->left=newNode(80); r->right->right=newNode(85);
    return r;
}

static BTNode* build_mirror_example_source(){
    BTNode *r=newNode(4);
    r->left=newNode(5); r->right=newNode(2);
    r->left->right=newNode(6);
    r->right->right=newNode(3);
    return r;
}

static BTNode* build_printSmaller_example(){
    BTNode *r=newNode(50);
    r->left=newNode(30); r->right=newNode(60);
    r->left->left=newNode(25); r->left->right=newNode(65);
    r->right->left=newNode(10); r->right->right=newNode(75);
    return r;
}

static BTNode* build_greatGrand_example(){
    BTNode *r=newNode(50);
    r->left=newNode(30); r->right=newNode(60);
    r->left->left=newNode(25); r->left->right=newNode(65);
    r->right->left=newNode(10); r->right->right=newNode(75);
    r->left->right->left=newNode(20);
    r->left->right->left->right=newNode(15);
    return r;
}

/* ==========================
   === Q1~Q8 구현 ===
   ========================== */

// Q1 identical
int identical(BTNode *tree1, BTNode *tree2)
{
    if (tree1 == NULL && tree2 == NULL) return 1;
    if (tree1 == NULL || tree2 == NULL) return 0;
    if (tree1->item != tree2->item) return 0;
    return identical(tree1->left, tree2->left) && identical(tree1->right, tree2->right);
}

// Q2 maxHeight (빈 트리: -1)
int maxHeight(BTNode *node)
{
    if (node == NULL) return -1;
    int left = maxHeight(node->left);
    int right = maxHeight(node->right);
    return (left > right ? left : right) + 1;
}

// Q3 countOneChildNodes
int countOneChildNodes(BTNode *node)
{
    if (node == NULL) return 0;
    int left = countOneChildNodes(node->left);
    int right = countOneChildNodes(node->right);
    if ((node->left == NULL) != (node->right == NULL)) return 1 + left + right;
    return left + right;
}

// Q4 sumOfOddNodes
int sumOfOddNodes(BTNode *node) {
    if (node == NULL) return 0;
    int sum = sumOfOddNodes(node->left) + sumOfOddNodes(node->right);
    if (node->item % 2 != 0) sum += node->item;
    return sum;
}

// Q5 mirrorTree
void mirrorTree(BTNode *node) {
    if (node == NULL) return;
    BTNode *tmp = node->left;
    node->left = node->right;
    node->right = tmp;
    mirrorTree(node->left);
    mirrorTree(node->right);
}

// Q6 printSmallerValues (중위, 공백 포함)
void printSmallerValues(BTNode *node, int m) {
    if (node == NULL) return;
    printSmallerValues(node->left, m);
    if (node->item < m) printf("%d ", node->item);
    printSmallerValues(node->right, m);
}

// Q7 smallestValue — TODO: 동하가 구현
int smallestValue(BTNode *node) {
     if(node==NULL) return INT_MAX;
	int left_min = smallestValue(node->left);
    int right_min = smallestValue(node->right);
    int min_value = node->item;
    if(min_value >left_min) min_value = left_min;
    if(min_value>right_min) min_value = right_min;
    return min_value;
}

// Q8 hasGreatGrandchild — TODO: 동하가 구현
int hasGreatGrandchild(BTNode *node) {if(node==NULL) return -1;
    int leftDepth = hasGreatGrandchild(node->left);
    int rightDepth = hasGreatGrandchild(node->right);
    int depth = (leftDepth > rightDepth ? leftDepth:rightDepth)+1;
    if(depth>=3) printf("%d ",node->item);
    return depth;
}

/* ==========================
   테스트 (각 Qn 당 1~2개)
   ========================== */
static void test_identical(){
    BTNode *t1=NULL,*t2=NULL; build_identical_example(&t1,&t2);
    assert(identical(t1,t2)==1);
    t2->right->right->item = 999;
    assert(identical(t1,t2)==0);
    freeTree(t1); freeTree(t2);
    printf("[Q1] ✓ passed\n");
}

static void test_maxHeight(){
    BTNode *r=build_maxHeight_example();
    assert(maxHeight(r)==2); freeTree(r);
    assert(maxHeight(NULL)==-1);
    printf("[Q2] ✓ passed\n");
}

static void test_countOneChildNodes(){
    BTNode *r=build_oneChild_example();
    assert(countOneChildNodes(r)==2); freeTree(r);
    printf("[Q3] ✓ passed\n");
}

static void test_sumOfOddNodes(){
    BTNode *r=build_sumOdd_example();
    assert(sumOfOddNodes(r)==131); freeTree(r);
    printf("[Q4] ✓ passed\n");
}

static void test_mirrorTree(){
    BTNode *src=build_mirror_example_source();
    char before[128]={0}, after[128]={0};
    inorderToBuf(src,before,sizeof(before));
    mirrorTree(src);
    inorderToBuf(src,after,sizeof(after));
    assert(strcmp(before,"5 6 4 2 3 ")==0);
    assert(strcmp(after, "3 2 4 6 5 ")==0);
    freeTree(src);
    printf("[Q5] ✓ passed\n");
}

static void test_printSmallerValues(){
    BTNode *r=build_printSmaller_example();
    begin_capture(); printSmallerValues(r,55); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strstr(got,"50") && strstr(got,"30") && strstr(got,"25") && strstr(got,"10"));
    freeTree(r);
    printf("[Q6] ✓ passed\n");
}

static void test_smallestValue(){
    BTNode *r=build_printSmaller_example();
    assert(smallestValue(r)==10); freeTree(r);
    printf("[Q7] ✓ passed\n");
}

static void test_hasGreatGrandchild(){
    BTNode *r=build_greatGrand_example();
    begin_capture(); hasGreatGrandchild(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strstr(got,"50"));
    freeTree(r);
    printf("[Q8] ✓ passed\n");
}

/* ==========================
   MAIN
   ========================== */
int main(void){
    printf("==== Binary Trees – All-in-one Test ====\n");
    test_identical();
    test_maxHeight();
    test_countOneChildNodes();
    test_sumOfOddNodes();
    test_mirrorTree();
    test_printSmallerValues();
    test_smallestValue();
    test_hasGreatGrandchild();

    printf("\n✅ ALL TESTS PASSED\n");
    return 0;
}
