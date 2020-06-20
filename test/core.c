#include <ugly/core.h>

#undef NDEBUG
#include <assert.h>

#include <string.h> // strcmp
#include <stdlib.h> // malloc, free


static void swap_primitives(void)
{
	int x = 5;
	int y = 7;
	swap(&x, &y, sizeof(int));
	assert(x == 7);
	assert(y == 5);
}

static void swap_pointers(void)
{
	char* s1 = "Hello, generic";
	char* s2 = "World!";
	swap(&s1, &s2, sizeof(char*));
	assert(strcmp(s1, "World!") == 0);
	assert(strcmp(s2, "Hello, generic") == 0);
}

static void static_array_size(void)
{
	#define N 50
	int array[N];
	assert(ARRAY_SIZE(array) == N);
	#undef N
}

struct string {
	index_t length;
	char characters[];
};

static void struct_base_address(void)
{
	const index_t len = sizeof("TEST");
	struct string* str = malloc(sizeof(struct string) + len);
	assert(str != NULL);

	char* c_str = str->characters;
	c_str[0] = 'T';
	c_str[1] = 'E';
	c_str[2] = 'S';
	c_str[3] = 'T';
	c_str[4] = '\0';
	assert(strcmp(c_str, "TEST") == 0);

	struct string* string = containerof(c_str, struct string, characters);
	assert(string == str);
}

err_t main(int argc, const char* argv[])
{
	swap_primitives();
	swap_pointers();
	static_array_size();
	struct_base_address();
	return 0;
}
