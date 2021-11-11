/**
 * @file list.h
 * @brief Dynamic arrays (elsewhere known as vectors).
 */

#ifndef UGLY_LIST_H
#define UGLY_LIST_H

#include "core.h"

/// Dynamic array with contiguous storage, O(1) access and O(n) insert/remove.
typedef struct {
	index_t length;
	index_t capacity;
	byte_t *data;
	size_t elem_size;
	struct allocator alloc;
} list_t;

/**
 * @brief Initializes a generic list.
 *
 * @param list list to be initialized, should be destroyed later.
 * @param length initial list capacity in number of elements.
 * @param type_size size, in bytes, of each list element.
 * @param alloc memory allocator to be used (in-place reallocation support is recommended).
 *
 * @return 0 on success or ENOMEM in case alloc fails.
 */
err_t list_init(list_t *list, index_t length, size_t type_size, struct allocator alloc);

/// Frees any resources allocated by the given list.
void list_destroy(list_t *list);

/// Gets the number of elements currently stored in the list.
index_t list_size(const list_t *list);

/// Checks whether the list is empty.
inline bool list_empty(const list_t *list)
{
	return list_size(list) <= 0;
}

/// Returns a pointer to the element being indexed in the dynamic list.
void *list_ref(const list_t *list, index_t index);

/**
 * @brief Inserts an element at the given index of the list.
 * All previous elements keep their indexes and all of the next ones are moved.
 *
 * @return 0 on success or ENOMEM in case ALLOC fails.
 */
err_t list_insert(list_t *list, index_t index, const void *element);

/// Equivalent to `list_insert()` at the end of the list.
err_t list_append(list_t *list, const void *element);

/**
 * @brief Pops the indexed element from the list to the given address.
 * All previous elements keep their indexes and all of the next ones are moved.
 */
void list_remove(list_t *list, index_t index, void *restrict sink);

/// Swaps list elements in the given indexes.
void list_swap(list_t *list, index_t a, index_t b);

/// Sorts list in-place using the given function to order its elements.
void list_sort(list_t *list, compare_fn_t compare);

/**
 * @brief Searches a SORTED list for some element, using an ordering function.
 * @return Returns the index where the element was found and a negative value otherwise.
 */
index_t list_search(const list_t *list, const void *key, compare_fn_t compare);

#endif // UGLY_LIST_H
