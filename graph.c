#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#include "priorityqueue.h"

/* basic unit of the graph */
typedef struct graph_vertex {
	void *label;
	struct graph_vertex *predecessor;
	double cost;
	int visited;
} vertex;

/* graph adt */
typedef struct graph_T {
	hashmap_T *label_to_vertex_map;
	hashmap_T *adj_list;
	int num_edges;
} graph_T;

int vertex_hash(const void *E) {
	unsigned long value = (unsigned long)((vertex*)E)->label;
	return (int)(value ^ (value >> 32));
}

int vertex_equality(const void *E_1, const void *E_2) {
	return ((vertex*)E_1)->label == ((vertex*)E_2)->label;
}

vertex *alloc_vertex(void *label) {
	vertex *v = malloc(sizeof *v);
	if (v == NULL) return NULL;
	v->label = label;
	v->predecessor = NULL;
	v->cost = 0.0;
	v->visited = 0;
	return v;
}

graph_T *alloc_graph() {
	graph_T *graph = malloc(sizeof *graph);
	if (graph == NULL) {
		fprintf(stderr, "**graph memory allocation failure** : failed to allocate new graph\n");
		exit(EXIT_FAILURE);
	}
	
	/* FIXME: using an identity map for label:vertex pair is probably not the right choice here */
	graph->label_to_vertex_map = alloc_identityhashmap();
	graph->adj_list = alloc_hashmap(vertex_hash, vertex_equality);
	graph->num_edges = 0;
	return graph;
}

void dealloc_graph(graph_T *this) {
	/* BROKEN: CONCURRENT MODIFICATION HAPPENS
	hashmap_T_iterator inner_entries_itr, outer_entries_itr;
	hashmap_entry *inner_entry, *outer_entry;
	
	for (outer_entries_itr = hashmap_getiterator(this->adj_list); hashmap_iterator_hasnext(&outer_entries_itr);) {
		outer_entry = hashmap_iterator_next(&outer_entries_itr);
		
		// free the vertex
		free(outer_entry->key);
		
		// each vertex maps to another hashmap, process those
		for (inner_entries_itr = hashmap_getiterator(outer_entry->value); hashmap_iterator_hasnext(&inner_entries_itr);) {
			inner_entry = hashmap_iterator_next(&inner_entries_itr);
			// free the weight
			free(inner_entry->value);
		}
		
		// free the inner hashmap afterwards
		dealloc_hashmap(outer_entry->value);
	}

	dealloc_hashmap(this->adj_list);
	dealloc_hashmap(this->label_to_vertex_map);
	free(this);
	*/
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
	dealloc_hashmap(this->label_to_vertex_map);
	free(this);
}

int graph_add_vertex(graph_T *this, void *label) {
	if (hashmap_get(this->label_to_vertex_map, label) == NULL) {
		vertex *new_vertex = alloc_vertex(label);
		hashmap_put(this->label_to_vertex_map, label, new_vertex);
		hashmap_put(this->adj_list, new_vertex, alloc_hashmap(vertex_hash, vertex_equality));
		return 1;
	}
	return 0;
}

int graph_add_edge(graph_T *this, void *a, void *b, double weight) {
	vertex *a_, *b_;
	a_ = hashmap_get(this->label_to_vertex_map, a);
	b_ = hashmap_get(this->label_to_vertex_map, b);
	if (a_ != NULL && b_ != NULL) {
		double *vertex_weight;
		/* edge already exists */
		if (hashmap_get(hashmap_get(this->adj_list, a_), b_) != NULL) return 0;
		
		vertex_weight = malloc(sizeof *vertex_weight);
		if (vertex_weight == NULL) {
			dealloc_graph(this);
			fprintf(stderr, "**graph memory allocation failure** : failed to allocate an edge weight\n");
			exit(EXIT_FAILURE);
		}
		
		*vertex_weight = weight;
		hashmap_put(hashmap_get(this->adj_list, a_), b_, vertex_weight);
		
		vertex_weight = malloc(sizeof *vertex_weight);
		if (vertex_weight == NULL) {
			dealloc_graph(this);
			fprintf(stderr, "**graph memory allocation failure** : failed to allocate an edge weight\n");
			exit(EXIT_FAILURE);
		}
		
		*vertex_weight = weight;
		hashmap_put(hashmap_get(this->adj_list, b_), a_, vertex_weight);
		this->num_edges += 2;
		return 1;
	}
	return 0;
}

int graph_remove_edge(graph_T *this, void *a, void *b) {
	vertex *a_, *b_;
	a_ = hashmap_get(this->label_to_vertex_map, a);
	b_ = hashmap_get(this->label_to_vertex_map, b);
	if (a_ != NULL && b_ != NULL && hashmap_get(hashmap_get(this->adj_list, a_), b_) != NULL) {
		/* free the dynamically allocated weights in both connections */
		free(hashmap_get(hashmap_get(this->adj_list, a_), b_));
		free(hashmap_get(hashmap_get(this->adj_list, b_), a_));
		/* disconnect the vertex in both inner hashmaps */
		hashmap_remove(hashmap_get(this->adj_list, a_), b_);
		hashmap_remove(hashmap_get(this->adj_list, b_), a_);
		this->num_edges -= 2;
		return 1;
	}
	return 0;
}

int graph_remove_vertex(graph_T *this, void *label) {
	/* BROKEN: CONCURRENT MODIFICATION HAPPENS
	vertex *removal;
	removal = hashmap_get(this->label_to_vertex_map, label);
	if (removal != NULL) {
		
		hashmap_entry *entry;
		hashmap_T_iterator neighbor_itr = hashmap_getiterator(hashmap_get(this->adj_list, removal));
		while (hashmap_iterator_hasnext(&neighbor_itr)) {
			entry = hashmap_iterator_next(&neighbor_itr);
			printf("..\n");
			graph_remove_edge(this, label, ((vertex*)entry->key)->label);
		}
		dealloc_hashmap(hashmap_get(this->adj_list, removal));
		hashmap_remove(this->adj_list, removal);
		hashmap_remove(this->label_to_vertex_map, label);
		free(removal);
		return 1;
	}
	return 0;
	*/
	vertex *removal;
	removal = hashmap_get(this->label_to_vertex_map, label);
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
		hashmap_remove(this->label_to_vertex_map, label);
		free(removal);
		return 1;
	}
	return 0;
	
}

int graph_has_edge(graph_T *this, void *a, void *b) {
	vertex *a_, *b_;
	a_ = hashmap_get(this->label_to_vertex_map, a);
	b_ = hashmap_get(this->label_to_vertex_map, b);
	return a_ != NULL && b_ != NULL ? hashmap_get(hashmap_get(this->adj_list, a_), b_) != NULL : 0;
}

/** implementation of dijkstra's algorithm - BEGIN **/
int cost_comparator(const void *v_1, const void *v_2) {
	double cost_1 = ((vertex*)v_1)->cost;
	double cost_2 = ((vertex*)v_2)->cost;
	return (cost_1 > cost_2) - (cost_1 < cost_2);
}

double graph_cheapest_path(graph_T *this, void *a, void *b) {
	double cheapest = -1;
	const double INF = 1.0/0.0;
	size_t i;
	hashmap_T_iterator edge_itr;
	hashmap_entry *current_neighbors;
	priorityqueue_T *pq;
	
	vertex *process, *neighbor;
	vertex *origin = hashmap_get(this->label_to_vertex_map, a);
	vertex *end = hashmap_get(this->label_to_vertex_map, b);
	if (origin == NULL || end == NULL) return -1.0;
	
	/* set cost of all vertices except origin as INF - begin */
	{
		vertex *current_vertex;
		hashmap_entry *adj_list_entry;
		hashmap_T_iterator adj_list_itr = hashmap_getiterator(this->adj_list);
		
		while (hashmap_iterator_hasnext(&adj_list_itr)) {
			adj_list_entry = hashmap_iterator_next(&adj_list_itr);
			current_vertex = adj_list_entry->key;
			current_vertex->cost = (current_vertex == origin) ? 0 : INF;
			current_vertex->visited = 0;
			current_vertex->predecessor = NULL;
		}
	}
	/* end */
	
	pq = alloc_priorityqueue(cost_comparator);
	priorityqueue_enqueue(pq, origin);
	/* dirty way of checking if the queue is empty */
	while (priorityqueue_peek(pq) != NULL) {
		double new_distance;
		
		process = priorityqueue_dequeue(pq);
		process->visited = 1;
		
		if (process == end) {
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
				priorityqueue_enqueue(pq, neighbor);
				new_distance += process->cost;
				
				/* hint to update priority */
				if (new_distance < neighbor->cost) {
					neighbor->cost = new_distance;
					neighbor->predecessor = process;
					priorityqueue_update(pq, neighbor);
				}
			}
		}
	}
	dealloc_priorityqueue(pq);
	return cheapest;
}
/** END **/