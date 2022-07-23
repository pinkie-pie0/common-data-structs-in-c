#ifndef HASHMAP_H
#define HASHMAP_H
typedef struct hashmap_entry {void *key; void *value;} hashmap_entry;
typedef struct hashmap_ds hashmap_ds;
typedef struct hashmap_ds_iterator {size_t index; hashmap_ds *map; hashmap_entry **entries;} hashmap_ds_iterator;

hashmap_ds_iterator hashmap_getiterator(hashmap_ds *this);
int hashmap_iterator_hasnext(hashmap_ds_iterator *itr);
hashmap_entry *hashmap_iterator_next(hashmap_ds_iterator *itr);

hashmap_ds *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*));
hashmap_ds *alloc_identityhashmap(void);
void dealloc_hashmap(hashmap_ds *this);
void *hashmap_put(hashmap_ds *this, void *key, void *value);
void *hashmap_get(hashmap_ds *this, void *key);
void *hashmap_get_keyref(hashmap_ds *this, void *key);
void *hashmap_remove(hashmap_ds *this, void *key);
hashmap_entry **hashmap_getentries(hashmap_ds *this);
#endif
