/**
 * @file alloc.h
 * @brief Built-in allocator schemes on top of a user-provided buffer.
 */

#ifndef UGLY_ALLOC_H
#define UGLY_ALLOC_H

#include "core.h"

/// Arena allocator context.
typedef struct {
	byte_t *end;
	byte_t *current;
	byte_t *previous;
} arena_allocator_t;

/**
 * @brief Sets up (or resets) an arena allocator.
 *
 * @param arena arena allocator state.
 * @param buffer backing memory buffer.
 * @param buffer_size buffer size, in bytes.
 *
 * @return a fast allocator with no fragmentation, but that never actually
 * frees any memory and only supports in-place reallocation of the single most
 * recently allocated block.
 */
struct allocator make_arena_allocator(arena_allocator_t *arena,
                                      void *buffer, size_t buffer_size);

/// Stack allocator context.
typedef struct {
	byte_t *end;
	byte_t *current;
} stack_allocator_t;

/**
 * @brief Sets up (or resets) a stack allocator.
 *
 * @param stack stack allocator state.
 * @param buffer backing memory buffer.
 * @param buffer_size buffer size, in bytes.
 *
 * @return a fragmentationless allocator that does some bookkeeping and is thus
 * capable of freeing memory and doing reallocations, but only in LIFO fashion.
 */
struct allocator make_stack_allocator(stack_allocator_t *stack,
                                      void *buffer, size_t buffer_size);

/// Pool allocator context;
typedef struct {
	struct pool_free_node *free_list_head;
	size_t chunk_size;
} pool_allocator_t;

/**
 * @brief Sets up (or resets) a fixed-chunk-size pool allocator.
 *
 * @param pool pool allocator state.
 * @param buffer backing memory buffer.
 * @param buffer_size buffer size, in bytes.
 * @param chunk_size maximum allocation size.
 *
 * @return a zero-overhead allocator which supports frees and will work with
 * requests for in-place reallocation (even if it doesn't make much sense in
 * this case as all allocated chunks have the same size anyway).
 */
struct allocator make_pool_allocator(pool_allocator_t *pool,
                                     void *buffer, size_t buffer_size,
                                     size_t chunk_size);

#endif // UGLY_ALLOC_H
