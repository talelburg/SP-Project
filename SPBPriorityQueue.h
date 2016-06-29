#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * Implements a bounded priority queue container type.
 * The queue is represented using an object of type SPList, please refer to
 * SPList.h for usage.
 * In addition, the queue has a maximum size, and will not hold more itmes than
 * this size at any given time. Items have an integer index and double value.
 *
 * The following functions are available:
 *
 *   spBPQueueCreate               - Creates a new empty queue
 *   spBPQueueCopy                 - Copies an existing queue
 *   spBPQueueDestroy              - Deletes an existing queue and frees all resources
 *   spBPQueueClear		      	   - Clears all the data from the queue
 *   spBPQueueSize                 - Returns the size of a given queue
 *   spBPQueueGetMaxSize		   - Returns the maximum size supported by a given queue
 *	 spBPQueueEnqueue			   - Inserts a new item to its proper place in a given queue
 *	 spBPQueueDequeue			   - Deletes the minimal item from a given queue
 *	 spBPQueuePeek				   - Returns a copy of the first element in a given queue
 *	 spBPQueuePeekLast			   - Returns a copy of the last element in a given queue
 *	 spBPQueueMinValue			   - Returns the minimal value in a given queue
 *	 spBPQueueMaxValue			   - Returns the maximal value in a given queue
 *	 spBPQueueIsEmpty			   - Returns true if and only if the given queue is empty
 *	 spBPQueueIsFull			   - Returns true if and only if the given queue is full
 *
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new queue.
 *
 * This function creates a new empty queue.
 * @param maxSize The maximum size of the queue to be created.
 * @return
 * 	NULL - If allocations failed or maxSize <= 0
 *  Otherwise, the new queue.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target queue.
 *
 * The new copy will contain all the elements from the source queue in the same
 * order. The new queue's maximal size will be the same as the source's.
 *
 * @param source The target queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A queue containing the same elements with same order and same maximal size
 * as queue otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing queue. Clears all elements by using the
 * stored free function.
 *
 * @param source Target queue to be deallocated.
 * If queue is NULL nothing will be done. Otherwise source and all of its members are deallocated.
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target queue, and resets.
 *
 * The elements are deallocated using the stored freeing function
 * @param source Target queue to remove all elements from
 * If source was NULL nothing is done. Otherwise, all elements in the Queue
 * are removed.
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a queue.
 *
 * @param source The target queue whose size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximal number of elements in a queue.
 *
 * @param source The target queue whose maximal size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the maximal number of elements in the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserts a new element into the given queue. If queue was full,
 * the item with maximal value (including the new element) is removed.
 * @param source Target queue to insert the new element into.
 * @param element Target element to be inserted into the queue.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as source or as element
 * SP_BPQUEUE_FULL if the queue was full and also the new element was bigger than previous maximum
 * SP_BPQUEUE_OUT_OF_MEM if allocation failure occurred
 * SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the first item from the given queue.
 * @param source Target queue to remove first element from.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as source
 * SP_BPQUEUE_EMPTY if source is an empty queue
 * SP_BPQUEUE_SUCCESS otherwise
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a copy of the first element in the given queue.
 * @param source Target queue to get first element from.
 * @return
 * NULL if NULL was sent as source, if source is an empty queue or if
 * allocation failure occured
 * The first element in the list othwerwise.
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a copy of the last element in the given queue.
 * @param source Target queue to get last element from.
 * @return
 * NULL if NULL was sent as source, if source is an empty queue or if
 * allocation failure occured
 * The last element in the list othwerwise.
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Returns the minimal value contatined in the given queue.
 * @param source Target queue to get minimal value from.
 * @return
 * -1.0 if NULL was sent as source or source is an empty queue.
 * The minimal value in source otherwise.
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * Returns the maximal value contatined in the given queue.
 * @param source Target queue to get maximal value from.
 * @return
 * -1.0 if NULL was sent as source or source is an empty queue.
 * The maximal value in source otherwise.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns true if and only if the given queue is empty.
 * @param source Target queue to be checked.
 * @assert source != NULL
 * @return true if and only if the queue is empty.
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns true if and only if the given queue is full.
 * @param source Target queue to be checked.
 * @assert source != NULL
 * @return true if and only if the queue is full.
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
