#include "yamalloc.h"

#if defined(YAMALLOC_FREE_LIST) && defined(YAMALLOC_RED_BLACK)
#error "Only one memory allocation method can be selected"
#endif

#if !defined(YAMALLOC_FREE_LIST) && !defined(YAMALLOC_RED_BLACK)
#error                                                                         \
    "No memory allocation method selected, please define YAMALLOC_FREE_LIST or YAMALLOC_RED_BLACK"
#endif

#ifdef YAMALLOC_FREE_LIST
#include "yamalloc_free_list.h"
#endif // YAMALLOC_FREE_LIST

#ifdef YAMALLOC_RED_BLACK
#include "yamalloc_red_black.h"
#endif // YAMALLOC_RED_BLACK_TREE

void *yamalloc(size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yamalloc(size);
#elif YAMALLOC_RED_BLACK
	return red_black_yamalloc(size);
#endif
}

void *yacalloc(size_t num, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yacalloc(num, size);
#elif YAMALLOC_RED_BLACK
	return red_black_yacalloc(num, size);
#endif
}

void *yarealloc(void *ptr, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return free_list_yarealloc(ptr, size);
#elif YAMALLOC_RED_BLACK
	return red_black_yarealloc(ptr, size);
#endif
}

void yafree(void *ptr)
{
#ifdef YAMALLOC_FREE_LIST
	free_list_yafree(ptr);
#elif YAMALLOC_RED_BLACK
	red_black_yafree(ptr);
#endif
}
