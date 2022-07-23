#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

typedef struct pqueue_ds {
	int (*compare)(const void*, const void*);
	size_t size, capacity;
	hashmap_ds *indexmap;
	void **heap;
} pqueue_ds;

pqueue_ds *alloc_pqueue(int comparator(const void*,const void*)) {
	pqueue_ds *pq = malloc(sizeof *pq);
	if (pq == NULL) {
		fprintf(stderr, "**priorityqueue memory allocation failure** : failed to allocate new priorityqueue\n");
		exit(EXIT_FAILURE);
	}
	
	pq->size = 0;
	pq->capacity = 16;
	pq->compare = comparator;
	pq->heap = malloc(16 * sizeof *pq->heap);
	
	if (pq->heap == NULL) {
		free(pq);
		fprintf(stderr, "**priorityqueue memory allocation failure** : failed to allocate the heap\n");
		exit(EXIT_FAILURE);
	}
	
	pq->indexmap = alloc_identityhashmap();
	return pq;
}

void dealloc_pqueue(pqueue_ds *const this) {
	size_t i;
	hashmap_entry **index_mappings;
	
	free(this->heap);
	
	index_mappings = hashmap_getentries(this->indexmap);
	for (i = 0; index_mappings[i] != NULL; i++) {
		free(index_mappings[i]->value);
	}
	free(index_mappings);
	dealloc_hashmap(this->indexmap);
	
	free(this);
}

static void reheapify_up(pqueue_ds *const this, int initial) {
	void *temp;
	int parent = (initial - 1) / 4;
	while (parent >= 0 && this->compare(this->heap[initial], this->heap[parent]) < 0) {
		*(int*)hashmap_get(this->indexmap, this->heap[initial]) = parent;
		*(int*)hashmap_get(this->indexmap, this->heap[parent]) = initial;
		
		temp = this->heap[parent];
		this->heap[parent] = this->heap[initial];
		this->heap[initial] = temp;
		
		initial = parent;
		parent = (parent - 1) / 4;
	}
}

static void reheapify_down(pqueue_ds *const this, int initial) {
	void *temp;
	size_t size = this->size;
	while(1) {
		int i, child, smallest = initial;
		
		/* go through all children of the parent (initial) to determine if there's a smallest child */
		for (i = 1; i <= 4; i++) {
			child = (4 * initial) + i;
			if (child < size && this->compare(this->heap[child], this->heap[smallest]) < 0) {
				smallest = child;
			}
		}

		if (smallest != initial) {
			*(int*)hashmap_get(this->indexmap, this->heap[initial]) = smallest;
			*(int*)hashmap_get(this->indexmap, this->heap[smallest]) = initial;
			
			temp = this->heap[initial];
			this->heap[initial] = this->heap[smallest];
			this->heap[smallest] = temp;
			initial = smallest;
		} else {
			break;
		}
	}
}

int pqueue_enqueue(pqueue_ds *const this, void *element) {
	int *newindex;
	
	/* element already exists in priority queue */
	if (hashmap_get(this->indexmap, element) != NULL) return 0;
	
	if (this->size >= this->capacity) {
		size_t new_capacity = this->capacity * 2;
		void **oldheap = this->heap;
		this->heap = realloc(this->heap, new_capacity * sizeof *this->heap);
		if (this->heap == NULL) {
			free(oldheap);
			fprintf(stderr, "**priorityqueue memory allocation failure** : failed to expand the heap\n");
			exit(EXIT_FAILURE);
		}
		this->capacity = new_capacity;
	}
	this->heap[this->size++] = element;
	
	newindex = malloc(sizeof *newindex);
	*newindex = this->size-1;
	hashmap_put(this->indexmap, element, newindex);
	
	reheapify_up(this, *newindex);
	return 1;
}

void *pqueue_dequeue(pqueue_ds *const this) {
	void *oldval = NULL;
	if (this->size != 0) {
		oldval = this->heap[0];
		
		free(hashmap_get(this->indexmap, oldval));
		hashmap_remove(this->indexmap, oldval);
		
		if (this->size == 1) {
			this->heap[0] = NULL;
			this->size--;
			return oldval;
		}
		
		this->heap[0] = this->heap[--this->size];
		*(int*)hashmap_get(this->indexmap, this->heap[0]) = 0;
		this->heap[this->size] = NULL;
		reheapify_down(this, 0);
	}
	return oldval;
}

void pqueue_update(pqueue_ds *const this, void *element) {
	int index = *(int*)hashmap_get(this->indexmap, element);
	int parent = (index - 1) / 4;
	if (index > 0 && this->compare(this->heap[index], this->heap[parent]) < 0) {
		reheapify_up(this, index);
	} else {
		reheapify_down(this, index);
	}
}

void *pqueue_remove(pqueue_ds *const this, void *element) {
	/* remove element-index mapping from the hashmap first */
	int last, index = *(int*)hashmap_get(this->indexmap, element);
	free(hashmap_get(this->indexmap, element));
	hashmap_remove(this->indexmap, element);
	
	/* swap with the last item in the heap, and update its element-index mapping */
	last = --this->size;
	
	/* if the item we're removing isn't the last item in the heap, then a swap and reheapification is needed */
	if (index != last) {
		this->heap[index] = this->heap[last];
		this->heap[this->size] = NULL;
		*(int*)hashmap_get(this->indexmap, this->heap[index]) = index;
	
		/* call pqueue_update */
		pqueue_update(this, this->heap[index]);
	} else {
		this->heap[last] = NULL;
	}
	return element;
}

void *pqueue_peek(pqueue_ds *const this) {
	return this->size != 0 ? this->heap[0] : NULL;
}
