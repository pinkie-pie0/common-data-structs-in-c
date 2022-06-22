#ifndef GRAPH_H
#define GRAPH_H
#include "deque.h"
typedef struct graph_T graph_T;

graph_T *alloc_graph(int label_hash(const void*), int label_equals(const void*, const void*));
void dealloc_graph(graph_T *this);
int graph_add_vertex(graph_T *this, void *label);
int graph_add_edge(graph_T *this, void *a, void *b, double weight);
int graph_remove_edge(graph_T *this, void *a, void *b);
int graph_remove_vertex(graph_T *this, void *label);
int graph_has_edge(graph_T *this, void *a, void *b);
deque_T *graph_breadth_first_search(graph_T *this, void *origin);
/*int graph_minimum_colors(graph_T *this, void *origin);*/
double graph_cheapest_path(graph_T *this, void *a, void *b, deque_T *stack);
#endif