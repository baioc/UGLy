UGLy - Unsafe Generic LibrarY
====

[![latest version](https://img.shields.io/badge/version-0.3.0-blue)](https://baioc.gitlab.io/UGLy/files)
[![pipeline status](https://gitlab.com/baioc/UGLy/badges/master/pipeline.svg?ignore_skipped=true)](https://gitlab.com/baioc/UGLy)
[![test coverage](https://gitlab.com/baioc/UGLy/badges/master/coverage.svg)](https://baioc.gitlab.io/UGLy/coverage)

This is a portable  C(17) library used to apply the DRY principle and avoid re-implementing the same common data structures, procedures and macros everywhere, facilitating modern C systems development.

Contributions are welcome, check out our [issue board](https://gitlab.com/baioc/UGLy/-/boards).

UGLy uses "unsafe" generics (`void *`) in the sense that all data types are seen as a sequence of bytes and the user is responsible for making sure they are properly interpreted.
It should be noted that the library's containers always use copy semantics and will never call `free` on a stored pointer, meaning the user is still expected to manage the lifetimes of dynamically allocated objects.
**In summary: "you allocate it, you free it".**

The suggested use of UGLy would be as internal implementation for libraries which wrap it in a more "type-and-memory safe" API.


Build instructions
----

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
----

### Generic data structures / containers

Currently implemented generic data structures:
- [`map_t`](include/ugly/map.h): dynamically sized mapping between fixed-size keys and values. All operations have an amortized average constant complexity when using a proper hashing function.
- [`list_t`](include/ugly/list.h): dynamically sized sequence of fixed-size elements which are contiguously allocated and indexed in O(1) time. Insertions and remotions have amortized O(1) complexity when done at the end of the list and O(n) otherwise.
- [`stack_t`](include/ugly/stack.h): dynamic LIFO structure for fixed-size elements. All operations have O(1) complexity (amortized in the case of insertions and deletions).

### Custom memory allocator support

Whenever memory allocations are needed, the user can choose to provide his own allocator or use one of the [generic built-in ones](include/ugly/alloc.h) (most of which allocate on a user-provided arena buffer):
- `STDLIB_ALLOCATOR`: simply calls `malloc`, `realloc` and `free` from stdlib.
- `pool_allocator_t`: fixed maximum allocation size and no external fragmentation while supporting deallocations in any order.
- `bump_allocator_t`: variable allocation size, zero memory overhead, never frees.
- `stack_allocator_t`: variable allocation size, can free and do in-place reallocations but only in Last-In-First-Out fashion.

### Useful macros and type definitions

We include some macros which tend to be needed every now and then when programming in C, like `containerof`.
Additionally, instead of using primitives for everything, UGLy defines some [core](include/ugly/core.h) types which should elucidate their intended semantics in procedure signatures.

### Unsafe but not untested

We have an automatic [CTest suite](test/) set up on CI to ensure new changes don’t cause major breaks.
