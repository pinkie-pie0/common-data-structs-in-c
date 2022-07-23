#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
typedef struct priorityqueue_ds priorityqueue_ds;

priorityqueue_ds *alloc_priorityqueue(int comparator(const void*,const void*));
void dealloc_priorityqueue(priorityqueue_ds *this);
int priorityqueue_enqueue(priorityqueue_ds *this, void *element);
void *priorityqueue_dequeue(priorityqueue_ds *this);
void priorityqueue_update(priorityqueue_ds *this, void *element);
void *priorityqueue_peek(priorityqueue_ds *this);
void *priorityqueue_remove(priorityqueue_ds *this, void *element);
#endif
