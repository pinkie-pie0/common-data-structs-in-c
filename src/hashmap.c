#include <stdio.h>
#include <stdlib.h>

#define DS_NAME "hashmap"
#include "err/ds_assert.h"

#define INITIAL_CAPACITY 16

typedef struct hashmap_entry hashmap_entry;
typedef struct hashmap_ds {
	int (*hash)(const void*);
	int (*is_equals)(const void*, const void*);
	size_t size, capacity, load_factor;
	struct hashmap_entry {
		void *key, *value;
		size_t psl;
	} **table;
} hashmap_ds;

void *hashmap_put(hashmap_ds *, void *key, void *value);

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
	while (entry == NULL) {
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
}

static int reference_equality(const void *E_1, const void *E_2) {
	return E_1 == E_2;
}

static void hashmap_rehash(hashmap_ds *const this) {
	size_t i;
	
	/* initialize all values of the temporary hashmap */
	hashmap_ds temp;
	temp.hash = this->hash;
	temp.is_equals = this->is_equals;
	temp.size = 0;
	temp.capacity = this->capacity << 1;
	temp.load_factor = (temp.capacity * 3) >> 2;
	temp.table = malloc(temp.capacity * sizeof *temp.table);
	DS_ASSERT(temp.table != NULL, "failed to allocate memory for rehashed table");
	
	for (i = 0; i < temp.capacity; i++) {
		temp.table[i] = NULL;
	}
	
	/* put all pairs from the hashmap's old table */
	for (i = 0; i < this->capacity; i++) {
		if (this->table[i] != NULL) {
			hashmap_put(&temp, this->table[i]->key, this->table[i]->value);
			free(this->table[i]);
		}
	}
	
	/* only things we need to change are capacity, load factor, and the new table */
	this->capacity = temp.capacity;
	this->load_factor = temp.load_factor;
	free(this->table);
	this->table = temp.table;
}

hashmap_ds *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*)) {
	size_t i;
	
	hashmap_ds *this = malloc(sizeof *this);
	DS_ASSERT(this != NULL, "failed to allocate memory for new " DS_NAME);
	
	this->hash = hash;
	this->is_equals = is_equals;
	
	this->size = 0;
	this->capacity = INITIAL_CAPACITY;
	this->load_factor = (INITIAL_CAPACITY * 3) >> 2;
	this->table = malloc(INITIAL_CAPACITY * sizeof *this->table);
	DS_ASSERT(this->table != NULL, "failed to allocate memory for the " DS_NAME "'s table");
	
	for (i = 0; i < INITIAL_CAPACITY; i++) {
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
		if (this->table[i] != NULL) free(this->table[i]);
	}
	free(this->table);
	free(this);
}

static hashmap_entry **hashmap_search(hashmap_ds *const this, void *key) {
	size_t mask = this->capacity - 1;
	size_t i = ((size_t)absval(this->hash(key))) & mask;
	for (; this->table[i] != NULL; i = (i+1) & mask) {
		if (this->is_equals(this->table[i]->key, key)) {
			break;
		}
	}
	return &this->table[i];
}

void *hashmap_put(hashmap_ds *const this, void *key, void *value) {
	size_t i;
	hashmap_entry *insertion;
	
	
	if (this->size >= this->load_factor) hashmap_rehash(this);
	i = ((size_t)absval(this->hash(key))) % this->capacity;
	
	insertion = malloc(sizeof *insertion);
	DS_ASSERT(insertion != NULL, "failed to allocate memory for a new table entry");
	insertion->key = key;
	insertion->value = value;
	for (insertion->psl = 0; this->table[i] != NULL; i = (i+1) % this->capacity, insertion->psl++) {
		if (this->is_equals(this->table[i]->key, key)) {
			void *oldval = this->table[i]->value;
			this->table[i]->value = value;
			free(insertion);
			return oldval;
		} else if (insertion->psl > this->table[i]->psl) {
			/* swap */
			hashmap_entry *temp = this->table[i];
			this->table[i] = insertion;
			
			/* find new spot */
			insertion = temp;
		}
	}
	
	this->table[i] = insertion;
	this->size++;
	return NULL;
}

void *hashmap_get(hashmap_ds *const this, void *key) {
	hashmap_entry **entryref = hashmap_search(this, key);
	return *entryref != NULL ? (*entryref)->value : NULL;
}

void *hashmap_get_keyref(hashmap_ds *const this, void *key) {
	hashmap_entry **entryref = hashmap_search(this, key);
	return *entryref != NULL ? (*entryref)->key : NULL;
}

void *hashmap_remove(hashmap_ds *const this, void *key) {
	void *oldval = NULL;
	hashmap_entry **entryref = hashmap_search(this, key);
	
	if (*entryref != NULL) {
		size_t i, mask = this->capacity - 1;
		
		oldval = (*entryref)->value;
		free(*entryref);
		*entryref = NULL;
		
		/* engage backward shifting */
		i = (((size_t)(entryref - this->table)) + 1) & mask;
		for (; this->table[i] != NULL && this->table[i]->psl > 0; i = (i+1) & mask) {
			this->table[i]->psl--;
			
			/* account for unsigned wraparound if any */
			this->table[(i-1) & mask] = this->table[i];
			this->table[i] = NULL;
		}
		
		this->size--;
	}
	
	return oldval;
}

hashmap_entry **hashmap_getentries(hashmap_ds *const this) {
	size_t i, capacity;
	
	/* allocate a list of entries + one more slot that indicates the end of a list using NULL*/
	hashmap_entry **iteration, **entries = malloc((this->size+1) * sizeof *entries);
	DS_ASSERT(entries != NULL, "failed to allocate a list of entries");
	
	entries[this->size] = NULL;
	iteration = entries;
	for (i = 0, capacity = this->capacity; i < capacity; i++) {
		if (this->table[i] != NULL) *iteration++ = this->table[i];
	}
	return entries;
}
