#include <ugly/map.h>

#undef NDEBUG
#include <assert.h>

#include <string.h> // strcmp, memcpy
#include <stdlib.h> // rand, malloc, free, NULL, atoi
#include <errno.h>
#include <time.h>
#include <stdio.h>

#include <ugly/core.h> // ARRAY_SIZE
#include <ugly/hash.h> // fnv_1a


static int strrefcmp(const void *a, const void *b)
{
	const char *str1 = *(const char **)a;
	const char *str2 = *(const char **)b;
	return strcmp(str1, str2);
}

static hash_t strhash(const void *ptr, size_t bytes)
{
	const char *str = *(const char **)ptr;
	const size_t n = strlen(str); // since bytes is actualy sizeof(char *) ...
	return fnv_1a(str, n);
}

static int test_each(const void *key, void *value, void *arr)
{
	const char *number = *(char **)key;
	const int digit = *(int *)value;
	const char **num_array = (const char **)arr;
	const int diff = strcmp(number, num_array[digit]);
	assert(diff == 0);
	return diff;
}

void test(void)
{
	const char *numbers[] = {"zero", "one", "two", "three", "four", "five"};
	const int n = ARRAY_SIZE(numbers);

	// initialize map: string -> int, should be empty
	map_t dict;
	err_t err = map_init(&dict, 2, sizeof(char *), sizeof(int),
	                     strrefcmp, strhash, STDLIB_ALLOCATOR);
	assert(!err);
	assert(map_empty(&dict));

	// put entries and check final size
	for (int i = 0; i < n; ++i) {
		err = map_insert(&dict, &numbers[i], &i);
		assert(!err);
	}
	assert(map_size(&dict) == n);

	// choose and copy a random key
	const int del = rand() % n;
	const size_t del_size = strlen(numbers[del]) + 1;
	char *deleted = malloc(del_size);
	memcpy(deleted, numbers[del], del_size);
	assert(deleted != NULL);
	assert(strcmp(deleted, numbers[del]) == 0);

	// before removal, test if map_insert with existing key returns negative
	err = map_insert(&dict, &numbers[del], &del);
	assert(err < 0);

	// delete that randomly selected entry, test size and access
	err = map_remove(&dict, &deleted);
	free(deleted);
	assert(!err);
	assert(map_size(&dict) == n - 1);
	assert(map_get(&dict, &numbers[del]) == NULL);

	// for all other entries, check if value matches
	for (int i = 0; i < n; ++i) {
		if (i == del) continue;
		int *numeral = map_get(&dict, &numbers[i]);
		assert(numeral != NULL);
		assert(*numeral == i);
	}

	// try deleting the same entry
	err = map_remove(&dict, &numbers[del]);
	assert(err == ENOKEY);

	// do the same equality test, but with map_for_each
	err = map_for_each(&dict, test_each, numbers);
	assert(!err);

	// deallocate map
	map_destroy(&dict);
}


static int ulongrefcmp(const void *a, const void *b)
{
	return *(const unsigned long *)a - *(const unsigned long *)b;
}

static hash_t ulonghash(const void *ptr, size_t bytes)
{
	const unsigned long n = *(const unsigned long *)ptr;
	return n * n;
}

void benchmark(int n, int reserve)
{
	n = n > 0 ? n : 1000000;
	reserve = reserve >= 0 && reserve <= n ? reserve : 0;

	map_t dict;
	err_t err = map_init(&dict, reserve, sizeof(unsigned long), sizeof(int),
	                     ulongrefcmp, ulonghash, STDLIB_ALLOCATOR);
	assert(!err);

	const clock_t begin = clock();
	for (int i = 0; i < n; ++i) {
		const unsigned long key = rand();
		const int value = i;
		map_insert(&dict, &key, &value);
	}
	const clock_t end = clock();

	map_destroy(&dict);

	const float elapsedNs = end*1e9/CLOCKS_PER_SEC - begin*1e9/CLOCKS_PER_SEC;
	printf("Total: %.3f ms\n", elapsedNs / 1e6);
	printf("Per element: %.0f ns\n", elapsedNs / n);
}


int main(int argc, char const *argv[])
{
	int n = argc > 1 ? atoi(argv[1]) : 0;
	int reserve = argc > 2 ? atoi(argv[2]) : 0;

	test();
	benchmark(n, reserve);

	return 0;
}
