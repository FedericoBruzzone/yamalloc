#ifndef YAMALLOC_RED_BLACK_H
#define YAMALLOC_RED_BLACK_H

#include <stddef.h>
#include <stdint.h>

typedef enum Color { RED, BLACK } Color;
typedef struct RedBlackNode {
	size_t size;
	void *ptr;
	struct RedBlackNode *left, *right, *parent;
	Color color;
} RedBlackNode;
extern void red_black_left_rotate(RedBlackNode *x);
extern void red_black_right_rotate(RedBlackNode *x);

extern void *red_black_yamalloc(size_t size);
extern void *red_black_yacalloc(size_t num, size_t size);
extern void *red_black_yarealloc(void *ptr, size_t size);
extern void red_black_yafree(void *ptr);

#endif // YAMALLOC_RED_BLACK_H
