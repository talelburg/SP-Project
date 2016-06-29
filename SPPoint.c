#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "SPPoint.h"

struct sp_point_t {
	double* data;
	int dim;
	int index;
};

SPPoint spPointCreate(double* data, int dim, int index)
{
	SPPoint point = NULL;
	int i;
	if (data == NULL || dim <= 0 || index < 0)
	{
		return NULL;
	}
	point = (SPPoint) malloc(sizeof(*point));
	if (point == NULL)
	{
		return NULL;
	}
	point->data = (double*) malloc(dim * sizeof(double));
	if (point->data == NULL)
	{
		return NULL;
	}
	point->dim = dim;
	point->index = index;
	for (i = 0; i < dim; i++)
	{
		point->data[i] = data[i];
	}
	return point;
}

SPPoint spPointCopy(SPPoint source)
{
	assert(source != NULL);
	return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint point)
{
	if (point != NULL)
	{
		free(point->data);
		free(point);
	}
}

int spPointGetDimension(SPPoint point)
{
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point)
{
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis)
{
	assert(point != NULL);
	assert(axis < point->dim);
	return (point->data)[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q)
{
	double distance;
	int i;
	assert(p != NULL);
	assert(q != NULL);
	assert(p->dim == p->dim);
	distance = 0;
	for (i = 0; i < p->dim; i++)
	{
			distance += ((p->data)[i] - (q->data)[i]) * ((p->data)[i] - (q->data)[i]);
	}
	return distance;
}
