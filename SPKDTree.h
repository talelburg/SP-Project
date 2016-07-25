#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPKDTreeSplitMethod.h"

typedef enum sp_kdtree_msg_t {
	SP_KDTREE_SUCCESS,
	SP_KDTREE_INVALID_ARGUMENT,
	SP_KDTREE_ALLOC_FAIL
} SP_KDTREE_MSG;

typedef struct sp_kd_tree_node_t* SPKDTreeNode;

/*
 * This function initializes a KD-Tree according to the array of points
 * arr. size must be the amount of points in arr, dims must be the number of dimensions
 * in every point in arr. splitMethod is the method to choose the dimension to split by -
 * max spread, random or incremental. msg is a pointer in which the value of the return 
 * message will be stored.
 *
 * @param arr - the array of points
 * @param size - the number of points in arr
 * @param dims - the number of dimensions in each point in arr
 * @param splitMethod - the method to split the KD-Tree
 * @assert msg != NULL
 * @param msg a pointer in which the return message will be stored
 * @return  An array of the k nearest neighbors indexes - on success
			NULL - if an error occurred
 * The return message will be as follows:
 * SP_KDTREE_INVALID_ARGUMENT - if arr == NULL or size <= 0 or dims <= 9 or dims >= 29
 * SP_KDTREE_ALLOC_FAIL - if an allocation failure occured
 * SP_KDTREE_SUCCESS - in case of success
 *
 */
SPKDTreeNode SPKDTreeInit(SPPoint* arr, int size, int dims, SP_KDTREE_SPLIT_METHOD splitMethod, SP_KDTREE_MSG* msg);

/*
 * This is a recursive helper function, to help with initializing the KD-Tree. It follows
 * the pseudocode in the instruction pdf file. If there's one point, the node is a leaf,
 * otherwise it splits by the chosen dimension and holds the median value.
 *
 */
SPKDTreeNode SPKDTreeInitHelp(SPKDArray kdArr, int size, SP_KDTREE_SPLIT_METHOD splitMethod, int lastIndex, SP_KDTREE_MSG * msg);

/*
 * Used to search the k nearest neighbors in a kdTree,
 * this function does the following:
 * 		1. Initialize a bpq for the result
 * 		2. Recursively call SPKDTreeKNNRecursive to fill the bpq
 * 		3. return an array containing the items in the bpq
 *
 * @param tree - the kdTree
 * @param p - the search is relative to the point p
 * @param k - the k in 'k nearest neighbors'
 * @return  An array of the k nearest neighbors indexes - on success
			NULL - if an error occurred
*/
int* SPKDTreeKNN(SPKDTreeNode tree, SPPoint p, int k, SP_KDTREE_MSG* msg);

/*
 * 	This is a helper function for SPKDTreeKNN.
 * 	It follows the pseudo code in the instructions pdf file to recursively search
 * 	points that are close to the point p.
 * 	Each time we get to a leaf, we enqueue it to the given bpq.
 *
*/
void SPKDTreeKNNRecursive(SPKDTreeNode treeNode, SPPoint p, SPBPQueue bpq, SP_KDTREE_MSG* msg);

void SPKDTreeDestroy(SPKDTreeNode tree);

#endif /* SPKDTREE_H_ */
