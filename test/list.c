#include <ugly/list.h>

#undef NDEBUG
#include <assert.h>

#include <stdlib.h> // realloc, malloc, free, rand
#include <string.h> // memcpy, strcmp

#include <ugly/core.h> // ARRAY_SIZE


static void list_primitives(void)
{
	const int array[] = {-6, 0, 2, 3, 6, 7, 11};
	const int n = ARRAY_SIZE(array);
	list_t numbers;

	// initially, size should be 0
	int err = list_init(&numbers, 0, sizeof(int), STDLIB_ALLOCATOR);
	assert(!err);
	assert(list_size(&numbers) == 0);

	// reverse insert numbers in reverse order -> normal order
	for (int i = n - 1; i >= 0; --i) {
		err = list_insert(&numbers, 0, &array[i]);
		assert(!err);
	}

	// size should be the number of numbers
	assert(list_size(&numbers) == n);

	// check if the list's front is the expected value, then pop it and repeat
	for (int i = 0; i < n; ++i) {
		int num = *((int *)list_ref(&numbers, 0));
		assert(num == array[i]);
		list_remove(&numbers, 0, &num);
	}

	// after removals, size should be zero
	assert(list_size(&numbers) == 0);

	list_destroy(&numbers);
}

static void list_pointers(void)
{
	const char *array[] = {"Alyssa", "Bob", "Carlos"};
	const int n = ARRAY_SIZE(array);
	list_t names;

	// initially, size should be 0
	int err = list_init(&names, 3, sizeof(char *), STDLIB_ALLOCATOR);
	assert(!err);
	assert(list_size(&names) == 0);

	// only the pointers are inserted into the list
	for (int i = 0; i < n; ++i) {
		const size_t size = strlen(array[i]) + 1;
		char *copy = malloc(sizeof(char) * size); // must be freed
		assert(copy != NULL);
		memcpy(copy, array[i], size);
		err = list_append(&names, &copy);
		assert(!err);
	}

	// frees each pointer (they are still on the list)
	for (int i = 0; i < n; ++i) {
		char *str = *((char **)list_ref(&names, i));
		free(str);
	}
	assert(list_size(&names) == n);

	list_destroy(&names);
}

static int strrefcmp(const void *a, const void *b)
{
	const char *str1 = *(const char **)a;
	const char *str2 = *(const char **)b;
	return strcmp(str1, str2);
}

static bool list_sorted(const list_t* list, compare_fn_t compare)
{
	for (index_t i = 1; i < list->length; ++i) {
		const void* curr = list_ref(list, i);
		const void* prev = list_ref(list, i - 1);
		if (compare(prev, curr) > 0) return false;
	}
	return true;
}

static void list_sorting(void)
{
	const char *array[] = {"Gb", "Ab", "F#", "B", "D"};
	list_t notes;
	err_t err = list_init(&notes, 0, sizeof(char *), STDLIB_ALLOCATOR);
	assert(!err);

	// insert elements in the list in unsorted fashion
	for (int i = 0; i < ARRAY_SIZE(array); ++i) {
		const index_t idx = list_insert(&notes, i, &array[i]);
		assert(idx >= 0);
	}
	assert(!list_sorted(&notes, strrefcmp));

	// test list_sort
	list_sort(&notes, strrefcmp);
	assert(list_sorted(&notes, strrefcmp));

	// test bsearch
	char *note = "B";
	index_t found = list_search(&notes, &note, strrefcmp);
	assert(found == 1);
	assert(strcmp(*(char **)list_ref(&notes, found), note) == 0);
	list_remove(&notes, found, &note);
	found = list_search(&notes, &note, strrefcmp);
	assert(found < 0);
	assert(list_sorted(&notes, strrefcmp));

	// test list_swap by doing a Fisher-Yates-Knuth in-place shuffle
	const int length = list_size(&notes);
	for (int i = length - 1; i > 0; --i) {
		const int j = rand() % (i + 1);
		if (j == i) continue;
		list_swap(&notes, i, j);
	}
	assert(list_size(&notes) == length);

	list_destroy(&notes);
}

int main(void)
{
	list_primitives();
	list_pointers();
	list_sorting();
}
