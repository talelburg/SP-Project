#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "SPPoint.h"

/*
 * A data structure used to initialize a KD-Tree
 */

typedef enum sp_kdarray_msg_t {
	SP_KDARRAY_INVALID_ARGUMENT,
	SP_KDARRAY_ALLOC_FAIL,
	SP_KDARRAY_INDEX_OUT_OF_RANGE,
	SP_KDARRAY_SUCCESS
} SP_KDARRAY_MSG;

typedef struct sp_kdarray_t* SPKDArray;

/**
 * Creates a new KD-Array from the given array of points. 
 * size must contain the number of points in arr, dims must
 * contain the number of dimensions of every point in arr.
 * 
 * @param arr - the array of points
 * @param size - number of points in arr
 * @param dims - dimension of the points in arr
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains the KD-Array.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_KDARRAY_INVALID_ARGUMENT - if arr == NULL or size <= 0 or dims <= 9 or dims >= 29
 * - SP_KDARRAY_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_SUCCESS - in case of success
 */
SPKDArray SPKDArrayInit(SPPoint* arr, int size, int dims, SP_KDARRAY_MSG* msg);

/**
 * Splits the KD-Array kdArr according to the dimension coor.
 * 
 * @param kdArr - the KD-Array to be split
 * @param coor - the dimension to split by
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a dynamically allocated
 * 			2 member array. The first member is the left KD-Array, the second is the right.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_KDARRAY_INVALID_ARGUMENT - if kdArr == NULL or coor < 0
 * - SP_KDARRAY_INDEX_OUT_OF_RANGE - if coor >= kdArr->dims
 * - SP_KDARRAY_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_SUCCESS - in case of success
 */
SPKDArray* SPKDArraySplit(SPKDArray kdArr, int coor, SP_KDARRAY_MSG* msg);

/**
 * Returns the point in position index according to the dimension dim.
 * If dim = -1, the point will be returned according to no index, meaning
 * according to the index in kdArr->points.
 * 
 * @param kdArr - the KD-Array 
 * @param index - the index of the requested point
 * @param dim - the dimension according to which we work
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, the requested point.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_KDARRAY_INVALID_ARGUMENT - if kdArr == NULL or dim < -1 or index < 0 or index >= kdArr->size
 * - SP_KDARRAY_INDEX_OUT_OF_RANGE - if dim >= kdArr->dims
 * - SP_KDARRAY_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_SUCCESS - in case of success
 */
SPPoint SPKDArrayGetPointByDim(SPKDArray kdArr, int index, int dim, SP_KDARRAY_MSG* msg);

/**
 * Returns the dimension of the points in the KD-Array.
 * 
 * @param kdArr - the KD-Array
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, the dimension.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_KDARRAY_INVALID_ARGUMENT - if kdArr == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int SPKDArrayGetDims(SPKDArray kdArr, SP_KDARRAY_MSG* msg);

/**
 * Returns the number of points in the KD-Array.
 * 
 * @param kdArr - the KD-Array
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, the number of points.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_KDARRAY_INVALID_ARGUMENT - if kdArr == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int SPKDArrayGetSize(SPKDArray kdArr, SP_KDARRAY_MSG* msg);

/**
 * Frees all memory resources associated with kdArr. 
 * If kdArr == NULL nothig is done.
 */
void SPKDArrayDestroy(SPKDArray kdArr);

#endif /* SPKDARRAY_H_ */
