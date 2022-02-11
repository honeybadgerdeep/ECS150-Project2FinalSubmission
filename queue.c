#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/**
 * @brief queue - Struct representing queue data structure
 * 
 * queueNode front: 	Front of the queue
 * queueNode back: 		Back of the queue
 * unsigned int length:	Length of the queue 
 */
struct queue {
	queueNode front;
	queueNode back;
	unsigned int length;
} queue;

 /* @brief queue_node - Struct representing queue data structure
 * 
 * queueNode nextNode: 	Next node on the queue
 * void* value: 		Value stored by the queue
 */
struct queue_node {
	queueNode nextNode;
	void* value;
} queue_node;

queue_t queue_create(void)
{
	// Attempt to allocate space for a queue struct
	queue_t queue = malloc(sizeof(queue));
	
	// Malloc failed
	if (queue == NULL) {
		return NULL;
	}

	// Initialize all pointers to null. Length starts at 0.
	queue->front = NULL;
	queue->back = NULL;
	queue->length = 0;

	return queue;
}

/***
 * Destroy queue struct and free any allocated memory.
 * @brief Destroy queue.
 * 
 * @param queue pointer to queue struct is being destroyed.
*/
int queue_destroy(queue_t queue)
{
	// If the queue is NULL or empty, return -1.
	if (queue == NULL) {
		return -1;
	}

	if (queue_length(queue) == 0) {
		return -1;
	}

	// Queue is empty, so simply free() pointer to struct.
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{	
	// If queue or data are NULL, return -1.
	if (queue == NULL) {
		return -1;
	}

	if (data == NULL) {
		return -1;
	}

	// Attempt to allocate space for a new node element.
	queueNode newElement = malloc(sizeof(queueNode));

	// Malloc error
	if (newElement == NULL) {
		return -1;
	}

	// A freshly new element has no known next node.
	newElement->nextNode = NULL;	

	// Value stored by the node is the data to be enqueued.
	newElement->value = data;

	if (queue_length(queue) == 0) {
		// If the node length is 0, 
		// then the new element is both the back and front
		queue->front = newElement;
		queue->back = newElement;
	} else {
		// newElement is the new back to this queue
		queueNode temp = queue->back;
		temp->nextNode = newElement;
		queue->back = newElement;
	}

	queue->length++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	// If the queue or data are NULL or the queue is empty, return -1
	if (queue == NULL) {
		return -1;
	}

	if (data == NULL) {
		return -1;
	}

	if (queue_length(queue) == 0) {
		return -1;
	}

	// Store current queue->front value and save to data
	queueNode toDequeue = queue->front;
	*data = toDequeue->value;

	// Reassign front to the next node of the previous front node
	queue->front = toDequeue->nextNode;

	// If this was the last element in the queue, 
	// the queue has no more back.	
	if (queue_length(queue) == 1) {
		queue->back = NULL;
	}

	queue->length--;
	
	// Free dequeued element
	free(toDequeue);

	return 0;
}

int queue_delete(queue_t queue, void *data)
{		
	// If the queue or data are NULL, return -1.
	if (queue == NULL) {
		return -1;
	}

	if (data == NULL) {
		return -1;
	}

	// Loop through queue, from front->->back.
	queueNode node = queue->front;
	queueNode prevNode = NULL;
	while (node != NULL) {
		// If there is a match
		if (node->value == data) {
			if (node->nextNode != NULL) {
				// If there is a node after the found node, 
				// it must be re-linked to the queue upon
				// removal of the matching node
				if (prevNode != NULL) {
					prevNode->nextNode = node->nextNode;
				} else {
					queue->front = node->nextNode;
				}
			} else {
				// If there is a node before the found node,
				// it must be re-linked to the rest of the 
				// queue upon removal of the matching node
				if (prevNode != NULL) {
					prevNode->nextNode = NULL;
					queue->back = prevNode;
				} else {
					queue->back = NULL;
					queue->front = NULL;
				}
			}
			free(node);
			return 0;
		}
		// Iterate
		prevNode = node;
		node = node->nextNode;
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	// If queue or data are NULL, return -1
	if (queue == NULL) {
		return -1;
	}

	if (data == NULL) {
		return -1;
	}

	// Loop through node from front->->back
	queueNode node = queue->front;

	while (node != NULL) {
		// Attempt to run the function and collect the return value.
		int returnSignal = func(queue, node->value, arg);

		// If the function returns one, the loop stops, 
		// and the struct element's pertinent value is stored
		if (returnSignal == 1) {
			*data = node->value;
			return 0;
		}

		// Iterate
		node = node->nextNode;
	}

	return 0;
}

int queue_length(queue_t queue)
{
	// If the queue is NULL, return -1
	if (queue == NULL) {
		return -1;
	}
	// Return the length property of the queue struct
	return queue->length;
}

