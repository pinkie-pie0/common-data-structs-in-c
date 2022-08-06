#include <stdio.h>
#include <stdlib.h>
#define TOMBSTONE ((void*)1)

#define DS_NAME "hashmap"
#include "err/ds_assert.h"

typedef struct hashmap_entry hashmap_entry;
typedef struct hashmap_ds {
	int (*hash)(const void*);
	int (*is_equals)(const void*, const void*);
	size_t size, capacity;
	struct hashmap_entry {
		void *key, *value;
	} **table;
} hashmap_ds;

typedef struct hashmap_ds_iterator {
	size_t index;
	hashmap_ds *map;
	hashmap_entry **entries;
} hashmap_ds_iterator;

hashmap_ds_iterator hashmap_getiterator(hashmap_ds *const this) {
	hashmap_ds_iterator itr;
	itr.index = 0;
	itr.map = this;
	itr.entries = this->table;
	return itr;
}

int hashmap_iterator_hasnext(hashmap_ds_iterator *const itr) {
	return itr->index < itr->map->size;
}

hashmap_entry *hashmap_iterator_next(hashmap_ds_iterator *const itr) {
	hashmap_entry *entry = NULL;
	if (!hashmap_iterator_hasnext(itr)) return NULL;
	while (entry == NULL || entry == TOMBSTONE) {
		entry = *itr->entries++;
	}
	itr->index++;
	return entry;
}

static int absval(int num) {
	return num < 0 ? -num : num;
}

static int reference_hash(const void *E) {
	/* Thomas Wang's 64 bit to 32 bit hash function */
	size_t value = (size_t)E;
	value = (~value) + (value << 18);
	value = value ^ (value >> 31);
	value = value * 21;
	value = value ^ (value >> 11);
	value = value + (value << 6);
	value = value ^ (value >> 22);
	return (int)value;
	
	/*return (int)(value ^ (value >> 32));*/
}

static int reference_equality(const void *E_1, const void *E_2) {
	return E_1 == E_2;
}

hashmap_ds *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*)) {
	size_t i;
	
	hashmap_ds *this = malloc(sizeof *this);
	DS_ASSERT(this != NULL, "failed to allocate memory for new " DS_NAME);
	
	this->hash = hash;
	this->is_equals = is_equals;
	
	this->size = 0;
	this->capacity = 64;
	this->table = malloc(64 * sizeof *this->table);
	DS_ASSERT(this->table != NULL, "failed to allocate memory for the " DS_NAME "'s table");
	
	for (i = 0; i < 64; i++) {
		this->table[i] = NULL;
	}
	
	return this;
}

hashmap_ds *alloc_identityhashmap() {
	return alloc_hashmap(reference_hash, reference_equality);
}

void dealloc_hashmap(hashmap_ds *const this) {
	size_t i, capacity;
	for (i = 0, capacity = this->capacity; i < capacity; i++) {
		if (this->table[i] != NULL && this->table[i] != TOMBSTONE) free(this->table[i]);
	}
	free(this->table);
	free(this);
}

void *hashmap_put(hashmap_ds *const this, void *key, void *value) {
	size_t i = ((size_t)absval(this->hash(key))) % this->capacity;
	
	for(; this->table[i] != NULL && this->table[i] != TOMBSTONE; i = (i+1) % this->capacity) {
		if (this->is_equals(this->table[i]->key, key)) {
			void *oldval = this->table[i]->value;
			this->table[i]->value = value;
			return oldval;
		}
	}
	
	this->table[i] = malloc(sizeof *this->table[i]);
	DS_ASSERT(this->table[i] != NULL, "failed to allocate memory for a new table entry");
	
	this->table[i]->key = key;
	this->table[i]->value = value;
	this->size++;
	return NULL;
}

void *hashmap_get(hashmap_ds *const this, void *key) {
	size_t i = ((size_t)absval(this->hash(key))) % this->capacity;
	
	for (; this->table[i] != NULL; i = (i+1) % this->capacity) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			return this->table[i]->value;
		}
	}
	return NULL;
}

void *hashmap_get_keyref(hashmap_ds *const this, void *key) {
	size_t i = ((size_t)absval(this->hash(key))) % this->capacity;
	
	for (; this->table[i] != NULL; i = (i+1) % this->capacity) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			return this->table[i]->key;
		}
	}
	return NULL;
}

void *hashmap_remove(hashmap_ds *const this, void *key) {
	size_t i = ((size_t)absval(this->hash(key))) % this->capacity;
	
	for (; this->table[i] != NULL; i = (i+1) % this->capacity) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			void *oldval = this->table[i]->value;
			free(this->table[i]);
			this->table[i] = TOMBSTONE;
			this->size--;
			return oldval;
		}
	}
	return NULL;
}

hashmap_entry **hashmap_getentries(hashmap_ds *const this) {
	size_t i, capacity;
	
	/* allocate a list of entries + one more slot that indicates the end of a list using NULL*/
	hashmap_entry **iteration, **entries = malloc((this->size+1) * sizeof *entries);
	DS_ASSERT(entries != NULL, "failed to allocate a list of entries");
	
	entries[this->size] = NULL;
	iteration = entries;
	for (i = 0, capacity = this->capacity; i < capacity; i++) {
		if (this->table[i] != NULL && this->table[i] != TOMBSTONE) *iteration++ = this->table[i];
	}
	return entries;
}
