/**
 * @file yamalloc.c
 * @brief Yet Another Malloc implementation
 *
 * @note YAMALLOC_THREAD_SAFE is a macro that enables thread safety. If you
 * are using this library in a multi-threaded environment, make sure to define
 * this macro.
 */

#include "yamalloc.h"
#ifdef YAMALLOC_FREE_LIST
#include "yamalloc_free_list.h"
#endif

void *yamalloc(size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return yamalloc_free_list(size);
#else
	(void)size;
	return NULL;
#endif
}

void *yacalloc(size_t num, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return yacalloc_free_list(num, size);
#else
	(void)num;
	(void)size;
	return NULL;
#endif
}

void *yarealloc(void *ptr, size_t size)
{
#ifdef YAMALLOC_FREE_LIST
	return yarealloc_free_list(ptr, size);
#else
	(void)ptr;
	(void)size;
	return NULL;
#endif
}

void yafree(void *ptr)
{
#ifdef YAMALLOC_FREE_LIST
	yafree_free_list(ptr);
#else
    (void)ptr;
	return;
#endif
}
