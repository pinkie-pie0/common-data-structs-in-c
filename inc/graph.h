#ifndef GRAPH_H
#define GRAPH_H
#include "deque.h"
typedef struct graph_ds graph_ds;

graph_ds *alloc_graph(int label_hash(const void*), int label_equals(const void*, const void*));
void dealloc_graph(graph_ds *this);
int graph_add_vertex(graph_ds *this, void *label);
int graph_add_edge(graph_ds *this, void *a, void *b, double weight);
int graph_remove_edge(graph_ds *this, void *a, void *b);
int graph_remove_vertex(graph_ds *this, void *label);
int graph_has_edge(graph_ds *this, void *a, void *b);
deque_ds *graph_breadth_first_search(graph_ds *this, void *origin);
deque_ds *graph_depth_first_search(graph_ds *this, void *origin);
/*int graph_minimum_colors(graph_ds *this, void *origin);*/
double graph_cheapest_path(graph_ds *this, void *a, void *b, deque_ds *stack);
#endif
