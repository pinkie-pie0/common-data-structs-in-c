#include <stdio.h>
#include <stdlib.h>

#define DS_NAME "deque"
#include "err/ds_assert.h"
#include "deque.h"

struct deque_ds {
	size_t head;
	size_t tail;
	size_t len;
	size_t capacity;
	void **deque;
};

deque_ds *alloc_deque(void) {
	deque_ds *this = malloc(sizeof *this);
	DS_ASSERT(this != NULL, "failed to allocate memory for new " DS_NAME);
	
	this->head = 0;
	this->tail = 0;
	this->len = 0;
	this->capacity = 16;
	this->deque = malloc(16 * sizeof *this->deque);
	return this;
}

void dealloc_deque(deque_ds *const this) {
	free(this->deque);
	free(this);
}

static void deque_resize(deque_ds *const this) {
	size_t i_1, i_2, len = this->len, old_capacity = this->capacity;
	
	void **new_deque, **old_deque = this->deque;
	this->capacity <<= 1;
	new_deque = malloc(this->capacity * sizeof *new_deque);
	DS_ASSERT(new_deque != NULL, "failed to allocate memory for expanding the " DS_NAME);
	
	for (i_1 = this->head, i_2 = 0; i_2 < len; i_1 = (i_1 + 1) & (old_capacity - 1), i_2++) {
		new_deque[i_2] = old_deque[i_1];
	}
	
	free(old_deque);
	this->deque = new_deque;
	this->head = 0;
	this->tail = old_capacity;
}

void deque_pushleft(deque_ds *const this, void *el) {
	if (this->len == this->capacity) deque_resize(this);
	this->head = (this->head - 1) & (this->capacity - 1);
	this->deque[this->head] = el;
	this->len++;
}

/* analogous to enqueue and stack push */
void deque_pushright(deque_ds *const this, void *el) {
	if (this->len == this->capacity) deque_resize(this);
	this->deque[this->tail] = el;
	this->tail = (this->tail + 1) & (this->capacity - 1);
	this->len++;
}

/* analogous to dequeue */
void *deque_popleft(deque_ds *const this) {
	void *el;
	if (this->len == 0) return NULL;
	el = this->deque[this->head];
	this->deque[this->head] = NULL;
	this->head = (this->head + 1) & (this->capacity - 1);
	this->len--;
	return el;
}

/* analogus to stack pop */
void *deque_popright(deque_ds *const this) {
	void *el;
	if (this->len == 0) return NULL;
	this->tail = (this->tail - 1) & (this->capacity - 1);
	el = this->deque[this->tail];
	this->deque[this->tail] = NULL;
	this->len--;
	return el;
}

void *deque_peekleft(deque_ds *const this) {
	return this->deque[this->head];
}

void *deque_peekright(deque_ds *const this) {
	return this->deque[(this->tail - 1) & (this->capacity - 1)];
}

int deque_isempty(deque_ds *const this) {
	return this->len == 0;
}
