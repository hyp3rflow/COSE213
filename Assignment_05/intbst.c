#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h>	// time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int data;
	struct node *left;
	struct node *right;
} NODE;

typedef struct
{
	NODE *root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create(void);

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy(TREE *pTree);

/* internal function (not mandatory)
*/
static void _destroy(NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert(TREE *pTree, int data);

/* internal function (not mandatory)
*/
static void _insert(NODE *root, NODE *newPtr);

NODE *_makeNode(int data);

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete(TREE *pTree, int dltKey);

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
static NODE *_delete(NODE *root, int dltKey, int *success);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve(TREE *pTree, int key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve(NODE *root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE *pTree);
static void _traverse(NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE *pTree);
/* internal traversal function
*/
static void _infix_print(NODE *root, int level);

/* 
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE *pTree);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TREE *tree;
	int data;

	// creates a null tree
	tree = BST_Create();

	if (!tree)
	{
		printf("Cannot create a tree!\n");
		return 100;
	}

	//int randt;
	//scanf("%d", &randt);

	fprintf(stdout, "How many numbers will you insert into a BST: ");

	int numbers;
	scanf("%d", &numbers);

	fprintf(stdout, "Inserting: ");

	//srand(randt);
	srand(time(NULL));
	for (int i = 0; i < numbers; i++)
	{
		data = rand() % (numbers * 3) + 1; // random number (1 ~ numbers * 3)

		fprintf(stdout, "%d ", data);

		// insert funtion call
		int ret = BST_Insert(tree, data);
		if (!ret)
			break;
	}
	fprintf(stdout, "\n");

	// inorder traversal
	fprintf(stdout, "Inorder traversal: ");
	BST_Traverse(tree);
	fprintf(stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf(stdout, "Tree representation:\n");
	printTree(tree);

	while (1)
	{
		fprintf(stdout, "Input a number to delete: ");
		int num;
		int ret = scanf("%d", &num);
		if (ret != 1)
			break;

		ret = BST_Delete(tree, num);
		if (!ret)
		{
			fprintf(stdout, "%d not found\n", num);
			continue;
		}

		// print tree with right-to-left infix traversal
		fprintf(stdout, "Tree representation:\n");
		printTree(tree);

		if (BST_Empty(tree))
		{
			fprintf(stdout, "Empty tree!\n");
			break;
		}
	}

	BST_Destroy(tree);

	return 0;
}

TREE *BST_Create(void)
{
	TREE *treePtr;
	if (!(treePtr = malloc(sizeof(TREE))))
	{
		return NULL;
	}

	treePtr->root = NULL;

	return treePtr;
}

int BST_Insert(TREE *pTree, int data)
{
	NODE *newPtr = _makeNode(data);

	if (!newPtr)
		return 0;

	if (!(pTree->root))
	{
		pTree->root = newPtr;
	}
	else
	{
		_insert(pTree->root, newPtr);
	}

	return 1;
}

NODE *_makeNode(int data)
{
	NODE *newPtr;
	if (!(newPtr = malloc(sizeof(NODE))))
		return 0;

	newPtr->data = data;
	newPtr->left = NULL;
	newPtr->right = NULL;

	return newPtr;
}

static void _insert(NODE *root, NODE *newPtr)
{
	if (newPtr->data >= root->data)
	{
		if (root->right)
		{
			_insert(root->right, newPtr);
		}
		else
		{
			root->right = newPtr;
		}
	}
	else
	{
		if (root->left)
		{
			_insert(root->left, newPtr);
		}
		else
		{
			root->left = newPtr;
		}
	}

	return;
}

void BST_Traverse(TREE *pTree)
{
	if (pTree->root)
		_traverse(pTree->root);

	return;
}

static void _traverse(NODE *root)
{
	if (root->left)
		_traverse(root->left);
	fprintf(stdout, "%d ", root->data);
	if (root->right)
		_traverse(root->right);

	return;
}

void printTree(TREE *pTree)
{
	if (pTree->root)
		_infix_print(pTree->root, 0);

	return;
}

static void _infix_print(NODE *root, int level)
{
	if (root->right)
		_infix_print(root->right, level + 1);
	for (int i = 0; i < level; i++)
	{
		fprintf(stdout, "\t");
	}
	fprintf(stdout, "%d\n", root->data);
	if (root->left)
		_infix_print(root->left, level + 1);

	return;
}

int BST_Empty(TREE *pTree)
{
	if (pTree->root == NULL)
		return 1;
	return 0;
}

int *BST_Retrieve(TREE *pTree, int key)
{
	if (pTree->root == NULL)
		return 0;

	return &(_retrieve(pTree->root, key)->data);
}

static NODE *_retrieve(NODE *root, int key)
{
	if (root->data == key)
		return root;
	else if (root->right && root->data < key)
	{
		_retrieve(root->right, key);
	}
	else if (root->left && root->data > key)
	{
		_retrieve(root->left, key);
	}
	else
		return 0;
}

int BST_Delete(TREE *pTree, int dltKey)
{
	if (pTree->root == NULL)
		return 0;

	int flag = 0;
	pTree->root = _delete(pTree->root, dltKey, &flag);
	return flag;
}

static NODE *_delete(NODE *root, int dltKey, int *success)
{
	if (root == NULL)
		return root;

	if (dltKey < root->data)
	{
		root->left = _delete(root->left, dltKey, success);
	}
	else if (dltKey > root->data)
	{
		root->right = _delete(root->right, dltKey, success);
	}
	else
	{
		NODE *tmp;
		*success = 1;
		if (!(root->right))
		{
			tmp = root->left;
			free(root);
			return tmp;
		}
		else if (!(root->left))
		{
			tmp = root->right;
			free(root);
			return tmp;
		}

		tmp = root->right;
		while (tmp && tmp->left)
		{
			tmp = tmp->left;
		}

		root->data = tmp->data;
		root->right = _delete(root->right, tmp->data, success);
	}

	return root;
}

void BST_Destroy(TREE *pTree)
{
	if (pTree)
	{
		_destroy(pTree->root);
		free(pTree);
	}
}

static void _destroy(NODE *root)
{
	if (!root)
		return;
	if (root->left)
		_destroy(root->left);
	if (root->right)
		_destroy(root->right);
	free(root);
}