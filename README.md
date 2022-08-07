# common-data-structs-in-c
Self-explanatory, these are commonly used data structures implemented in C. All of this is done for practice.

### things to note
* ~~Hashmap implementation isn't complete; no way to rehash or remove key-value pairs.~~ Lookup could be improved to benefit basic robin hood hashing, but I'm clueless on that as of now.
* Priority queue really should just be called priority set instead since duplicate items aren't allowed.
  * Priority queue uses a 4-ary heap instead of a 2-ary heap.

## how-to
Just type `make`, this will generate an executable called `driver` that tests the following data structures.
