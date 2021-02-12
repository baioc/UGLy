/**
 * @file stack.h
 * @brief LIFO stacks.
 */

#ifndef UGLY_STACK_H
#define UGLY_STACK_H

#include "list.h"

/// Dynamic LIFO stack with constant access, peek, push (amortized) and pop.
typedef list_t stack_t;

/**
 * @brief Initializes a generic stack.
 *
 * @param stack stack to be initialized, should be destroyed later.
 * @param length initial stack capacity in number of elements.
 * @param type_size size, in bytes, of each stack element.
 * @param alloc memory allocator to be used (performs well with a stack allocator).
 *
 * @return 0 on success or ENOMEM in case alloc fails.
 */
inline err_t stack_init(stack_t *stack, index_t length, size_t type_size, struct allocator alloc)
{
	return list_init(stack, length, type_size, alloc);
}

/// Frees any resources allocated by the given stack.
inline void stack_destroy(stack_t *stack)
{
	list_destroy(stack);
}

/// Gets the current depth (number of elements) of the stack.
inline index_t stack_size(const stack_t *stack)
{
	return list_size(stack);
}

/// Checks if the stack is empty.
inline bool stack_empty(const stack_t *stack)
{
	return list_empty(stack);
}

/// Returns a pointer to the Nth element below the top of the dynamic stack.
inline void *stack_peek(const stack_t *stack, index_t n)
{
	return list_ref(stack, stack_size(stack) - 1 - n);
}

/**
 * @brief Pushes an element at the given address onto the top of the stack.
 * @return 0 on success or ENOMEM in case ALLOC fails.
 */
inline err_t stack_push(stack_t *stack, const void *element)
{
	return list_append(stack, element);
}

/// Pops the top of the stack and copies it to the given address.
inline void stack_pop(stack_t *stack, void *restrict sink)
{
	list_remove(stack, list_size(stack) - 1, sink);
}

#endif // UGLY_STACK_H
