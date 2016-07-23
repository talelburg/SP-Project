#include "SPKDArray.h"

struct sp_kdarray_t
{
	SPPoint* points;
	int** pointsByCoors;
	int dims;
	int size;
};

struct sp_sorting_helper_t
{
	SPPoint point;
	int dim;
	int originalIndex;
};
typedef struct sp_sorting_helper_t* SPSortingHelper;

int SPSortingHelperCompare(SPSortingHelper s1, SPSortingHelper s2)
{
	assert(s1 != NULL && s2 != NULL);
	if (spPointGetAxisCoor(s1->point, s1->dim) == spPointGetAxisCoor(s2->point, s2->dim)) {
		return spPointGetIndex(s1->point) - spPointGetIndex(s2->point);
	}
	else if (spPointGetAxisCoor(s1->point, s1->dim) > spPointGetAxisCoor(s2->point, s2->dim)) {
		return 1;
	}
	return -1;
}

SPKDArray SPKDArrayInit(SPPoint* arr, int size, int dims, SP_KDARRAY_MSG* msg)
{
	SPKDArray kdArr;
	int i, j, k;
	SPSortingHelper* sorter;
	assert(msg != NULL);
	if (arr == NULL || size <= 0 || dims <= 9 || dims >= 29)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return NULL;
	}
	kdArr = (SPKDArray)malloc(sizeof(*kdArr));
	if (kdArr == NULL)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		return NULL;
	}
	kdArr->dims = dims;
	kdArr->size = size;
	kdArr->points = (SPPoint*)malloc(size * sizeof(SPPoint));
	kdArr->pointsByCoors = (int**)malloc(dims * sizeof(int*));
	if (kdArr->points == NULL || kdArr->pointsByCoors == NULL)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		free(kdArr->points);
		free(kdArr);
		return NULL;
	}
	for (i = 0; i < dims; i++)
	{
		kdArr->pointsByCoors[i] = (int*)malloc(size * sizeof(int));
		if (kdArr->pointsByCoors[i] == NULL)
		{
			*msg = SP_KDARRAY_ALLOC_FAIL;
			for (j = 0; j < i; j++)
				free(kdArr->pointsByCoors[j]);
			free(kdArr->pointsByCoors);
			free(kdArr->points);
			free(kdArr);
			return NULL;
		}
	}
	for (i = 0; i < size; i++)
		kdArr->points[i] = spPointCopy(arr[i]);
	for (k = 0; k < dims; k++)
	{
		sorter = (SPSortingHelper*)malloc(size * sizeof(SPSortingHelper));
		if (sorter == NULL)
		{
			*msg = SP_KDARRAY_ALLOC_FAIL;
			SPKDArrayDestroy(kdArr);
			return NULL;
		}
		for (i = 0; i < size; i++)
		{
			sorter[i] = (SPSortingHelper)malloc(sizeof(*sorter[i]));
			if (sorter[i] == NULL)
			{
				*msg = SP_KDARRAY_ALLOC_FAIL;
				for (j = 0; j < i; j++)
					free(sorter[j]);
				free(sorter);
				SPKDArrayDestroy(kdArr);
				return NULL;
			}
			sorter[i]->point = spPointCopy(kdArr->points[i]);
			sorter[i]->dim = k;
			sorter[i]->originalIndex;
		}
		qsort(sorter, size, sizeof(SPSortingHelper), SPSortingHelperCompare);
		for (i = 0; i < size; i++)
		{
			kdArr->pointsByCoors[i] = sorter[i]->originalIndex;
			free(sorter[i]);
		}
		free(sorter);
	}
	*msg = SP_KDARRAY_SUCCESS;
	return kdArr;
}

SPKDArray* SPKDArraySplit(SPKDArray kdArr, int coor, SP_KDARRAY_MSG* msg)
{
	bool* isLeft;
	SPKDArray* ret;
	int i, j, leftIndex, rightIndex;
	int sizeOfLeft, sizeOfRight;
	int* leftMap;
	int* rightMap;
	assert(msg != NULL);
	if (kdArr == NULL || coor < 0)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return NULL;
	}
	if (coor >= kdArr->dims)
	{
		*msg = SP_KDARRAY_INDEX_OUT_OF_RANGE;
		return NULL;
	}
	ret = (SPKDArray*)malloc(2 * sizeof(SPKDArray));
	if (ret == NULL)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		free(ret);
		return NULL;
	}
	ret[0] = (SPKDArray)malloc(sizeof(*ret[0]));
	ret[1] = (SPKDArray)malloc(sizeof(*ret[1]));
	if (ret[0] == NULL || ret[1] == NULL)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		free(ret[0]);
		free(ret[1]);
		free(ret);
		return NULL;
	}
	ret[0]->pointsByCoors = (int**)malloc(kdArr->dims * sizeof(int*));
	ret[0]->points = (SPPoint*)malloc(sizeOfLeft * sizeof(SPPoint));
	ret[1]->pointsByCoors = (int**)malloc(kdArr->dims * sizeof(int*));
	ret[1]->points = (SPPoint*)malloc(sizeOfRight * sizeof(SPPoint));
	if (ret[0]->pointsByCoors == NULL || ret[0]->points == NULL || ret[1]->pointsByCoors == NULL || ret[1]->points)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		free(ret[0]->pointsByCoors);
		free(ret[0]->points);
		free(ret[0]);
		free(ret[1]->pointsByCoors);
		free(ret[1]->points);
		free(ret[1]);
		free(ret);
		return NULL;
	}
	for (i = 0; i < kdArr->dims; i++)
	{
		ret[0]->pointsByCoors[i] = (int*)malloc(sizeOfLeft * sizeof(int));
		ret[1]->pointsByCoors[i] = (int*)malloc(sizeOfRight * sizeof(int));
		if (ret[0]->pointsByCoors[i] == NULL || ret[1]->pointsByCoors[i] == NULL)
		{
			*msg = SP_KDARRAY_ALLOC_FAIL;
			for (j = 0; j <= i; j++)
			{
				free(ret[0]->pointsByCoors[j]);
				free(ret[1]->pointsByCoors[j]);
			}
			free(ret[0]->pointsByCoors);
			free(ret[0]->points);
			free(ret[0]);
			free(ret[1]->pointsByCoors);
			free(ret[1]->points);
			free(ret[1]);
			free(ret);
			return NULL;
		}
	}
	sizeOfLeft = kdArr->size / 2 + kdArr->size % 2;
	sizeOfRight = kdArr->size - sizeOfLeft;
	isLeft = (bool*)malloc(kdArr->size * sizeof(bool));
	leftMap = (int*)malloc(kdArr->size * sizeof(int));
	rightMap = (int*)malloc(kdArr->size * sizeof(int));
	if (isLeft == NULL || leftMap == NULL || rightMap == NULL)
	{
		*msg = SP_KDARRAY_ALLOC_FAIL;
		SPKDArrayDestroy(ret[0]);
		SPKDArrayDestroy(ret[1]);
		free(ret);
		free(isLeft);
		free(leftMap);
		free(rightMap);
		return NULL;
	}
	ret[0]->dims = kdArr->dims;
	ret[0]->size = sizeOfLeft;
	ret[1]->dims = kdArr->dims;
	ret[1]->size = sizeOfRight;
	for (i = 0; i < kdArr->size; i++)
		isLeft[kdArr->pointsByCoors[coor][i]] = (i < sizeOfLeft ? true : false);
	rightIndex = 0;
	leftIndex = 0;
	for (i = 0; i < kdArr->size; i++)
	{
		if (isLeft[i])
		{
			rightMap[i] = -1;
			leftMap[i] = leftIndex;
			ret[0]->points[leftIndex] = spPointCopy(kdArr->points[i]);
			leftIndex++;
		}
		else
		{
			leftMap[i] = -1;
			rightMap[i] = rightIndex;
			ret[1]->points[rightIndex] = spPointCopy(kdArr->points[i]);
			rightIndex++;
		}
	}
	for (i = 0; i < kdArr->dims; i++)
	{
		leftIndex = 0;
		rightIndex = 0;
		for (j = 0; j < kdArr->size; j++)
		{
			if (isLeft[kdArr->pointsByCoors[i][j]])
			{
				ret[0]->pointsByCoors[i][leftIndex] = leftMap[kdArr->pointsByCoors[i][j]];
				leftIndex++;
			}
			else
			{
				ret[1]->pointsByCoors[i][rightIndex] = rightMap[kdArr->pointsByCoors[i][j]];
				rightIndex++;
			}
		}
	}
	free(isLeft);
	free(leftMap);
	free(rightMap);
	return ret;
}

SPPoint SPKDArrayGetPointByDim(SPKDArray kdArr, int index, int dim, SP_KDARRAY_MSG* msg)
{
	assert(msg != NULL);
	if (kdArr == NULL || dim < -1 || index < 0 || index >= kdArr->size)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return NULL;
	}
	if (dim >= kdArr->dims)
	{
		*msg = SP_KDARRAY_INDEX_OUT_OF_RANGE;
		return NULL;
	}
	if (dim == -1)
		return SPPointCopy(kdArr->points[index]);
	else
		return SPPointCopy(kdArr->points[kdArr->pointsByCoors[dim][index]]);
}

int SPKDArrayGetDims(SPKDArray kdArr, SP_KDARRAY_MSG * msg)
{
	assert(msg != NULL);
	if (kdArr == NULL)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return NULL;
	}
	return kdArr->dims;
}

int SPKDArrayGetSize(SPKDArray kdArr, SP_KDARRAY_MSG * msg)
{
	assert(msg != NULL);
	if (kdArr == NULL)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return NULL;
	}
	return kdArr->size;
}

void SPKDArrayDestroy(SPKDArray kdArr)
{
	int i;
	if (kdArr != NULL)
	{
		for (i = 0; i < kdArr->dims; i++)
			free(kdArr->pointsByCoors[i]);
		free(kdArr->pointsByCoors);
		for (i = 0; i < kdArr->size; i++)
			spPointDestroy(kdArr->points[i]);
		free(kdArr->points);
		free(kdArr);
	}
}
