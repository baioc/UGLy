/**
 * @file core.h
 * @brief Core types, macros and procedures.
 *
 * Exports standard `bool`, `size_t` and `NULL`.
 */

#ifndef UGLY_CORE_H
#define UGLY_CORE_H

/** @cond */
#include <stdbool.h>
#include <stddef.h>
/** @endcond */

/// Given that C is a "byte-manipulation language", this should be standard.
typedef unsigned char byte_t;

/// A type used for error codes, should be zero when there were no errors.
typedef int err_t;

/// Signed type used to index data structures.
typedef long index_t;

/**
 * @brief Generic comparison function for the contents pointed to by its arguments.
 *
 * - It returns a number ==0 when A == B.
 * - It returns a number < 0 when A < B.
 * - It returns a number <=0 when A <= B.
 * - It returns a number > 0 when A > B.
 * - It returns a number >=0 when A >= B.
 *
 * For instance, if A and B are pointers to int, this could be (*a - *b);
 */
typedef int (*compare_fn_t)(const void *a, const void *b);

/// Single-procedure memory allocator interface.
struct allocator {
	/**
	 * @brief Generic allocation procedure:
	 *
	 * - PTR == NULL and SIZE != 0 -> returns a new allocation of at least SIZE bytes.
	 * - PTR != NULL and SIZE == 0 -> frees the allocated contents of PTR.
	 * - PTR != NULL and SIZE != 0 -> reallocates at least SIZE bytes and frees PTR.
	 * - PTR == NULL and SIZE == 0 -> does nothing.
	 *
	 * The third option should be used when the memory allocating system supports
	 * in-place resizing of previously allocated blocks; it may also decide to use
	 * another location for the resized block, in which case the previous one has
	 * its contents copied and is then freed. When this reallocation fails, the old
	 * block should be preserved.
	 *
	 * @return When performing allocations, this function returns a pointer to the
	 * (re)allocated memory block when it was sucessfull or NULL if it fails.
	 * Return is undefined for all other cases (notably in frees).
	 */
	void *(*method)(struct allocator *ctx, void *ptr, size_t size);

	/// Reference to some geneeric environment used by this allocator's method.
	void *environment;
};

/// Swaps some number of bytes at the given addresses.
void memswap(void *a, void *b, size_t size);

void *stdlib_alloc(struct allocator *ctx, void *ptr, size_t size);
/// Default stdlib-based allocator.
#define STDLIB_ALLOCATOR \
	(struct allocator){ .method = stdlib_alloc, .environment = NULL }

/// Gets number of elements in a STATIC array.
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/// Given the address of a certain field, finds the address of its container struct.
#define containerof(ptr, container, field) \
	((container *)((byte_t *)(ptr) - offsetof(container, field)))

#endif // UGLY_CORE_H
