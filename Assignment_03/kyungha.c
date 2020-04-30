#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h>  // toupper

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

/* internal insert function
   inserts data into a new node
   return   1 if successful
         0 if memory overflow
*/
static int _insert(LIST *pList, NODE *pPre, tTOKEN *dataInPtr)
{
    NODE *node;
    node = (NODE *)malloc(sizeof(NODE));
    if (!node)
        return 0;

    node->dataPtr = dataInPtr;

    if (pPre == NULL)
    {
        node->llink = NULL;
        if (emptyList(pList) == 1)
        {
            node->rlink = NULL;
            pList->rear = node;
        }
        else
        {
            node->rlink = pList->head;
            pList->head->llink = node;
        }
        pList->head = node;
    }
    else
    {
        if (pPre->rlink == NULL)
        {
            node->rlink = NULL;
            pList->rear = node;
        }
        else
        {
            node->rlink = pPre->rlink;
            node->rlink->llink = node;
        }
        pPre->rlink = node;
        node->llink = pPre;
    }
    pList->count++;

    return 1;
}

/* internal delete function
   deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr)
{
    (*dataOutPtr) = pLoc->dataPtr;
    pLoc->dataPtr = NULL;
    free(pLoc);
}

/* internal search function
   searches list and passes back address of node
   containing target and its logical predecessor
   return   1 found
         0 not found
*/
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu)
{
    if (pList->count == 0)
    {
        *pPre = NULL;
        *pLoc = NULL;
        return 0;
    }
    *pPre = NULL;
    *pLoc = pList->head;

    while ((*pLoc) != NULL && strcmp((*pLoc)->dataPtr->token, pArgu) < 0)
    {
        if (strcmp((*pLoc)->dataPtr->token, pArgu) == 0)
            return 1;
        (*pPre) = (*pLoc);
        (*pLoc) = (*pLoc)->rlink;
    }
    if ((*pLoc) != NULL)
    {
        if (strcmp((*pLoc)->dataPtr->token, pArgu) == 0)
            return 1;
    }
    return 0;
}

/* Allocates dynamic memory for a list head node and returns its address to caller
   return   head node pointer
         NULL if overflow
*/
LIST *createList(void)
{
    LIST *headder;
    headder = (LIST *)malloc(sizeof(LIST));
    if (!headder)
    {
        return NULL;
    }
    headder->count = 0;
    return headder;
}

/* Deletes all data in token structure and recycles memory
   return   NULL head pointer
*/
void destroyToken(tTOKEN *pToken)
{
    pToken->token = NULL;
    free(pToken->token);
    pToken->freq = 0;
    free(pToken);
}

/* Deletes all data in list and recycles memory
*/
void destroyList(LIST *pList)
{
    NODE *pLoc;
    tTOKEN *dataOut;
    while (emptyList(pList) == 0)
    {
        pLoc = pList->head;
        if (removeNode(pList, pLoc->dataPtr->token, &dataOut))
            ;
        destroyToken(dataOut);
    }
    free(pList);
}

/* Inserts data into list
   return   0 if overflow
         1 if successful
         2 if duplicated key
*/
int addNode(LIST *pList, tTOKEN *dataInPtr)
{
    NODE *pPre;
    NODE *pLoc;
    int check = _search(pList, &pPre, &pLoc, dataInPtr->token);
    if (check == 1)
    {
        pLoc->dataPtr->freq++;
        return 2;
    }
    else
    {
        int checkk = _insert(pList, pPre, dataInPtr);
        if (checkk == 1)
            return 1;
        else
            return 0;
    }
}

/* Removes data from list
   return   0 not found
         1 deleted
*/
int removeNode(LIST *pList, char *keyPtr, tTOKEN **dataOut)
{
    NODE *pPre;
    NODE *pLoc;
    int check = _search(pList, &pPre, &pLoc, keyPtr);
    if (check == 1)
    {
        if (pList->count == 1)
        {
            pList->head = NULL;
            pList->rear = NULL;
            pLoc->rlink = NULL;
            pLoc->llink = NULL;
        }
        else if (pLoc == pList->head)
        {
            pList->head = pLoc->rlink;
            pList->head->llink = NULL;
            pLoc->rlink = NULL;
        }
        else if (pLoc == pList->rear)
        {
            pList->rear = pPre;
            pPre->rlink = NULL;
            pLoc->llink = NULL;
        }
        else
        {
            pPre->rlink = pLoc->rlink;
            pLoc->rlink->llink = pPre;
            pLoc->rlink = NULL;
            pLoc->llink = NULL;
        }

        _delete(pList, pPre, pLoc, dataOut);
        pList->count--;
        return 1;
    }
    else
        return 0;
}

/* interface to search function
   Argu   key being sought
   dataOut   contains found data
   return   1 successful
         0 not found
*/
int searchList(LIST *pList, char *pArgu, tTOKEN **pDataOut)
{
    NODE *pPre;
    NODE *pLoc;
    int check = _search(pList, &pPre, &pLoc, pArgu);
    if (check == 1)
    {
        *pDataOut = pLoc->dataPtr;
        return 1;
    }
    else
        return 0;
}

/* returns number of nodes in list
*/
int countList(LIST *pList)
{
    return pList->count;
}

/* returns   1 empty
         0 list has data
*/
int emptyList(LIST *pList)
{
    if (pList->count == 0)
        return 1;
    else
        return 0;
}

int fullList(LIST *pList)
{
    NODE *node;
    node = (NODE *)malloc(sizeof(NODE));
    if (!node)
        return 1;
    else
        free(node);
    return 0;
}

/* prints data from list (forward)
*/
void printList(LIST *pList)
{
    NODE *loc;
    loc = pList->head;
    while (loc != NULL)
    {
        fprintf(stderr, "%s\t%d\n", (char *)(loc->dataPtr->token), (loc->dataPtr->freq));
        loc = loc->rlink;
    }
}

/* prints data from list (backward)
*/
void printListR(LIST *pList)
{
    NODE *loc;
    loc = pList->rear;
    while (loc != NULL)
    {
        fprintf(stderr, "%s\t%d\n", (char *)(loc->dataPtr->token), loc->dataPtr->freq);
        loc = loc->llink;
    }
}

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
   return   token structure pointer
         NULL if overflow
*/
tTOKEN *createToken(char *str)
{
    tTOKEN *node;
    node = (tTOKEN *)malloc(sizeof(tTOKEN));
    if (!node)
        return NULL;
    node->token = strdup(str);
    node->freq = 1;
    return node;
}

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
        /*printf("main / createToken\n");
      if(list->head != NULL)
         fprintf(stderr, "%s\t%d\n", (char*)(list->head->dataPtr->token), (list->head->dataPtr->freq));
      fprintf(stderr, "main / %s\t%d\n", (char*)(pToken->token), (pToken->freq));*/

        // insert function call
        ret = addNode(list, pToken);
        //fprintf(stdout, "main / addNode is : %d\n", ret);

        if (ret == 2) // duplicated
            destroyToken(pToken);
    }

    //fprintf(stderr, "%s\t%d\n", (char*)(list->head->dataPtr->token), (list->head->dataPtr->freq));

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