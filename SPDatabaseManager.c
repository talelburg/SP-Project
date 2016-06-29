#include "SPDatabaseManager.h"

bool spDatabaseManagerSave(SPConfig config, int index, int featuresAmount, SPPoint* features)
{
	int i, j, k;
	char* imagePath;
	FILE *file;
	int dim;
	SP_CONFIG_MSG msg;
	double *coordinate = (double*) malloc(sizeof(double));
	char* charCoordinate; // Used to change a double into chars
	char* charFeaturesAmount; // Used to change an int into chars

	if(coordinate == NULL)
	{
		free(coordinate);
		return 0;
	}

	if(spConfigGetImagePath(imagePath, config, index) != SP_CONFIG_SUCCESS)
	{
		free(coordinate);
		return 0;
	}

	dim = spConfigGetPCADim(config, &msg);
	if(msg != SP_CONFIG_SUCCESS)
	{
		free(coordinate);
		return 0;
	}

	file = fopen(imagePath, "w");
	if(file == NULL)
	{		
		free(coordinate);
		return 0;
	}
	
	charFeaturesAmount = (char*) &featuresAmount;
	for(k = 0; sizeof(int) / sizeof(char); k++)
		fputc(charCoordinate[k], file);
	for(i = 0; i < featuresAmount; i++)
	{
		for(j = 0; j < dim; j++)
		{
			*coordinate = spPointGetAxisCoor(features[i], j);
			charCoordinate = (char*) coordinate;
			for(k = 0; sizeof(double) / sizeof(char); k++)
				fputc(charCoordinate[k], file);
		}
	}

	fflush(file);
	fclose(file);
	
	free(coordinate);

	return 1;
}

SPPoint* spDatabaseManagerLoad(SPConfig config, int index, int* featuresAmount)
{
	int i, j, k;
	char* imagePath;
	FILE *file;
	int dim;
	SP_CONFIG_MSG msg;
		
	char* charFeaturesAmount = (char*) malloc(sizeof(int)); // Used to change an chars into int
	int* _featuresAmount;
	char* charCoordinate = (char*) malloc(sizeof(double)); // Used to change chars into double
	double* coordinate;
	double* data;

	SPPoint* result;

	if(charFeaturesAmount == NULL || charCoordinate == NULL)
	{
		free(charFeaturesAmount);
		free(charCoordinate);
		return NULL;
	}

	if(spConfigGetImagePath(imagePath, config, index) != SP_CONFIG_SUCCESS)
	{
		free(charFeaturesAmount);
		free(charCoordinate);
		return NULL;
	}

	dim = spConfigGetPCADim(config, &msg);
	if(msg != SP_CONFIG_SUCCESS)
	{		
		free(charFeaturesAmount);
		free(charCoordinate);
		return NULL;
	}

	data = (double*) malloc(dim * sizeof(double));
	if(data == NULL)
	{
		free(charFeaturesAmount);
		free(charCoordinate);
		return NULL;
	}
		
	file = fopen(imagePath, "r");
	if(file == NULL)
	{
		free(charFeaturesAmount);
		free(charCoordinate);
		free(data);
		return NULL;
	}

	for(k = 0; k < sizeof(int) / sizeof(char); k++)
	{
		charFeaturesAmount[k] = fgetc(file);
		if(charFeaturesAmount[k] == EOF)
		{
			free(charFeaturesAmount);
			free(charCoordinate);			
			free(data);
			fclose(file);
			return NULL;
		}
	}
	_featuresAmount = (int*)charFeaturesAmount;
	*featuresAmount = *_featuresAmount;
	result = (SPPoint*) malloc((*featuresAmount) * sizeof(SPPoint));
	for(i = 0; i < *featuresAmount; i++)
	{
		for(j = 0; j < dim; j++)
		{
			for(k = 0; k < sizeof(double)/sizeof(char); k++)
			{
				charCoordinate[k] = fgetc(file);
				if(charCoordinate[k] == EOF)
				{
					free(charFeaturesAmount);
					free(charCoordinate);			
					free(data);
					fclose(file);
					free(result);
					return NULL;
				}
			}
			data[j] = *coordinate;
		}
		result[i] = spPointCreate(data, dim, index);
		if(result[i] == NULL)
		{
			free(charFeaturesAmount);
			free(charCoordinate);			
			free(data);
			fclose(file);
			free(result);
			return NULL;
		}
	}
	
	free(charFeaturesAmount);
	free(charCoordinate);
	free(data);
	fclose(file);
	
	return result;
}