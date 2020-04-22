#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h>	// toupper

#define QUIT 1
#define FORWARD_PRINT 2
#define BACKWARD_PRINT 3
#define SEARCH 4
#define DELETE 5
#define COUNT 6

// User structure type definition
typedef struct
{
	char *token;
	int freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN *dataPtr;
	struct node *llink;
	struct node *rlink;
} NODE;

typedef struct
{
	int count;
	NODE *pos; // unused
	NODE *head;
	NODE *rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void);

/* Deletes all data in list and recycles memory
*/
void destroyList(LIST *pList);

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode(LIST *pList, tTOKEN *dataInPtr);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode(LIST *pList, char *keyPtr, tTOKEN **dataOut);

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST *pList, char *pArgu, tTOKEN **pDataOut);

/* returns number of nodes in list
*/
int countList(LIST *pList);
/* returns	1 empty
			0 list has data
*/
int emptyList(LIST *pList);

//int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList(LIST *pList);

/* prints data from list (backward)
*/
void printListR(LIST *pList);

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken(char *str);

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
void destroyToken(tTOKEN *pToken);

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf("%c", &ch);
	ch = toupper(ch);
	switch (ch)
	{
	case 'Q':
		return QUIT;
	case 'F':
		return FORWARD_PRINT;
	case 'B':
		return BACKWARD_PRINT;
	case 'S':
		return SEARCH;
	case 'D':
		return DELETE;
	case 'C':
		return COUNT;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (!fp)
	{
		fprintf(stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}

	// creates an empty list
	list = createList();
	if (!list)
	{
		printf("Cannot create list\n");
		return 100;
	}

	while (fscanf(fp, "%s", str) == 1)
	{
		pToken = createToken(str);

		// insert function call
		ret = addNode(list, pToken);

		if (ret == 2) // duplicated
			destroyToken(pToken);
	}

	fclose(fp);

	fprintf(stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");

	while (1)
	{
		int action = get_action();

		switch (action)
		{
		case QUIT:
			destroyList(list);
			return 0;

		case FORWARD_PRINT:
			printList(list);
			break;

		case BACKWARD_PRINT:
			printListR(list);
			break;

		case SEARCH:
			fprintf(stderr, "Input a string to find: ");
			fscanf(stdin, "%s", str);

			if (searchList(list, str, &pToken))
			{
				fprintf(stdout, "(%s, %d)\n", pToken->token, pToken->freq);
			}
			else
				fprintf(stdout, "%s not found\n", str);
			break;

		case DELETE:
			fprintf(stderr, "Input a string to delete: ");
			fscanf(stdin, "%s", str);

			if (removeNode(list, str, &pToken))
			{
				fprintf(stdout, "%s deleted\n", pToken->token);
				destroyToken(pToken);
			}
			else
				fprintf(stdout, "%s not found\n", str);
			break;

		case COUNT:
			fprintf(stdout, "%d\n", countList(list));
			break;
		}

		if (action)
			fprintf(stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}

LIST *createList(void)
{
	LIST *listNode = (LIST *)malloc(sizeof(LIST));
	if (!listNode)
		return NULL;

	listNode->count = 0;
	listNode->pos = NULL;
	listNode->head = NULL;
	listNode->rear = NULL;

	return listNode;
}

void destroyList(LIST *pList)
{
	NODE *pLoc = pList->head;
	while (pLoc)
	{
		NODE *tmp = pLoc->rlink;
		tTOKEN *token;
		_delete(pList, NULL, pLoc, &token);
		destroyToken(token);
		pLoc = tmp;
	}
	free(pList);
	return;
}

int addNode(LIST *pList, tTOKEN *dataInPtr)
{
	static int cnt = 0;

	NODE *pPre, *pLoc;
	pPre = NULL, pLoc = pList->head;
	if (_search(pList, &pPre, &pLoc, dataInPtr->token))
	{
		// fprintf(stderr, "> %d\n", ++cnt);
		pLoc->dataPtr->freq++;
		return 2;
	}
	else
	{
		return _insert(pList, pPre, dataInPtr); // count++ed.
	}
	// printf(stderr, ">> %d\n", cnt);
}

int removeNode(LIST *pList, char *keyPtr, tTOKEN **dataOut)
{
	NODE *pPre = NULL, *pLoc = pList->head;
	if (_search(pList, &pPre, &pLoc, keyPtr))
	{
		_delete(pList, pPre, pLoc, dataOut);
		return 1;
	}

	return 0;
}

int searchList(LIST *pList, char *pArgu, tTOKEN **pDataOut)
{
	NODE *pPre, *pLoc;
	pPre = NULL;
	pLoc = pList->head;
	if (_search(pList, &pPre, &pLoc, pArgu))
	{
		*pDataOut = pLoc->dataPtr;

		return 1;
	}
	return 0;
}

int countList(LIST *pList)
{
	return pList->count;
}

int emptyList(LIST *pList)
{
	return pList->count ? 0 : 1;
}

void printList(LIST *pList)
{
	NODE *current = pList->head;

	while (current)
	{
		fprintf(stdout, "%s\t%d\n", current->dataPtr->token, current->dataPtr->freq);
		current = current->rlink;
	}

	return;
}

void printListR(LIST *pList)
{
	NODE *current = pList->rear;
	while (current)
	{
		fprintf(stdout, "%s\t%d\n", current->dataPtr->token, current->dataPtr->freq);
		current = current->llink;
	}

	return;
}

static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr)
{
	NODE *newNode = (NODE *)malloc(sizeof(NODE));
	if (!newNode)
		return 0;

	newNode->dataPtr = dataInPtr;
	newNode->llink = NULL;
	newNode->rlink = NULL;
	(pList->count)++;

	if (!pPre)
	{
		newNode->llink = NULL;
		newNode->rlink = pList->head; // first
		if (pList->head)
		{
			pList->head->llink = newNode;
		}
		pList->head = newNode;
		if (!(pList->rear)) // empty
		{
			pList->rear = newNode;
		}
	}
	else
	{
		newNode->rlink = pPre->rlink;
		newNode->llink = pPre;

		if (pList->rear == pPre)
		{
			pList->rear = newNode;
		}
		else
		{
			pPre->rlink->llink = newNode;
		}

		pPre->rlink = newNode;
	}
	return 1;
}

static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr)
{
	if (!pLoc)
		return;

	if (!pPre)
	{
		pList->head = pLoc->rlink;
	}
	else
	{
		pPre->rlink = pLoc->rlink;
	}
	if (pList->rear == pLoc)
	{
		pList->rear = pPre;
	}
	else
	{
		pLoc->rlink->llink = pPre;
	}

	*dataOutPtr = pLoc->dataPtr;
	free(pLoc);

	return;
}

static int _search(LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu)
{
	while (*pLoc)
	{
		int result = strcmp(pArgu, (*pLoc)->dataPtr->token);
		if (result < 0)
			return 0;
		else if (result == 0)
			return 1;
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}

	return 0;
}

tTOKEN *createToken(char *str)
{
	tTOKEN *tok = (tTOKEN *)malloc(sizeof(tTOKEN));
	if (!tok)
		return NULL;

	tok->token = strdup(str);
	tok->freq = 1;

	return tok;
}

void destroyToken(tTOKEN *pToken)
{
	free(pToken->token);
	free(pToken);
	return;
}
