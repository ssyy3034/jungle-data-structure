//////////////////////////////////////////////////////////////////////////////////
/* Comprehensive Test Suite for Linked List Questions
   All implementations included - Ready to compile and run! */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
} LinkedList;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

void initList(LinkedList *ll) {
    ll->head = NULL;
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
// Q1: insertSortedLL
//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item) {
     if (ll == NULL) {
		return -1;
	}
    ListNode *cur = ll->head;
    int index = 0;

    // 정렬된 위치 탐색
    while (cur != NULL && cur->item < item) {
        index++;
        cur = cur->next;
    }

    // 중복 검사
    if (cur != NULL && cur->item == item) {
        return -1; // 같은 값 발견
    }

    // 삽입 위치에 노드 삽입
    insertNode(ll, index, item);
    return index;
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q2: alternateMergeLinkedList
//////////////////////////////////////////////////////////////////////////////////

void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2) {
    if (!ll1 || !ll2) return;

    ListNode *p = ll1->head;   // ll1 현재 노드
    ListNode *q = ll2->head;   // ll2 현재 노드
    if (!p || !q) return;      // 둘 중 하나라도 비면 할 일 없음

    while (p && q) {
        ListNode *pnext = p->next; // ll1 다음
        ListNode *qnext = q->next; // ll2 다음

        // ll1 노드 뒤에 ll2 노드 하나 끼우기
        p->next = q;

        if (!pnext) {
            // ll1이 여기서 끝남: 방금 붙인 q는 ll1 소유로 확정
            q->next = NULL;        // ll1을 깔끔히 종결
            ll2->head = qnext;     // ll2는 남은 꼬리부터 시작
            break;
        }

        // 계속 번갈아: q 다음에 원래 ll1의 다음을 연결
        q->next = pnext;

        // 다음 쌍으로 이동
        p = pnext;
        q = qnext;
    }

    // ll2가 먼저 다 소진된 경우(q == NULL)면 ll2는 빈 리스트
    if (q == NULL) ll2->head = NULL;

    // 크기 재계산(가장 단순)
    int s = 0;
    for (ListNode *t = ll1->head; t; t = t->next) s++;
    ll1->size = s;
    s = 0;
    for (ListNode *t = ll2->head; t; t = t->next) s++;
    ll2->size = s;
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q3: moveOddItemsToBack
//////////////////////////////////////////////////////////////////////////////////

void moveOddItemsToBack(LinkedList *ll) {
    if (ll == NULL || ll->head == NULL) return;

    ListNode *evenHead = NULL, *evenTail = NULL;
    ListNode *oddHead  = NULL, *oddTail  = NULL;

    ListNode *cur = ll->head;
    while (cur) {
        ListNode *next = cur->next;  // 미리 저장 (링크 바꾸기 전에)
        cur->next = NULL;            // 현재 노드를 독립시켜 놓고

        if ((cur->item % 2) == 0) {  // 짝수
            if (evenTail) {
                evenTail->next = cur;
                evenTail = cur;
            } else {
                evenHead = evenTail = cur;
            }
        } else {                     // 홀수
            if (oddTail) {
                oddTail->next = cur;
                oddTail = cur;
            } else {
                oddHead = oddTail = cur;
            }
        }
        cur = next;
    }

    // 짝수 뒤에 홀수 연결 (짝수가 없을 수도 있음)
    if (evenHead) {
        ll->head = evenHead;
        evenTail->next = oddHead;   // oddHead가 NULL이면 자연스럽게 끝
    } else {
        ll->head = oddHead;         // 전부 홀수였던 경우
    }

    // 크기는 변하지 않음
    // ll->size 그대로 유지 (필요하면 다시 세도 됨)
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q4: moveEvenItemsToBack
//////////////////////////////////////////////////////////////////////////////////

void moveEvenItemsToBack(LinkedList *list) {
    if (list == NULL || list->head == NULL) return;

    ListNode *evenHead = NULL, *evenTail = NULL;
    ListNode *oddHead  = NULL, *oddTail  = NULL;

    ListNode *cur = list->head;
    while (cur) {
        ListNode *next = cur->next;  // 미리 저장 (링크 바꾸기 전에)
        cur->next = NULL;            // 현재 노드를 독립시켜 놓고

        if ((cur->item % 2) == 0) {  // 짝수
            if (evenTail) {
                evenTail->next = cur;
                evenTail = cur;
            } else {
                evenHead = evenTail = cur;
            }
        } else {                     // 홀수
            if (oddTail) {
                oddTail->next = cur;
                oddTail = cur;
            } else {
                oddHead = oddTail = cur;
            }
        }
        cur = next;
    }

    // 짝수 뒤에 홀수 연결 (짝수가 없을 수도 있음)
    if (oddHead) {
        list->head = oddHead;
        oddTail->next = evenHead;   // oddHead가 NULL이면 자연스럽게 끝
    } else {
        list->head = evenHead;         // 전부 홀수였던 경우0
		
    }

}

//////////////////////////////////////////////////////////////////////////////////
// Q5: frontBackSplitLinkedList
//////////////////////////////////////////////////////////////////////////////////

void frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList) {
    	if (!ll || !resultFrontList || !resultBackList) return;

    int n = ll->size;
    if (n <= 0) { // 빈 리스트
        resultFrontList->head = NULL; resultFrontList->size = 0;
        resultBackList->head  = NULL; resultBackList->size  = 0;
		ll->head = NULL; ll->size = 0;
        return;
    }
    if (n == 1) { // 한 개면 앞:1, 뒤:0
        resultFrontList->head = ll->head; resultFrontList->size = 1;
        resultBackList->head  = NULL;     resultBackList->size  = 0;
        // 원본을 보존하고 싶으면 ll는 그대로 두어도 되고,
        // 분할만 하려면 ll->head는 그대로 두되 링크는 이미 한 개라 손댈 게 없음.
        
		ll->head = NULL; ll->size = 0;
		return;
    }

	int frontCount = (n+1)/2;

	ListNode *tail = findNode(ll,frontCount-1);
	ListNode *backHead = (tail? tail->next : NULL);

	if (tail) tail->next = NULL;

	resultFrontList->head = ll->head;
	resultFrontList->size = frontCount;
	resultBackList->head = backHead;
	resultBackList->size = n-frontCount;

	ll->head = NULL; ll->size = 0;
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q6: moveMaxToFront
//////////////////////////////////////////////////////////////////////////////////

int moveMaxToFront(ListNode **ptrHead) {
    
		if (ptrHead == NULL || *ptrHead == NULL)
			return -1; // 빈 리스트

    ListNode *curr = *ptrHead;
    ListNode *prev = NULL;          // 현재 노드의 이전 노드
    ListNode *max_node = curr;      // 최댓값 노드
    ListNode *max_prev = NULL;      // 최댓값 노드의 이전 노드
    int max_item = curr->item;      // 최댓값

    //  최댓값 노드 찾기
    while (curr != NULL) {
        if (curr->item > max_item) {
            max_item = curr->item;
            max_node = curr;
            max_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    
    }
    // 이미 맨 앞이면 이동 불필요
    if (max_node == *ptrHead)
        return 0;

    //  max_node를 원래 위치에서 제거
    if (max_prev != NULL)
        max_prev->next = max_node->next;

    //  max_node를 맨 앞으로 이동
    max_node->next = *ptrHead;
    *ptrHead = max_node;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q7: RecursiveReverse
//////////////////////////////////////////////////////////////////////////////////

void RecursiveReverse(ListNode **ptrHead) {
    ListNode *curr = *ptrHead;

	if(curr ==NULL || curr->next ==NULL) return;

	ListNode *rest = curr->next;
	RecursiveReverse(&rest);

	curr->next->next = curr;
	curr->next = NULL;

	*ptrHead = rest;

    
}

//////////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////////

void test_insertSortedLL() {
    printf("\n=== Testing Q1: insertSortedLL ===\n");
    LinkedList ll;
    int result;
    
    // Test 1: Insert into empty list
    initList(&ll);
    result = insertSortedLL(&ll, 5);
    assert(result == 0);
    assert(ll.size == 1);
    printf("✓ Test 1 passed: Insert into empty list\n");
    removeAllItems(&ll);
    
    // Test 2: Insert in ascending order
    initList(&ll);
    assert(insertSortedLL(&ll, 2) == 0);
    assert(insertSortedLL(&ll, 3) == 1);
    assert(insertSortedLL(&ll, 5) == 2);
    assert(insertSortedLL(&ll, 7) == 3);
    assert(insertSortedLL(&ll, 9) == 4);
    int expected[] = {2, 3, 5, 7, 9};
    assert(compareList(&ll, expected, 5));
    printf("✓ Test 2 passed: Insert in ascending order\n");
    
    // Test 3: Insert in middle
    result = insertSortedLL(&ll, 8);
    assert(result == 4);
    int expected2[] = {2, 3, 5, 7, 8, 9};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 3 passed: Insert in middle (8 at index 4)\n");
    
    // Test 4: Duplicate - should return -1
    result = insertSortedLL(&ll, 5);
    assert(result == -1);
    assert(ll.size == 6);
    printf("✓ Test 4 passed: Reject duplicate value\n");
    
    // Test 5: Insert at beginning
    initList(&ll);
    insertSortedLL(&ll, 10);
    insertSortedLL(&ll, 20);
    result = insertSortedLL(&ll, 5);
    assert(result == 0);
    printf("✓ Test 5 passed: Insert at beginning\n");
    
    removeAllItems(&ll);
}

void test_alternateMergeLL() {
    printf("\n=== Testing Q2: alternateMergeLinkedList ===\n");
    LinkedList ll1, ll2;
    
    // Test 1: Basic merge
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    insertNode(&ll1, 2, 3);
    insertNode(&ll2, 0, 4);
    insertNode(&ll2, 1, 5);
    insertNode(&ll2, 2, 6);
    insertNode(&ll2, 3, 7);
    
    alternateMergeLinkedList(&ll1, &ll2);
    int expected1[] = {1, 4, 2, 5, 3, 6};
    assert(compareList(&ll1, expected1, 6));
    int expected2[] = {7};
    assert(compareList(&ll2, expected2, 1));
    printf("✓ Test 1 passed: Basic alternate merge\n");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 2: ll1 larger than ll2
    initList(&ll1);
    initList(&ll2);
    for (int i = 1; i <= 6; i++) insertNode(&ll1, ll1.size, i);
    for (int i = 10; i <= 12; i++) insertNode(&ll2, ll2.size, i);
    
    alternateMergeLinkedList(&ll1, &ll2);
    assert(ll1.size == 9);
    assert(ll2.size == 0);
    printf("✓ Test 2 passed: ll1 larger, ll2 becomes empty\n");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 3: Empty ll2
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    alternateMergeLinkedList(&ll1, &ll2);
    assert(ll1.size == 2);
    printf("✓ Test 3 passed: Empty ll2\n");
    removeAllItems(&ll1);
}

void test_moveOddItemsToBack() {
    printf("\n=== Testing Q3: moveOddItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveOddItemsToBack(&ll);
    int expected1[] = {2, 4, 18, 3, 7, 15};
    assert(compareList(&ll, expected1, 6));
    printf("✓ Test 1 passed: Mixed odd and even\n");
    removeAllItems(&ll);
    
    // Test 2: Another mix
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveOddItemsToBack(&ll);
    int expected2[] = {2, 18, 4, 7, 3, 15};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 2 passed: Another arrangement\n");
    removeAllItems(&ll);
    
    // Test 3: All odd
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveOddItemsToBack(&ll);
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: All odd numbers\n");
    removeAllItems(&ll);
    
    // Test 4: All even
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveOddItemsToBack(&ll);
    assert(compareList(&ll, input4, 3));
    printf("✓ Test 4 passed: All even numbers\n");
    removeAllItems(&ll);
}

void test_moveEvenItemsToBack() {
    printf("\n=== Testing Q4: moveEvenItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveEvenItemsToBack(&ll);
    int expected1[] = {3, 7, 15, 2, 4, 18};
    assert(compareList(&ll, expected1, 6));
    printf("✓ Test 1 passed: Mixed odd and even\n");
    removeAllItems(&ll);
    
    // Test 2: Another mix
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveEvenItemsToBack(&ll);
    int expected2[] = {7, 3, 15, 2, 18, 4};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 2 passed: Another arrangement\n");
    removeAllItems(&ll);
    
    // Test 3: All odd
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveEvenItemsToBack(&ll);
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: All odd numbers\n");
    removeAllItems(&ll);
    
    // Test 4: All even
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveEvenItemsToBack(&ll);
    assert(compareList(&ll, input4, 3));
    printf("✓ Test 4 passed: All even numbers\n");
    removeAllItems(&ll);
}

void test_frontBackSplitLL() {
    printf("\n=== Testing Q5: frontBackSplitLinkedList ===\n");
    LinkedList ll, front, back;
    
    // Test 1: Odd number of elements
    initList(&ll);
    initList(&front);
    initList(&back);
    int input1[] = {2, 3, 5, 6, 7};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront1[] = {2, 3, 5};
    int expectedBack1[] = {6, 7};
    assert(compareList(&front, expectedFront1, 3));
    assert(compareList(&back, expectedBack1, 2));
    printf("✓ Test 1 passed: Odd number of elements (5)\n");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 2: Even number of elements
    initList(&ll);
    int input2[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) insertNode(&ll, ll.size, input2[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront2[] = {1, 2};
    int expectedBack2[] = {3, 4};
    assert(compareList(&front, expectedFront2, 2));
    assert(compareList(&back, expectedBack2, 2));
    printf("✓ Test 2 passed: Even number of elements (4)\n");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 3: Single element
    initList(&ll);
    insertNode(&ll, 0, 42);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront3[] = {42};
    assert(compareList(&front, expectedFront3, 1));
    assert(back.size == 0);
    printf("✓ Test 3 passed: Single element\n");
    removeAllItems(&front);
    removeAllItems(&back);
}

void test_moveMaxToFront() {
    printf("\n=== Testing Q6: moveMaxToFront ===\n");
    LinkedList ll;
    
    // Test 1: Max in middle
    initList(&ll);
    int input1[] = {30, 20, 40, 70, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    moveMaxToFront(&(ll.head));
    int expected1[] = {70, 30, 20, 40, 50};
    assert(compareList(&ll, expected1, 5));
    printf("✓ Test 1 passed: Max at index 3\n");
    removeAllItems(&ll);
    
    // Test 2: Max at end
    initList(&ll);
    int input2[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input2[i]);
    moveMaxToFront(&(ll.head));
    int expected2[] = {50, 10, 20, 30, 40};
    assert(compareList(&ll, expected2, 5));
    printf("✓ Test 2 passed: Max at end\n");
    removeAllItems(&ll);
    
    // Test 3: Max already at front
    initList(&ll);
    int input3[] = {100, 20, 30};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveMaxToFront(&(ll.head));
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: Max already at front\n");
    removeAllItems(&ll);
    
    // Test 4: Single element
    initList(&ll);
    insertNode(&ll, 0, 42);
    moveMaxToFront(&(ll.head));
    int expected4[] = {42};
    assert(compareList(&ll, expected4, 1));
    printf("✓ Test 4 passed: Single element\n");
    removeAllItems(&ll);
}

void test_recursiveReverse() {
    printf("\n=== Testing Q7: RecursiveReverse ===\n");
    LinkedList ll;
    
    // Test 1: Multiple elements
    initList(&ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    RecursiveReverse(&(ll.head));
    int expected1[] = {5, 4, 3, 2, 1};
    assert(compareList(&ll, expected1, 5));
    printf("✓ Test 1 passed: Reverse 5 elements\n");
    removeAllItems(&ll);
    
    // Test 2: Two elements
    initList(&ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) insertNode(&ll, ll.size, input2[i]);
    RecursiveReverse(&(ll.head));
    int expected2[] = {20, 10};
    assert(compareList(&ll, expected2, 2));
    printf("✓ Test 2 passed: Reverse 2 elements\n");
    removeAllItems(&ll);
    
    // Test 3: Single element
    initList(&ll);
    insertNode(&ll, 0, 18);
    RecursiveReverse(&(ll.head));
    int expected3[] = {18};
    assert(compareList(&ll, expected3, 1));
    printf("✓ Test 3 passed: Single element (no change)\n");
    removeAllItems(&ll);
    
    // Test 4: Odd number of elements
    initList(&ll);
    int input4[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    RecursiveReverse(&(ll.head));
    int expected4[] = {3, 2, 1};
    assert(compareList(&ll, expected4, 3));
    printf("✓ Test 4 passed: Reverse 3 elements\n");
    removeAllItems(&ll);
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  Linked List Test Suite - All 7 Questions             ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_insertSortedLL();
    test_alternateMergeLL();
    test_moveOddItemsToBack();
    test_moveEvenItemsToBack();
    test_frontBackSplitLL();
    test_moveMaxToFront();
    test_recursiveReverse();
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ ALL TESTS PASSED!                                  ║\n");
    printf("║  Total: 26 test cases across 7 functions              ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    return 0;
}