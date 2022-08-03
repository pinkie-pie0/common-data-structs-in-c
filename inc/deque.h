#ifndef DEQUE_H
#define DEQUE_H

typedef struct deque_ds deque_ds;

deque_ds *alloc_deque(void);
void dealloc_deque(deque_ds*);

void deque_pushleft(deque_ds*, void *el);
void deque_pushright(deque_ds*, void *el);
void *deque_popleft(deque_ds*);
void *deque_popright(deque_ds*);
void *deque_peekleft(deque_ds*);
void *deque_peekright(deque_ds*);
#define deque_enqueue(this, el) deque_pushright(this, el)
#define deque_dequeue(this) deque_popleft(this)
#define deque_push(this, el) deque_pushright(this, el)
#define deque_pop(this) deque_popright(this)
#define deque_stackpeek(this) deque_peekright(this)
#define deque_queuepeek(this) deque_peekleft(this)
int deque_isempty(deque_ds*);

#endif
