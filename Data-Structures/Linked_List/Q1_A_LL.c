//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 1 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList


///////////////////////// function prototypes ////////////////////////////////////

//You should not change the prototype of this function
int insertSortedLL(LinkedList *ll, int item);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);


//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;

	//Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	printf("1: Insert an integer to the sorted linked list:\n");
	printf("2: Print the index of the most recent input value:\n");
	printf("3: Print sorted linked list:\n");
	printf("0: Quit:");

	while (c != 0)
	{
		printf("\nPlease input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertSortedLL(&ll, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			printf("The value %d was added at index %d\n", i, j);
			break;
		case 3:
			printf("The resulting sorted linked list is: ");
			printList(&ll);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}


	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item)
{
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


///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll){

	ListNode *cur; // 탐색할 노드 포인터
	if (ll == NULL) // 리스트가 NULL이면 종료
		return;
	cur = ll->head; // 탐색 포인터를 헤드로 초기화

	if (cur == NULL) // 리스트가 비어있으면
		printf("Empty"); // Empty 출력
	while (cur != NULL) // 탐색 포인터가 NULL이 아닐 때까지
	{
		printf("%d ", cur->item); // 아이템 출력
		cur = cur->next; // 다음 노드로 이동
	}
	printf("\n");
}


void removeAllItems(LinkedList *ll)
{
	ListNode *cur = ll->head; // 탐색 포인터
	ListNode *tmp; // 임시 포인터

	while (cur != NULL){
		tmp = cur->next; // 다음 노드 임시 저장
		free(cur); // 현재 노드 메모리 해제
		cur = tmp;// 다음 노드로 이동 한 노드씩 이동하면서 모든 노드 해제
	}
	ll->head = NULL; // 헤드 NULL로 초기화
	ll->size = 0; // ll 사이즈 0으로 초기화
}


ListNode *findNode(LinkedList *ll, int index){

	ListNode *temp; //탐색 포인터

	if (ll == NULL || index < 0 || index >= ll->size) //ll이 NULL이거나 index가 음수이거나 index가 ll의 size보다 크면 NULL 반환
		return NULL;

	temp = ll->head; //탐색 포인터를 헤드로 초기화

	if (temp == NULL || index < 0) //temp가 NULL이거나 index가 음수이면
		return NULL;

	while (index > 0){
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		index--;
	}

	return temp;
}

int insertNode(LinkedList *ll, int index, int value){

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}


	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL){
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}


int removeNode(LinkedList *ll, int index){

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL){

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
