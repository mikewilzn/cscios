# CSCI 380: Operating Systems

## Coins

Modify `Coins.c` to support half dollars, coins worth 50 cents.

* Add a new member to the struct
* Output "penny" if only one is needed. Use a `const char *` variable and conditional to do this.
* Test various inputs to ensure your logic is correct.


## Queue

Your task is to modify the code in queue.h and queue.c to fully implement the following functions.

* `q_new`: Create a new, empty queue.
* `q_free`: Free all storage used by a queue.
* `q_insert_head`: Attempt to insert a new element at the head of the queue (LIFO discipline).
* `q_insert_tail`: Attempt to insert a new element at the tail of the queue (FIFO discipline).
* `q_remove_head`: Attempt to remove the element at the head of the queue.
* `q_size`: Compute the number of elements in the queue.
* `q_reverse`: Reorder the list so that the queue elements are reversed in order.


## Prelude

* Create a character array of size 80 for the string buffer. Declare the array size as a CONSTANT.
* Use `fork` to create a new process. See the `fork` example and the `man` page (i.e., `man 2 fork`).
* Read the string ensuring you do NOT allow a buffer overflow. Investigate `fgets`, `strtok`, and `strcspn`.
* Use `getpid` to obtain a PID. You may also find `getppid` helpful. Consult the man pages.
* Include header file `<unistd.h>`.

#### Output Format
```
shell> Hi there!
Parent says: PID = 1547, child PID = 1548, parent PID = 1237
Child says:  PID = 1548, parent PID = 1547, 'Hi there!'
```
