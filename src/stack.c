#include "stack.h"

extern inline err_t stack_init(stack_t *stack, index_t length, size_t type_size, struct allocator alloc);

extern inline void stack_destroy(stack_t *stack);

extern inline index_t stack_size(const stack_t *stack);

extern inline bool stack_empty(const stack_t *stack);

extern inline void *stack_peek(const stack_t *stack, index_t n);

extern inline err_t stack_push(stack_t *stack, const void *element);

extern inline void stack_pop(stack_t *stack, void *restrict sink);
