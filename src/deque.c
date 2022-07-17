#include <stdio.h>
#include <stdlib.h>

typedef struct deque_T {
	size_t head, tail, len, capacity;
	void **deque;
} deque_T;

deque_T *alloc_deque(void) {
	deque_T *deque = malloc(sizeof *deque);
	if (deque == NULL) {
		fprintf(stderr, "**deque memory allocation failure** : failed to allocate new deque\n");
		exit(EXIT_FAILURE);
	}
	deque->head = 0;
	deque->tail = 0;
	deque->len = 0;
	deque->capacity = 16;
	deque->deque = malloc(16 * sizeof *deque->deque);
	return deque;
}

void dealloc_deque(deque_T *this) {
	free(this->deque);
	free(this);
}

static void deque_resize(deque_T *this) {
	size_t i_1, i_2, len = this->len, old_capacity = this->capacity;
	
	void **new_deque, **old_deque = this->deque;
	this->capacity <<= 1;
	new_deque = malloc(this->capacity * sizeof *new_deque);
	
	if (new_deque == NULL) {
		dealloc_deque(this);
		fprintf(stderr, "**deque memory allocation failure** : failed to expand the deque\n");
		exit(EXIT_FAILURE);
	}
	
	for (i_1 = this->head, i_2 = 0; i_2 < len; i_1 = (i_1 + 1) & (old_capacity - 1), i_2++) {
		new_deque[i_2] = old_deque[i_1];
	}
	
	free(old_deque);
	this->deque = new_deque;
	this->head = 0;
	this->tail = old_capacity;
}

void deque_pushleft(deque_T *this, void *el) {
	if (this->len == this->capacity) deque_resize(this);
	this->head = (this->head - 1) & (this->capacity - 1);
	this->deque[this->head] = el;
	this->len++;
}

/* analogous to enqueue and stack push */
void deque_pushright(deque_T *this, void *el) {
	if (this->len == this->capacity) deque_resize(this);
	this->deque[this->tail] = el;
	this->tail = (this->tail + 1) & (this->capacity - 1);
	this->len++;
}

/* analogous to dequeue */
void *deque_popleft(deque_T *this) {
	void *el;
	if (this->len == 0) return NULL;
	el = this->deque[this->head];
	this->deque[this->head] = NULL;
	this->head = (this->head + 1) & (this->capacity - 1);
	this->len--;
	return el;
}

/* analogus to stack pop */
void *deque_popright(deque_T *this) {
	void *el;
	if (this->len == 0) return NULL;
	this->tail = (this->tail - 1) & (this->capacity - 1);
	el = this->deque[this->tail];
	this->deque[this->tail] = NULL;
	this->len--;
	return el;
}

void *deque_peekleft(deque_T *this) {
	return this->deque[this->head];
}

void *deque_peekright(deque_T *this) {
	return this->deque[(this->tail - 1) & (this->capacity - 1)];
}

int deque_isempty(deque_T *this) {
	return this->len == 0;
}