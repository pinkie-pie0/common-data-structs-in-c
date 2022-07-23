#ifndef PQUEUE_H
#define PQUEUE_H
typedef struct pqueue_ds pqueue_ds;

pqueue_ds *alloc_pqueue(int comparator(const void*,const void*));
void dealloc_pqueue(pqueue_ds *this);
int pqueue_enqueue(pqueue_ds *this, void *element);
void *pqueue_dequeue(pqueue_ds *this);
void pqueue_update(pqueue_ds *this, void *element);
void *pqueue_peek(pqueue_ds *this);
void *pqueue_remove(pqueue_ds *this, void *element);
#endif
