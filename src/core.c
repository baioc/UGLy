#include "core.h"

#include <string.h> // memcpy


void swap(void* restrict a, void* restrict b, size_t size)
{
	for (byte_t temp; size != 0; a++, b++, size--) {
		temp = *(byte_t*)a;
		*(byte_t*)a = *(byte_t*)b;
		*(byte_t*)b = temp;
	}
}

typedef index_t (*lerp_fn_t)(const void* x,
                             const void* min, const void* max,
                             index_t low, index_t high);
void* lerpsearch(const void* key, const void* base, index_t length,
                 size_t elem_size, int (*compare)(const void*, const void*),
                 lerp_fn_t lerp)
{
	index_t low = 0;
	index_t high = length - 1;
	const void* min = base;
	const void* max = base + elem_size*high;

	while (low <= high) {
		// interpolate key and check bounds on expected index
		const index_t expected = lerp(key, min, max, low, high);
		if (expected < low || high < expected) return NULL;

		// get the "predicted" address and compare it to the searched key
		const void* lerped = base + elem_size*expected;
		const int diff = compare(key, lerped);

		// either found, or at least one of the bounds needs change
		if (diff == 0) {
			return (void*)lerped;
		} else if (diff > 0) {
			low = expected + 1;
			min = lerped + elem_size;
		} else {
			high = expected - 1;
			max = lerped - elem_size;
		}
	}

	return NULL;
}
