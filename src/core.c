#include "core.h"

void swap(void* restrict a, void* restrict b, size_t size)
{
	for (byte_t temp; size != 0; a++, b++, size--) {
		temp = *(byte_t*)a;
		*(byte_t*)a = *(byte_t*)b;
		*(byte_t*)b = temp;
	}
}
