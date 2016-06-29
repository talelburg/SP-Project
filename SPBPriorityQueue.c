#include <stdlib.h>
#include <assert.h>
#include "SPBPriorityQueue.h"
#include "SPList.h"
#include "SPListElement.h"
#include <stdio.h>

struct sp_bp_queue_t {
	int maxSize;
	SPList queue;
	double max, min;
};

SPBPQueue spBPQueueCreate(int maxSize) {
	SPBPQueue temp = NULL;
	if (maxSize <= 0) { // Illegal argument
		return NULL;
	}
	temp = (SPBPQueue) malloc(sizeof(*temp));
	if (temp == NULL) { // Allocation failure
		return NULL;
	}
	temp->maxSize = maxSize;
	temp->max = -1.0;
	temp->min = -1.0;
	temp->queue = spListCreate();
	if (temp->queue == NULL) { // Allocation failure
		free(temp);
		return NULL;
	}
	return temp; // All went well
}

SPBPQueue spBPQueueCopy(SPBPQueue source) {
	SPBPQueue temp;
	SP_LIST_MSG feedback;
	if (source == NULL) { // Invalid argument
		return NULL;
	}
	temp = spBPQueueCreate(spBPQueueGetMaxSize(source));
	if (temp == NULL) { // Allocation failure
		return NULL;
	}
	if (spListGetSize(source->queue) == 0) { // If source's queue is empty we are done
		return temp;
	}
	SP_LIST_FOREACH(SPListElement, data, source->queue) { // Copy all elements
		feedback = spListInsertLast(temp->queue, data);
		if (feedback == SP_LIST_OUT_OF_MEMORY) { // Something went wrong while inserting some element
			spBPQueueDestroy(temp);
			return NULL;
		}
	}
	temp->max = spBPQueueMaxValue(source);
	temp->min = spBPQueueMinValue(source);
	return temp; // All went well
}

void spBPQueueDestroy(SPBPQueue source) {
	if (source != NULL) { // If source is NULL there's nothing to do
		spListDestroy(source->queue);
		free(source);
		source = NULL;
	}
}

void spBPQueueClear(SPBPQueue source) {
	if (source != NULL) { // If source is NULL there's nothing to do
		spListClear(source->queue);
		source->max = 0.0;
		source->min = 0.0;
	}
}

int spBPQueueSize(SPBPQueue source) {
	if (source == NULL) { // Invalid argument
		return -1;
	}
	return spListGetSize(source->queue);
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	if (source == NULL) { // Invalid argument
		return -1;
	}
	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element) {
	SPListElement copy, data;
	SP_LIST_MSG feedback;
	int i;
	if (source == NULL || element == NULL || source->queue == NULL) { // Invalid arguments
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	copy = spListElementCopy(element);
	if (copy == NULL) { // Allocation failure
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	data = spListGetFirst(source->queue);
	if (data == NULL) { // Queue is currently empty
		feedback = spListInsertFirst(source->queue, copy);
		if (feedback == SP_LIST_OUT_OF_MEMORY) { // Allocation failure
			spListElementDestroy(copy);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}
		// There is now one element in the queue
		source->min = spListElementGetValue(copy);
		source->max = spListElementGetValue(copy);
		spListElementDestroy(copy);
		return SP_BPQUEUE_SUCCESS;
	}
	while (data != NULL && spListElementCompare(data, copy) < 0) { // Find where to insert new element
		data = spListGetNext(source->queue);
	}
	if (data == NULL) { // New element is bigger than all elements
		if (spBPQueueIsFull(source)) { // Queue is full
			spListElementDestroy(copy);
			return SP_BPQUEUE_FULL;
		}
		feedback = spListInsertLast(source->queue, copy);
		if (feedback == SP_LIST_OUT_OF_MEMORY) { // allocation failure
			spListElementDestroy(copy);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}
		source->max = spListElementGetValue(copy); // Update maximum
		spListElementDestroy(copy);
		return SP_BPQUEUE_SUCCESS;
	}

	feedback = spListInsertBeforeCurrent(source->queue, copy); // Insert
	if (feedback == SP_LIST_OUT_OF_MEMORY) { // Allocation failure
		spListElementDestroy(copy);
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	if (spListElementGetValue(data) == spBPQueueMinValue(source)) { // If new element is smallest
		source->min = spListElementGetValue(copy); // Update minimum
	}
	if (spListGetSize(source->queue) == spBPQueueGetMaxSize(source) + 1) { // Queue was full
		data = spListGetFirst(source->queue);
		for (i = 0; i < spListGetSize(source->queue) - 1; i++) { // Go to last element
			data = spListGetNext(source->queue);
		}
		spListRemoveCurrent(source->queue); // Remove last element
	}
	spListElementDestroy(copy);
	return SP_BPQUEUE_SUCCESS; //All done
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	SPListElement data;
	if (source == NULL) { // Invalid argument
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	data = spListGetFirst(source->queue);
	if (data == NULL) { // Queue is empty
		return SP_BPQUEUE_EMPTY;
	}
	spListRemoveCurrent(source->queue); // Remove first element
	return SP_BPQUEUE_SUCCESS; // All well
}

SPListElement spBPQueuePeek(SPBPQueue source) {
	if (source == NULL) { // Invalid argument
		return NULL;
	}
	return spListElementCopy(spListGetFirst(source->queue)); // Return first element

}

SPListElement spBPQueuePeekLast(SPBPQueue source) {
	SPListElement data;
	int i;
	if (source == NULL) { // Invalid argument
		return NULL;
	}
	data = spListGetFirst(source->queue);
	if (data == NULL) { // Queue is empty
		return NULL;
	}
	for (i = 0; i < spListGetSize(source->queue) - 1; i++) { // Go to last element
		data = spListGetNext(source->queue);
	}
	return spListElementCopy(data); // Return last element
}

double spBPQueueMinValue(SPBPQueue source) {
	if (source == NULL) { // Invalid argument
		return -1.0;
	}
	return source->min;
}

double spBPQueueMaxValue(SPBPQueue source) {
	if (source == NULL) { // Invalid argument
		return -1.0;
	}
	return source->max;
}

bool spBPQueueIsEmpty(SPBPQueue source) {
	assert(source != NULL); // Invalid argument
	return spBPQueueSize(source) == 0;
}

bool spBPQueueIsFull(SPBPQueue source) {
	assert(source != NULL); // Invalid argument
	return spBPQueueSize(source) == spBPQueueGetMaxSize(source);
}
