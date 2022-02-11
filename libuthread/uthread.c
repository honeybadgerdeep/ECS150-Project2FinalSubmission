#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

queue_t readyQueue = NULL;
queue_t zombieQueue = NULL;
TCB* currentThread = NULL;
int numTIDs = 0;

TCB* newTCB(int TID) {
    TCB* tcb = malloc(sizeof(TCB));
    
    // Error allocating memory for tcb struct.
    if (tcb == NULL) {
        return NULL;
    }
    
    tcb->TID = TID;
    uthread_ctx_t* ctx = malloc(sizeof(uthread_ctx_t));

    // Error issuing context.
    if (ctx == NULL) {
        return NULL;
    }

    tcb->context = ctx;

    // TCB status BLOCKED by default, to be queued.
    tcb->status = BLOCKED;

    void* stack = uthread_ctx_alloc_stack();

    // Error issuing stack for thread.
    if (stack == NULL) {
        return NULL;
    }

    tcb->stack = stack;

    tcb->joinedToThread = NULL;

    return tcb;
}

void destroyTCB(TCB* tcb) {
	// If the TCB is nonexistent, do not proceed.
	if (tcb == NULL) {
		return;
	}

	// Free any active struct attributes.
	if (tcb->stack) {
		free(tcb->stack);
	}

	if (tcb->context) {
		free(tcb->context);
	}

	// Free TCB struct.
	free(tcb);
}


int uthread_start(int preempt)
{
	if (preempt == 1) {
		preempt_start();
	}

	readyQueue = queue_create();
	zombieQueue = queue_create();
	currentThread = NULL;
	numTIDs = 0;
	
	TCB* mainThread = newTCB(0);

	// Context of the thread should be the current running process.

	if (mainThread == NULL) {
		return -1;
	}

	mainThread->status = RUNNING;
	currentThread = mainThread;

	return 0;
}

int uthread_stop(void)
{
	// If uthread_stop not called by the main thread
	if (currentThread->TID != 0)  {
		return -1;
	}

	// There are more threads to be run in the queue.
	if (queue_length(readyQueue) > 0) {
		return -1;
	} else {
		// Ready queue is empty and can be destroyed.
		queue_destroy(readyQueue);

		// Check zombie queue for any uncollected dead threads.
		while (queue_length(zombieQueue) > 0) {
			TCB* zombie = NULL;
			queue_dequeue(zombieQueue, (void**)&zombie);
			destroyTCB(zombie);			
		}

		queue_destroy(zombieQueue);
		return 0;
	}
	return -1;
}

int uthread_create(uthread_func_t func)
{
	// At maximum thread capacity
	if (numTIDs == USHRT_MAX) {
		return -1;	
	}

	numTIDs++;

	TCB* newThread = newTCB(numTIDs);

	if (newThread == NULL) {
		return -1;
	}

	// Initialize the thread with a function
	int initStatus = uthread_ctx_init(newThread->context, newThread->stack, func);

	/** 
	 * Enqueue the new thread or return an error code, 
	 * based on success of uthread_ctx_init.
	 */
	if (initStatus == 0) {
		newThread->status = READY;
		queue_enqueue(readyQueue, newThread);
		return newThread->TID;
	} else {
		return -1;
	}
}

uthread_t uthread_self(void)
{
	// No thread currently running
	if (currentThread == NULL) {
		return 0;
	}

	return currentThread->TID;
}

void uthread_yield(void)
{
	TCB* next = NULL;
	queue_dequeue(readyQueue, (void**)&next);

	// Dequeue did not sufficiently work
	if (next == NULL) {
		return;
	}

	uthread_ctx_t* from = currentThread->context;
	uthread_ctx_t* to = next->context;
	
	currentThread->status = READY;

	next->status = RUNNING;

	queue_enqueue(readyQueue, currentThread);

	currentThread = next;

	uthread_ctx_switch(from, to);
}

// T1.join(T2, NULL);
// T2
void uthread_exit(int retval)
{
	// retval is a function which returns a value of int
	// that integer needs to saved into retval
	currentThread->status = DEAD;

	// Save the return value
	currentThread->retVal = retval;

	if (currentThread->joinedToThread != NULL) {
		// Switch from BLOCKED to READY
		currentThread->joinedToThread->status = READY;
		queue_enqueue(readyQueue, currentThread->joinedToThread);
	} else {
		queue_enqueue(zombieQueue, currentThread);
	}

	uthread_yield();
}

int findThread(queue_t q, void* thr, void* tid) {
	TCB* thread = (TCB*)thr;
	uthread_t match = (uthread_t)(long)tid;
	(void)q;

	if (thread->TID == match) {
		return 1;
	}

	return 0;
}

int uthread_join(uthread_t tid, int *retval)
{
	// Thread cannot join main thread or itself.
	if (tid == 0 || tid == currentThread->TID) {
		return -1;
	}

	TCB* searchThread = NULL;

	queue_iterate(zombieQueue, findThread, (void*)(long)tid, (void**)&searchThread);

	if (searchThread != NULL) {

		// Target thread @tid already joined.
		if (searchThread->joinedToThread != NULL) {
			return -1;				
		}

		// Store return value of zombie thread if needed.
		if (retval != NULL) {
			*retval = searchThread->retVal;
		}

		queue_delete(zombieQueue, searchThread);
		destroyTCB(searchThread);
		return 0;
	}

	queue_iterate(readyQueue, findThread, (void*)(long)tid, (void**)&searchThread);
	//return queue_length(readyQueue);

	if (searchThread != NULL) {

		// Target thread @tid already joined.
		if (searchThread->joinedToThread != NULL) {
			return -1;				
		}

		currentThread->status = BLOCKED;
		// Allocate sufficient space to store address to current TCB.
		searchThread->joinedToThread = malloc(sizeof(TCB));
		searchThread->joinedToThread = currentThread;

		/* yield() without putting current thread back into queue as it is blocked */

		TCB* next = NULL;
        	queue_dequeue(readyQueue, (void**)&next);

        	// Dequeue did not sufficiently work
        	if (next == NULL) {
                	return -1;
        	}

        	uthread_ctx_t* from = currentThread->context;
        	uthread_ctx_t* to = next->context;

        	currentThread->status = BLOCKED;

        	next->status = RUNNING;

        	currentThread = next;

        	uthread_ctx_switch(from, to);

		// Store the return value of the joined thread if needed.
		if (retval != NULL) {
			*retval = searchThread->retVal;
		}

		destroyTCB(searchThread);
		return 0;
	}

	// Thread @tid cannot be found.
	return -1;
}