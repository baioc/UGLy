UGLy - Unsafe Generic LibrarY
======

UGLy is a C(99) library used to apply the DRY principle and avoid re-implementing the same common data structures and procedures everywhere.

It uses "unsafe" generics (`void *`) in the sense that all data types are seen as a sequence of bytes and the user is responsible for making sure they are interpreted properly.
It should be noted that the library's containers always use copy semantics and never "take ownership" of given elements, meaning the user is still expected to manage the lifetimes of dynamically allocated objects ("you allocate it, you free it").

The suggested use of UGLy would be as internal implementation for libraries which wrap it in a more "type-and-memory safe" API.


Features
------

### Common data structures

Currently implemented generic data structures:
- [`list_t`](include/ugly/list.h): dynamically sized sequence of fixed-size elements which are contiguously allocated and indexed in O(1) time. Insertions and remotions are O(n).
- [`map_t`](include/ugly/map.h): dynamically sized mapping between fixed-size keys and values. All operations have an average constant complexity when using a proper hashing function.
- [`stack_t`](include/ugly/stack.h): LIFO structure for fixed-size elements. All operations have O(1) complexity (amortized insertion).

### Custom memory allocator support

Whenever dynamic memory allocation is needed, the user can choose to provide a custom single-procedure allocator, or simply use a default one from C's stdlib.

### Descriptive type definitions

Instead of using primitives for everything, UGLy defines some [core](include/ugly/core.h) types which should elucidate their intended semantics in procedure signatures.

### Useful macros

We also include some macros which tend to be needed every now and then when programming in C.


To-Do
------

This is a list of desired features which are not yet implemented; feel free to contribute.

- Use a self-balancing BST instead of a hash-table to implement `map_t` when the user doesn't provide a custom hashing function.
- Shrink container underlying arrays (`list_t` and `map_t`) when they are mostly empty.
- A set data structure, probably just a wrapper over `map_t` but ideally with more efficient union, intersection and difference operations.
- Generic FIFO queue, probably implemented on top of a circular buffer deque.
- Macros to define an "intrusive" linked list type and its operations for a specific base data type.
- Same as above, but for doubly-linked lists.
- A library of custom allocators.
- Bit-map based implementation of sets which are dense and enumerable.
- Bit-manipulating macros.
- Heap-based priority queue.
- Expand the hash library with additional hashing functions for common data types.
- Directed graph data structure.
