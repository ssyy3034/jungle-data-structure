// bst_iterative_allinone.c
// Build: gcc -std=c11 -O2 bst_iterative_allinone.c -o bst_iter
// Run  : ./bst_iter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>  // dup, dup2, close
#include <fcntl.h>   // open

#define REQUIRE_IMPL() do { \
  fprintf(stderr, "❌ TODO: implement %s\n", __func__); \
  exit(100); \
} while(0)

/* =============== BST 타입 =============== */
typedef struct _bstnode{
    int item;
    struct _bstnode *left;
    struct _bstnode *right;
} BSTNode;

/* ====== Queue for level-order ====== */
typedef struct _QueueNode {
    BSTNode *data;
    struct _QueueNode *nextPtr;
} QueueNode;

/* ====== Stack for iterative traversals ====== */
typedef struct _StackNode {
    BSTNode *data;
    struct _StackNode *next;
} StackNode;

/* =============== 함수 시그니처 =============== */
void levelOrderIterative(BSTNode *root);
void inOrderIterative(BSTNode *root);
void preOrderIterative(BSTNode *root);
void postOrderIterativeS1(BSTNode *root);
void postOrderIterativeS2(BSTNode *root);

/* Queue */
void     enqueue(QueueNode **head, QueueNode **tail, BSTNode *node);
BSTNode* dequeue(QueueNode **head, QueueNode **tail);
int      isEmpty(QueueNode *head);          // ✅ 네가 말한 이름으로 고정
void     qClear(QueueNode **head, QueueNode **tail);

/* Stack */
void     push(StackNode **top, BSTNode *node);
BSTNode* pop(StackNode **top);
int      isEmptyStack(StackNode *top);      // ✅ 네가 말한 이름으로 고정
BSTNode* peek(StackNode *top);              // ✅ S1에서 사용
void     sClear(StackNode **top);

/* Tree utils */
void insertBSTNode(BSTNode **node, int value);
void removeAll(BSTNode **node);

/* (선택) 연산 카운터 */
static int g_q_ops = 0;
static int g_s_ops = 0;

/* =============== 구현부 (네 코드 기준 이름 유지) =============== */

void levelOrderIterative(BSTNode *root) {
    if (root == NULL) return;
    QueueNode *head = NULL, *tail = NULL;
    enqueue(&head, &tail, root);
    while (!isEmpty(head)) {
        BSTNode *n = dequeue(&head, &tail);
        printf("%d ", n->item);
        if (n->left)  enqueue(&head, &tail, n->left);
        if (n->right) enqueue(&head, &tail, n->right);
    }
}

void inOrderIterative(BSTNode *root) {
    StackNode *top = NULL;
    BSTNode *curr = root;
    while (curr != NULL || !isEmptyStack(top)) {
        if (curr) { push(&top, curr); curr = curr->left; }
        else {
            curr = pop(&top);
            printf("%d ", curr->item);
            curr = curr->right;
        }
    }
}

void preOrderIterative(BSTNode *root) {
    if (root == NULL) return;
    StackNode *top = NULL;
    push(&top, root);
    while (!isEmptyStack(top)) {
        BSTNode *n = pop(&top);
        printf("%d ", n->item);
        if (n->right) push(&top, n->right);
        if (n->left)  push(&top, n->left);
    }
}

void postOrderIterativeS1(BSTNode *root) {
    if (root == NULL) return;
    StackNode *top = NULL;
    BSTNode *curr = root, *prev = NULL;
    while (curr != NULL || !isEmptyStack(top)) {
        while (curr) { push(&top, curr); curr = curr->left; }
        BSTNode *t = peek(top);
        if (t->right == NULL || t->right == prev) {
            printf("%d ", t->item);
            pop(&top);
            prev = t;
            curr = NULL;
        } else {
            curr = t->right;
        }
    }
}

void postOrderIterativeS2(BSTNode *root) {
    if (root == NULL) return;
    StackNode *s1 = NULL, *s2 = NULL;
    push(&s1, root);
    while (!isEmptyStack(s1)) {
        BSTNode *n = pop(&s1);
        push(&s2, n);
        if (n->left)  push(&s1, n->left);
        if (n->right) push(&s1, n->right);
    }
    while (!isEmptyStack(s2)) {
        printf("%d ", pop(&s2)->item);
    }
}

/* ===== Queue ===== */
void enqueue(QueueNode **head, QueueNode **tail, BSTNode *node)
{
    QueueNode *n = (QueueNode*)malloc(sizeof(QueueNode));
    if(!n){ perror("malloc"); exit(1); }
    n->data = node; n->nextPtr = NULL;
    if (isEmpty(*head)) *head = n;
    else (*tail)->nextPtr = n;
    *tail = n;
    g_q_ops++;
}
BSTNode* dequeue(QueueNode **head, QueueNode **tail)
{
    if (isEmpty(*head)) return NULL;
    QueueNode *t = *head;
    BSTNode *x = t->data;
    *head = t->nextPtr;
    if (*head == NULL) *tail = NULL;
    free(t);
    g_q_ops++;
    return x;
}
int isEmpty(QueueNode *head){ return head==NULL; }
void qClear(QueueNode **head, QueueNode **tail)
{
    while(!isEmpty(*head)) (void)dequeue(head, tail);
}

/* ===== Stack ===== */
void push(StackNode **top, BSTNode *node)
{
    StackNode *n = (StackNode*)malloc(sizeof(StackNode));
    if(!n){ perror("malloc"); exit(1); }
    n->data = node; n->next = *top; *top = n;
    g_s_ops++;
}
BSTNode* pop(StackNode **top)
{
    if (isEmptyStack(*top)) return NULL;
    StackNode *t = *top; BSTNode *x = t->data;
    *top = t->next; free(t);
    g_s_ops++;
    return x;
}
int isEmptyStack(StackNode *top){ return top==NULL; }
BSTNode* peek(StackNode *top){ return top ? top->data : NULL; }
void sClear(StackNode **top)
{
    while(!isEmptyStack(*top)) (void)pop(top);
}

/* ===== Tree 기본 ===== */
void insertBSTNode(BSTNode **node, int value)
{
    if (*node==NULL){
        *node=(BSTNode*)malloc(sizeof(BSTNode));
        if(!*node){ perror("malloc"); exit(1); }
        (*node)->item=value; (*node)->left=(*node)->right=NULL; return;
    }
    if (value < (*node)->item) insertBSTNode(&(*node)->left, value);
    else if (value > (*node)->item) insertBSTNode(&(*node)->right, value);
}
void removeAll(BSTNode **node)
{
    if (*node==NULL) return;
    removeAll(&(*node)->left);
    removeAll(&(*node)->right);
    free(*node); *node=NULL;
}

/* =============== 출력 캡처 (dup/dup2) =============== */
static int saved_stdout_fd = -1;
static char cap_path[FILENAME_MAX] = "capture_stdout.txt";
static void begin_capture(void){
    fflush(stdout);
    saved_stdout_fd = dup(fileno(stdout));
    int fd = open(cap_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd == -1) { perror("open capture"); exit(1); }
    if (dup2(fd, fileno(stdout)) == -1) { perror("dup2->stdout"); exit(1); }
    close(fd);
}
static void end_capture(void){
    fflush(stdout);
    if (saved_stdout_fd != -1){
        if (dup2(saved_stdout_fd, fileno(stdout)) == -1) { perror("dup2 restore"); exit(1); }
        close(saved_stdout_fd);
        saved_stdout_fd = -1;
    }
}
static void read_capture(char *out, size_t cap){
    FILE *f=fopen(cap_path,"r"); if(!f){ out[0]='\0'; return; }
    size_t n=fread(out,1,cap-1,f); out[n]='\0'; fclose(f);
}

/* =============== 테스트 빌더 =============== */
/*    20
     /  \
   15    50
  / \   /  \
10  18 25  80
*/
static void build_pdf_tree(BSTNode **root){
    int seq[] = {20,15,50,10,18,25,80};
    for(size_t i=0;i<sizeof(seq)/sizeof(seq[0]);++i) insertBSTNode(root, seq[i]);
}
static const char *EXP_LEVEL = "20 15 50 10 18 25 80 ";
static const char *EXP_IN    = "10 15 18 20 25 50 80 ";
static const char *EXP_PRE   = "20 15 10 18 50 25 80 ";
static const char *EXP_POST  = "10 18 15 25 80 50 20 ";

/* =============== TESTS =============== */
static void test_levelOrder(){
    BSTNode *r=NULL; build_pdf_tree(&r);
    g_q_ops = 0;
    begin_capture(); levelOrderIterative(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strcmp(got, EXP_LEVEL)==0 && "level order mismatch");
    assert(g_q_ops>0 && "queue ops must be used");
    removeAll(&r);
    printf("[Q1] levelOrderIterative ✓ %s\n", got);
}
static void test_inOrder(){
    BSTNode *r=NULL; build_pdf_tree(&r);
    g_s_ops = 0;
    begin_capture(); inOrderIterative(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strcmp(got, EXP_IN)==0 && "inorder mismatch");
    assert(g_s_ops>0 && "stack ops must be used");
    removeAll(&r);
    printf("[Q2] inOrderIterative ✓ %s\n", got);
}
static void test_preOrder(){
    BSTNode *r=NULL; build_pdf_tree(&r);
    g_s_ops = 0;
    begin_capture(); preOrderIterative(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strcmp(got, EXP_PRE)==0 && "preorder mismatch");
    assert(g_s_ops>0 && "stack ops must be used");
    removeAll(&r);
    printf("[Q3] preOrderIterative ✓ %s\n", got);
}
static void test_postOrderS1(){
    BSTNode *r=NULL; build_pdf_tree(&r);
    g_s_ops = 0;
    begin_capture(); postOrderIterativeS1(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strcmp(got, EXP_POST)==0 && "postorder(S1) mismatch");
    assert(g_s_ops>0 && "stack ops must be used");
    removeAll(&r);
    printf("[Q4] postOrderIterativeS1 ✓ %s\n", got);
}
static void test_postOrderS2(){
    BSTNode *r=NULL; build_pdf_tree(&r);
    g_s_ops = 0;
    begin_capture(); postOrderIterativeS2(r); end_capture();
    char got[256]={0}; read_capture(got,sizeof(got));
    assert(strcmp(got, EXP_POST)==0 && "postorder(S2) mismatch");
    assert(g_s_ops>0 && "stack ops must be used");
    removeAll(&r);
    printf("[Q5] postOrderIterativeS2 ✓ %s\n", got);
}

int main(void){
    printf("==== BST Iterative — All-in-One (your naming) ====\n");
    test_levelOrder();
    test_inOrder();
    test_preOrder();
    test_postOrderS1();
    test_postOrderS2();
    printf("\n✅ ALL TESTS PASSED\n");
    return 0;
}
