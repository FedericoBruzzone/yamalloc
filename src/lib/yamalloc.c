#include "yamalloc.h"

#if (defined(YAMALLOC_LINKED_LIST) && defined(YAMALLOC_FREE_LIST_LL)) ||       \
    (defined(YAMALLOC_FREE_LIST_LL) && defined(YAMALLOC_FREE_LIST_RBT)) ||     \
    (defined(YAMALLOC_FREE_LIST_RBT) && defined(YAMALLOC_LINKED_LIST))
#error "Only one memory allocation method can be selected"
#endif

#if !defined(YAMALLOC_LINKED_LIST) && !defined(YAMALLOC_FREE_LIST_LL) &&       \
    !defined(YAMALLOC_FREE_LIST_RBT)
#error                                                                         \
    "No memory allocation method selected, please define YAMALLOC_FREE_LIST or YAMALLOC_RED_BLACK"
#endif

#ifdef YAMALLOC_LINKED_LIST
#include "yamalloc_linked_list.h"
#endif // YAMALLOC_LINKED_LIST

#ifdef YAMALLOC_FREE_LIST_LL
#include "yamalloc_free_list_ll.h"
#endif // YAMALLOC_FREE_LIST_LL

#ifdef YAMALLOC_FREE_LIST_RBT
#include "yamalloc_red_black.h"
#endif // YAMALLOC_FREE_LIST_RBT

void *yamalloc(size_t size)
{
#ifdef YAMALLOC_LINKED_LIST
	return linked_list_yamalloc(size);
#elif YAMALLOC_FREE_LIST_LL
	return free_list_ll_yamalloc(size);
#elif YAMALLOC_FREE_LIST_RBT
	return free_list_rbt_yamalloc(size);
#endif
}

void *yacalloc(size_t num, size_t size)
{
#ifdef YAMALLOC_LINKED_LIST
	return linked_list_yacalloc(num, size);
#elif YAMALLOC_FREE_LIST_LL
	return free_list_ll_yacalloc(num, size);
#elif YAMALLOC_FREE_LIST_RBT
	return free_list_rbt_yacalloc(num, size);
#endif
}

void *yarealloc(void *ptr, size_t size)
{
#ifdef YAMALLOC_LINKED_LIST
	return linked_list_yarealloc(ptr, size);
#elif YAMALLOC_FREE_LIST_LL
	return free_list_ll_yarealloc(ptr, size);
#elif YAMALLOC_FREE_LIST_RBT
	return free_list_rbt_yarealloc(ptr, size);
#endif
}

void yafree(void *ptr)
{
#ifdef YAMALLOC_LINKED_LIST
	linked_list_yafree(ptr);
#elif YAMALLOC_FREE_LIST_LL
	free_list_ll_yafree(ptr);
#elif YAMALLOC_FREE_LIST_RBT
	free_list_rbt_yafree(ptr);
#endif
}
