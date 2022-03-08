#include "map.h"

#include <assert.h>
#include <string.h> // memcpy
#include <errno.h>
#include <stdalign.h> // alignas

#include "core.h" // byte_t, bool, stdlib_alloc
#include "hash.h" // fnv_1a


// Ideally, this would be tuned based on hash function and usual keys.
#define MAX_LOAD_FACTOR 0.75

struct map_entry {
	bool in_use;
	bool undead;
	alignas(max_align_t) byte_t key[];
};


static void clear_entries(byte_t *entries, index_t n, size_t size)
{
	const byte_t *end = entries + n * size;
	for (; entries < end; entries += size) {
		struct map_entry *entry = (struct map_entry *)entries;
		entry->in_use = false;
		entry->undead = false;
	}
}

// Finds the nearest power of 2 equal or greater than x.
static unsigned nearest_pow2(int x)
{
	assert(x >= 0);
	if (x < 2) return 1;
	else if ((x & (x-1)) == 0) return x;
	unsigned power = 2;
	x--;
	while (x >>= 1) power <<= 1;
	return power;
}

err_t map_init(map_t *map, index_t n, size_t key_size, size_t value_size,
               compare_fn_t key_cmp, hash_fn_t key_hash, struct allocator alloc)
{
	assert(n >= 0);
	assert(key_size > 0);
	assert(value_size >= 0);
	assert(key_cmp != NULL);

	/// adjust initial capacity by load factor and round up to nearest power of 2
	n = nearest_pow2(n / MAX_LOAD_FACTOR);

	map->count = 0;
	map->filled = 0;
	map->capacity = n;
	map->key_size = key_size;
	map->value_size = value_size;
	map->keyeq = key_cmp;
	map->hash = key_hash != NULL ? key_hash : fnv_1a;

	map->alloc = alloc.method != NULL ? alloc : STDLIB_ALLOCATOR;
	const size_t entry_size = sizeof(struct map_entry) + key_size;
	map->keys = map->alloc.method(&map->alloc, NULL, n * entry_size);
	if (map->keys == NULL && n != 0) return ENOMEM;
	map->values = map->alloc.method(&map->alloc, NULL, n * value_size);
	if (map->values == NULL && n != 0 && value_size != 0) {
		map->alloc.method(&map->alloc, map->keys, 0);
		return ENOMEM;
	}

	clear_entries(map->keys, n, entry_size);
	return 0;
}

void map_destroy(map_t *map)
{
	map->alloc.method(&map->alloc, map->keys, 0);
	map->alloc.method(&map->alloc, map->values, 0);
}

index_t map_size(const map_t *map)
{
	return map->count;
}

extern inline bool map_empty(const map_t *map);

static index_t find_entry(const map_t *map, const byte_t *keys, size_t n,
                          const void *key)
{
	// this procedure does not loop infinitely because there will always be
	// at least some unused buckets due to a maximum load factor smaller than 1
	static_assert(MAX_LOAD_FACTOR < 1.0, "MAX_LOAD_FACTOR puts bucket array under too much load");

	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	index_t tombstone = -1;

	// n should be a power of 2 so we may swap % operations for bitmasks
	assert((n & (n-1)) == 0);
	const size_t m = n - 1;

	// linearly probes buckets starting from hash(k) % n
	for (index_t k = map->hash(key, map->key_size) & m;; k = (k + 1) & m) {
		struct map_entry *entry = (struct map_entry *)(keys + k * entry_size);
		// probed bucket may be "free", and then could be a re-usable tombstone
		if (!entry->in_use) {
			if (entry->undead && tombstone < 0)
				tombstone = k;
			else if (!entry->undead) // actually free space
				return tombstone >= 0 ? tombstone : k;
		// or it may have the key we were looking for
		} else if (map->keyeq(key, entry->key) == 0) {
			return k;
		}
	}
}

void *map_get(const map_t *map, const void *key)
{
	if (map->count <= 0) return NULL;
	const index_t k = find_entry(map, map->keys, map->capacity, key);
	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	struct map_entry *entry = (struct map_entry *)(map->keys + k * entry_size);
	return entry->in_use ? map->values + k * map->value_size : NULL;
}

static err_t rehash_table(map_t *map, index_t n)
{
	// initialize and clear a new bucket array with the desired capacity
	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	byte_t *new_keys = map->alloc.method(&map->alloc, NULL, n * entry_size);
	if (new_keys == NULL) return ENOMEM;
	byte_t *new_values = map->alloc.method(&map->alloc, NULL, n * map->value_size);
	if (new_values == NULL && map->value_size != 0) {
		map->alloc.method(&map->alloc, new_keys, 0);
		return ENOMEM;
	}
	clear_entries(new_keys, n, entry_size);

	// copy every old entry to a newly-computed place in the rehashed table
	map->filled = 0;
	for (index_t i = 0; i < map->capacity; ++i) {
		struct map_entry *src = (struct map_entry *)(map->keys + i * entry_size);
		if (!src->in_use) continue;
		const index_t k = find_entry(map, new_keys, n, src->key);
		struct map_entry *dest = (struct map_entry *)(new_keys + k * entry_size);
		memcpy(dest, src, entry_size);
		byte_t *old_value = map->values + i * map->value_size;
		byte_t *new_value = new_values + k * map->value_size;
		memcpy(new_value, old_value, map->value_size);
		map->filled++;
	}

	// update table's bucket list (remember to free the old one)
	map->capacity = n;
	map->alloc.method(&map->alloc, map->keys, 0);
	map->alloc.method(&map->alloc, map->values, 0);
	map->keys = new_keys;
	map->values = new_values;
	return 0;
}

err_t map_insert(map_t *map, const void *key, const void *value)
{
	/// check if the table's capacity needs to grow to reduce its load factor
	if (map->filled + 1 > map->capacity * MAX_LOAD_FACTOR) {
		const index_t new_capacity = map->capacity > 0 ? map->capacity * 2 : 8;
		const err_t error = rehash_table(map, new_capacity);
		if (error) return error;
	}

	// finds entry address; should be done after rehashing (if it happens)
	const index_t k = find_entry(map, map->keys, map->capacity, key);
	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	struct map_entry *entry = (struct map_entry *)(map->keys + k * entry_size);

	// if entry was vacant, increase hashtable's load
	const bool was_vacant = !entry->in_use;
	if (was_vacant) {
		entry->in_use = true;
		map->count++;
		if (!entry->undead) map->filled++;
	}

	// copy key and value pair
	memcpy(entry->key, key, map->key_size);
	memcpy(map->values + k * map->value_size, value, map->value_size);
	return was_vacant ? 0 : -1;
}

err_t map_remove(map_t *map, const void *key)
{
	if (map->count <= 0) return ENOKEY;

	const index_t k = find_entry(map, map->keys, map->capacity, key);
	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	struct map_entry *entry = (struct map_entry *)(map->keys + k * entry_size);
	if (!entry->in_use) return ENOKEY;

	// we need to mark the deleted entry as a tombsone to enable linear probing
	entry->in_use = false;
	entry->undead = true;
	map->count--;

	return 0;
}

err_t map_for_each(const map_t *map,
                   err_t (*proc)(const void *k, void *v, void *fwd), void *forward)
{
	err_t err = 0;
	const size_t entry_size = sizeof(struct map_entry) + map->key_size;
	for (index_t i = 0; i < map->capacity; ++i) {
		struct map_entry *e = (struct map_entry *)(map->keys + i * entry_size);
		if (!e->in_use) continue;
		err = proc(e->key, map->values + i * map->value_size, forward);
		if (err) break;
	}
	return err;
}
