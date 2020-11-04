#include "list.h"

#include <assert.h>
#include <string.h> // memcpy
#include <stdlib.h> // bsearch, qsort
#include <errno.h>

#include "core.h" // NULL, memswap, STDLIB_ALLOCATOR


err_t list_init(list_t *list, index_t length, size_t type_size, struct allocator alloc)
{
	assert(length >= 0);
	assert(type_size > 0);

	list->length = 0;
	list->capacity = length;
	list->elem_size = type_size;

	list->alloc = alloc.method != NULL ? alloc : STDLIB_ALLOCATOR;
	list->data = list->alloc.method(&list->alloc, NULL, length * list->elem_size);
	if (list->data == NULL && length != 0) return ENOMEM;

	return 0;
}

void list_destroy(list_t *list)
{
	list->alloc.method(&list->alloc, list->data, 0);
}

index_t list_size(const list_t *list)
{
	return list->length;
}

extern inline bool list_empty(const list_t *list);

inline void *list_ref(const list_t *list, index_t index)
{
	assert(0 <= index);
	assert(index < list->length);
	return list->data + index * list->elem_size;
}

static err_t list_grow(list_t *list)
{
	list->capacity = list->capacity > 0 ? list->capacity * 2 : 8;
	void *new = list->alloc.method(&list->alloc, list->data, list->capacity * list->elem_size);
	if (new == NULL) return ENOMEM;
	list->data = new;
	return 0;
}

err_t list_append(list_t *list, const void *element)
{
	// grow current capacity in case its not enough
	if (list->length + 1 > list->capacity) {
		const err_t error = list_grow(list);
		if (error) return error;
	}

	// append copy to the end of the list
	byte_t *end = list->data + list->length * list->elem_size;
	memcpy(end, element, list->elem_size);
	list->length++;

	return 0;
}

inline void list_swap(list_t *list, index_t a, index_t b)
{
	memswap(list_ref(list, a), list_ref(list, b), list->elem_size);
}

err_t list_insert(list_t *list, index_t index, const void *element)
{
	assert(0 <= index);
	assert(index <= list->length);

	// append
	const err_t error = list_append(list, element);
	if (error) return error;

	// swap backwards until inserted in the correct position
	for (index_t i = list->length - 1; i > index; --i)
		list_swap(list, i - 1, i);

	return 0;
}

void list_remove(list_t *list, index_t index, void *restrict sink)
{
	assert(0 <= index);
	assert(index < list->length);

	// send copy to output
	const byte_t *source = list_ref(list, index);
	memcpy(sink, source, list->elem_size);

	// swap "removed" (free space) forward
	for (index_t i = index; i < list->length - 1; ++i)
		list_swap(list, i, i + 1);

	list->length--;
}

index_t list_search(const list_t *lst, const void *key, compare_fn_t cmp)
{
	const byte_t *p = bsearch(key, lst->data, lst->length, lst->elem_size, cmp);
	return p == NULL ? -1 : (p - lst->data) / lst->elem_size;
}

void list_sort(list_t *list, compare_fn_t compare)
{
	qsort(list->data, list->length, list->elem_size, compare);
}
