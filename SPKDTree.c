#include "SPKDTree.h"

// A struct to represent a KD-Tree
struct sp_kd_tree_node_t 
{
	int dim;
	double val;
	SPKDTreeNode left, right;
	SPPoint* data;
};

SPKDTreeNode SPKDTreeInit(SPPoint * arr, int size, int dims, SP_KDTREE_SPLIT_METHOD splitMethod, SP_KDTREE_MSG * msg)
{
	SPKDArray kdArr;
	SP_KDARRAY_MSG kdArrMsg;
	SPKDTreeNode ret;
	assert(msg != NULL);
	if (arr == NULL || size <= 0 || dims <= 0)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return NULL;
	}
	
	kdArr = SPKDArrayInit(arr, size, dims, &kdArrMsg); // Initialize the KD-Array
	if (kdArrMsg == SP_KDARRAY_ALLOC_FAIL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		return NULL;
	}
	
	// Employ recursive helper
	ret = SPKDTreeInitHelp(kdArr, size, splitMethod, -1, msg); 
	
	// All done
	SPKDArrayDestroy(kdArr);
	return ret;
}

SPKDTreeNode SPKDTreeInitHelp(SPKDArray kdArr, int size, SP_KDTREE_SPLIT_METHOD splitMethod, int lastIndex, SP_KDTREE_MSG * msg)
{
	int dim, dims, maxSpread, maxSpreadIndex, spread, i, minVal, maxVal;
	SP_KDARRAY_MSG kdArrMsg;
	SPKDArray* split;
	SPKDTreeNode ret;
	SPPoint workPoint;
	assert(msg != NULL);
	if (kdArr == NULL || size <= 0 || lastIndex < -1)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return NULL;
	}
	// Allocate memory
	ret = (SPKDTreeNode)malloc(sizeof(*ret));
	if (ret == NULL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		return NULL;
	}
	if (size == 1)
	{
		// Returned node will be a leaf
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
		*(ret->data) = SPKDArrayGetPointByDim(kdArr, 0, -1, &kdArrMsg); // Gets the only point in the KD-Array
	}
	else
	{
		// Returned node is not a leaf
		dims = SPKDArrayGetDims(kdArr, &kdArrMsg);
		
		// Calculate the splitting dimension
		switch (splitMethod)
		{
		case SP_KDTREE_MAX_SPREAD:
			maxSpread = 0;
			maxSpreadIndex = 0;
			// calculate the maximum spread dimension
			for (i = 0; i < dims; i++)
			{
				workPoint = SPKDArrayGetPointByDim(kdArr, 0, i, &kdArrMsg);
				minVal = spPointGetAxisCoor(workPoint, i);
				spPointDestroy(workPoint);
				workPoint = SPKDArrayGetPointByDim(kdArr, size - 1, i, &kdArrMsg);
				maxVal = spPointGetAxisCoor(workPoint, i);
				spPointDestroy(workPoint);
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
		
		split = SPKDArraySplit(kdArr, dim, &kdArrMsg); // Split the KD-Array according to the splitting dimension
		workPoint = SPKDArrayGetPointByDim(split[0], SPKDArrayGetSize(split[0], &kdArrMsg) - 1, dim, &kdArrMsg);
		ret->dim = dim;
		ret->val = spPointGetAxisCoor(workPoint, dim);
		
		// Employ recursion to calculate subtrees
		ret->left = SPKDTreeInitHelp(split[0], SPKDArrayGetSize(split[0], &kdArrMsg), splitMethod, lastIndex + 1, msg);
		ret->right = SPKDTreeInitHelp(split[1], SPKDArrayGetSize(split[1], &kdArrMsg), splitMethod, lastIndex + 1, msg);
		ret->data = NULL;
		
		// Get rid of unneeded memory
		SPKDArrayDestroy(split[0]);
		SPKDArrayDestroy(split[1]);
		free(split);
		spPointDestroy(workPoint);
	}
	
	// All done
	*msg = SP_KDTREE_SUCCESS;
	return ret;
}

int* SPKDTreeKNN(SPKDTreeNode tree, SPPoint p, int k, SP_KDTREE_MSG* msg)
{
	int i;
	int* res;
	SPBPQueue bpq;
	SPListElement head;

	if(tree == NULL || k <= 0)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return NULL;
	}

	bpq = spBPQueueCreate(k);
	if(bpq == NULL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		return NULL;
	}

	res = (int*) malloc(k * sizeof(int));
	if(res == NULL)
	{
		*msg = SP_KDTREE_ALLOC_FAIL;
		spBPQueueDestroy(bpq);
		return NULL;
	}

	// Call SPKDTreeKNNRecursive to fill the bpq with the k nearest neighbors
	SPKDTreeKNNRecursive(tree, p, bpq, msg);
	if (*msg != SP_KDTREE_SUCCESS)
	{
		spBPQueueDestroy(bpq);
		return NULL;
	}

	// Cast the bpq to an array
	for(i = 0; i < k; i++)
	{
		head = spBPQueuePeek(bpq);
		res[i] = spListElementGetIndex(head);
		spListElementDestroy(head);
		spBPQueueDequeue(bpq);
	}

	spBPQueueDestroy(bpq);

	*msg = SP_KDTREE_SUCCESS;
	return res;
}

void SPKDTreeKNNRecursive(SPKDTreeNode treeNode, SPPoint p, SPBPQueue bpq, SP_KDTREE_MSG* msg)
{
	SPListElement listElement;
	SPPoint treePoint;
	bool searchedLeft;
	double dist;

	if(bpq == NULL || treeNode == NULL)
	{
		*msg = SP_KDTREE_INVALID_ARGUMENT;
		return;
	}

	// If treeNode is a leaf
	if(treeNode->left == NULL && treeNode->right == NULL)
	{
		treePoint = *(treeNode->data);
		listElement = spListElementCreate(spPointGetIndex(treePoint), spPointL2SquaredDistance(p, treePoint));
		spBPQueueEnqueue(bpq, listElement);
		spListElementDestroy(listElement);
		*msg = SP_KDTREE_SUCCESS;
		return;
	}

	// Turn to search the tree that would've contain the point p (if it was in the tree)
	if(spPointGetAxisCoor(p, treeNode->dim) <= treeNode->val)
	{
		searchedLeft = true;
		SPKDTreeKNNRecursive(treeNode->left, p, bpq, msg);
		if (*msg != SP_KDTREE_SUCCESS)
			return;
	}
	else
	{
		searchedLeft = false;
		SPKDTreeKNNRecursive(treeNode->right, p, bpq, msg);
		if (*msg != SP_KDTREE_SUCCESS)
			return;
	}

	// dist = |treeNode.val - p[treeNode.dim]|
	dist = treeNode->val - spPointGetAxisCoor(p, treeNode->dim);
	if(dist < 0)
		dist *= -1;
	//dist *= dist;

	if(!spBPQueueIsFull(bpq) || dist < spBPQueueMaxValue(bpq))
	{
		if(searchedLeft)
			SPKDTreeKNNRecursive(treeNode->right, p, bpq, msg);
		else
			SPKDTreeKNNRecursive(treeNode->left, p, bpq, msg);
	}
	
}

void SPKDTreeDestroy(SPKDTreeNode tree)
{
	if (tree != NULL)
	{
		SPKDTreeDestroy(tree->left);
		SPKDTreeDestroy(tree->right);
		if (tree->data != NULL)
			spPointDestroy(*(tree->data));
		free(tree->data);
		free(tree);
	}
}
