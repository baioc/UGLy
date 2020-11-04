#include "core.h"

#include <stdlib.h> // realloc


void memswap(void *a, void *b, size_t size)
{
	for (byte_t temp; size != 0; a++, b++, size--) {
		temp = *(byte_t *)a;
		*(byte_t *)a = *(byte_t *)b;
		*(byte_t *)b = temp;
	}
}

void *stdlib_alloc(struct allocator *ctx, void *ptr, size_t size)
{
	return realloc(ptr, size);
}
