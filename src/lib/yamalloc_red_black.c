#include "yamalloc_red_black.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
// #include <sys/mman.h>
#define __USE_XOPEN_EXTENDED
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
// Simulate sbrk using VirtualAlloc
void *sbrk(intptr_t increment)
{
	static void *heap_end = NULL;
	void *prev_heap_end;

	if (heap_end == NULL) {
		heap_end = VirtualAlloc(
		    NULL, CHUNK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!heap_end)
			return (void *)-1;
	}

	prev_heap_end = heap_end;

	if (VirtualAlloc(heap_end, increment, MEM_COMMIT, PAGE_READWRITE) ==
	    NULL) {
		return (void *)-1;
	}

	heap_end = (char *)heap_end + increment;
	return prev_heap_end;
}
#elif defined(__linux__) || defined(__APPLE__)
#define sbrk sbrk
#endif

#define ALIGNMENT 8

static RedBlackNode *root = NULL;

#ifdef YAMALLOC_THREAD_SAFE
#include <pthread.h>
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
 * @brief Rotate the tree to the left
 *
 * This function rotates the tree to the left.
 *
 * @param[in] x The node to rotate
 */
void left_rotate(RedBlackNode *x)
{
	RedBlackNode *y = x->right;
	x->right = y->left;
	if (y->left)
		y->left->parent = x;
	y->parent = x->parent;
	if (!x->parent) // No parent
		root = y;
	else if (x == x->parent->left) // x was left child
		x->parent->left = y;
	else // x was right child
		x->parent->right = y;
	y->left = x;
	x->parent = y;
}

/**
 * @brief Rotate the tree to the right
 *
 * This function rotates the tree to the right.
 *
 * @param[in] x The node to rotate
 */
void right_rotate(RedBlackNode *x)
{
	RedBlackNode *y = x->left;
	x->left = y->right;
	if (y->right)
		y->right->parent = x;
	y->parent = x->parent;
	if (!x->parent) // No parent
		root = y;
	else if (x == x->parent->right) // x was right child
		x->parent->right = y;
	else // x was left child
		x->parent->left = y;
	y->right = x;
	x->parent = y;
}
