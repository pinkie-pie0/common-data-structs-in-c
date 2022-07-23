#include <stdio.h>
#include <stdlib.h>
#define TOMBSTONE ((void*)1)

typedef struct hashmap_entry {void *key; void *value;} hashmap_entry;
typedef struct hashmap_ds {
	int (*hash)(const void*);
	int (*is_equals)(const void*, const void*);
	size_t size, capacity;
	hashmap_entry **table;
} hashmap_ds;

typedef struct hashmap_ds_iterator {
	size_t index;
	hashmap_ds *map;
	hashmap_entry **entries;
} hashmap_ds_iterator;

hashmap_ds_iterator hashmap_getiterator(hashmap_ds *this) {
	hashmap_ds_iterator itr;
	itr.index = 0;
	itr.map = this;
	itr.entries = this->table;
	return itr;
}

int hashmap_iterator_hasnext(hashmap_ds_iterator *itr) {
	return itr->index < itr->map->size;
}

hashmap_entry *hashmap_iterator_next(hashmap_ds_iterator *itr) {
	hashmap_entry *entry = NULL;
	if (!hashmap_iterator_hasnext(itr)) return NULL;
	while (entry == NULL || entry == TOMBSTONE) {
		entry = *itr->entries++;
	}
	itr->index++;
	return entry;
}

static int reference_hash(const void *E) {
	unsigned long value = (unsigned long)E;
	return (int)(value ^ (value >> 32));
}

static int reference_equality(const void *E_1, const void *E_2) {
	return E_1 == E_2;
}

hashmap_ds *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*)) {
	size_t i;
	
	hashmap_ds *map = malloc(sizeof *map);
	if (map == NULL) {
		fprintf(stderr, "**hashmap memory allocation failure** : failed to allocate new map\n");
		exit(EXIT_FAILURE);
	}
	map->hash = hash;
	map->is_equals = is_equals;
	
	map->size = 0;
	map->capacity = 64;
	map->table = malloc(64 * sizeof *map->table);
	
	if (map->table == NULL) {
		free(map);
		fprintf(stderr, "**hashmap memory allocation failure** : failed to allocate a table\n");
		exit(EXIT_FAILURE);
	}
	
	for (i = 0; i < 64; i++) {
		map->table[i] = NULL;
	}
	
	return map;
}

hashmap_ds *alloc_identityhashmap() {
	return alloc_hashmap(reference_hash, reference_equality);
}

void dealloc_hashmap(hashmap_ds *this) {
	size_t i, capacity;
	for (i = 0, capacity = this->capacity; i < capacity; i++) {
		if (this->table[i] != NULL && this->table[i] != TOMBSTONE) free(this->table[i]);
	}
	free(this->table);
	free(this);
}

void *hashmap_put(hashmap_ds *this, void *key, void *value) {
	int i = this->hash(key);
	if (i < 0) i *= -1;
	i %= this->capacity;
	
	for(; this->table[i] != NULL && this->table[i] != TOMBSTONE; i++) {
		if (this->is_equals(this->table[i]->key, key)) {
			void *oldval = this->table[i]->value;
			this->table[i]->value = value;
			return oldval;
		}
		if (i == this->capacity - 1) {i = -1;}
	}
	
	this->table[i] = malloc(sizeof *this->table[i]);
	if (this->table[i] == NULL) {
		dealloc_hashmap(this);
		fprintf(stderr, "**hashmap memory allocation failure ** : failed to allocate a table entry\n");
		exit(EXIT_FAILURE);
	}
	this->table[i]->key = key;
	this->table[i]->value = value;
	this->size++;
	return NULL;
}

void *hashmap_get(hashmap_ds *this, void *key) {
	int i = this->hash(key);
	if (i < 0) i *= -1;
	i %= this->capacity;
	
	for (; this->table[i] != NULL; i++) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			return this->table[i]->value;
		}
		if (i == this->capacity - 1) {i = -1;}
	}
	return NULL;
}

void *hashmap_get_keyref(hashmap_ds *this, void *key) {
	int i = this->hash(key);
	if (i < 0) i *= -1;
	i %= this->capacity;
	
	for (; this->table[i] != NULL; i++) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			return this->table[i]->key;
		}
		if (i == this->capacity - 1) {i = -1;}
	}
	return NULL;
}

void *hashmap_remove(hashmap_ds *this, void *key) {
	int i = this->hash(key);
	if (i < 0) i *= -1;
	i %= this->capacity;
	
	for (; this->table[i] != NULL; i++) {
		if (this->table[i] == TOMBSTONE) continue;
		if (this->is_equals(this->table[i]->key, key)) {
			void *oldval = this->table[i]->value;
			free(this->table[i]);
			this->table[i] = TOMBSTONE;
			this->size--;
			return oldval;
		}
		if (i == this->capacity - 1) {i = -1;}
	}
	return NULL;
}

hashmap_entry **hashmap_getentries(hashmap_ds *this) {
	size_t i, capacity;
	
	/* allocate a list of entries + one more slot that indicates the end of a list using NULL*/
	hashmap_entry **iteration, **entries = malloc((this->size+1) * sizeof *entries);
	if (entries == NULL) {
		dealloc_hashmap(this);
		fprintf(stderr, "**hashmap memory allocation failure ** : failed to allocate a list of entries\n");
		exit(EXIT_FAILURE);
	}
	
	entries[this->size] = NULL;
	iteration = entries;
	for (i = 0, capacity = this->capacity; i < capacity; i++) {
		if (this->table[i] != NULL && this->table[i] != TOMBSTONE) *iteration++ = this->table[i];
	}
	return entries;
}
