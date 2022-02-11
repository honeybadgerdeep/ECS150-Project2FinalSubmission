# ECS 150 WQ 2022 - Project 2

## Background

In this project, we learned about creating and scheduling new threads, providing a synchronization mechanism for threads to join other threads, and being preemptive.

The implementation of this program follows four distinct steps:

1. Phase 1 - Queue API
2. Phase 2 - uthread API
3. Phase 3 - uthread_join()
3. Phase 4 - preemption

To implement the thread management, we had to be informed on the queue implementation. In this case, that meant learning how queue worked and how they are programmatically applied in C.

In addition to knowing about queue implementation, we had to understand how multiple threads can run within the same process. This meant going over the concepts of how new threads are created and terminated and how we can perform context switching during concurrent execution. 

Given the threads, a robust program was required to create and run independent threads concurrently. 
This required some substantial research and knowledge in memory allocation, thread scheduling, and functions in the C-language. 

This project encouraged us to write high-quality C programs to make the collaboration in the development process easier. Additionally, it motivated us to make our code understandable to others.

## Implementation

### Queue Implementation: 

This portion of our solution took some time to get right. Initially, we employed a dynamic array to store the queue. However, resizing this array repeatedly
proved to be a challenge, and we decided to use a linked list of pointers to represent the queue instead.

The queue was therefore represented by a struct which stored pointers to the front and back nodes on the queue list, and an integer keeps track of the size. 
queueNodes were constructed to keep track of individual nodes, storing a pointer to the next node and the value corresponding to the node's position on the 
linked-list.

During the implementation of each function in `queue.c`, we also kept in mind the given constraint that all operations, other than iterate and delete, must have a time complexity of O(1). 

To create a queue we allocate an empty queueNode using our function `queue_create()` and then use function `queue_enqueue()` and `queue_dequeue()` to insert and delete the data item. We do this by adding a data item at the back of the queue and deleting a data item from the front. In order to avoid errors related to only incrementing the front and back indices, we increment the front and the back indices in a circular manner.

Additionally, we implement `queue_delete()` function which deletes the data item in the queue. To implement this, we looped through non-empty queue to possibly find occurrence of data and to remove the corresponding queueNode from the linked-list, re-linking any surrounding nodes to keep the queue together.

Another important function in the queue implementation is `queue_iterate()` which helps us in iterating through our queue. For this function, we looped through 
the linked list, breaking the loop if the iterator ran into a function call returning a value of 1. 

### Thread Management Implementation: 

For our implementation of thread management, we understood `pthread.h` at a high level, implemented the API in pthread.c, implemented context switching, and implemented TCB struct for threads. 

We started off by creating two queues: `readyQueue` and `zombieQueue`. 
`readyQueue` represents the queue of TCB's that are "ready" to be run and `zombieQueue` represents the queue of dead TCB's.

Once the uthread library is initialized using `uthread_start()`, `uthread_create()` function initializes the thread and adds it to our `readyQueue`. 
In this function, we also referred to the `context.c` file to initialize the thread's execution context and based on this we enqueue the new thread or return an error code.

The thread calls on the `uthread_yield()` function to choose and execute the next available thread. We did this by performing a context switch using the `uthread_ctx_switch()` function from `context.c` file. 
Basically, we are setting the status of the current thread from running to ready and the status of the next thread from ready to running. 

Additionally we implemented `uthread_join()` in our thread management system to wait for the system to terminate the executing threads. 

For this function, we considered to following steps when implementing it:

1. T1 (first thread) is the current thread and is running
2. Join the two threads: T1 and T2
3. Find T2 thread and get information about it
4. If T2 is alive and ready, we put the TID of T1 into `joinedToThread` of T2 thread
5. If T2 dies then we set the status of T1 to blocked and we call yield(leaves)

In order to free library’s resources after all the user threads have been completed, our main function calls `uthread_stop()` by dequeueing the `readyQueue` and freeing the memory. 

## Testing

To test our program, we ran multiple tests on it. 

After implementing the first phase of this project, we ran the `queue_tester_example.c` tester to check if our program passes the basic tests. After multiple, segmention faults and memory allocation errors, we were able to get it to pass the tests. 

To test our phase 2, we ran the `uthread_hello` and `uthread_yield` test programs. Though we passed one of these tests, we were not able to pass the other one. Therefore, we had to go back to our queue implemenation to debug our code and fix the errors. 

Throughout the process of testing, we ran into multiple errors like segmentation
faults and wrong outputs which helped us see where our program was acting out at.

## References

Throughout this project, we consulted online resources.

These online resources were particulary noteworthy and helpful.

- <https://stackoverflow.com/questions/8233161/compare-int-and-unsigned-int>
- <https://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts>
- <https://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html>
- <https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions>
- <https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm>
- <https://www.gnu.org/software/libc/manual/html_mono/libc.html#Blocking-Signals>
