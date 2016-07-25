#include "SPDatabaseManager.h"

const int endian_var = 1;
#define is_bigendian() ( (*(char*)&endian_var) == 0 )

#define STRING_LEN (1024)


/*
 * What's going on?
 * I'm glad you asked,
 *
 * 	We decode the features in the following format:
 * 	bin(<FeaturesAmount>)bin(<features[0]>)bin(<features[1]>)bin(<features[2]>)...
 *
 * 	When bin(<var>) means the variable data as saved in memory.
 *
 * 	As double and int demands more than 1 byte, we took care of the difference between
 * 	big and little endian.
 */

bool spDatabaseManagerSave(SPConfig config, int index, int featuresAmount, SPPoint* features)
{
	int i, j, k;
	char featsPath[STRING_LEN];
	FILE *file;
	int dim;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	double *coordinate = (double*) malloc(sizeof(double));
	char* charCoordinate; // Used to change a double to a char array
	char* charFeaturesAmount; // Used to change an int to a char array
	
	if(coordinate == NULL)
	{
		free(coordinate);
		return 0;
	}

	if(spConfigGetFeatsPath(featsPath, config, index) != SP_CONFIG_SUCCESS)
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

	file = fopen(featsPath, "w");
	if(file == NULL)
	{		
		free(coordinate);
		return 0;
	}
	
	charFeaturesAmount = (char*) &featuresAmount;
	for(k = 0; k < (int) (sizeof(int) / sizeof(char)); k++)
	{
		if(is_bigendian())
			fputc(charFeaturesAmount[sizeof(int) / sizeof(char) - k - 1], file);
		else
			fputc(charFeaturesAmount[k], file);
	}
	for(i = 0; i < featuresAmount; i++)
	{
		for(j = 0; j < dim; j++)
		{
			*coordinate = spPointGetAxisCoor(features[i], j);
			charCoordinate = (char*) coordinate;
			for(k = 0; k < (int) (sizeof(double) / sizeof(char)); k++)
			{
				if(is_bigendian())
					fputc(charCoordinate[sizeof(double) / sizeof(char) - k - 1], file);
				else
					fputc(charCoordinate[k], file);
			}
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
	int ind;
	char featsPath[STRING_LEN];
	FILE *file;
	int dim;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
		
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

	if(spConfigGetFeatsPath(featsPath, config, index) != SP_CONFIG_SUCCESS)
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
		
	file = fopen(featsPath, "r");
	if(file == NULL)
	{
		free(charFeaturesAmount);
		free(charCoordinate);
		free(data);
		return NULL;
	}

	for(k = 0; k < (int) (sizeof(int) / sizeof(char)); k++)
	{
		if(is_bigendian())
			ind = sizeof(int) / sizeof(char) - k - 1;
		else
			ind = k;

		if(feof(file))
		{
			free(charFeaturesAmount);
			free(charCoordinate);			
			free(data);
			fclose(file);
			return NULL;
		}
		charFeaturesAmount[ind] = fgetc(file);
	}
	_featuresAmount = (int*)charFeaturesAmount;
	*featuresAmount = *_featuresAmount;
	result = (SPPoint*) malloc((*featuresAmount) * sizeof(SPPoint));
	for(i = 0; i < *featuresAmount; i++)
	{
		for(j = 0; j < dim; j++)
		{
			for(k = 0; k < (int) (sizeof(double)/sizeof(char)); k++)
			{
				if(is_bigendian())
					ind = sizeof(double)/sizeof(char) - k - 1;
				else
					ind = k;

				if(feof(file))
				{
					free(charFeaturesAmount);
					free(charCoordinate);			
					free(data);
					fclose(file);
					free(result);
					return NULL;
				}
				charCoordinate[ind] = fgetc(file);
			}
			coordinate = (double*) charCoordinate;
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
