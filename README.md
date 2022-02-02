# CSCI 380: Operating Systems

### Coins

Modify `Coins.c` to support half dollars, coins worth 50 cents.

* Add a new member to the struct
* Output "penny" if only one is needed. Use a `const char *` variable and conditional to do this.
* Test various inputs to ensure your logic is correct.


### Queue

Your task is to modify the code in queue.h and queue.c to fully implement the following functions.

* `q_new`: Create a new, empty queue.
* `q_free`: Free all storage used by a queue.
* `q_insert_head`: Attempt to insert a new element at the head of the queue (LIFO discipline).
* `q_insert_tail`: Attempt to insert a new element at the tail of the queue (FIFO discipline).
* `q_remove_head`: Attempt to remove the element at the head of the queue.
* `q_size`: Compute the number of elements in the queue.
* `q_reverse`: Reorder the list so that the queue elements are reversed in order.
