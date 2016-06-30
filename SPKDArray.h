#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef enum sp_kdarray_msg_t {
	SP_KDARRAY_INVALID_ARGUMENT,
	SP_KDARRAY_ALLOC_FAIL,
	SP_KDARRAY_INDEX_OUT_OF_RANGE,
	SP_KDARRAY_SUCCESS
} SP_KDARRAY_MSG;

typedef struct sp_kdarray_t* SPKDArray;

SPKDArray SPKDArrayInit(SPPoint* arr, int size, int dims, SP_KDARRAY_MSG* msg);

SPKDArray* SPKDARRAYSplit(SPKDArray kdArr, int coor, SP_KDARRAY_MSG* msg);

void SPKDArrayDestroy(SPKDArray kdArr);

#endif /* SPKDARRAY_H_ */
