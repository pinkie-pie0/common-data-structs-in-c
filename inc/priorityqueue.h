#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
typedef struct priorityqueue_T priorityqueue_T;

priorityqueue_T *alloc_priorityqueue(int comparator(const void*,const void*));
void dealloc_priorityqueue(priorityqueue_T *this);
int priorityqueue_enqueue(priorityqueue_T *this, void *element);
void *priorityqueue_dequeue(priorityqueue_T *this);
void priorityqueue_update(priorityqueue_T *this, void *element);
void *priorityqueue_peek(priorityqueue_T *this);
void *priorityqueue_remove(priorityqueue_T *this, void *element);
#endif
