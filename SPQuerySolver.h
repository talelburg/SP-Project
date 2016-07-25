#ifndef SPQUERYSOLVER_H_
#define SPQUERYSOLVER_H_

#include "SPPoint.h"
#include "SPKDTree.h"

typedef struct sp_image_hits_t SPImageHits;

/*
 *	Compares two SPImageHits
 *	returns positive integer if b > a
 *	returns negative integer if a < b
 *	returns 0 if a == b
 *
*/
int imageHitsComp(const void * a, const void * b);

/*
 * Given a query and a kdTree containing all the features in the database,
 * For each query feature we find the k nearest features, the function returns the
 * image indexes of the images that their features were part of the k nearest features the most.
 *
 *
 * @param kdTreeRoot - the kdTree containing all the features in the database
 * @param queryFeatures - the features that represent the query
 * @param queryFeaturesAmount - the length of 'queryFeatures'
 * @param k - the k in 'k nearest neighbors'
 * @param numOfSimilar - the number of similar images to return as result
 * @param imagesAmount - the amount of images in the database
 * @return  An array of the indexes of the 'numOfSimilar' most similar images - On success
			NULL - If an error occurred
*/
int* SPQuerySolverSolve(SPKDTreeNode kdTreeRoot, SPPoint* queryFeatures, int queryFeaturesAmount, int k, int numOfSimilar, int imagesAmount);

#endif /* SPQUERYSOLVER_H_ */
