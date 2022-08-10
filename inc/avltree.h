#ifndef AVLTREE_H
#define AVLTREE_H

/**
 * Forward declaration of the avltree data structure. Internally implemented with a nested struct pointer
 * (node containg the value and its left and right children). In addition, the node contains a height variable
 * as it helps the tree keep balance whilist insertions and deletions occur.
 */
typedef struct avltree_ds avltree_ds;

/**
 * Forward declaration of an avltree iterator. Internally implemented with a deque that iterates through
 * the nodes in an in-order traversal.
 */
typedef struct avltree_ds_iterator avltree_ds_iterator;

/**
 * Allocates an avltree instance with the given comparator function.
 *
 * @param[in] comparator function that compares values
 * @return instance of the avltree
 */
avltree_ds *alloc_avltree(int comparator(const void*, const void*));

/**
 * Deallocates an avltree.
 *
 * @param this deallocates the given avltree
 */
void dealloc_avltree(avltree_ds *this);

/**
 * Inserts a value in the given avltree in the order according to
 * its given comparator.
 *
 * @param this given avltree instance
 * @param[in] val given pointer to value
 * @return truey if operation was succesful, falsey otherwise
 */
int avltree_insert(avltree_ds *this, void *val);

/**
 * Removes a value in the given avltree if it already exists.
 *
 * @param this given avltree instance
 * @param[in] val given pointer to value
 * @return truey if operation was succesful, falsey otherwise
 */
int avltree_remove(avltree_ds *this, void *val);

/**
 * Retrives the smallest value existing in the avltree.
 *
 * @param this given avltree instance
 * @return pointer to min value or NULL if tree is empty
 */
void *avltree_min(avltree_ds *this);

/**
 * Retrives the largest value existing in the avltree.
 *
 * @param this given avltree instance
 * @return pointer to max value or NULL if tree is empty
 */
void *avltree_max(avltree_ds *this);

/**
 *
 *
 * @param[in] this given avltree instance
 * @return allocated avltree iterator
 */
avltree_ds_iterator *alloc_avltree_iterator(avltree_ds *this);

/**
 * Deallocates an avltree iterator.
 *
 * @param itr given avltree iterator
 */
void dealloc_avltree_iterator(avltree_ds_iterator *itr);

/**
 * Determines whether there are value left to iterate.
 *
 * @param itr given avltree iterator
 * @return truey if there's an value left, falsey otherwise
 */
int avltree_iterator_hasnext(avltree_ds_iterator *itr);

/**
 * Retrives the next value in the avltree iterator. Note that
 * if this function is called when avltree_iterator_hasnext()
 * returns false, the program aborts abruptly.
 *
 * @param itr given avltree iterator
 * @return pointer to the next value
 */
void *avltree_iterator_next(avltree_ds_iterator *itr);

#endif
