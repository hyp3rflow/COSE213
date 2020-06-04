#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h>	// time

#define MAX_ELEM 20

typedef struct
{
	int *heapArr;
	int last;
	int capacity;
} HEAP;

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP *heapCreate(int capacity);

/* Free memory for heap
*/
void heapDestroy(HEAP *heap);

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert(HEAP *heap, int data);

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp(HEAP *heap, int index);

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete(HEAP *heap, int *data);

/* Reestablishes heap by moving data in root down to its correct location in the heap
	실제로 걍 0 들어감
*/
static void _reheapDown(HEAP *heap, int index);

/* Print heap array */
void heapPrint(HEAP *heap);

int main(void)
{
	HEAP *heap;
	int data;
	int i;

	heap = heapCreate(MAX_ELEM);

	srand(time(NULL));

	for (i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number

		fprintf(stdout, "Inserting %d: ", data);

		// insert function call
		heapInsert(heap, data);

		heapPrint(heap);
	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete(heap, &data);

		printf("Deleting %d: ", data);

		heapPrint(heap);
	}

	heapDestroy(heap);

	return 0;
}

HEAP *heapCreate(int capacity)
{
	HEAP *ptr;
	if (!(ptr = malloc(sizeof(HEAP))))
	{
		return NULL;
	}
	if (!(ptr->heapArr = malloc(sizeof(int) * capacity)))
	{
		return NULL;
	}

	ptr->capacity = capacity;
	ptr->last = -1;

	return ptr;
}

void heapDestroy(HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
}

int heapInsert(HEAP *heap, int data)
{
	if (heap->last + 1 == heap->capacity)
		return 0;

	heap->heapArr[++heap->last] = data;
	_reheapUp(heap, heap->last);
	return 1;
}

static void _reheapUp(HEAP *heap, int index)
{
	if (index)
	{
		int parent = (index - 1) / 2;
		if (heap->heapArr[index] > heap->heapArr[parent])
		{
			int tmp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[parent];
			heap->heapArr[parent] = tmp;

			_reheapUp(heap, parent);
		}
	}
}

int heapDelete(HEAP *heap, int *data)
{
	if (heap->last < 0)
		return 0;
	*data = heap->heapArr[0];

	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->heapArr[heap->last--] = *data;

	_reheapDown(heap, 0);

	return 1;
}

static void _reheapDown(HEAP *heap, int index)
{
	int left = index * 2 + 1, right = index * 2 + 2;
	int largest;
	if (left <= heap->last)
	{
		if (right <= heap->last)
		{
			largest = heap->heapArr[left] > heap->heapArr[right] ? left : right;
		}
		else
		{
			largest = left;
		}

		if (heap->heapArr[index] < heap->heapArr[largest])
		{
			int tmp = heap->heapArr[largest];
			heap->heapArr[largest] = heap->heapArr[index];
			heap->heapArr[index] = tmp;
			_reheapDown(heap, largest);
		}
	}
}

void heapPrint(HEAP *heap)
{
	if (!heap || !heap->heapArr)
		return;

	for (int i = 0; i <= heap->last; i++)
	{
		printf("%6d", heap->heapArr[i]);
	}
	printf("\n");

	if (heap->last == heap->capacity - 1)
		printf("\n");

	return;
}