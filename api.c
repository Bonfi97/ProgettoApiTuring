//#pragma GCC optimize ("O3")
//#pragma GCC optimize ("-fpredictive-commoning")
//#pragma GCC optimize ("-freorder-blocks-and-partition")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>



int staticCounter = 0;


typedef enum boolEnum {
	TRUE = 1, FALSE = 0
} boolType;

enum rbtColor_e {
	red, black
};

typedef enum moveType_e {
	U, W, R
}moveType_t;

typedef struct transitionResultsListElem_s {
	char tapeOut;
	int nextState;
	signed char tapeMovement;
	boolType isFinal;
	moveType_t moveType;
	struct transitionResultsListElem_s *next;
} transitionResultsListElem;

typedef struct hashTableElem_s {
	int state;
	//int listSize;
	transitionResultsListElem *listHead;
}hashTableElem;

typedef struct hashTable_s {
	hashTableElem *hashTable;
	int size;
	int insertedElements;
}hashTableType;

void hashTableInsert(hashTableType *hashTable, int, char, char, int, moveType_t);
unsigned int hash(unsigned int x);
int hashTableLookUpPosition(hashTableElem* hashTable, int size, int state);
void freeHashTable(hashTableType * hashTable);



typedef struct statesRbtElem_s {
	int state;
	struct statesRbtElem_s *p, *l, *r;
	enum rbtColor_e color;
	int listSize;
	transitionResultsListElem *listHead;
} statesRbtElem;

typedef struct returnPair_s {
	statesRbtElem *z, *root;
} returnPair;

statesRbtElem *insertInStateRbt(statesRbtElem *, int, char, char, int, moveType_t);

statesRbtElem RBTNULL;

typedef struct accListElem_s {
	int val;
	struct accListElem_s *next;
}accListElem;

typedef struct tapeChunkElem_s {
	char *chunk;
	unsigned char size;
	int ref;

}tapeChunkElem;

struct tapeContainer_s {
	tapeChunkElem *tape[2];
};

typedef struct quequeElem_s {
	struct tapeContainer_s tapeContainer;
	int currentState;
	long executedMoves, currentTapePosition;
	struct quequeElem_s *next;
} queueElem;

typedef struct runQueue_s {
	queueElem *head, *tail;
	int size;
} runQueue;


returnPair rbInsertFixup(statesRbtElem *, statesRbtElem *);
statesRbtElem *leftRotate(statesRbtElem *, statesRbtElem *);
statesRbtElem *rightRotate(statesRbtElem *, statesRbtElem *);
transitionResultsListElem *transitionResultsListPushTop(transitionResultsListElem *, char, char, int, moveType_t, boolType);

long MIN(long, long);
long customAbs(long v);
int fileParser(FILE *);
void showtree(hashTableType *, char inTape);
void printQueue(runQueue *queue);
void setFinal(hashTableType *, int);
transitionResultsListElem *getTransitionList(int currentState, char currentTapeChar);

#ifdef _MSC_VER
char startQueue(struct tapeContainer_s *, long);
void enqueue(runQueue *queue, queueElem *new);
queueElem *dequeue(runQueue *queue);

void tapeWrite(struct tapeContainer_s *container, long index, char in);
char getTape(struct tapeContainer_s *container, long index);
queueElem *newQueueElem(queueElem *src, int nextState, char tapeOut, int tapeMovement, moveType_t moveType);
void freeQueueElem(queueElem *elem);
#else
char startQueue(struct tapeContainer_s *, long)  __attribute__((hot));
void enqueue(runQueue *queue, queueElem *new) __attribute__((hot));
queueElem *dequeue(runQueue *queue) __attribute__((hot));

void tapeWrite(struct tapeContainer_s *container, long index, char in) __attribute__((hot));
char getTape(struct tapeContainer_s *container, long index) __attribute__((hot));
queueElem *newQueueElem(queueElem *src, int nextState, char tapeOut, int tapeMovement, moveType_t moveType) __attribute__((hot));
void freeQueueElem(queueElem *elem) __attribute__((hot));
#endif


hashTableType tapeInArray[UCHAR_MAX + 1];
long maximunMoves;

boolType zeroAccStateFlag = FALSE;
//boolType noAccStateFlag = TRUE;
//boolType accUnreachableFlag = TRUE;
boolType noAccStateFlag = TRUE;
boolType accUnreachableFlag = TRUE;


long chunkNumber = 0;
//#define CHUNK_SIZE 50
//int chunkSize = 8192;
//int chunkSize = (1<<15);
//int chunkSize = (1 << 16)*1.2;
long chunkSize = (1 << 9);
unsigned char initialTapeSize = 10;
unsigned char tapeSizeIncrement = 2;

#define hashTableLoadFactor 0.4

#define  noSeekOptFlag 0
#define seekBackOpt 1

int main(int argc, char *argv[]) {
	FILE *fp;
	int i;
	char c;

	//printf("@@@@@@@@@@@@@@@@@@@@\n");

	for (i = 0; i < UCHAR_MAX + 1; i++) {
		tapeInArray[i].hashTable = NULL;
		tapeInArray[i].insertedElements = 0;
		tapeInArray[i].size = 0;
	}


	fp = stdin;
#ifdef _MSC_VER
	fp = fopen("41.txt", "r");
#endif
	if (fileParser(fp) == 0)
		return 1;

	//hashTableElem *nullList = malloc(sizeof(hashTableElem));
	//nullList->listHead = NULL;

	//for (i = 0; i < UCHAR_MAX + 1; i++) {
	//	if (tapeInArray[i].hashTable == NULL) {
	//		tapeInArray[i].hashTable = nullList;
	//	}
	//}

	//exit(1);

	//printf("SHOW TREE:\n");

	//for (i = 0; i < UCHAR_MAX + 1; i++) {
	//	if (tapeInArray[i].size >0) {
	//		showtree(&(tapeInArray[i]), i);
	//	}
	//}

	//fflush(stdout);
	//exit(1);

	//printf("END PARSING\n\n");
	////maximunMoves = 10;



	c = getc(fp);
	if (c == '\r') {
		c = getc(fp);
	}
	while (c != EOF) {
		if (noAccStateFlag || accUnreachableFlag) {
			//printf("0\n");
			putchar('0');
			//printf(" boh");
			putchar('\n');

			while (c != '\n' && c != EOF) {
				c = getc(fp);
				if (c == '\r') {
					c = getc(fp);
				}
			}
		}
		else if (zeroAccStateFlag) {
			putchar('1');
			putchar('\n');
			while (c != '\n' && c != EOF) {
				c = getc(fp);
				if (c == '\r') {
					c = getc(fp);
				}
			}
		}
		else {
			struct tapeContainer_s *container = malloc(sizeof(struct tapeContainer_s));
			container->tape[0] = NULL;
			container->tape[1] = malloc(sizeof(tapeChunkElem));
			container->tape[1]->size = initialTapeSize;
			container->tape[1]->ref = 1;
			container->tape[1]->chunk = malloc(sizeof(char) * (1 << container->tape[1]->size));
			memset(container->tape[1]->chunk, '_', sizeof(char) * (1 << container->tape[1]->size));

			//for (int v = 0; v < 2 * maximunMoves + 1 ; v++)
			//	printf("%c", tape[v]);
			//printf("\n");
			//fflush(stdout);
			long index = maximunMoves;
			while (c != '\n' && c != EOF) {
				if (index < 2 * maximunMoves + 1) {
					tapeWrite(container, index, c);
					//printf("%d\n",c);
				}
				index++;
				c = getc(fp);
				if (c == '\r') {
					c = getc(fp);
				}
			}

			//for (int v = 0; v < (2 * maximunMoves + 1); v++) {
			//	if (v == maximunMoves)
			//		printf("^%C^", getTape(container, v));
			//	else
			//		printf("%c", getTape(container, v));
			//}
			//printf("\n");
			//system("pause");
			//fflush(stdout);

			//for (int v = 0; v < chunkNumber; v++)
			//	printf("chunkArray[%d] = %d\n", v, chunkArray[v]);
			//system("pause");
			if (index > maximunMoves) {
				char ris = startQueue(container, maximunMoves);
				free(container);
				//printf("%c\n", ris);
				putchar(ris);
				putchar('\n');
			}
			//else {
			//	exit(1);
			//}

			//system("pause");
		}

		if (c != EOF) {
			c = getc(fp);
			if (c == '\r') {
				c = getc(fp);
			}
		}
	}


	//for (i = 0; i < UCHAR_MAX + 1; i++) {
	//	if (tapeInArray[i] .size >0) {
	//		showtree(&(tapeInArray[i]), i);
	//	}
	//}


	//for (i = 0; i < UCHAR_MAX + 1; i++) {
	//	if (tapeInArray[i].size > 0) {
	//		freeHashTable(&(tapeInArray[i]));
	//	}
	//}
	//free(nullList);


#ifdef _MSC_VER
	system("pause");
#endif
	return 0;
}

int fileParser(FILE *fp) {
	int i;

	fscanf(fp, "tr\n");

	char buf[14];
	char b, c, d;
	int a, e;
	boolType stop;
	for (i = 0, stop = FALSE; !stop; i++) {
		fscanf(fp, "%13s ", buf);

		if (strcmp(buf, "acc") == 0)
			stop = TRUE;
		else {
			moveType_t moveType;
			//fscanf(" %c %c %c %d\n", &b, &c, &d, &e);
			fscanf(fp, " %c", &b);
			fscanf(fp, " %c", &c);
			fscanf(fp, " %c", &d);
			fscanf(fp, " %d\n", &e);
			a = atoi(buf);
			//printf("%d %c %c %c %d\n", a, b, c, d, e);
			if (b == c) {
				if (a == e && d == 'S')
					moveType = U;
				else
					moveType = R;
			}
			else {
				moveType = W;
			}
			hashTableInsert(&(tapeInArray[(unsigned char)b]), a, c, d, e, moveType);
			//tapeInArray[(unsigned char)b] = insertInStateRbt(tapeInArray[(unsigned char)b], a, c, d, e, moveType);
		}


	}

	stop = FALSE;
	//accListElem *listHead = NULL;
	while (!stop) {
		fscanf(fp, "%13s\n", buf);

		if (strcmp(buf, "max") == 0)
			stop = TRUE;
		else {
			//accListElem *new = malloc(sizeof(accListElem));
			//new->next = listHead;
			//new->val = atoi(buf);
			noAccStateFlag = FALSE;
			int tmp = atoi(buf);
			if (tmp == 0)
				zeroAccStateFlag = TRUE;
			//printf("acc : %d\n", tmp);
			for (i = 0; i < UCHAR_MAX; i++) {
				noAccStateFlag = FALSE;
				setFinal(&(tapeInArray[i]), tmp);
			}
		}
	}

	/*if (listHead == NULL) {
		noAccStateFlag = TRUE;
	}
	else {
		accListElem *p = listHead, *tmp;
		while (p) {
			for (i = 0; i < UCHAR_MAX; i++) {
				setFinal(tapeInArray[i], p->val);
			}
			tmp = p;
			p = p->next;
			free(p);
		}
	}*/

	fscanf(fp, "%ld\n", &maximunMoves);

	fscanf(fp, "run\n");

	//printf("max: %d\n", maximunMoves);

	return 1;
}

void showtree(hashTableType * hashTable, char inTape) {
	int i;
	transitionResultsListElem * p;

	for (i = 0; i < hashTable->size; i++) {
		for (p = hashTable->hashTable[i].listHead; p; p = p->next) {
			printf("%d %c %c %d %d", hashTable->hashTable[i].state, inTape, p->tapeOut, p->tapeMovement, p->nextState);
			if (p->isFinal)
				printf(" Finale");
			printf("\n");
		}
	}

}

void freeHashTable(hashTableType * hashTable) {
	transitionResultsListElem * p;
	int i;

	for (i = 0; i < hashTable->size; i++) {
		if (hashTable->hashTable[i].listHead != NULL) {
			p = hashTable->hashTable[i].listHead;
			while (p) {
				transitionResultsListElem *tmp = p;
				p = p->next;
				free(tmp);
			}
		}
	}
	free(hashTable->hashTable);

}

void setFinal(hashTableType * hashTable, int state) {
	transitionResultsListElem * p;
	int i;

	for (i = 0; i < hashTable->size; i++) {
		if (hashTable->hashTable[i].listHead != NULL) {
			for (p = hashTable->hashTable[i].listHead; p; p = p->next) {
				if (p->nextState == state) {
					p->isFinal = TRUE;
					accUnreachableFlag = FALSE;
				}

			}
		}
	}

}

void hashTableInsert(hashTableType * hashTable, int currentState, char tapeOut, char tapeMovement, int nextState, moveType_t moveType) {
	if (hashTable->size == 0) {
		hashTable->insertedElements = 0;
		hashTable->size = 128;
		hashTable->hashTable = calloc(hashTable->size, sizeof(hashTableElem));
	}

	hashTable->insertedElements += 1;

	if (((double)hashTable->insertedElements) / hashTable->size > hashTableLoadFactor) {
		int oldSize = hashTable->size;
		hashTableElem *oldHashTable = hashTable->hashTable;
		hashTable->size = hashTable->size*2;
		hashTable->hashTable = calloc(hashTable->size, sizeof(hashTableElem));
		int i;
		for (i = 0; i < oldSize; i++) {
			if (oldHashTable[i].listHead != NULL) {
				int index = hashTableLookUpPosition(hashTable->hashTable, hashTable->size, oldHashTable[i].state);
				hashTable->hashTable[index].listHead = oldHashTable[i].listHead;
				//hashTable->hashTable[index].listSize = oldHashTable[i].listSize;
				hashTable->hashTable[index].state = oldHashTable[i].state;
			}
		}
		free(oldHashTable);
	}

	int index = hashTableLookUpPosition(hashTable->hashTable, hashTable->size, currentState);
	hashTable->hashTable[index].listHead = transitionResultsListPushTop(hashTable->hashTable[index].listHead, tapeOut, tapeMovement, nextState, moveType, FALSE);
	hashTable->hashTable[index].state = currentState;
}

int hashTableLookUpPosition(hashTableElem* hashTable, int size, int state) {
	int i;
	unsigned int calculatedHash = hash(state);
	//unsigned int calculatedHash = state;
	for (i = 0; i < size; i++) {
		int index = (unsigned)(calculatedHash + i) & (size - 1);
		if ((hashTable[index].state == state && hashTable[index].listHead != NULL) || hashTable[index].listHead == NULL) {
			return index;
		}
		//if (i > 50) {
		//	exit(1);
		//}
	}
}

unsigned int hash(unsigned int x) {
	//x = (x ^ 61) ^ (x >> 16);
	//x = x + (x << 3);
	//x = x ^ (x >> 4);
	//x = x * 0x27d4eb2d;
	//x = x ^ (x >> 15);

	//x = ((x >> 16) ^ x) * 0x45d9f3b;
	//x = ((x >> 16) ^ x) * 0x45d9f3b;
	//x = (x >> 16) ^ x;

	x *= 2654435769;

	//x = (x + 0x7ed55d16) + (x << 12);
	//x = (x ^ 0xc761c23c) ^ (x >> 19);
	//x = (x + 0x165667b1) + (x << 5);
	//x = (x + 0xd3a2646c) ^ (x << 9);
	//x = (x + 0xfd7046c5) + (x << 3);
	//x = (x ^ 0xb55a4f09) ^ (x >> 16);

	//x = (x ^ 0xdeadbeef) + (x << 4);
	//x = x ^ (x >> 10);
	//x = x + (x << 7);
	//x = x ^ (x >> 13);

	//x ^= (x >> 20) ^ (x >> 12);
	//x = x ^ (x >> 7) ^ (x >> 4);

	return x;
}

//statesRbtElem *insertInStateRbt(statesRbtElem *root, int currentState, char tapeOut, char tapeMovement, int nextState, moveType_t moveType) {
//	statesRbtElem *y = &RBTNULL, *x = root, *z;
//	boolType alreadyInside = FALSE;
//
//	while (x != &RBTNULL && !alreadyInside) {
//		y = x;
//		if (currentState == x->state)
//			alreadyInside = TRUE;
//		else if (currentState < x->state)
//			x = x->l;
//		else
//			x = x->r;
//	}
//
//	if (alreadyInside) {
//		//printf("Already inside\n");
//		y->listHead = transitionResultsListPushTop(y->listHead, tapeOut, tapeMovement, nextState, moveType, &(y->listSize), FALSE);
//		//if (y->listHead == NULL) {
//		//	printf("err\n");
//		//	return NULL;
//		//}
//	}
//	else {
//		//printf("new elem\n");
//		z = malloc(sizeof(statesRbtElem));
//		/*if (!(z = malloc(sizeof(statesRbtElem)))) {
//			printf("errore allocazione di un nodo di statesRbtElem in insertInStateRbt\n");
//			return NULL;
//		}*/
//
//		z->p = y;
//		z->state = currentState;
//		z->l = &RBTNULL;
//		z->r = &RBTNULL;
//
//		if (y == &RBTNULL)
//			root = z;
//		else if (z->state < y->state)
//			y->l = z;
//		else
//			y->r = z;
//
//		z->listHead = NULL;
//		z->listHead = transitionResultsListPushTop(z->listHead, tapeOut, tapeMovement, nextState, moveType, &(z->listSize), FALSE);
//		//if (z->listHead == NULL) {
//		//	printf("err\n");
//		//	return NULL;
//		//}
//		z->listSize = 1;
//
//		z->color = red;
//		returnPair tmp;
//		tmp = rbInsertFixup(root, z);
//		z = tmp.z;
//		root = tmp.root;
//	}
//
//	return root;
//}
//
//returnPair rbInsertFixup(statesRbtElem *root, statesRbtElem *z) {
//	statesRbtElem *x, *y;
//	if (z == root) {
//		root->color = black;
//	}
//	else {
//		x = z->p;
//		if (x->color == red) {
//			if (x == x->p->l) {
//				y = x->p->r;
//				if (y->color == red) {
//					x->color = black;
//					y->color = black;
//					x->p->color = red;
//					returnPair tmp;
//					tmp = rbInsertFixup(root, x->p);
//					x->p = tmp.z;
//					root = tmp.root;
//				}
//				else if (z == x->r) {
//					z = x;
//					root = leftRotate(root, z);
//					x = z->p;
//				}
//				x->color = black;
//				x->p->color = red;
//				root = rightRotate(root, x->p);
//			}
//			else {
//				y = x->p->l;
//				if (y->color == red) {
//					x->color = black;
//					y->color = black;
//					x->p->color = red;
//					returnPair tmp;
//					tmp = rbInsertFixup(root, x->p);
//					x->p = tmp.z;
//					root = tmp.root;
//				}
//				else if (z == x->l) {
//					z = x;
//					root = rightRotate(root, z);
//					x = z->p;
//				}
//				x->color = black;
//				x->p->color = red;
//				root = leftRotate(root, x->p);
//			}
//		}
//
//	}
//	returnPair ret;
//	ret.z = z;
//	ret.root = root;
//	return ret;
//}
//
//statesRbtElem *leftRotate(statesRbtElem *root, statesRbtElem *x) {
//	statesRbtElem * y;
//	y = x->r;
//	x->r = y->l;
//
//	if (y->l != &RBTNULL)
//		y->l->p = x;
//	y->p = x->p;
//	if (x->p == &RBTNULL)
//		root = y;
//	else if (x == x->p->l)
//		x->p->l = y;
//	else
//		x->p->r = y;
//	y->l = x;
//	x->p = y;
//	return root;
//}
//
//statesRbtElem *rightRotate(statesRbtElem *root, statesRbtElem *x) {
//	statesRbtElem * y;
//	y = x->l;
//	x->l = y->r;
//
//	if (y->r != &RBTNULL)
//		y->r->p = x;
//	y->p = x->p;
//	if (x->p == &RBTNULL)
//		root = y;
//	else if (x == x->p->r)
//		x->p->r = y;
//	else
//		x->p->l = y;
//	y->r = x;
//	x->p = y;
//	return root;
//}

transitionResultsListElem *transitionResultsListPushTop(transitionResultsListElem *h, char tapeOut, char tapeMovement, int nextSate, moveType_t moveType, boolType isFinal) {
	transitionResultsListElem *p, *tmp = NULL;
	char movementConverted;
	if (tapeMovement == 'R')
		movementConverted = 1;
	else if (tapeMovement == 'L')
		movementConverted = -1;
	else
		movementConverted = 0;

	if (h != NULL) {
		p = h;
		while (p) {
			if (p->nextState == nextSate && p->tapeMovement == movementConverted && p->tapeOut == tapeOut) {
				//printf("check");
				return h;
			}
			tmp = p;
			p = p->next;
		}
	}

	transitionResultsListElem *new;
	new = malloc(sizeof(transitionResultsListElem));
	//if (new = malloc(sizeof(transitionResultsListElem))) {
	new->tapeOut = tapeOut;
	new->tapeMovement = movementConverted;
	new->nextState = nextSate;
	new->isFinal = isFinal;
	new->next = NULL;
	new->moveType = moveType;
	//new->moveType = W;

	if (h == NULL) {
		h = new;
	}
	else {
		tmp->next = new;
	}
	/*}
	else {
		printf("errore allocazione transitionResultsListElem in transitionResultsListPushTop\n");
		h = NULL;
	}*/
	return h;
}

char startQueue(struct tapeContainer_s *container, long originalTapeIndex) {

	staticCounter = 0;

	runQueue queue;
	transitionResultsListElem *listHead;
	char status = '0';

	queueElem *new;
	new = malloc(sizeof(queueElem));

	queue.head = new;
	queue.tail = new;

	new->next = NULL;
	new->currentState = 0;
	new->currentTapePosition = originalTapeIndex;
	new->executedMoves = 0;
	new->tapeContainer.tape[0] = NULL;
	new->tapeContainer.tape[1] = container->tape[1];

	//queue.size = 1;

	//runQueue waitQueue;
	//waitQueue.head = NULL;
	//waitQueue.size = 0;
	//waitQueue.tail = NULL;

	//while ((queue.head != NULL || waitQueue.head != NULL) && status != '1')
	while (queue.head != NULL && status != '1') {

		//printf("status = %c\n", status);
		//printf("\n");
		//printQueue(&queue);

		queue.head->executedMoves++;

		//if (queue.head->executedMoves >= maximunMoves)
		if (queue.head->executedMoves > maximunMoves) {
			status = 'U';
			//printf("U BRANCH\n");
			queueElem *tmp = dequeue(&queue);
			freeQueueElem(tmp);
		}
		else {
			//tapeChunkElem *chunk = queue.head->tapeChunkArray[queue.head->currentTapePosition / chunkSize];
			char actualTapeChar = getTape(&(queue.head->tapeContainer), queue.head->currentTapePosition);
			listHead = getTransitionList(queue.head->currentState, actualTapeChar);

			//transitionResultsListElem *p;
			//int i;
			//for (i = 0, p = listHead; i < listSize; i++, p = p->next) {
			//	printf("%d %c %c %d %d", queue.head->currentState, actualTapeChar, p->tapeOut, p->tapeMovement, p->nextState);
			//	if (p->isFinal)
			//		printf(" Finale");
			//	printf("\n");
			//}

			if (listHead == NULL) {
				//printf("NO MOVE BRANCH\n");
				queueElem *tmp = dequeue(&queue);
				freeQueueElem(tmp);
			}
			else {
				if (queue.head->executedMoves == maximunMoves) {
					//printf("MAX MOVES BRANCH\n");
					transitionResultsListElem *p;
					for (p = listHead; p; p = p->next) {
						if (p->isFinal) {
							status = '1';
							goto END;
							//break;
						}
					}
					//if (status != '1') {
					enqueue(&queue, dequeue(&queue));
					//}
				}
				else {
					//printf("DEFAULT BRANCH\n");
					queueElem *tmp = queue.head;
					transitionResultsListElem *p;
					boolType isForked = FALSE;
					for (p = listHead->next; p; p = p->next) {
						//printf("LIST MORE THAN 1\n");
						if (p->isFinal) {
							status = '1';
							goto END;
							//break;
							//printf("is Final\n");
						}
						else {
							//queueElem *new = malloc(sizeof(queueElem));
							//new->currentState = p->nextState;
							//new->tape = malloc(sizeof(char)*originalTapeSize);
							//memcpy(new->tape, tmp->tape, sizeof(char)*originalTapeSize);
							//new->tape[tmp->currentTapePosition] = p->tapeOut;
							//new->currentTapePosition = tmp->currentTapePosition + p->tapeMovement;
							//new->executedMoves = tmp->executedMoves;
							if (p->moveType == U) {
								status = 'U';
							}
							else {
								//isForked = TRUE;
								queueElem *new = newQueueElem(tmp, p->nextState, p->tapeOut, p->tapeMovement, p->moveType);
								enqueue(&queue, new);
							}
						}
					}
					if (listHead->isFinal) {
						status = '1';
						goto END;
						//printf("is Final\n");
					}
					else {
						if (listHead->moveType == U) {
							status = 'U';
							freeQueueElem(dequeue(&queue));
						}
						else {
							tmp->currentState = listHead->nextState;
							//tmp->tape[tmp->currentTapePosition] = listHead->tapeOut;
							if (listHead->moveType == W) {
								tapeWrite(&(tmp->tapeContainer), tmp->currentTapePosition, listHead->tapeOut);
							}
							tmp->currentTapePosition = tmp->currentTapePosition + listHead->tapeMovement;
							if (isForked == FALSE) {
								enqueue(&queue, dequeue(&queue));
							}
						}
					}
				}

				//}

			}
		}

		//if (queue.head == NULL && waitQueue.head != NULL) {
		//	enqueue(&queue, dequeue(&waitQueue));
		//}

	}
	//if (queue.head == NULL)
	//	printf("head == NULL\n");
	//if (status == '1')
	//	printf("STATUS == 1\n");
END:;

	while (queue.head != NULL) {
		freeQueueElem(dequeue(&queue));
	}

	//while (waitQueue.head != NULL) {
	//	freeQueueElem(dequeue(&waitQueue));
	//}

	return status;
}

inline void enqueue(runQueue *queue, queueElem *new) {

	if (queue->tail == NULL) {
		queue->head = new;
		queue->tail = new;
		//queue->tail->next = NULL;
		return;
	}

	queue->tail->next = new;
	queue->tail = new;
	queue->tail->next = NULL;

	//if (queue->tail == NULL) {
	//	printf("!!!!!!!!!!!!\nBOH\n!!!!!!!!!!!!!!\n");
	//	queue->head = new;
	//	queue->tail = new;
	//}
	//else {
	//	queue->tail->next = new;
	//	queue->tail = new;
	//}
	//queue->size++;


	//if (queue->tail == NULL)
	//	queue->tail = new;
	//else {
	//	queue->tail->next = new;
	//	queue->tail = new;
	//}
	//queue->tail->next = NULL;
	//if (queue->head == NULL)
	//	queue->head = queue->tail;

	//queue->size++;
}

inline queueElem *dequeue(runQueue *queue) {

	if (queue->head == NULL) {
		return NULL;
	}

	queueElem *old = queue->head;
	queue->head = queue->head->next;

	if (queue->head == NULL)
		queue->tail = NULL;
	return old;



	//queueElem *old = NULL;
	//if (queue->size == 0)
	//	printf("errore coda vuota\n");
	//else {
	//	old = queue->head;
	//	queue->head = queue->head->next;
	//	queue->size--;
	//}


	//queueElem *old = queue->head;
	//queue->head = queue->head->next;
	//if (queue->tail == old) {
	//	queue->tail = NULL;
	//}
	//queue->size--;
	////queue->head = queue->head->next == NULL ? queue->head : queue->head->next;
	//return old;
}

inline transitionResultsListElem *getTransitionList(int currentState, char currentTapeChar) {
	hashTableType *tmp = &(tapeInArray[(unsigned char)currentTapeChar]);

	if (tmp->hashTable == NULL) {
		return NULL;
	}

	int index = hashTableLookUpPosition(tmp->hashTable, tmp->size, currentState);
	return tmp->hashTable[index].listHead;

}

inline void tapeWrite(struct tapeContainer_s *container, long index, char in) {
	//container->tape[index / maximunMoves]->chunk;
	unsigned char i = (index >= maximunMoves);
	long n = customAbs(index - maximunMoves);
	if (container->tape[i] == NULL) {
		container->tape[i] = malloc(sizeof(tapeChunkElem));
		container->tape[i]->size = initialTapeSize;
		container->tape[i]->chunk = malloc(sizeof(char)* (1 << container->tape[i]->size));
		memset(container->tape[i]->chunk, '_', sizeof(char)*(1 << container->tape[i]->size));
		container->tape[i]->ref = 1;
	}

	while (n >= (1 << container->tape[i]->size)) {
		unsigned char oldSize = container->tape[i]->size;
		container->tape[i]->size += tapeSizeIncrement;
		long m = MIN(1 << container->tape[i]->size, maximunMoves + 1);
		//long m = 1 << container->tape[i]->size;
		container->tape[i]->chunk = realloc(container->tape[i]->chunk, sizeof(char)* m);
		memset(container->tape[i]->chunk + (1 << oldSize), '_', sizeof(char)* (m - (1 << oldSize)));
	}

	if (container->tape[i]->ref > 1) {
		container->tape[i]->ref--;
		tapeChunkElem *tmp = container->tape[i];
		container->tape[i] = malloc(sizeof(tapeChunkElem));
		container->tape[i]->chunk = malloc(sizeof(char)* (1 << tmp->size));
		memcpy(container->tape[i]->chunk, tmp->chunk, sizeof(char)* (1 << tmp->size));
		container->tape[i]->size = tmp->size;
		container->tape[i]->ref = 1;
	}
	//printf("%d > %d\n", abs(index - maximunMoves), (2 << container->tape[i]->size));


	container->tape[i]->chunk[n] = in;
}

inline char getTape(struct tapeContainer_s *container, long index) {
	unsigned char i = (index >= maximunMoves);
	long n = customAbs(index - maximunMoves);
	if (container->tape[i] == NULL) {
		return '_';
	}
	else if (n >= (1 << container->tape[i]->size)) {
		return '_';
	}
	return container->tape[i]->chunk[n];

}

queueElem *newQueueElem(queueElem *src, int nextState, char tapeOut, int tapeMovement, moveType_t moveType) {
	queueElem *new = malloc(sizeof(queueElem));
	new->currentState = nextState;

	new->tapeContainer.tape[0] = src->tapeContainer.tape[0];
	new->tapeContainer.tape[1] = src->tapeContainer.tape[1];

	if (new->tapeContainer.tape[0] != NULL) {
		new->tapeContainer.tape[0]->ref++;
	}
	if (new->tapeContainer.tape[1] != NULL) {
		new->tapeContainer.tape[1]->ref++;
	}

	if (moveType == W) {
		tapeWrite(&(new->tapeContainer), src->currentTapePosition, tapeOut);
	}
	new->executedMoves = src->executedMoves;
	new->currentTapePosition = src->currentTapePosition + tapeMovement;
	return new;

}

void freeQueueElem(queueElem *elem) {

	if (elem->tapeContainer.tape[0] != NULL) {
		if (elem->tapeContainer.tape[0]->ref == 1) {
			free(elem->tapeContainer.tape[0]->chunk);
			free(elem->tapeContainer.tape[0]);
		}
		else {
			elem->tapeContainer.tape[0]->ref--;
		}
	}
	if (elem->tapeContainer.tape[1] != NULL) {
		if (elem->tapeContainer.tape[1]->ref == 1) {
			free(elem->tapeContainer.tape[1]->chunk);
			free(elem->tapeContainer.tape[1]);
		}
		else {
			elem->tapeContainer.tape[1]->ref--;
		}
	}

	free(elem);

}


void printQueue(runQueue *queue) {
	queueElem *tmp = queue->head;
	int boo;
	staticCounter++;
	printf("Iterazione %d:\n", staticCounter);
	while (tmp != NULL) {
		printf("tmp: %d\n", tmp);
		printf("tmp->currentState: %d\n", tmp->currentState);
		printf("tmp->currentTapePosition: %d\n", tmp->currentTapePosition);
		printf("tmp->next: %d\n", tmp->next);
		printf("tmp->executedMoves: %d\n", tmp->executedMoves);
		printf("tmp->tapeContainer.tape[0]: %d", tmp->tapeContainer.tape[0]);
		if (tmp->tapeContainer.tape[0] != NULL)
			printf("  (ref = %d)\n", tmp->tapeContainer.tape[0]->ref);
		else
			printf("  (ref = 1)\n");

		printf("tmp->tapeContainer.tape[1]: %d", tmp->tapeContainer.tape[1]);
		if (tmp->tapeContainer.tape[1] != NULL)
			printf("  (ref = %d)\n", tmp->tapeContainer.tape[1]->ref);
		else
			printf("  (ref = 1)\n");
		printf("\ttape:\n");
		for (int v = 0; v < (2 * maximunMoves + 1); v++) {
			boo = v;
			if (v == tmp->currentTapePosition)
				printf("^%C^", getTape(&(tmp->tapeContainer), v));
			else
				printf("%c", getTape(&(tmp->tapeContainer), v));
		}

		printf("\nv=%d\n", boo);
		printf("\n");
		fflush(stdout);
		tmp = tmp->next;
	}

}

inline long MIN(long a, long b) {
	return a ^ ((b ^ a) & -(b < a));
}

inline long customAbs(long v) {
	int const mask = v >> sizeof(long) * CHAR_BIT - 1;
	return (v + mask) ^ mask;
}
