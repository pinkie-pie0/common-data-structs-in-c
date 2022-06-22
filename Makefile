CC = gcc
TARGETS = driver
DEPENDENCIES = graph.c priorityqueue.c hashmap.c deque.c
CFLAGS = -Wall -std=c89 -pedantic-errors -g -Os

all: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) $(CFLAGS) $(DEPENDENCIES) -o $@ $<