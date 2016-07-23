#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"

typedef enum sp_kdtree_msg_t {
	SP_KDTREE_SUCCESS,
	SP_KDTREE_INVALID_ARGUMENT,
	SP_KDTREE_ALLOC_FAIL,
	SP_KDTREE_UNKNOWN_ERROR
} SP_KDTREE_MSG;

typedef enum sp_kdtree_split_method_t {
	SP_KDTREE_RANDOM,
	SP_KDTREE_MAX_SPREAD,
	SP_KDTREE_INCREMENTAL
} SP_KDTREE_SPLIT_METHOD;

typedef struct sp_kd_tree_node_t* SPKDTreeNode;

SPKDTreeNode SPKDTreeInit(SPPoint* arr, int size, int dims, SP_KDTREE_SPLIT_METHOD splitMethod, SP_KDTREE_MSG* msg);

SPKDTreeNode SPKDTreeInitHelp(SPKDArray kdArr, int size, SP_KDTREE_SPLIT_METHOD splitMethod, int lastIndex, SP_KDTREE_MSG * msg);

int* SPKDTreeKNearestNeighbours(SPKDTreeNode tree, SPPoint p, int k);

void SPKDTreeKNNRecursive(SPKDTreeNode tree, SPPoint p, SPBPQueue bpq);

void SPKDTreeDestroy(SPKDTreeNode tree);

#endif /* SPKDTREE_H_ */