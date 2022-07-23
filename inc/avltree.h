#ifndef AVLTREE_H
#define AVLTREE_H
typedef struct avltree_ds avltree_ds;
typedef struct avltree_ds_iterator avltree_ds_iterator;

avltree_ds *alloc_avltree(int comparator(const void*, const void*));
void dealloc_avltree(avltree_ds*);
int avltree_insert(avltree_ds*, void *val);
int avltree_remove(avltree_ds*, void *val);
void *avltree_min(avltree_ds*);
void *avltree_max(avltree_ds*);

avltree_ds_iterator *alloc_avltree_iterator(avltree_ds*);
void dealloc_avltree_iterator(avltree_ds_iterator*);
int avltree_iterator_hasnext(avltree_ds_iterator*);
void *avltree_iterator_next(avltree_ds_iterator*);

#endif
