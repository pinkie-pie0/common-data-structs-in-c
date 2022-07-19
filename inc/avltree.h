#ifndef AVLTREE_H
#define AVLTREE_H
typedef struct avltree_T avltree_T;
typedef struct avltree_T_iterator avltree_T_iterator;

avltree_T *alloc_avltree(int comparator(const void*, const void*));
void dealloc_avltree(avltree_T*);
int avltree_insert(avltree_T*, void *val);
int avltree_remove(avltree_T*, void *val);
void *avltree_min(avltree_T*);
void *avltree_max(avltree_T*);

avltree_T_iterator *alloc_avltree_iterator(avltree_T*);
void dealloc_avltree_iterator(avltree_T_iterator*);
int avltree_iterator_hasnext(avltree_T_iterator*);
void *avltree_iterator_next(avltree_T_iterator*);

#endif
