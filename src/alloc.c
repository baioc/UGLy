#include "alloc.h"

#include <assert.h>
#include <stdalign.h> // alignof, alignas
#include <stddef.h> // max_align_t
#include <stdint.h> // uintptr_t

#include "core.h" // containerof

#define MAX_ALIGNMENT alignof(max_align_t)


static inline byte_t *align_forward(byte_t *ptr, byte_t alignment)
{
	const unsigned modulo = (uintptr_t)ptr % alignment;
	if (modulo == 0) return ptr; // already aligned
	const unsigned padding = alignment - modulo;
	return ptr + padding;
}


static void *arena_alloc(struct allocator *ctx, void *ptr, size_t size)
{
	assert(ctx != NULL);
	arena_allocator_t *arena = (arena_allocator_t *)ctx->environment;

	// we don't deallocate
	if (size == 0) {
		return NULL;

	// we only reallocate the most recently allocated block
	} else if (ptr != NULL && (byte_t *)ptr != arena->previous) {
		return NULL;

	// reallocation is pretty easy
	} else if (ptr != NULL && (byte_t *)ptr == arena->previous) {
		if (arena->previous + size > arena->end) return NULL; // not enough space
		// goto BUMP;

	// we always suppose the current arena pointer has been bumped correctly
	} else if (ptr == NULL) {
		if (arena->current + size > arena->end) return NULL; // not enough space
		arena->previous = arena->current;
		// goto BUMP;
	}

// BUMP:
	arena->current = align_forward(arena->previous + size, MAX_ALIGNMENT);
	return arena->previous;
}

struct allocator make_arena_allocator(arena_allocator_t *arena,
                                      void *buffer, size_t buffer_size)
{
	// set up initial context
	arena->current = align_forward(buffer, MAX_ALIGNMENT);
	arena->end = (byte_t *)buffer + buffer_size;
	assert(buffer_size > 0);
	arena->previous = arena->end; // since we haven't done any allocations yet

	// return actual allocator
	return (struct allocator){ .environment = arena, .method = arena_alloc };
}


struct stack_block {
	size_t offset_to_next;
	alignas(max_align_t) byte_t payload[];
};

static void *stack_alloc(struct allocator *ctx, void *ptr, size_t size)
{
	assert(ctx != NULL);
	stack_allocator_t *stack = (stack_allocator_t *)ctx->environment;
	byte_t *user_payload;
	struct stack_block *previous;

	// unspecified by the allocator protocol
	if (ptr == NULL && size == 0) {
		return NULL;

	// deallocation
	} else if (ptr != NULL && size == 0) {
		struct stack_block *block = containerof(ptr, struct stack_block, payload);
		if ((byte_t *)block + block->offset_to_next != stack->current) return NULL; // LIFO only!
		stack->current = (byte_t *)block;
		return NULL;

	// new allocation
	} else if (ptr == NULL && size != 0) {
		previous = (struct stack_block *)stack->current;
		user_payload = previous->payload;
		// goto CHECK_AND_BUMP;

	// reallocation
	} else if (ptr != NULL && size != 0) {
		struct stack_block *block = containerof(ptr, struct stack_block, payload);
		if ((byte_t *)block + block->offset_to_next != stack->current) return NULL; // LIFO only!
		user_payload = ptr;
		previous = block;
		// goto CHECK_AND_BUMP;
	}

// CHECK_AND_BUMP:
	byte_t *const unaligned_next = user_payload + size;
	if (unaligned_next > stack->end) return NULL; // not enough space
	stack->current = align_forward(unaligned_next, alignof(struct stack_block));
	previous->offset_to_next = stack->current - (byte_t *)previous;
	return user_payload;
}

struct allocator make_stack_allocator(stack_allocator_t *stack,
                                      void *buffer, size_t buffer_size)
{
	stack->current = align_forward(buffer, alignof(struct stack_block));
	stack->end = (byte_t *)buffer + buffer_size;
	assert(buffer_size > 0);
	return (struct allocator){ .environment = stack, .method = stack_alloc };
}


struct pool_free_node {
	struct pool_free_node *next;
};

static void *pool_alloc(struct allocator *ctx, void *ptr, size_t size)
{
	assert(ctx != NULL);
	pool_allocator_t *pool = (pool_allocator_t *)ctx->environment;

	// unspecified by the allocator protocol
	if (ptr == NULL && size == 0) {
		return NULL;

	// free: turn the given chunk into a node and add it to the free list
	} else if (ptr != NULL && size == 0) {
		struct pool_free_node *node = (struct pool_free_node *)ptr;
		node->next = pool->free_list_head;
		pool->free_list_head = node;
		return NULL;

	// alloc: pop the first chunk from the free list
	} else if (ptr == NULL && size != 0) {
		if (size > pool->chunk_size) return NULL; // invalid object size
		else if (pool->free_list_head == NULL) return NULL; // OOM
		void *new_object = pool->free_list_head;
		pool->free_list_head = pool->free_list_head->next;
		return new_object;

	// reallocation
	} else if (ptr != NULL && size != 0) {
		return size <= pool->chunk_size ? ptr : NULL;
	}
}

struct allocator make_pool_allocator(pool_allocator_t *pool,
                                     void *buffer, size_t buffer_size,
                                     size_t chunk_size)
{
	// if objects are too small, we have no choice but to use node alignment
	assert(chunk_size > 0);
	if (chunk_size < sizeof(struct pool_free_node)) {
		chunk_size = sizeof(struct pool_free_node);
	}
	pool->chunk_size = chunk_size;

	// add every chunk to the free list
	const int chunks = buffer_size / chunk_size;
	pool->free_list_head = NULL;
	for (int i = 0; i < chunks; ++i) {
		struct pool_free_node *node = (struct pool_free_node *)((byte_t *)buffer + i * chunk_size);
		node->next = pool->free_list_head;
		pool->free_list_head = node;
	}

	return (struct allocator){ .environment = pool, .method = pool_alloc };
}
