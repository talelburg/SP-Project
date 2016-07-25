#include "SPQuerySolver.h"

struct sp_image_hits_t
{
	int index;
	int hits;
};

int imageHitsComp(const void * a, const void * b)
{
	SPImageHits *x = (SPImageHits*)a;
	SPImageHits *y = (SPImageHits*)b;

	if(x->hits != y->hits)
		return y->hits - x->hits;

	return y->index - x->index;
}

int* SPQuerySolverSolve(SPKDTreeNode kdTreeRoot, SPPoint* queryFeatures, int queryFeaturesAmount, int k, int numOfSimilar, int imagesAmount)
{
	int i, j;
	int* res;
	SPImageHits* imageHits;
	int* nearestNeighbours;
	SP_KDTREE_MSG kdTreeMsg;
	
	res = (int*)malloc(numOfSimilar * sizeof(int));
	imageHits = (SPImageHits*)malloc(imagesAmount * sizeof(SPImageHits));
	if(!res || !imageHits)
	{
		free(res);
		free(imageHits);
		return NULL;
	}

	for(i = 0; i < imagesAmount; i++)
	{
		imageHits[i].index = i;
		imageHits[i].hits = 0;
	}

	// Count image hits
	for(i = 0; i < queryFeaturesAmount; i++)
	{
		nearestNeighbours = SPKDTreeKNN(kdTreeRoot, queryFeatures[i], k, &kdTreeMsg);
		if(kdTreeMsg != SP_KDTREE_SUCCESS)
		{
			free(res);
			free(imageHits);
			return NULL;
		}
		for(j = 0; j < k; j++)
			imageHits[nearestNeighbours[j]].hits += 1;
		free(nearestNeighbours);
	}

	// Sort by hits
	qsort(imageHits, imagesAmount, sizeof(SPImageHits), imageHitsComp);

	// Copy best k image indexes
	for(i = 0; i < numOfSimilar; i++)
		res[i] = imageHits[i].index;
	free(imageHits);

	return res;
}
