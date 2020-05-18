
#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h>	// isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE 50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char data;
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
TREE *createTree(void);

/* Deletes all data in tree and recycles memory
*/
void destroyTree(TREE *pTree);

static void _destroy(NODE *root);

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode(char ch);

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree(char *expr, TREE *pTree);

/* Print node in tree using inorder traversal
*/
void traverseTree(TREE *pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE *pTree);

/* internal traversal function
*/
static void _infix_print(NODE *root, int level);

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix(char *expr);

////////////////////////////////////////////////////////////////////////////////
void destroyTree(TREE *pTree)
{
	if (pTree)
	{
		_destroy(pTree->root);
	}

	free(pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree(TREE *pTree)
{
	_infix_print(pTree->root, 0);

	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree(TREE *pTree)
{
	_traverse(pTree->root);

	return;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TREE *tree;
	char expr[1024];

	fprintf(stdout, "\nInput an expression (postfix): ");

	while (fscanf(stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();

		if (!tree)
		{
			printf("Cannot create tree\n");
			return 100;
		}

		// postfix expression -> expression tree
		int ret = postfix2tree(expr, tree);
		if (!ret)
		{
			fprintf(stdout, "invalid expression!\n");
			continue;
		}

		// expression tree -> infix expression
		fprintf(stdout, "\nInfix expression : ");
		traverseTree(tree);

		// print tree with right-to-left infix traversal
		fprintf(stdout, "\n\nTree representation:\n");
		printTree(tree);

		// evaluate postfix expression
		float val = evalPostfix(expr);
		fprintf(stdout, "\nValue = %f\n", val);

		// destroy tree
		destroyTree(tree);

		fprintf(stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}

TREE *createTree(void)
{
	TREE *newTree;
	if (!(newTree = malloc(sizeof(TREE))))
	{
		return NULL;
	}
	else
	{
		newTree->root = NULL;
		return newTree;
	}
}

int postfix2tree(char *expr, TREE *pTree)
{
	NODE *stack[MAX_STACK_SIZE];
	int top = -1;
	int cnt = 0;
	for (int i = 0; expr[i]; i++)
	{
		NODE *newNode = _makeNode(expr[i]);
		// valid counter check
		if (cnt < 0)
		{
			_destroy(newNode);
			goto EXCEPTION;
		}

		if (isdigit(expr[i]))
		{
			stack[++top] = newNode;
			cnt++;
		}
		else
		{
			// invalid
			if (top < 1)
			{
				_destroy(newNode);
				goto EXCEPTION;
			}

			int flag = 0;
			char oper[] = {'+', '-', '/', '*'};
			for (int j = 0; j < 4; j++)
			{
				if (expr[i] == oper[j])
					flag = 1;
			}
			if (!flag)
			{
				_destroy(newNode);
				goto EXCEPTION;
			}

			newNode->right = stack[top--];
			newNode->left = stack[top--];
			stack[++top] = newNode;
			cnt--;
		}
	}

	if (!(top == 0 && cnt == 1))
	{
		goto EXCEPTION;
	}
	else
	{
		pTree->root = stack[top];
		return 1;
	}

EXCEPTION:
	for (int i = 0; i <= top; i++)
	{
		_destroy(stack[i]);
	}
	free(pTree);
	return 0;
}

static void _traverse(NODE *root)
{
	if (!root)
	{
		return;
	}

	if (isdigit(root->data))
	{
		fprintf(stdout, "%c", root->data);
	}
	else
	{
		fprintf(stdout, "(");
		_traverse(root->left);
		fprintf(stdout, "%c", root->data);
		_traverse(root->right);
		fprintf(stdout, ")");
	}
}

static void _infix_print(NODE *root, int level)
{
	if (!root)
	{
		return;
	}

	if (isdigit(root->data))
	{
		for (int i = 0; i < level; i++)
		{
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "%c\n", root->data);
	}
	else
	{
		_infix_print(root->right, level + 1);
		for (int i = 0; i < level; i++)
		{
			fprintf(stdout, "\t");
		}
		fprintf(stdout, "%c\n", root->data);
		_infix_print(root->left, level + 1);
	}
}

float evalPostfix(char *expr)
{
	float stack[MAX_STACK_SIZE];
	int top = -1;
	for (int i = 0; expr[i]; i++)
	{
		if (isdigit(expr[i]))
		{
			stack[++top] = expr[i] - '0';
		}
		else
		{
			float second = stack[top--];
			float first = stack[top--];

			if (expr[i] == '+')
			{
				stack[++top] = first + second;
			}
			else if (expr[i] == '-')
			{
				stack[++top] = first - second;
			}
			else if (expr[i] == '*')
			{
				stack[++top] = first * second;
			}
			else
			{
				stack[++top] = first / second;
			}
		}
	}

	return stack[top];
}

static void _destroy(NODE *root)
{
	if (root->left)
	{
		_destroy(root->left);
	}
	if (root->right)
	{
		_destroy(root->right);
	}
	free(root);
}

static NODE *_makeNode(char ch)
{
	NODE *newNode;
	if (!(newNode = malloc(sizeof(NODE))))
	{
		return NULL;
	}
	else
	{
		newNode->data = ch;
		newNode->left = NULL;
		newNode->right = NULL;
	}

	return newNode;
}