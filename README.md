# common-data-structs-in-c
Self-explanatory, these are commonly used data structures implemented in C. All of this is done for practice.

### data structures implemented
* avltree
* deque
  * uses a circular dynamic array.
* graph
  * uses a nested hashmap akin to unordered_map<vertex, unordered_map<vertex, double>> as adjaceny list.
* hashmap
  * uses robin-hood hashing. (lookup could probably be improved?)
* pqueue
  * uses a 4-ary heap.
  * this should really just be called pset instead since duplicate items aren't allowed.

## how to compile
Just type `make`, this will generate an executable called `driver` that tests the following data structures.
