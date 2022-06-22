# common-data-structs-in-c
Self-explanatory, these are commonly used data structures implemented in C. All of this is done for practice.

### things to note
* Hashmap implementation isn't complete; no way to rehash or remove key-value pairs.
* Priority queue really should just be called priority set instead since duplicate items aren't allowed.
* Dijkstra's algorithm sometimes segfault for reasons idk why. More information about this in driver.c's comments.
  * The coredump file is provided.

## how-to
Just type `make`, this will generate an executable called `driver` that tests the following data structures.
