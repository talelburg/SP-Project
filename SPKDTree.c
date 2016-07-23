#include "SPKDTree.h"

struct sp_kd_tree_node_t 
{
	int dim;
	double val;
	SPKDTreeNode left, right, parent;
	SPPoint* data;
};

SPKDTreeNode SPKDTreeInit(SPPoint * arr, int size, int dims, SP_KDTREE_SPLIT_METHOD splitMethod, SP_KDTREE_MSG * msg)
{
	SPKDArray kdArr;
	SP_KDARRAY_MSG kdArrMsg;
	SPKDTreeNode ret;
	assert(msg != NULL);
	if (arr == NULL || size <= 0 || dims <= 9 || dims >= 29)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return NULL;
	}
	kdArr = SPKDArrayInit(arr, size, dims, &kdArrMsg);
	if (kdArrMsg == SP_KDARRAY_ALLOC_FAIL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		return NULL;
	}
	else if (kdArrMsg != SP_KDARRAY_SUCCESS)
	{
		*msg = SP_KDTREE_UNKNOWN_ERROR;
		return NULL;
	}
	return SPKDTreeInitHelp(kdArr, size, splitMethod, -1, msg);
}

SPKDTreeNode SPKDTreeInitHelp(SPKDArray kdArr, int size, SP_KDTREE_SPLIT_METHOD splitMethod, int lastIndex, SP_KDTREE_MSG * msg)
{
	int dim, dims, maxSpread, maxSpreadIndex, spread, i, minVal, maxVal;
	SP_KDARRAY_MSG* kdArrMsg;
	SPKDArray* split;
	SPKDArray kdArr;
	SPKDTreeNode ret, right, left;
	assert(msg != NULL);
	if (kdArr == NULL || size <= 0 || lastIndex < -1)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return NULL;
	}
	ret = (SPKDTreeNode)malloc(sizeof(*ret));
	if (ret == NULL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		return NULL;
	}
	kdArrMsg = (SP_KDARRAY_MSG*)malloc(sizeof(SP_KDARRAY_MSG));
	if (kdArrMsg == NULL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		free(ret);
		return NULL;
	}
	if (size == 1)
	{
		ret->dim = -1;
		ret->val = -1.0;
		ret->left = (ret->right = NULL);
		ret->data = (SPPoint*)malloc(sizeof(SPPoint));
		if (ret->data == NULL)
		{
			*msg = SP_KDTREE_ALLOC_FAIL;
			free(ret);
			return NULL;
		}
		*(ret->data) = SPKDArrayGetPointByDim(kdArr, 0, -1, kdArrMsg);
	}
	else
	{
		dims = SPKDArrayGetDims(kdArr, kdArrMsg);
		switch (splitMethod)
		{
		case SP_KDTREE_MAX_SPREAD:
			maxSpread = 0;
			maxSpreadIndex = 0;
			for (i = 0; i < dims; i++)
			{
				minVal = SPPointGetAxisCoor(SPKDArrayGetPointByDim(kdArr, 0, i, kdArrMsg), i);
				maxVal = SPPointGetAxisCoor(SPKDArrayGetPointByDim(kdArr, size - 1, i, kdArrMsg), i);
				spread = maxVal - minVal;
				if (spread > maxSpread)
				{
					maxSpread = spread;
					maxSpreadIndex = i;
				}
			}
			dim = maxSpreadIndex;
			break;
		case SP_KDTREE_RANDOM:
			dim = rand() % dims;
			break;
		case SP_KDTREE_INCREMENTAL:
			dim = (lastIndex + 1) % dims;
			break;
		}
		split = SPKDArraySplit(kdArr, dim, kdArrMsg);
		ret->dim = dim;
		ret->val = SPPointGetAxisCoor(SPKDArrayGetPointByDim(split[0], SPKDArrayGetSize(split[0], kdArrMsg), dim, kdArrMsg), dim);
		ret->left = SPKDTreeInitHelp(split[0], SPKDArrayGetSize(split[0], kdArrMsg), dims, splitMethod, lastIndex + 1, msg);
		ret->right = SPKDTreeInitHelp(split[1], SPKDArrayGetSize(split[1], kdArrMsg), dims, splitMethod, lastIndex + 1, msg);
		ret->data = NULL;
	}
	*msg = SP_KDTREE_SUCCESS;
	return ret;
}

SPPoint * SPKDTreeKNearestNeighbours(SPKDTreeNode tree, SPPoint p, int k)
{
	return NULL;
}

void SPKDTreeDestroy(SPKDTreeNode tree)
{
	if (tree != NULL)
	{
		SPKDTreeDestroy(tree->left);
		SPKDTreeDestroy(tree->right);
		SPKDTreeDestroy(tree->parent);
		if (tree->data != NULL)
			spPointDestroy(*(tree->data));
		free(tree->data);
		free(tree);
	}
}
