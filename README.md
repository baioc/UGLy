UGLy - Unsafe Generic LibrarY
======

UGLy is a C(11) library used to apply the DRY principle and avoid re-implementing the same common data structures, procedures and macros everywhere.

It uses "unsafe" generics (`void *`) in the sense that all data types are seen as a sequence of bytes and the user is responsible for making sure they are properly interpreted (and memory aligned).
It should be noted that the library's containers always use copy semantics and never "take ownership" of given elements, meaning the user is still expected to manage the lifetimes of dynamically allocated objects ("you allocate it, you free it").

The suggested use of UGLy would be as internal implementation for libraries which wrap it in a more "type-and-memory safe" API.


Build instructions
------

This is a CMake project:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```
(for debug builds, use `-DCMAKE_BUILD_TYPE=Debug`)

Then, if your default build system is GNU make:
```bash
make
```

And to run tests:
```bash
ctest
```


Features
------

### Common data structures

Currently implemented generic data structures:
- [`list_t`](include/ugly/list.h): dynamically sized sequence of fixed-size elements which are contiguously allocated and indexed in O(1) time. Insertions and remotions have amortized O(1) complexity when done at the end of the list and O(n) otherwise.
- [`map_t`](include/ugly/map.h): dynamically sized mapping between fixed-size keys and values. All operations have an amortized average constant complexity when using a proper hashing function.
- [`stack_t`](include/ugly/stack.h): dynamic LIFO structure for fixed-size elements. All operations have O(1) complexity (amortized in the case of insertions and deletions).

### Custom memory allocator support

Whenever memory allocations are needed, the user can choose to provide his own custom allocator or use one of the [generic built-in ones](include/ugly/alloc.h) (most of which allocate on a user-provided buffer):
- `STDLIB_ALLOCATOR`: simply calls `malloc`, `realloc` and `free` from stdlib.
- `pool_allocator_t`: fixed maximum allocation size, zero overhead and no external fragmentation while supporting deallocations in any order.
- `stack_allocator_t`: variable allocation size, can free and do in-place reallocations but only in Last-In-First-Out fashion.
- `arena_allocator_t`: variable allocation size, almost no memory overhead, never frees.

### Descriptive type definitions

Instead of using primitives for everything, UGLy defines some [core](include/ugly/core.h) types which should elucidate their intended semantics in procedure signatures.

### Useful macros

We also include some macros which tend to be needed every now and then when programming in C.

### Tests

We have a simple [CTest suite](test/) set up to try and ensure new changes won't cause major breaks.


To-Do
------

This is a list of desired features which are not yet implemented; feel free to contribute.

- Use a self-balancing BST instead of a hash-table to implement `map_t` when the user doesn't provide a custom hashing function.
- A set data structure, probably just a wrapper over `map_t` but ideally with more efficient union, intersection and difference operations.
- Generic FIFO queue, probably implemented on top of a circular buffer deque.
- Macros to define an "extrusive" linked list type and its operations for a specific base data type.
- Same as above, but for doubly-linked lists.
- Bit-map based implementation of sets which are dense and enumerable.
- Bit-manipulating macros.
- Heap-based priority queue.
- Improve `map_t`'s probing method (check out the algorithm used in Lua tables).
- Expand the hash library with additional hashing functions for common data types.
- Directed graph data structure.
