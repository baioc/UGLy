/**
 * @file map.h
 * @brief Associative arrays (aka maps).
 */

#ifndef UGLY_MAP_H
#define UGLY_MAP_H

#include "core.h"
#include "hash.h" // hash_fn_t

/// Generic hash table with constant amortized access, insertions and deletes.
typedef struct {
	index_t count;
	index_t filled;
	index_t capacity;
	byte_t *keys;
	byte_t *values;
	size_t key_size;
	size_t value_size;
	compare_fn_t keyeq;
	hash_fn_t hash;
	struct allocator alloc;
} map_t;

/**
 * @brief Initializes a generic map.
 *
 * @param map map to be initialized, should be destroyed later.
 * @param n initial mapping capacity.
 * @param key_size size, in bytes, of the map's keys.
 * @param value_size size, in bytes, of the map's associated values.
 * @param key_cmp function that enables equality comparison between keys.
 * @param key_hash a hash function for keys.
 * @param alloc memory allocator to be used.
 *
 * @return 0 on success or ENOMEM in case alloc fails.
 */
err_t map_init(map_t *map, index_t n, size_t key_size, size_t value_size,
               compare_fn_t key_cmp, hash_fn_t key_hash, struct allocator alloc);

/// Frees any resources allocated by the map.
void map_destroy(map_t *map);

/// Gets the number of mappings contained in the map.
index_t map_size(const map_t *map);

/// Checks whether the map is empty.
inline bool map_empty(const map_t *map)
{
	return map_size(map) <= 0;
}

/**
 * @brief Finds the value associated with the given key.
 * @return dynamic address of the associated value, or NULL when not found.
 */
void *map_get(const map_t *map, const void *key);

/**
 * @brief Puts the <key -> value> entry on the map.
 * @return ENOMEM in case any allocation fails, a negative number if an entry
 * with the given key already existed and had its value overwritten; zero otherwise.
 */
err_t map_insert(map_t *map, const void *key, const void *value);

/**
 * @brief Removes a key's entry from the map.
 * @return 0 on success or ENOKEY if the key wasn't in the map to begin with.
 */
err_t map_remove(map_t *map, const void *key);

/**
 * @brief Iterates (in unspecified order) through all entries in the map, calling
 * the given procedure on each one with an extra forwarded argument.
 * @return The iteration will be halted in case the procedure yields a non-zero
 * value, which will be then immediately returned. Returns 0 otherwise.
 */
err_t map_for_each(const map_t *map,
                   err_t (*func)(const void *, void *, void *),
                   void *forward);

#endif // UGLY_MAP_H
