#ifndef AVLTREE_H
#define AVLTREE_H
typedef struct avltree_T avltree_T;

avltree_T *alloc_avltree(int comparator(const void*, const void*));
void dealloc_avltree(avltree_T*);
void avltree_insert(avltree_T*, void *val);
void avltree_remove(avltree_T*, void *val);
void *avltree_min(avltree_T*);
void *avltree_max(avltree_T*);

#endif
