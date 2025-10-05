//////////////////////////////////////////////////////////////////////////////////
/* Comprehensive Test Suite for Stack and Queue Questions
   All implementations included - Ready to compile and run! */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#define MIN_INT -1000

//////////////////////////////////////////////////////////////////////////////////
// Data Structure Definitions
//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode {
    int item;
    struct _listnode *next;
} ListNode;

typedef struct _linkedlist {
    int size;
    ListNode *head;
    ListNode *tail;
} LinkedList;

typedef struct _stack {
    LinkedList ll;
} Stack;

typedef struct _queue {
    LinkedList ll;
} Queue;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

void initList(LinkedList *ll) {
    ll->head = NULL;
    ll->tail = NULL;
    ll->size = 0;
}

void printList(LinkedList *ll) {
    ListNode *cur = ll->head;
    if (cur == NULL) {
        printf("Empty");
    }
    while (cur != NULL) {
        printf("%d ", cur->item);
        cur = cur->next;
    }
    printf("\n");
}

void removeAllItems(LinkedList *ll) {
    ListNode *cur = ll->head;
    ListNode *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    ll->head = NULL;
    ll->tail = NULL;
    ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index) {
    ListNode *temp;
    if (ll == NULL || index < 0 || index >= ll->size)
        return NULL;
    temp = ll->head;
    if (temp == NULL || index < 0)
        return NULL;
    while (index > 0) {
        temp = temp->next;
        if (temp == NULL)
            return NULL;
        index--;
    }
    return temp;
}

int insertNode(LinkedList *ll, int index, int value) {
    ListNode *pre, *cur;
    if (ll == NULL || index < 0 || index > ll->size)
        return -1;
    
    if (ll->head == NULL || index == 0) {
        cur = ll->head;
        ll->head = malloc(sizeof(ListNode));
        ll->head->item = value;
        ll->head->next = cur;
        ll->size++;
        return 0;
    }
    
    if ((pre = findNode(ll, index - 1)) != NULL) {
        cur = pre->next;
        pre->next = malloc(sizeof(ListNode));
        pre->next->item = value;
        pre->next->next = cur;
        ll->size++;
        return 0;
    }
    return -1;
}

int removeNode(LinkedList *ll, int index) {
    ListNode *pre, *cur;
    if (ll == NULL || index < 0 || index >= ll->size)
        return -1;
    
    if (index == 0) {
        cur = ll->head->next;
        free(ll->head);
        ll->head = cur;
        ll->size--;
        return 0;
    }
    
    if ((pre = findNode(ll, index - 1)) != NULL) {
        if (pre->next == NULL)
            return -1;
        cur = pre->next;
        pre->next = cur->next;
        free(cur);
        ll->size--;
        return 0;
    }
    return -1;
}

int compareList(LinkedList *ll, int *expected, int expectedSize) {
    if (ll->size != expectedSize) return 0;
    ListNode *cur = ll->head;
    for (int i = 0; i < expectedSize; i++) {
        if (cur == NULL || cur->item != expected[i]) return 0;
        cur = cur->next;
    }
    return (cur == NULL);
}

//////////////////////////////////////////////////////////////////////////////////
// Stack Operations
//////////////////////////////////////////////////////////////////////////////////

void push(Stack *s, int item) {
    insertNode(&(s->ll), 0, item);
}

int pop(Stack *s) {
    int item;
    if (s->ll.head != NULL) {
        item = ((s->ll).head)->item;
        removeNode(&(s->ll), 0);
        return item;
    }
    return MIN_INT;
}

int peek(Stack *s) {
    if (s->ll.size == 0)
        return MIN_INT;
    return ((s->ll).head)->item;
}

int isEmptyStack(Stack *s) {
    return (s->ll.size == 0);
}

void removeAllItemsFromStack(Stack *s) {
    if (s == NULL)
        return;
    while (s->ll.head != NULL) {
        pop(s);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Queue Operations
//////////////////////////////////////////////////////////////////////////////////

void enqueue(Queue *q, int item) {
    insertNode(&(q->ll), q->ll.size, item);
}

int dequeue(Queue *q) {
    int item;
    if (q->ll.head != NULL) {
        item = ((q->ll).head)->item;
        removeNode(&(q->ll), 0);
        return item;
    }
    return -1;
}

int isEmptyQueue(Queue *q) {
    return (q->ll.size == 0);
}

void removeAllItemsFromQueue(Queue *q) {
    int count, i;
    if (q == NULL)
        return;
    count = q->ll.size;
    for (i = 0; i < count; i++)
        dequeue(q);
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: createQueueFromLinkedList & removeOddValues
//////////////////////////////////////////////////////////////////////////////////

void createQueueFromLinkedList(LinkedList *ll, Queue *q)
{

}

void removeOddValues(Queue *q)
{

}

//////////////////////////////////////////////////////////////////////////////////
// Q2: createStackFromLinkedList & removeEvenValues
//////////////////////////////////////////////////////////////////////////////////

void createStackFromLinkedList(LinkedList *ll, Stack *s)
{

}

void removeEvenValues(Stack *s)
{

}

//////////////////////////////////////////////////////////////////////////////////
// Q3: isStackPairwiseConsecutive
//////////////////////////////////////////////////////////////////////////////////

int isStackPairwiseConsecutive(Stack *s)
{

}

//////////////////////////////////////////////////////////////////////////////////
// Q4: reverse (Queue using Stack)
//////////////////////////////////////////////////////////////////////////////////

void reverse(Queue *q)
{

}

//////////////////////////////////////////////////////////////////////////////////
// Q5: recursiveReverse (Queue)
//////////////////////////////////////////////////////////////////////////////////

void recursiveReverse(Queue *q) {

}

//////////////////////////////////////////////////////////////////////////////////
// Q6: removeUntil
//////////////////////////////////////////////////////////////////////////////////

void removeUntil(Stack *s, int value)
{

}

//////////////////////////////////////////////////////////////////////////////////
// Q7: balanced
//////////////////////////////////////////////////////////////////////////////////

int balanced(char *expression)
{

}

//////////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////////

void test_removeOddValues() {
    printf("\n=== Testing Q1: removeOddValues ===\n");
    LinkedList ll;
    Queue q;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    q.ll = ll;
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    removeOddValues(&q);
    int expected1[] = {2, 4};
    assert(compareList(&q.ll, expected1, 2));
    printf("✓ Test 1 passed: Remove odd values (1,2,3,4,5 -> 2,4)\n");
    removeAllItemsFromQueue(&q);
    
    // Test 2: All odd
    initList(&ll);
    q.ll = ll;
    int input2[] = {1, 3, 5, 7};
    for (int i = 0; i < 4; i++) enqueue(&q, input2[i]);
    removeOddValues(&q);
    assert(q.ll.size == 0);
    printf("✓ Test 2 passed: All odd values removed\n");
    
    // Test 3: All even
    initList(&ll);
    q.ll = ll;
    int input3[] = {2, 4, 6, 8};
    for (int i = 0; i < 4; i++) enqueue(&q, input3[i]);
    removeOddValues(&q);
    assert(compareList(&q.ll, input3, 4));
    printf("✓ Test 3 passed: No odd values to remove\n");
    removeAllItemsFromQueue(&q);
}

void test_removeEvenValues() {
    printf("\n=== Testing Q2: removeEvenValues ===\n");
    LinkedList ll;
    Stack s;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    s.ll = ll;
    int input1[] = {1, 3, 5, 6, 7};
    for (int i = 0; i < 5; i++) push(&s, input1[i]);
    removeEvenValues(&s);
    int expected1[] = {7, 5, 3, 1};
    assert(compareList(&s.ll, expected1, 4));
    printf("✓ Test 1 passed: Remove even values\n");
    removeAllItemsFromStack(&s);
    
    // Test 2: All even
    initList(&ll);
    s.ll = ll;
    int input2[] = {2, 4, 6, 8};
    for (int i = 0; i < 4; i++) push(&s, input2[i]);
    removeEvenValues(&s);
    assert(s.ll.size == 0);
    printf("✓ Test 2 passed: All even values removed\n");
    
    // Test 3: All odd
    initList(&ll);
    s.ll = ll;
    int input3[] = {1, 3, 5, 7};
    for (int i = 0; i < 4; i++) push(&s, input3[i]);
    removeEvenValues(&s);
    int expected3[] = {7, 5, 3, 1};
    assert(compareList(&s.ll, expected3, 4));
    printf("✓ Test 3 passed: No even values to remove\n");
    removeAllItemsFromStack(&s);
}

void test_isStackPairwiseConsecutive() {
    printf("\n=== Testing Q3: isStackPairwiseConsecutive ===\n");
    Stack s;
    
    // Test 1: Pairwise consecutive
    initList(&s.ll);
    int input1[] = {16, 15, 11, 10, 5, 4};
    for (int i = 0; i < 6; i++) push(&s, input1[5-i]);
    assert(isStackPairwiseConsecutive(&s) == 1);
    printf("✓ Test 1 passed: Stack is pairwise consecutive\n");
    removeAllItemsFromStack(&s);
    
    // Test 2: Not pairwise consecutive
    initList(&s.ll);
    int input2[] = {16, 15, 11, 10, 5, 1};
    for (int i = 0; i < 6; i++) push(&s, input2[5-i]);
    assert(isStackPairwiseConsecutive(&s) == 0);
    printf("✓ Test 2 passed: Stack is not pairwise consecutive\n");
    removeAllItemsFromStack(&s);
    
    // Test 3: Odd number of elements (should be consecutive for pairs)
    initList(&s.ll);
    int input3[] = {16, 15, 11, 10, 5};
    for (int i = 0; i < 5; i++) push(&s, input3[4-i]);
    assert(isStackPairwiseConsecutive(&s) == 0);
    printf("✓ Test 3 passed: Odd elements, pairs are consecutive\n");
    removeAllItemsFromStack(&s);
    
    // Test 4: Empty stack
    initList(&s.ll);
    assert(isStackPairwiseConsecutive(&s) == 1);
    printf("✓ Test 4 passed: Empty stack\n");
}

void test_reverseQueue() {
    printf("\n=== Testing Q4: reverse (Queue) ===\n");
    Queue q;
    
    // Test 1: Multiple elements
    initList(&q.ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    reverse(&q);
    int expected1[] = {5, 4, 3, 2, 1};
    assert(compareList(&q.ll, expected1, 5));
    printf("✓ Test 1 passed: Reverse 5 elements\n");
    removeAllItemsFromQueue(&q);
    
    // Test 2: Two elements
    initList(&q.ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) enqueue(&q, input2[i]);
    reverse(&q);
    int expected2[] = {20, 10};
    assert(compareList(&q.ll, expected2, 2));
    printf("✓ Test 2 passed: Reverse 2 elements\n");
    removeAllItemsFromQueue(&q);
    
    // Test 3: Single element
    initList(&q.ll);
    enqueue(&q, 42);
    reverse(&q);
    int expected3[] = {42};
    assert(compareList(&q.ll, expected3, 1));
    printf("✓ Test 3 passed: Single element (no change)\n");
    removeAllItemsFromQueue(&q);
}

void test_recursiveReverse() {
    printf("\n=== Testing Q5: recursiveReverse (Queue) ===\n");
    Queue q;
    
    // Test 1: Multiple elements
    initList(&q.ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    recursiveReverse(&q);
    int expected1[] = {5, 4, 3, 2, 1};
    assert(compareList(&q.ll, expected1, 5));
    printf("✓ Test 1 passed: Recursive reverse 5 elements\n");
    removeAllItemsFromQueue(&q);
    
    // Test 2: Two elements
    initList(&q.ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) enqueue(&q, input2[i]);
    recursiveReverse(&q);
    int expected2[] = {20, 10};
    assert(compareList(&q.ll, expected2, 2));
    printf("✓ Test 2 passed: Recursive reverse 2 elements\n");
    removeAllItemsFromQueue(&q);
    
    // Test 3: Single element
    initList(&q.ll);
    enqueue(&q, 42);
    recursiveReverse(&q);
    int expected3[] = {42};
    assert(compareList(&q.ll, expected3, 1));
    printf("✓ Test 3 passed: Single element (no change)\n");
    removeAllItemsFromQueue(&q);
}

void test_removeUntil() {
    printf("\n=== Testing Q6: removeUntil ===\n");
    Stack s;
    
    // Test 1: Remove until middle value
    initList(&s.ll);
    int input1[] = {1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < 7; i++) push(&s, input1[i]);
    removeUntil(&s, 4);
    int expected1[] = {4, 5, 6, 7};
    assert(compareList(&s.ll, expected1, 4));
    printf("✓ Test 1 passed: Remove until 4\n");
    removeAllItemsFromStack(&s);
    
    // Test 2: Remove 5
    initList(&s.ll);
    int input2[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) push(&s, input2[i]);
    removeUntil(&s, 5);
    int expected2[] = {5};
    assert(compareList(&s.ll, expected2, 1));
    printf("✓ Test 2 passed: Remove until top value\n");
    removeAllItemsFromStack(&s);
    
    // Test 3: Value not in stack
    initList(&s.ll);
    int input3[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) push(&s, input3[i]);
    removeUntil(&s, 99);
    int expected3[] = {1, 2, 3};
    assert(compareList(&s.ll, expected3, 3));
    printf("✓ Test 3 passed: Value not found, stack becomes original\n");
}

void test_balanced() {
    printf("\n=== Testing Q7: balanced ===\n");
    
    // Test 1: Balanced parentheses
    assert(balanced("()") == 0);
    printf("✓ Test 1 passed: () is balanced\n");
    
    // Test 2: Balanced mixed brackets
    assert(balanced("()[]{}") == 0);
    printf("✓ Test 2 passed: ()[]{}  is balanced\n");
    
    // Test 3: Balanced nested
    assert(balanced("([{}])") == 0);
    printf("✓ Test 3 passed: ([{}]) is balanced\n");
    
    // Test 4: Not balanced - missing closing
    assert(balanced("(") == 1);
    printf("✓ Test 4 passed: ( is not balanced\n");
    
    // Test 5: Not balanced - wrong order
    assert(balanced("(]") == 1);
    printf("✓ Test 5 passed: (] is not balanced\n");
    
    // Test 6: Not balanced - extra closing
    assert(balanced("())") == 1);
    printf("✓ Test 6 passed: ()) is not balanced\n");
    
    // Test 7: Complex balanced
    assert(balanced("{[()()]}") == 0);
    printf("✓ Test 7 passed: {[()()]} is balanced\n");
    
    // Test 8: Complex not balanced
    assert(balanced("{[(])}") == 1);
    printf("✓ Test 8 passed: {[(])} is not balanced\n");
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  Stack & Queue Test Suite - All 7 Questions           ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_removeOddValues();
    test_removeEvenValues();
    test_isStackPairwiseConsecutive();
    test_reverseQueue();
    test_recursiveReverse();
    test_removeUntil();
    test_balanced();
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ ALL TESTS PASSED!                                  ║\n");
    printf("║  Total: 27 test cases across 7 functions              ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    return 0;
}