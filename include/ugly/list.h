#ifndef UGLY_LIST_H
#define UGLY_LIST_H

#include "core.h"

// Dynamic array with contiguous storage, constant access and amortized insert.
typedef struct {
	index_t length;
	index_t capacity;
	byte_t* data;
	size_t elem_size;
	allocator_fn_t allocator;
} list_t;

/** Initializes LIST with initial capacity for N elements of TYPE_SIZE bytes
 * and sets it to use ALLOC (will be set to a default when NULL) as allocator.
 *
 * Returns 0 on success or ENOMEM in case ALLOC fails.
 *
 * list_destroy() must be called on LIST afterwards. */
err_t list_init(list_t* list, index_t n, size_t type_size, allocator_fn_t alloc);

// Frees any resources allocated by LIST.
void list_destroy(list_t* list);

// Gets the number of elements currently stored inside LIST.
index_t list_size(const list_t* list);

// Checks if LIST is empty.
inline bool list_empty(const list_t* list)
{
	return list_size(list) <= 0;
}

// Returns a pointer to the element at INDEX in the dynamic array LIST.
void* list_ref(const list_t* list, index_t index);

/** Inserts TYPE_SIZE bytes of ELEMENT at position INDEX of the LIST.
 *
 * Returns 0 on success or ENOMEM in case ALLOC fails. */
err_t list_insert(list_t* list, index_t index, const void* element);

// Equivalent to list_insert() at the end of the list.
err_t list_append(list_t* list, const void* element);

// Pops the element at INDEX from LIST and copies it to SINK.
void list_remove(list_t* list, index_t index, void* restrict sink);

// Equivalent to list_remove() from the end of the list.
inline void list_pop(list_t* list, void* restrict sink)
{
	list_remove(list, list_size(list) - 1, sink);
}

// Swaps elements in indexes A and B of LIST.
void list_swap(list_t* list, index_t a, index_t b);

/** Searches a sorted LIST for KEY using COMP to compare its elements.
 *
 * Returns the index of KEY, or a negative value when not found. */
index_t list_search(const list_t* list, const void* key, compare_fn_t comp);

/** Inserts ELEM in sorted LIST, using COMP to sort its position.
 *
 * Returns the index where it was inserted, or a negative value when
 * list_insert() would have failed. */
index_t list_insert_sorted(list_t* list, const void* elem, compare_fn_t comp);

// Sorts LIST in-place using COMPARE to order its elements.
void list_sort(list_t* list, int (*compare)(const void*, const void*));

// Checks if LIST is sorted (ascending order) through the COMPARE function.
bool list_sorted(const list_t* list, compare_fn_t compare);

#endif // UGLY_LIST_H
