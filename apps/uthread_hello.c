/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int hello(void)
{
	printf("Hello world in thread %d!\n", uthread_self());
	uthread_exit(1);
	return 1;
}

int main(void)
{
	uthread_t tid;

	uthread_start(0);
	tid = uthread_create(hello);
	int* returnStatus = NULL; 
	uthread_join(tid, returnStatus);
       	printf("Current thread: %d\n", uthread_self());
	uthread_stop();
	if (returnStatus != NULL) {
		printf("Return status is: %d\n", *returnStatus);	
	} else {
		printf("Error!\n");
		return -1;
	}

	return 0;
}
