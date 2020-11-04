#include <ugly/core.h>

#undef NDEBUG
#include <assert.h>

#include <string.h> // strcpy, strcmp
#include <stdlib.h> // malloc, free


static void memswap_primitives(void)
{
	int x = 5;
	int y = 7;
	memswap(&x, &y, sizeof(int));
	assert(x == 7);
	assert(y == 5);
}

static void memswap_pointers(void)
{
	char *s1 = "Hello, generic";
	char *s2 = "World!";
	memswap(&s1, &s2, sizeof(char *));
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
	struct string *str = malloc(sizeof(struct string) + len);
	assert(str != NULL);

	char *c_str = str->characters;
	strcpy(c_str, "TEST");
	assert(strcmp(c_str, "TEST") == 0);

	struct string *string = containerof(c_str, struct string, characters);
	assert(string == str);
	free(str);
}

int main(void)
{
	memswap_primitives();
	memswap_pointers();
	static_array_size();
	struct_base_address();
}
