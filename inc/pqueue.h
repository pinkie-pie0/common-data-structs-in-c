#ifndef PQUEUE_H
#define PQUEUE_H

/**
 * Forward declaration of the pqueue data structure. Internally implemented as a 4-ary heap with a hashmap
 * ensuring no duplicate elements (think unordered_map<element, index>). Elements are enqueued accordingly
 * to the given comparator function.
 */
typedef struct pqueue_ds pqueue_ds;

/**
 * Allocates an pqueue instance with the given comparator function.
 *
 * @param[in] comparator function that compares values
 * @return instance of the pqueue
 */
pqueue_ds *alloc_pqueue(int comparator(const void*,const void*));

/**
 * Deallocates a pqueue.
 *
 * @param this deallocates the given pqueue
 */
void dealloc_pqueue(pqueue_ds *this);

/**
 * Inserts an element in the given pqueue.
 *
 * @param this given pqueue instance
 * @param[in] element given element
 * @return truey if element didnt exist prior to insertion, falsey otherwise
 */
int pqueue_enqueue(pqueue_ds *this, void *element);

/**
 * Removes and retrieves the head of the pqueue.
 *
 * @param this given pqueue instance
 * @return pointer to the element, or NULL if pqueue is empty
 */
void *pqueue_dequeue(pqueue_ds *this);

/**
 * Given an element, checks to see if its priority has been changed and if so
 * the pqueue is updated.
 *
 * @param this given pqueue instance
 * @param[in] element given element
 */
void pqueue_update(pqueue_ds *this, void *element);

/**
 * Given an element, tests if it exists in the pqueue and if so removes and returns
 * the pointer to it.
 *
 * @param this given pqueue instance
 * @param[in] element given element
 * @return pointer to the given element, or NULL if element doesn't exist in pqueue
 */
void *pqueue_remove(pqueue_ds *this, void *element);

/**
 * Retrieves the head of the pqueue without removing it.
 *
 * @param this given pqueue instance
 * @return pointer to the element, or NULL if pqueue is empty
 */
void *pqueue_peek(pqueue_ds *this);

#endif
