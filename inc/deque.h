#ifndef DEQUE_H
#define DEQUE_H

typedef struct deque_ds deque_ds;
#define deque_enqueue(this, el) deque_pushright(this, el)
#define deque_dequeue(this) deque_popleft(this)
#define deque_push(this, el) deque_pushright(this, el)
#define deque_pop(this) deque_popright(this)
#define deque_stackpeek(this) deque_peekright(this)
#define deque_queuepeek(this) deque_peekleft(this)

deque_ds *alloc_deque(void);
void dealloc_deque(deque_ds *this);
void deque_pushleft(deque_ds *this, void *el);
void deque_pushright(deque_ds *this, void *el);
void *deque_popleft(deque_ds *this);
void *deque_popright(deque_ds *this);
void *deque_peekleft(deque_ds *this);
void *deque_peekright(deque_ds *this);
int deque_isempty(deque_ds *this);
#endif
