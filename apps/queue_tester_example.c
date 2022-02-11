#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(ptr == &data);
}

int find_item(queue_t q, void *data, void *arg)
{
	int *a = (int*)data;
	int match = (int)(long)arg;
	(void)q;

	if (*a == match) {
		return 1;
	}

	return 0;
}

/* Iteration: Find an element that matches */
void test_iteration(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 52, 6, 7, 8, 9};
	size_t i;
	int* ptr = NULL;

	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		queue_enqueue(q, &data[i]);
	}

	TEST_ASSERT(queue_length(q) == 10);
	/*
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		int* pointer = NULL;
		queue_dequeue(q, (void**)&pointer);
		//printf("%d\n", *pointer);	
	}
	*/

	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	//TEST_ASSERT(queue_length(q) == 0);
	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(*ptr == 5);
	TEST_ASSERT(ptr == &data[4]);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_iteration();
	return 0;
}
