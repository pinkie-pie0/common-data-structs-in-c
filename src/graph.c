#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#include "pqueue.h"
#include "deque.h"

#define DS_NAME "graph"
#include "err/ds_assert.h"

/* graph abstract data type */
typedef struct graph_ds {
	int (*label_hash)(const void*);
	int (*label_equals)(const void*, const void*);
	hashmap_ds *adj_list;
	int num_edges;
} graph_ds;

/* basic unit of the graph */
typedef struct graph_vertex {
	void *label;
	size_t degree;
	struct graph_vertex *predecessor;
	graph_ds *this;
	double cost;
	int visited;
} vertex;

/*** HELPER FUNCTIONS - BEGIN ***/

enum COST_CONST {ZERO, INF};

static void init_vertex(vertex *v, void *label, graph_ds *const this) {
	v->label = label;
	v->degree = 0;
	v->predecessor = NULL;
	v->this = this;
	v->cost = 0.0;
	v->visited = 0;
}

static void graph_reset_vertices(graph_ds *const this, enum COST_CONST val) {
	double reset_val = (val == INF) ? 1.0/0.0 : 0.0;
	vertex *current_vertex;
	hashmap_entry *adj_list_entry;
	hashmap_ds_iterator adj_list_itr = hashmap_getiterator(this->adj_list);
	
	while (hashmap_iterator_hasnext(&adj_list_itr)) {
		adj_list_entry = hashmap_iterator_next(&adj_list_itr);
		current_vertex = adj_list_entry->key;
		current_vertex->cost = reset_val;
		current_vertex->visited = 0;
		current_vertex->predecessor = NULL;
	}
}

static vertex *corresponding_vertex(graph_ds *const this, void *label) {
	vertex v_check;
	init_vertex(&v_check, label, this);
	return hashmap_get_keyref(this->adj_list, &v_check);
}

static int cost_comparator(const void *v_1, const void *v_2) {
	double cost_1 = ((vertex*)v_1)->cost;
	double cost_2 = ((vertex*)v_2)->cost;
	return (cost_1 > cost_2) - (cost_1 < cost_2);
}

/*** HELPER FUNCTIONS - END ***/

static int vertex_hash(const void *V) {
	const vertex *v = V;
	return v->this->label_hash(v->label);
}

static int vertex_equality(const void *V_1, const void *V_2) {
	const vertex *v_1 = V_1;
	const vertex *v_2 = V_2;
	return v_1->this->label_equals(v_1->label, v_2->label);
}

graph_ds *alloc_graph(int label_hash(const void*), int label_equals(const void*, const void*)) {
	graph_ds *graph = malloc(sizeof *graph);
	DS_ASSERT(graph != NULL, "failed to allocate memory for new " DS_NAME);
	
	graph->label_hash = label_hash;
	graph->label_equals = label_equals;
	graph->adj_list = alloc_hashmap(vertex_hash, vertex_equality);
	graph->num_edges = 0;
	return graph;
}

void dealloc_graph(graph_ds *const this) {
	size_t i, j;
	hashmap_entry **inner_entries;
	hashmap_entry **outer_entries = hashmap_getentries(this->adj_list);
	for (i = 0; outer_entries[i] != NULL; i++) {
		/* free the vertex */
		free(outer_entries[i]->key);
		
		/* each vertex maps to another hashmap, process those */
		inner_entries = hashmap_getentries(outer_entries[i]->value);
		for (j = 0; inner_entries[j] != NULL; j++) {
			/* free the weight */
			free(inner_entries[j]->value);
		}
		
		/* we're done processing the inner hashmap */
		free(inner_entries);
		
		/* free the inner hashmap afterwards */
		dealloc_hashmap(outer_entries[i]->value);
	}
	
	free(outer_entries);
	dealloc_hashmap(this->adj_list);
	free(this);
}

int graph_add_vertex(graph_ds *const this, void *label) {
	vertex *new_vertex = malloc(sizeof *new_vertex);
	if (new_vertex == NULL) return 0;
	init_vertex(new_vertex, label, this);
	
	if (hashmap_get(this->adj_list, new_vertex) == NULL) {
		hashmap_put(this->adj_list, new_vertex, alloc_hashmap(vertex_hash, vertex_equality));
		return 1;
	}
	
	free(new_vertex);
	return 0;
}

int graph_add_edge(graph_ds *const this, void *a, void *b, double weight) {
	vertex *a_v = corresponding_vertex(this, a);
	vertex *b_v = corresponding_vertex(this, b);
	if (a_v != NULL && b_v != NULL) {
		double *vertex_weight;
		/* edge already exists */
		if (hashmap_get(hashmap_get(this->adj_list, a_v), b_v) != NULL) return 0;
		
		vertex_weight = malloc(sizeof *vertex_weight);
		if (vertex_weight == NULL) {
			dealloc_graph(this);
			fprintf(stderr, "**graph memory allocation failure** : failed to allocate an edge weight\n");
			exit(EXIT_FAILURE);
		}
		
		*vertex_weight = weight;
		hashmap_put(hashmap_get(this->adj_list, a_v), b_v, vertex_weight);
		
		vertex_weight = malloc(sizeof *vertex_weight);
		if (vertex_weight == NULL) {
			dealloc_graph(this);
			fprintf(stderr, "**graph memory allocation failure** : failed to allocate an edge weight\n");
			exit(EXIT_FAILURE);
		}
		
		*vertex_weight = weight;
		hashmap_put(hashmap_get(this->adj_list, b_v), a_v, vertex_weight);
		
		a_v->degree += 1;
		b_v->degree += 1;
		this->num_edges += 2;
		return 1;
	}
	return 0;
}

int graph_remove_edge(graph_ds *const this, void *a, void *b) {
	vertex *a_v = corresponding_vertex(this, a);
	vertex *b_v = corresponding_vertex(this, b);
	if (a_v != NULL && b_v != NULL && hashmap_get(hashmap_get(this->adj_list, a_v), b_v) != NULL) {
		/* free the dynamically allocated weights in both connections */
		free(hashmap_get(hashmap_get(this->adj_list, a_v), b_v));
		free(hashmap_get(hashmap_get(this->adj_list, b_v), a_v));
		/* disconnect the vertex in both inner hashmaps */
		hashmap_remove(hashmap_get(this->adj_list, a_v), b_v);
		hashmap_remove(hashmap_get(this->adj_list, b_v), a_v);
		
		a_v->degree -= 1;
		b_v->degree -= 1;
		this->num_edges -= 2;
		return 1;
	}
	return 0;
}

int graph_remove_vertex(graph_ds *const this, void *label) {
	vertex *removal = corresponding_vertex(this, label);
	if (removal != NULL) {
		size_t i;
		hashmap_entry **entries;
		entries = hashmap_getentries(hashmap_get(this->adj_list, removal));
		for (i = 0; entries[i] != NULL; i++) {
			graph_remove_edge(this, label, ((vertex*)entries[i]->key)->label);
		}
		free(entries);
		dealloc_hashmap(hashmap_get(this->adj_list, removal));
		hashmap_remove(this->adj_list, removal);
		free(removal);
		return 1;
	}
	return 0;
	
}

int graph_has_edge(graph_ds *const this, void *a, void *b) {
	vertex *a_v = corresponding_vertex(this, a);
	vertex *b_v = corresponding_vertex(this, b);
	return a_v != NULL && b_v != NULL ? hashmap_get(hashmap_get(this->adj_list, a_v), b_v) != NULL : 0;
}

static deque_ds *graph_breadth_first_search_internal(graph_ds *const this, void *origin, int return_labels) {
	hashmap_ds_iterator edge_itr;
	
	deque_ds *retval, *bfs;
	vertex *process, *origin_v = corresponding_vertex(this, origin);
	
	retval = alloc_deque();
	if (origin_v == NULL) return retval;
	
	graph_reset_vertices(this, ZERO);
	bfs = alloc_deque();
	deque_enqueue(retval, return_labels ? origin_v->label : origin_v);
	deque_enqueue(bfs, origin_v);
	origin_v->visited = 1;
	
	while (!deque_isempty(bfs)) {
		process = deque_dequeue(bfs);
		edge_itr = hashmap_getiterator(hashmap_get(this->adj_list, process));
		
		while(hashmap_iterator_hasnext(&edge_itr)) {
			process = hashmap_iterator_next(&edge_itr)->key;
			
			if (!process->visited) {
				process->visited = 1;
				deque_enqueue(retval, return_labels ? process->label : process);
				deque_enqueue(bfs, process);
			}
		}
	}
	
	dealloc_deque(bfs);
	return retval;
}

deque_ds *graph_breadth_first_search(graph_ds *const this, void *origin) {
	return graph_breadth_first_search_internal(this, origin, 1);
}

static void graph_depth_first_search_internal(graph_ds *const this, deque_ds *traversal_order, vertex *origin) {
	vertex *process;
	hashmap_ds_iterator edge_itr = hashmap_getiterator(hashmap_get(this->adj_list, origin));
	
	origin->visited = 1;
	deque_push(traversal_order, origin->label);
	
	while (hashmap_iterator_hasnext(&edge_itr)) {
		process = hashmap_iterator_next(&edge_itr)->key;
		if (!process->visited) {
			graph_depth_first_search_internal(this, traversal_order, process);
		}
	}
}

deque_ds *graph_depth_first_search(graph_ds *const this, void *origin) {
	deque_ds *retval = alloc_deque();
	vertex *origin_v = corresponding_vertex(this, origin);
	graph_reset_vertices(this, ZERO);
	if (origin_v != NULL) {
		graph_depth_first_search_internal(this, retval, origin_v);
	}
	return retval;
}

/* broken algorithm, needs a whole rewrite

static int largest_degree_comparator(const void *v_1, const void *v_2) {
	size_t degree_1 = ((vertex*)v_1)->degree;
	size_t degree_2 = ((vertex*)v_2)->degree;
	return (degree_1 < degree_2) - (degree_1 > degree_2);
}

int graph_minimum_colors(graph_ds *const this, void *origin) {
	hashmap_ds_iterator edge_itr, sub_edge_itr;
	int flag, minimum_colors = 0;
	
	deque_ds *backed = graph_breadth_first_search_internal(this, origin, 0);
	pqueue_ds *cluster = alloc_pqueue(largest_degree_comparator);
	
	vertex *process, *subprocess;
	
	while (!deque_isempty(backed)) {
		pqueue_enqueue(cluster, deque_dequeue(backed));
	}
	
	graph_reset_vertices(this, ZERO);
	while (pqueue_peek(cluster) != NULL) {
		process = pqueue_dequeue(cluster);
		process->visited = 1;
		flag = 0;
		minimum_colors++;
		
		do {
			edge_itr = hashmap_getiterator(hashmap_get(this->adj_list, process));
	
			while(hashmap_iterator_hasnext(&edge_itr)) {
				sub_edge_itr = hashmap_getiterator(hashmap_get(this->adj_list, hashmap_iterator_next(&edge_itr)->key));
				
				while(hashmap_iterator_hasnext(&sub_edge_itr)) {
					flag = 1;
					subprocess = hashmap_iterator_next(&sub_edge_itr)->key;
					printf("\treceived subneighbor %c\n", *(char*)subprocess->label);
					if (subprocess->visited) {
						flag = 0;
						break;
					}
				}
			}
			
			if (flag) {
				subprocess->visited = 1;
				pqueue_remove(cluster, subprocess);
				process = subprocess;
			}
			
		} while (flag);
	}
	
	return minimum_colors;
}
*/

/** implementation of dijkstra's algorithm - BEGIN **/
double graph_cheapest_path(graph_ds *const this, void *origin, void *end, deque_ds *stack) {
	
	double cheapest = -1;
	hashmap_ds_iterator edge_itr;
	hashmap_entry *current_neighbors;
	pqueue_ds *pq;
	
	vertex *process, *neighbor;
	vertex *origin_v = corresponding_vertex(this, origin);
	vertex *end_v = corresponding_vertex(this, end);
	
	if (origin_v == NULL || end_v == NULL) return -1.0;
	
	/* set cost of all vertices except origin as INF */
	graph_reset_vertices(this, INF);
	origin_v->cost = 0.0;
	
	pq = alloc_pqueue(cost_comparator);
	pqueue_enqueue(pq, origin_v);
	/* dirty way of checking if the queue is empty */
	while (pqueue_peek(pq) != NULL) {
		double new_distance;
		
		process = pqueue_dequeue(pq);
		process->visited = 1;
		
		if (process == end_v) {
			cheapest = process->cost;
			break;
		}

		edge_itr = hashmap_getiterator(hashmap_get(this->adj_list, process));
		while (hashmap_iterator_hasnext(&edge_itr)) {
			current_neighbors = hashmap_iterator_next(&edge_itr);
			neighbor = current_neighbors->key;
			new_distance = *(double*)(current_neighbors->value);
			
			if (!neighbor->visited) {
				/* this won't have effect if the vertex is already in PQ */
				pqueue_enqueue(pq, neighbor);
				new_distance += process->cost;
				
				/* hint to update priority */
				if (new_distance < neighbor->cost) {
					neighbor->cost = new_distance;
					neighbor->predecessor = process;
					pqueue_update(pq, neighbor);
				}
			}
		}
	}
	dealloc_pqueue(pq);
	
	if (stack != NULL) {
		vertex *traversal = end_v;
		while (traversal != NULL) {
			deque_push(stack, traversal->label);
			traversal = traversal->predecessor;
		}
	}
	return cheapest;
}
/** END **/
