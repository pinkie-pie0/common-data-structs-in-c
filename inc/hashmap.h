#ifndef HASHMAP_H
#define HASHMAP_H
typedef struct hashmap_entry {void *key; void *value;} hashmap_entry;
typedef struct hashmap_T hashmap_T;
typedef struct hashmap_T_iterator {size_t index; hashmap_T *map; hashmap_entry **entries;} hashmap_T_iterator;

hashmap_T_iterator hashmap_getiterator(hashmap_T *this);
int hashmap_iterator_hasnext(hashmap_T_iterator *itr);
hashmap_entry *hashmap_iterator_next(hashmap_T_iterator *itr);

hashmap_T *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*));
hashmap_T *alloc_identityhashmap(void);
void dealloc_hashmap(hashmap_T *this);
void *hashmap_put(hashmap_T *this, void *key, void *value);
void *hashmap_get(hashmap_T *this, void *key);
void *hashmap_get_keyref(hashmap_T *this, void *key);
void *hashmap_remove(hashmap_T *this, void *key);
hashmap_entry **hashmap_getentries(hashmap_T *this);
#endif
