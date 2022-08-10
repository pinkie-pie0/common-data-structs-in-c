#ifndef DEQUE_H
#define DEQUE_H

/**
 * Forward declaration of the deque data structure. Internally implemented as a growing dynamic array.
 */
typedef struct deque_ds deque_ds;

/**
 * Allocates a deque instance.
 *
 * @return deque instance
 */
deque_ds *alloc_deque(void);

/**
 * Deallocates a deque.
 *
 * @param this deallocates the given deque
 */
void dealloc_deque(deque_ds *this);

/**
 * Inserts the given element at left of the deque.
 *
 * @param this given deque instance
 * @param[in] el given element
 */
void deque_pushleft(deque_ds *this, void *el);

/**
 * Inserts the given element at right of the deque.
 *
 * @param this given deque instance
 * @param[in] el given element
 */
void deque_pushright(deque_ds *this, void *el);

/**
 * Removes and retrieves the element that was at left of the deque.
 *
 * @param this given deque instance
 * @return removed leftmost value of the deque, or NULL if deque is empty
 */
void *deque_popleft(deque_ds *this);

/**
 * Removes and retrieves the element that was at right of the deque.
 *
 * @param this given deque instance
 * @return removed righttmost value of the deque, or NULL if deque is empty
 */
void *deque_popright(deque_ds *this);

/**
 * Retrives element at the left of the deque without removing it.
 *
 * @param this given deque instance
 * @return leftmost value of the deque, or NULL if deque is empty
 */
void *deque_peekleft(deque_ds *this);

/**
 * Retrives element at the right of the deque without removing it.
 *
 * @param this given deque instance
 * @return right value of the deque, or NULL if deque is empty
 */
void *deque_peekright(deque_ds *this);

/**
 * @see deque_pushright(deque_ds*, void*)
 */
#define deque_enqueue(this, el) deque_pushright(this, el)

/**
 * @see deque_popleft(deque_ds*)
 */
#define deque_dequeue(this) deque_popleft(this)

/**
 * @see deque_pushright(deque_ds*, void*)
 */
#define deque_push(this, el) deque_pushright(this, el)

/**
 * @see deque_popright(deque_ds*)
 */
#define deque_pop(this) deque_popright(this)

/**
 * @see deque_peekright(deque_ds*)
 */
#define deque_stackpeek(this) deque_peekright(this)

/**
 * @see deque_peekleft(deque_ds*)
 */
#define deque_queuepeek(this) deque_peekleft(this)

/**
 * Tests whether the given deque is empty.
 *
 * @param this given deque instance
 * @return truey if deque is empty, falsey otherwise
 */
int deque_isempty(deque_ds *this);

#endif
