#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"

#define DS_NAME "pqueue"
#include "err/ds_assert.h"

typedef struct pqueue_ds {
	int (*compare)(const void*, const void*);
	size_t size, capacity;
	hashmap_ds *indexmap;
	void **heap;
} pqueue_ds;

pqueue_ds *alloc_pqueue(int comparator(const void*,const void*)) {
	pqueue_ds *this = malloc(sizeof *this);
	DS_ASSERT(this != NULL, "failed to allocate memory for new " DS_NAME);
	
	this->size = 0;
	this->capacity = 16;
	this->compare = comparator;
	this->heap = malloc(16 * sizeof *this->heap);
	DS_ASSERT(this->heap != NULL, "failed to allocate the heap");
	
	this->indexmap = alloc_identityhashmap();
	return this;
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

static void reheapify_up(pqueue_ds *const this, size_t initial) {
	void *temp;
	size_t parent = (initial - 1) / 4;
	while (initial != 0 && this->compare(this->heap[initial], this->heap[parent]) < 0) {
		*(size_t*)hashmap_get(this->indexmap, this->heap[initial]) = parent;
		*(size_t*)hashmap_get(this->indexmap, this->heap[parent]) = initial;
		
		temp = this->heap[parent];
		this->heap[parent] = this->heap[initial];
		this->heap[initial] = temp;
		
		initial = parent;
		parent = (parent - 1) / 4;
	}
}

static void reheapify_down(pqueue_ds *const this, size_t initial) {
	void *temp;
	size_t i, child, smallest = initial;
	while(initial < this->size) {
		/* go through all children of the parent (initial) to determine if there's a smallest child */
		for (i = 1; i <= 4; i++) {
			child = (4 * initial) + i;
			if (child < this->size && this->compare(this->heap[child], this->heap[smallest]) < 0) {
				smallest = child;
			}
		}

		if (smallest != initial) {
			*(size_t*)hashmap_get(this->indexmap, this->heap[initial]) = smallest;
			*(size_t*)hashmap_get(this->indexmap, this->heap[smallest]) = initial;
			
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
	size_t *newindex;
	
	/* element already exists in priority queue */
	if (hashmap_get(this->indexmap, element) != NULL) return 0;
	
	if (this->size >= this->capacity) {
		size_t new_capacity = this->capacity << 1;
		this->heap = realloc(this->heap, new_capacity * sizeof *this->heap);
		DS_ASSERT(this->heap != NULL, "failed to expand the heap");

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
		*(size_t*)hashmap_get(this->indexmap, this->heap[0]) = 0;
		this->heap[this->size] = NULL;
		reheapify_down(this, 0);
	}
	return oldval;
}

void pqueue_update(pqueue_ds *const this, void *element) {
	size_t index = *(size_t*)hashmap_get(this->indexmap, element);
	size_t parent = (index - 1) / 4;
	if (index > 0 && this->compare(this->heap[index], this->heap[parent]) < 0) {
		reheapify_up(this, index);
	} else {
		reheapify_down(this, index);
	}
}

void *pqueue_remove(pqueue_ds *const this, void *element) {
	/* remove element-index mapping from the hashmap first */
	size_t last, index = *(size_t*)hashmap_get(this->indexmap, element);
	free(hashmap_get(this->indexmap, element));
	hashmap_remove(this->indexmap, element);
	
	/* swap with the last item in the heap, and update its element-index mapping */
	last = --this->size;
	
	/* if the item we're removing isn't the last item in the heap, then a swap and reheapification is needed */
	if (index != last) {
		this->heap[index] = this->heap[last];
		this->heap[this->size] = NULL;
		*(size_t*)hashmap_get(this->indexmap, this->heap[index]) = index;
	
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
