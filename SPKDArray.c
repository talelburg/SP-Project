#include "SPKDArray.h"

// A struct representing the KD-Array
struct sp_kdarray_t
{
	SPPoint* points;
	int** pointsByCoors;
	int dims;
	int size;
};

// A struct to help sort the points according to the different dimensions
struct sp_sorting_helper_t
{
	SPPoint point;
	int dim;
	int originalIndex;
};
typedef struct sp_sorting_helper_t* SPSortingHelper;

// Comparator for aforementioned sorting
int SPSortingHelperCompare(const void* t1, const void* t2)
{
	SPSortingHelper *s1, *s2;
	assert(t1 != NULL && t2 != NULL);
	s1 = (SPSortingHelper*) t1;
	s2 = (SPSortingHelper*) t2;
	if (spPointGetAxisCoor((*s1)->point, (*s1)->dim) == spPointGetAxisCoor((*s2)->point, (*s2)->dim)) {
		return spPointGetIndex((*s1)->point) - spPointGetIndex((*s2)->point);
	}
	else if (spPointGetAxisCoor((*s1)->point, (*s1)->dim) > spPointGetAxisCoor((*s2)->point, (*s2)->dim)) {
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
	if (arr == NULL || size <= 0 || dims <= 0)
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
	
	// Set kdArr fields, allocate memory
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
	
	for (i = 0; i < size; i++) //  Copy the points
		kdArr->points[i] = spPointCopy(arr[i]);
	
	for (k = 0; k < dims; k++) // Now we sort the points according to dimension k
	{
		// Allocate helper array
		sorter = (SPSortingHelper*)malloc(size * sizeof(SPSortingHelper));
		if (sorter == NULL)
		{
			*msg = SP_KDARRAY_ALLOC_FAIL;
			SPKDArrayDestroy(kdArr);
			return NULL;
		}
		for (i = 0; i < size; i++)
		{
			// Allocate and initialize sorter[i]
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
			sorter[i]->originalIndex = i;
		}
		qsort(sorter, size, sizeof(SPSortingHelper), SPSortingHelperCompare); // Now sort
		for (i = 0; i < size; i++) // And update the appropriate line of pointsByCoors
		{
			kdArr->pointsByCoors[k][i] = sorter[i]->originalIndex;
			spPointDestroy(sorter[i]->point);
			free(sorter[i]);
		}
		free(sorter);
	}
	
	// Done
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
	
	// Allocate everything we need
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
	sizeOfLeft = kdArr->size / 2 + kdArr->size % 2;
	sizeOfRight = kdArr->size - sizeOfLeft;
	ret[0]->pointsByCoors = (int**)malloc(kdArr->dims * sizeof(int*));
	ret[0]->points = (SPPoint*)malloc(sizeOfLeft * sizeof(SPPoint));
	ret[1]->pointsByCoors = (int**)malloc(kdArr->dims * sizeof(int*));
	ret[1]->points = (SPPoint*)malloc(sizeOfRight * sizeof(SPPoint));
	if (ret[0]->pointsByCoors == NULL || ret[0]->points == NULL || ret[1]->pointsByCoors == NULL || ret[1]->points == NULL)
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
	for (i = 0; i < kdArr->size; i++) //  Check for each point if it belongs in the left half
		isLeft[kdArr->pointsByCoors[coor][i]] = (i < sizeOfLeft ? true : false);
	rightIndex = 0;
	leftIndex = 0;
	for (i = 0; i < kdArr->size; i++) // Update the maps, like in the suggested solution
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
	
	for (i = 0; i < kdArr->dims; i++) // Set pointsByCoors for the two KD-Arrays
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
	
	// All done
	free(isLeft);
	free(leftMap);
	free(rightMap);
	return ret;
}

SPPoint SPKDArrayGetPointByDim(SPKDArray kdArr, int index, int dim, SP_KDARRAY_MSG* msg)
{
	SPPoint ret;
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
		ret = spPointCopy(kdArr->points[index]);
	else
		ret = spPointCopy(kdArr->points[kdArr->pointsByCoors[dim][index]]);
	if (ret == NULL)
		*msg = SP_KDARRAY_ALLOC_FAIL;
	else
		*msg = SP_KDARRAY_SUCCESS;
	return ret;
}

int SPKDArrayGetDims(SPKDArray kdArr, SP_KDARRAY_MSG * msg)
{
	assert(msg != NULL);
	if (kdArr == NULL)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return -1;
	}
	return kdArr->dims;
}

int SPKDArrayGetSize(SPKDArray kdArr, SP_KDARRAY_MSG * msg)
{
	assert(msg != NULL);
	if (kdArr == NULL)
	{
		*msg = SP_KDARRAY_INVALID_ARGUMENT;
		return -1;
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
