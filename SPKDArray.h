#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

struct SPKDArray;

SPKDArray* SPKDArrayInit(SPPoint* arr, int size);
SPPoint* SPKDArrayFindClosestK(SPPoint feature, int k);

#endif