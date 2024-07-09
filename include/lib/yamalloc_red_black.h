#ifndef YAMALLOC_RED_BLACK_H
#define YAMALLOC_RED_BLACK_H

#include <stddef.h>
#include <stdint.h>

typedef enum Color { RED, BLACK } Color;
typedef struct RedBlackNode {
	size_t size;
	uint8_t is_free;
	struct RedBlackNode *left, *right, *parent;
	Color color;
} RedBlackNode;

extern void *red_black_yamalloc(size_t size);
extern void *red_black_yacalloc(size_t num, size_t size);
extern void *red_black_yarealloc(void *ptr, size_t size);
extern void red_black_yafree(void *ptr);

extern RedBlackNode *red_black_request_space(size_t size);
extern RedBlackNode *red_black_find_free_block(RedBlackNode **last,
					       size_t size);
extern void red_black_coalesce_free_blocks(void);

extern void red_black_left_rotate(RedBlackNode *x);
extern void red_black_right_rotate(RedBlackNode *x);

#endif // YAMALLOC_RED_BLACK_H
