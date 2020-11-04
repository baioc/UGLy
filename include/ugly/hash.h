#ifndef UGLY_HASH_H
#define UGLY_HASH_H

#include "core.h"

/// Return type for hash functions.
typedef unsigned long hash_t;

/// Generic function to hash a number bytes from a given address.
typedef hash_t (*hash_fn_t)(const void *ptr, size_t n);

/// FNV-1a hashing algorithm: http://www.isthe.com/chongo/tech/comp/fnv/
hash_t fnv_1a(const void *ptr, size_t n);

#endif // UGLY_HASH_H
