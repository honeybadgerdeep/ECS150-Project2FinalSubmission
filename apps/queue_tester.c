#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

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

/* Enqueue/Dequeue modified */
void multiple_enqueue_dequeue_test(void)
{
	int data1 = 4, *ptr1;
	int data2 = 8, *ptr2;
	queue_t q;

	fprintf(stderr, "*** TEST multiple_enqueue_dequeue_test ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_dequeue(q, (void**)&ptr1);
	queue_enqueue(q, &data2);
	queue_dequeue(q, (void**)&ptr2);
	TEST_ASSERT((ptr1 == &data1) & (ptr2 == &data2));
}

/* Destroy */
void queue_destroy_empty_test(void)
{
	int data = 7, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_destroy_empty_test ***\n");

	q = queue_create();

	TEST_ASSERT(queue_destroy(q) == 0);
}

/* Destroy */
void queue_destroy_not_empty_test(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_destroy ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1);
}

/* Destroy Null */
void queue_destroy_null(void)
{
	queue_t q = NULL;

	fprintf(stderr, "*** TEST queue_destroy_null ***\n");

	TEST_ASSERT(queue_destroy(q) == -1);
}

/* Enqueue Null */
void queue_enqueue_null(void)
{
	int data = NULL, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue_null ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(ptr == -1);
}

/* Dequeue Null */
void queue_dequeue_null(void)
{
	int *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_dequeue_null ***\n");

	q = queue_create();
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == -1);
}

/* Queue Delete */
void queue_delete_test(void)
{
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	queue_delete(q, (void*)2);
	TEST_ASSERT(queue_length(q) == 2);
}

/* Queue Delete Not Found */
void queue_delete_notfound(void)
{
	int data1 = 1;
	int data2 = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_notfound ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	int deleteAttemptResult = queue_delete(q, (void*)0);
	TEST_ASSERT(deleteAttemptResult == -1);
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
	fprintf(stderr, "*** TEST queue_delete_notfound ***\n");
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 52, 6, 7, 8, 9};
	size_t i;
	int* ptr = NULL;

	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
			queue_enqueue(q, &data[i]);
	}

	TEST_ASSERT(queue_length(q) == 10);
	queue_iterate(q, find_item, (void*)5, (void**)&ptr);
	
	TEST_ASSERT(ptr != NULL);
	TEST_ASSERT(*ptr == 5);
	TEST_ASSERT(ptr == &data[4]);
}

void erroneous_iteration(void)
{
	fprintf(stderr, "*** TEST erroneous_iteration ***\n");

	queue_t q = queue_create();

	int data = 6;
	int* ptr = NULL;

	queue_enqueue(q, &data);

	int iterationAttempt = queue_iterate(q, NULL, (void*)6, (void**)&ptr);

	TEST_ASSERT(iterationAttempt == -1);
}

void queue_length_check(void)
{
	fprintf(stderr, "*** TEST queue_length_check ***\n");	
	queue_t q = queue_create();

	int data[] = {3, 6, 7, 1 , 9, 0, -4, -5, 12};
	
	for (int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		queue_enqueue(q, &data[i]);
	}

	TEST_ASSERT(queue_length(q) == sizeof(data) / sizeof(data[0]));
}

int main(void)
{
	test_create();
	test_queue_simple();
	multiple_enqueue_dequeue_test();
	queue_destroy_empty_test();
	queue_destroy_not_empty_test();
	queue_destroy_null();
	queue_delete_test();
	queue_delete_notfound();
	test_iteration();
	erroneous_iteration();
	queue_length_check();
	
	return 0;
}
