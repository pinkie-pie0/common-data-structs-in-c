#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashmap.h"
#include "priorityqueue.h"
#include "graph.h"
#include "deque.h"

char letters_array[26];
char *alphabet = letters_array - 'A';
hashmap_T *map = NULL;
priorityqueue_T *pqueue = NULL;
graph_T *graph = NULL;
deque_T *deque = NULL;

int char_comparator(const void *a, const void *b) {
	char a_ = *(char*)a;
	char b_ = *(char*)b;
	return (a_ > b_) - (a_ < b_);
}

int char_equals(const void *a, const void *b) {
	return *(char*)a == *(char*)b;
}

int char_hash(const void *c) {
	return *(char*)c;
}

void test_deque(void);
void test_hashmap(void);
void test_pqueue(void);
void test_graph(void);

#define init_alphabet() \
	do { \
		int i = 0; \
		for (i = 0; i < 26; i++) letters_array[i] = i + 'A'; \
	} while (0)

#define alloc_ds() \
	do { \
		map = alloc_hashmap(char_hash, char_equals); \
		pqueue = alloc_priorityqueue(char_comparator); \
		graph = alloc_graph(char_hash, char_equals); \
		deque = alloc_deque(); \
	} while (0)

#define free_ds() \
	do { \
		dealloc_hashmap(map); map = NULL; \
		dealloc_priorityqueue(pqueue); pqueue = NULL; \
		dealloc_graph(graph); graph = NULL; \
		dealloc_deque(deque); deque = NULL; \
	} while (0)

/* Potential source of segmentation fault (in order of calls):
 * This segfault only happens sometimes. to replicate it I ran "./untilFail ./driver" which repeatedly runs the program until a non-zero exit occurs.
 *
 * file: driver.c         line 074: test_graph();
 * file: driver.c         line 210: print_shortest_path('B', 'A');
 * file: driver.c         line 175: graph_cheapest_path(graph, &alphabet[from], &alphabet[to], deque);
 * file: graph.c          line 333: process = priorityqueue_dequeue(pq);
 * file: priorityqueue.c  line 136: *(int*)hashmap_get(this->indexmap, this->heap[0]) = 0;
 */
int main(void) {
	/* deque_T *BFS; */
	srand(time(NULL));
	init_alphabet();
	alloc_ds();
	test_deque();
	test_hashmap();
	test_pqueue();
	test_graph();
	/*
	BFS = graph_breadth_first_search(graph, &alphabet['A']);
	puts("breadth first search incoming");
	while (!deque_isempty(BFS)) {
		putchar(*(char*)deque_popleft(BFS));
		if (!deque_isempty(BFS)) printf(", ");
	}
	
	putchar('\n');
	dealloc_deque(BFS);
	*/
	free_ds();
	return 0;
}

void test_deque(void) {
	int i;
	printf("=== TESTING DEQUE === \n");

	/* Stack Section */
	printf("Using as a stack...\n");
	
	for (i = 'A'; i <= 'G'; i++) {
		printf("pushed '%c'\n", i);
		deque_push(deque, &alphabet[i]);
	}
	printf("top of the stack: %c\n", *(char*)deque_stackpeek(deque));

	while(!deque_isempty(deque)) {
		printf("popped '%c'\n", *(char*)deque_pop(deque));
	}
	
	/* Queue Section */
	printf("\nUsing as a queue...\n");
	for (i = 'A'; i <= 'G'; i++) {
		printf("enqueued '%c'\n", i);
		deque_enqueue(deque, &alphabet[i]);
	}
	printf("front of the queue: %c\n", *(char*)deque_queuepeek(deque));
	
	while(!deque_isempty(deque)) {
		printf("dequeued '%c'\n", *(char*)deque_dequeue(deque));
	}
	
	printf("=== TESTING DONE  === \n\n");
}

void test_hashmap(void) {
	int i;
	char strings[][14] = {"mapped from A", "mapped from B", "mapped from C", "mapped from X", "mapped from Y", "mapped from Z"};
	
	printf("=== TESTING HASHMAP === \n");
	
	/* Putting into hashmap */
	for (i = 'A'; i <= 'C'; i++) {
		printf("putting %c\n", i);
		hashmap_put(map, &alphabet[i], strings[i-'A']);
	}
	
	for (i = 'X'; i <= 'Z'; i++) {
		printf("putting %c\n", i);
		hashmap_put(map, &alphabet[i], strings[i-'X'+3]);
	}
	
	/* Getting from hashmap */
	printf("\nNow retrieving values...\n");
	for (i = 'A'; i <= 'C'; i++) {
		printf("Key: %c, Value: [%s]\n", i, (char*)hashmap_get(map, &alphabet[i]));
	}
	for (i = 'X'; i <= 'Z'; i++) {
		printf("Key: %c, Value: [%s]\n", i, (char*)hashmap_get(map, &alphabet[i]));
	}
	
	/* Removing a key from hashmap */
	hashmap_remove(map, &alphabet['A']);
	printf("Removed the key 'A'\n");
	printf("Attempting to get with the removed key: [%s]\n", (char*)hashmap_get(map, &alphabet['A']));
	printf("=== TESTING DONE  === \n\n");
}

void test_pqueue(void) {
	int letter, i = 10;
	printf("=== TESTING PRIORITY QUEUE === \n");
	printf("note: priority queue ignores duplicate keys\n");
	
	/* Enqueuing random letters */
	while (i-- > 0) {
		letter = (rand() % 25) + 'A';
		priorityqueue_enqueue(pqueue, &alphabet[letter]);
		printf("enqueued '%c'\n", letter);
	}
	printf("front of the queue: %c\n", *(char*)priorityqueue_peek(pqueue));
	
	while(priorityqueue_peek(pqueue) != NULL) {
		printf("dequeued '%c'\n", *(char*)priorityqueue_dequeue(pqueue));
	}
	printf("=== TESTING DONE  === \n\n");
}

void print_shortest_path(int from, int to) {
	double cost = graph_cheapest_path(graph, &alphabet[from], &alphabet[to], deque);
	printf("shortest path from %c to %c: [", from, to);
	while (!deque_isempty(deque)) {
		putchar(*(char*)deque_pop(deque));
		if (!deque_isempty(deque)) printf(", ");
	}
	printf("]\t\t%scost: %.2f\n", cost ? "" : "\t", cost);
}

void test_graph(void) {
	int i, j;
	printf("=== TESTING UNDIRECTED GRAPH === \n");
	/* example graph taken from https://www.youtube.com/watch?v=pVfj6mxhdMw */
	printf("   6\n"
		" A-----B\n"
		" |    /|\\ 5\n"
		" | 2 / | \\\n"
		"1|  /  |  C\n"
		" | /  2| /\n"
		" |/    |/ 5\n"
		" D-----E\n"
		"    1\n");
	for (i = 'A'; i <= 'E'; i++) {
		graph_add_vertex(graph, &alphabet[i]);
	}
	
	graph_add_edge(graph, &alphabet['A'], &alphabet['B'], 6);
	graph_add_edge(graph, &alphabet['A'], &alphabet['D'], 1);
	graph_add_edge(graph, &alphabet['D'], &alphabet['B'], 2);
	graph_add_edge(graph, &alphabet['D'], &alphabet['E'], 1);
	graph_add_edge(graph, &alphabet['E'], &alphabet['B'], 2);
	graph_add_edge(graph, &alphabet['E'], &alphabet['C'], 5);
	graph_add_edge(graph, &alphabet['B'], &alphabet['C'], 5);
	
	/* source of segmentation fault (that happens sometimes...) */
	print_shortest_path('B', 'A');
	/* ORIGINAL TEST:
	for (i = 'A'; i <= 'E'; i++) {
		for (j = 'A'; j <= 'E'; j++) {
			print_shortest_path(i, j);
		}
		printf("\n");
	}
	*/
	printf("=== TESTING DONE  === \n\n");
}