#define SHOW_STEP 0
#define BALANCING 1

#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h>	// time
#include <string.h> //strcmp, strdup

#define max(x, y) (((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char *data;
	struct node *left;
	struct node *right;
	int height;
} NODE;

typedef struct
{
	NODE *root;
	int count; // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create(void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy(AVL_TREE *pTree);
static void _destroy(NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert(AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert(NODE *root, NODE *newPtr);

static NODE *_makeNode(char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve(AVL_TREE *pTree, char *key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve(NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse(AVL_TREE *pTree);
static void _traverse(NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree(AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print(NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight(NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight(NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft(NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}

	// creates a null tree
	tree = AVL_Create();

	if (!tree)
	{
		fprintf(stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen(argv[1], "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while (fscanf(fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf(stdout, "Insert %s>\n", str);
#endif
		// insert function call
		AVL_Insert(tree, str);

#if SHOW_STEP
		fprintf(stdout, "Tree representation:\n");
		printTree(tree);
#endif
	}

	fclose(fp);

#if SHOW_STEP
	fprintf(stdout, "\n");

	// inorder traversal
	fprintf(stdout, "Inorder traversal: ");
	AVL_Traverse(tree);
	fprintf(stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf(stdout, "Tree representation:\n");
	printTree(tree);
#endif
	fprintf(stdout, "Height of tree: %d\n", tree->root->height);
	fprintf(stdout, "# of nodes: %d\n", tree->count);

	// retrieval
	char *key;
	fprintf(stdout, "Query: ");
	while (fscanf(stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve(tree, str);

		if (key)
			fprintf(stdout, "%s found!\n", key);
		else
			fprintf(stdout, "%s NOT found!\n", str);

		fprintf(stdout, "Query: ");
	}

	// destroy tree
	AVL_Destroy(tree);

	return 0;
}

AVL_TREE *AVL_Create(void)
{
	AVL_TREE *ptr;

	if (ptr = malloc(sizeof(AVL_TREE)))
	{
		ptr->root = NULL;
		ptr->count = 0;

		return ptr;
	}

	return NULL;
}

int AVL_Insert(AVL_TREE *pTree, char *data)
{
	NODE *newNode;
	if (!(newNode = _makeNode(data)))
		return 0;

	pTree->root = _insert(pTree->root, newNode);

	pTree->count++;

	return 1;
}

static NODE *_insert(NODE *root, NODE *newPtr)
{
	if (!root)
		return newPtr;

	if (strcmp(newPtr->data, root->data) < 0)
		root->left = _insert(root->left, newPtr);
	else if (strcmp(newPtr->data, root->data) > 0)
		root->right = _insert(root->right, newPtr);
	else
		return root;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;

#if BALANCING
	int bf = getHeight(root->left) - getHeight(root->right);

	if (bf > 1 && strcmp(newPtr->data, root->left->data) < 0)
	{
		printf("%s : rotateRight\n", root->data);
		return rotateRight(root);
	}
	if (bf < -1 && strcmp(newPtr->data, root->right->data) > 0)
	{

		return rotateLeft(root);
	}
	if (bf > 1 && strcmp(newPtr->data, root->left->data) > 0)
	{
		root->left = rotateLeft(root->left);
		return rotateRight(root);
	}
	if (bf < -1 && strcmp(newPtr->data, root->right->data) < 0)
	{
		root->right = rotateRight(root->right);
		return rotateLeft(root);
	}
#endif

	return root;
}

static NODE *_makeNode(char *data)
{
	NODE *ptr;
	if (ptr = malloc(sizeof(NODE)))
	{
		ptr->data = strdup(data);
		ptr->height = 1;
		ptr->left = NULL;
		ptr->right = NULL;

		return ptr;
	}

	return NULL;
}

static int getHeight(NODE *root)
{
	return root != NULL ? root->height : 0;
}

static NODE *rotateRight(NODE *root)
{
	NODE *axis = root->left;
	NODE *new = axis->right;

	axis->right = root;
	root->left = new;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	axis->height = max(getHeight(axis->left), getHeight(axis->right)) + 1;

	return axis;
}

static NODE *rotateLeft(NODE *root)
{
	NODE *axis = root->right;
	NODE *new = axis->left;

	axis->left = root;
	root->right = new;

	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	axis->height = max(getHeight(axis->left), getHeight(axis->right)) + 1;

	return axis;
}

void AVL_Traverse(AVL_TREE *pTree)
{
	if (pTree->root)
		_traverse(pTree->root);
}
static void _traverse(NODE *root)
{
	if (root->left)
		_traverse(root->left);
	printf("%s ", root->data);
	if (root->right)
		_traverse(root->right);
}

void printTree(AVL_TREE *pTree)
{
	if (pTree->root)
		_infix_print(pTree->root, 0);
}

static void _infix_print(NODE *root, int level)
{
	if (root->right)
		_infix_print(root->right, level + 1);
	for (int i = 0; i < level; i++)
		printf("\t");
	printf("%s\n", root->data);
	if (root->left)
		_infix_print(root->left, level + 1);
}

char *AVL_Retrieve(AVL_TREE *pTree, char *key)
{
	if (pTree->root)
	{
		NODE *result = _retrieve(pTree->root, key);
		return result ? result->data : NULL;
	}

	return NULL;
}

static NODE *_retrieve(NODE *root, char *key)
{
	if (strcmp(root->data, key) == 0)
		return root;
	else if (root->left && strcmp(root->data, key) > 0)
	{
		return _retrieve(root->left, key);
	}
	else if (root->right && strcmp(root->data, key) < 0)
	{
		return _retrieve(root->right, key);
	}
	else
	{
		return NULL;
	}
}

void AVL_Destroy(AVL_TREE *pTree)
{
	if (pTree->root)
	{
		_destroy(pTree->root);
		free(pTree);
	}
}

static void _destroy(NODE *root)
{
	if (root->left)
		_destroy(root->left);
	if (root->right)
		_destroy(root->right);

	free(root->data);
	free(root);
}