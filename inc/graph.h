#ifndef GRAPH_H
#define GRAPH_H
#include "deque.h"

/**
 * Forward declaration for the graph data structure. Internally implemented as an weighted undirected graph that
 * uses a nested hashmap as the adjacency list (think unordered_map<node, unordered_map<node, double>>,
 * such syntax doesn't exist in C but that's how the graph is implemented, almost). There can only exist at most
 * one symmetric edge between any two vertices.
 */
typedef struct graph_ds graph_ds;

/**
 * Allocates a weighted undirected graph instance with given hash/equality functions.
 *
 * @return instance of the graph
 */
graph_ds *alloc_graph(int label_hash(const void*), int label_equals(const void*, const void*));

/**
 * Deallocates a graph.
 *
 * @param this deallocates the given graph
 */
void dealloc_graph(graph_ds *this);

/**
 * Adds a vertex to this graph with the given label.
 *
 * @param this given graph instance
 * @param[in] label given label
 * @return truey if operation was succesful, falsey otherwise
 */
int graph_add_vertex(graph_ds *this, void *label);

/**
 * Adds a weighted undirected edge between two labels that correspond to the vertices. The
 * labels must have been added already, and the edge must not already exist.
 *
 * @param this given graph instance
 * @param[in] a first label
 * @param[in] b second label
 * @param[in] weight given weight
 * @return truey if operation was succesful, falsey otherwise
 */
int graph_add_edge(graph_ds *this, void *a, void *b, double weight);

/**
 * Removes a weighted undirected edge between two labels that correspond to the vertices. The
 * labels must have been added already, and the edge must already exist.
 *
 * @param this given graph instance
 * @param[in] a first label
 * @param[in] b second label
 * @return truey if operation was succesful, falsey otherwise
 */
int graph_remove_edge(graph_ds *this, void *a, void *b);

/**
 * Removes a vertex in this graph that corresponds to the given label.
 *
 * @param this given graph instance
 * @param[in] label given label
 * @return truey if operation was succesful, falsey otherwise
 */
int graph_remove_vertex(graph_ds *this, void *label);

/**
 * Tests whether an undirected edges exist between two given labels.
 *
 * @param this given graph instance
 * @param[in] a first label
 * @param[in] b second label
 * @return truey if edge exists, falsey otherwise
 */
int graph_has_edge(graph_ds *this, void *a, void *b);

/**
 * Given an origin label, retrives a breadth first search listing from the
 * origin to other neighbors in the graph.
 *
 * @param this given graph instance
 * @param[in] origin given label corresponding the origin vertex
 * @return deque representing breadth first search starting from origin
 */
deque_ds *graph_breadth_first_search(graph_ds *this, void *origin);

/**
 * Given an origin label, retrives a depth first search listing from the
 * origin to other neighbors in the graph.
 *
 * @param this given graph instance
 * @param[in] origin given label corresponding the origin vertex
 * @return deque representing depth first search starting from origin
 */
deque_ds *graph_depth_first_search(graph_ds *this, void *origin);

/**
 *
 *
 * @param this given graph instance
 * @param[in] a first label
 * @param[in] b second label
 * @param[out] stack cheapest path taken from a to b (nullabe)
 */
double graph_cheapest_path(graph_ds *this, void *a, void *b, deque_ds *stack);

#endif
