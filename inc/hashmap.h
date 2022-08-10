#ifndef HASHMAP_H
#define HASHMAP_H

/**
 * Basic unit of the hashmap: an entry. Some functions like hashmap_getentries(hashmap_ds*) will return
 * a list of entry pointers which contains easily accessible key-value pairs at your disposal.
 */
typedef struct hashmap_entry {
	void *key;		/**< pointer to key */
	void *value;	/**< pointer to value */
	size_t psl;		/**< probe sequence length; outside modifications results in undefined behavior */
} hashmap_entry;

/**
 * Forward declaration of the hashmap data structure. Internally implemented as
 * a dynamic list of entry pointers using robin-hood open addressing.
 */
typedef struct hashmap_ds hashmap_ds;

/**
 * This struct gives functionality to iterate through a hashmap non-destructively. At most, the values
 * could be modified for any entry returned. Despite the internals being visible, this shall be treated
 * as an opaque structure with the given functions only, as directly modifying the members can result in
 * undefined behavior.
 *
 * @see hashmap_getiterator(hashmap_ds*)
 * @see hashmap_iterator_hasnext(hashmap_ds_iterator*)
 * @see hashmap_iterator_next(hashmap_ds_iterator*)
 */
typedef struct hashmap_ds_iterator {
	size_t index; 				/**< current key/value pair out of current hashmap size */
	hashmap_ds *map; 			/**< given hashmap instance */
	hashmap_entry **entries; 	/**< given entries to iterate on */
} hashmap_ds_iterator;

/**
 * Constructs a hashmap iterator.
 *
 * @param[in] this given hashmap instance
 * @return iterator for the hashmap instance
 */
hashmap_ds_iterator hashmap_getiterator(hashmap_ds *this);

/**
 * Determines whether there are entries left to iterate.
 *
 * @param itr given iterator instance
 * @return truey if there's an entry left, falsey otherwise
 */
int hashmap_iterator_hasnext(hashmap_ds_iterator *itr);

/**
 * Retrives an entry.
 *
 * @param itr given iterator instance
 * @return entry from the ietrator
 */
hashmap_entry *hashmap_iterator_next(hashmap_ds_iterator *itr);

/**
 * Allocates a hashmap instance with given hash/equality functions and a default capacity of 16,
 * and default load factor of 0.75.
 *
 * @param[in] hash key-hash function
 * @param[in] is_equals key-equality function
 * @return instance of the hashmap
 */
hashmap_ds *alloc_hashmap(int hash(const void*), int is_equals(const void*, const void*));

/**
 * Allocates a hashmap whose hash/equality is determined by the key's identity instead. Other properties
 * are still default according to alloc_hashmap()
 *
 * @return instance of the identity hashmap
 */
hashmap_ds *alloc_identityhashmap(void);

/**
 * Deallocates a hashmap.
 *
 * @param this deallocates the given hashmap
 */
void dealloc_hashmap(hashmap_ds *this);

/**
 * Puts a new key/value pair in the hashmap if it doesn't exist already, otherwise value is replaced.
 *
 * @param this given hashmap instance
 * @param[in] key given pointer to key
 * @param[in] value given pointer to value
 * @return NULL if key/value pair didn't exist prior to insertion, otherwise pointer to the old value that was associated with the key
 */
void *hashmap_put(hashmap_ds *this, void *key, void *value);

/**
 * Retrives a value mapped to the given key if it exists in the map.
 *
 * @param this given hashmap instance
 * @param[in] key given pointer to key
 * @return pointer to the value if the mapping exists, NULL otherwise
 */
void *hashmap_get(hashmap_ds *this, void *key);

/**
 * Retrives the exact pointer to key if the mapping exists in the hashmap. The given key
 * and the retrieved key may not necessarily share the memory address.
 *
 * @param this given hashmap instance
 * @param[in] key given pointer to key
 * @return pointer to the key if the mapping exists, NULL otherwise
 */
void *hashmap_get_keyref(hashmap_ds *this, void *key);

/**
 * Removes a key/value pair in the hashmap with the given key if it exists in the hashmap, otherwise no effect occurs.
 *
 * @param this given hashmap instance
 * @param[in] key given pointer to key
 * @return value that was mapped to key if it exist prior to deletion, NULL otherwise
 */
void *hashmap_remove(hashmap_ds *this, void *key);

/**
 * Allocates a NULL-terminated list of entries for the given hashmap instance. Must be freed manually if you don't use it anymore!
 *
 * @param[in] this given hashmap instance
 * @return dynamically allocated list of hashmap entries
 */
hashmap_entry **hashmap_getentries(hashmap_ds *this);

#endif
