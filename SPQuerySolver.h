#ifndef SPQUERYSOLVER_H_
#define QPQUERYSOLVER_H_

#include "SPPoint.h"
#include "SPKDArray.h"

int* SPQuerySolverSolve(SPKDTreeNode* kdTreeRoot, SPPoint* queryFeatures, int queryFeaturesAmount, int k, int numOfSimilar);

#endif