#ifndef SPQUERYSOLVER_H_
#define QPQUERYSOLVER_H_

#include "SPPoint.h"
#include "SPKDTree.h"

typedef struct sp_image_hits_t SPImageHits;

int imageHitsComp(const void * a, const void * b);

int* SPQuerySolverSolve(SPKDTreeNode kdTreeRoot, SPPoint* queryFeatures, int queryFeaturesAmount, int k, int numOfSimilar);

#endif