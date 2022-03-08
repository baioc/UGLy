#include <ugly/alloc.h>

#undef NDEBUG
#include <assert.h>

#include <string.h> // strcpy, strcmp
#include <stdlib.h> // rand


static void bump_allocator(void)
{
	// prepare the allocator
	bump_allocator_t bump;
	byte_t buffer[1024];
	struct allocator alloc = make_bump_allocator(&bump, buffer, sizeof(buffer));
	assert(alloc.method != NULL);

	// allocate something
	const char test_string[] = "this is a test";
	const unsigned bytes = sizeof(test_string);
	char *some_string = alloc.method(&alloc, NULL, bytes);
	assert(some_string != NULL);

	// see if we can read and write to this memory
	strcpy(some_string, test_string);
	assert(strcmp(some_string, test_string) == 0);

	// test valid in-place reallocation
	char *resized_string = alloc.method(&alloc, some_string, 2*bytes);
	assert(resized_string == some_string);
	assert(strcmp(resized_string, test_string) == 0);
	resized_string = alloc.method(&alloc, some_string, 0.5*bytes);
	assert(resized_string == some_string);

	// another allocation on top
	int *another_string = alloc.method(&alloc, NULL, 0.5*bytes);
	assert(another_string != NULL);

	// freeing doesn't work, but this shouldn't explode or anything
	alloc.method(&alloc, some_string, 0);
	alloc.method(&alloc, another_string, 0);

	// this one should fail
	int *big_allocation = alloc.method(&alloc, NULL, sizeof(buffer) - bytes);
	assert(big_allocation == NULL);

	// reset the bump and try again
	make_bump_allocator(&bump, buffer, sizeof(buffer));
	big_allocation = alloc.method(&alloc, NULL, sizeof(buffer) - bytes);
	assert(big_allocation != NULL);
	big_allocation[1] = 987; // just checking if this isn't misaligned
}

static void stack_allocator(void)
{
	// prepare the allocator
	stack_allocator_t stack;
	byte_t buffer[1024];
	struct allocator alloc = make_stack_allocator(&stack, buffer, sizeof(buffer));
	assert(alloc.method != NULL);

	// allocate something
	const int test_array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	const unsigned bytes = sizeof(test_array);
	int *some_array = alloc.method(&alloc, NULL, bytes);
	assert(some_array != NULL);

	// see if we can read and write to this memory
	memcpy(some_array, test_array, bytes);
	assert(memcmp(some_array, test_array, bytes) == 0);

	// test valid in-place reallocation
	int *resized_array = alloc.method(&alloc, some_array, 2*bytes);
	assert(resized_array == some_array);
	assert(memcmp(resized_array, test_array, bytes) == 0);
	resized_array = alloc.method(&alloc, some_array, 0.5*bytes);
	assert(resized_array == some_array);

	// another allocation on top
	int *another_array = alloc.method(&alloc, NULL, 0.5*bytes);
	assert(another_array != NULL);

	// this one should fail
	int *big_allocation = alloc.method(&alloc, NULL, sizeof(buffer) - bytes);
	assert(big_allocation == NULL);

	// but we can free some space and try again
	alloc.method(&alloc, another_array, 0);
	alloc.method(&alloc, some_array, 0);
	big_allocation = alloc.method(&alloc, NULL, sizeof(buffer) - bytes);
	assert(big_allocation != NULL);
}


static void pool_allocator(void)
{
#define MAX_ELEMS 128
#define C_SIZE sizeof(struct c)
	struct c { float re; float im; };

	// prepare the allocator
	pool_allocator_t pool;
	byte_t buffer[MAX_ELEMS * C_SIZE];
	struct allocator alloc = make_pool_allocator(&pool, buffer, sizeof(buffer), C_SIZE);
	assert(alloc.method != NULL);

	// allocate (and then reallocate) a bunch of objects
	struct c *to_be_freed = NULL;
	for (int i = 0; i < MAX_ELEMS; ++i) {
		struct c *test = alloc.method(&alloc, NULL, C_SIZE);
		assert(test != NULL);
		test->re = i;
		test->im = -i;
		assert(alloc.method(&alloc, test, C_SIZE) == test);
		assert(test->re == i);
		assert(test->im == -i);
		if (i == rand() % MAX_ELEMS) to_be_freed = test;
	}

	// this one should fail
	struct c *test = alloc.method(&alloc, NULL, C_SIZE);
	assert(test == NULL);

	// but we can free some space and try again
	alloc.method(&alloc, to_be_freed, 0);
	test = alloc.method(&alloc, NULL, C_SIZE);
	assert(test != NULL);

#undef C_SIZE
#undef MAX_ELEMS
}

int main(void)
{
	bump_allocator();
	stack_allocator();
	pool_allocator();
}
