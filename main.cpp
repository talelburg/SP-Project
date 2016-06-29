#include <cstdlib>
#include <stdio.h>
extern "C"
{
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPDatabaseManager.h"
}
#include "SPImageProc.h"

using namespace sp;

#define PRINT_ERROR(HUMAN_MSG) printf("Error: HUMAN_MSG")
#define LOGGER_PRINT_ERROR(HUMAN_MSG) spLoggerPrintMsg("Error: HUMAN_MSG")
#define ERR_LOGGER_DEFINED "Logger is already defined"

#include <string>

int main()
{
	int i = 0;
	int j = 0;
	char* configFileName = "Temp";
	char* loggerFileName;
	SP_CONFIG_MSG configMsg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG loggerMsg = SP_LOGGER_SUCCESS;

	// Config data
	int loggerLevel = 0;
	bool isExtractionMode = 0;
	int imagesAmount = 0;
	char* imagePath;

	ImageProc *imgProc;
	SPPoint* features;
	SPPoint** featuresByImage;
	int* imgFeaturesAmount;
	int totalFeaturesAmount = 0;

	SPConfig config = spConfigCreate(configFileName, &configMsg);
	if(configMsg != SP_CONFIG_SUCCESS)
	{
		spConfigDestroy(config);
		return 1;
	}

	configMsg = spConfigGetLoggerFilename(loggerFileName, config);
	if(configMsg != SP_CONFIG_SUCCESS)
	{
		PRINT_ERROR("Could not get LoggerFilename from config");
		spConfigDestroy(config);
		spLoggerDestroy();
		return 1;
	}
	loggerLevel = spConfigGetLoggerLevel(config, &configMsg);
	if(configMsg != SP_CONFIG_SUCCESS)
	{
		PRINT_ERROR("Could not get LoggerLevel from config");
		spConfigDestroy(config);
		spLoggerDestroy();
		return 1;
	}
	loggerMsg = spLoggerCreate(loggerFileName, (SP_LOGGER_LEVEL)loggerLevel);
	if(loggerMsg != SP_LOGGER_SUCCESS)
	{
		if(loggerMsg == SP_LOGGER_DEFINED)
			PRINT_ERROR(ERR_LOGGER_DEFINED);
		else if(loggerMsg == SP_LOGGER_OUT_OF_MEMORY)
			PRINT_ERROR(ERR_LOGGER_OUT_OF_MEMORY);
		else if(loggerMsg == SP_LOGGER_CANNOT_OPEN_FILE)
			PRINT_ERROR(ERR_LOGGER_CANNOT_OPEN_FILE);
		
		spConfigDestroy(config);
		spLoggerDestroy();
		return 1;
	}

	isExtractionMode = spConfigIsExtractionMode(config, &configMsg);
	if(configMsg != SP_CONFIG_SUCCESS)
	{
		LOGGER_PRINT_ERROR("Could not get IsExtractionMode from config");
		spConfigDestroy(config);
		spLoggerDestroy();
		return 1;
	}
	
	imgProc = new ImageProc(config);

	featuresByImage = (SPPoint**) malloc(imagesAmount * sizeof(SPPoint*));
	imgFeaturesAmount = (int*) malloc(imagesAmount * sizeof(int));
	if(featuresByImage == NULL || imgFeaturesAmount == NULL)
	{
		LOGGER_PRINT_ERROR(ERR_MEM_ALLOCATION);
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imgProc;
		free(featuresByImage);
		free(imgFeaturesAmount);
		return 1;
	}

	if(isExtractionMode)
	{
		totalFeaturesAmount = 0;
		for(i = 0; i < imagesAmount; i++)
		{
			featuresByImage[i] = imgProc->getImageFeatures(imagePath, i, imgFeaturesAmount + i);
			if(featuresByImage[i] == NULL)
			{
				LOGGER_PRINT_ERROR("Failed to extract image features");
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j <= i; j++)
					free(featuresByImage[j]);
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
			totalFeaturesAmount += imgFeaturesAmount[i];

			// Save
			if(!spDatabaseManagerSave(imagePath, i, imgFeaturesAmount[i], featuresByImage[i]))
			{				
				LOGGER_PRINT_ERROR("Failed to save features to database");
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j <= i; j++)
					free(featuresByImage[j]);
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
		}
	}
	else
	{		
		totalFeaturesAmount = 0;
		for(i = 0; i < imagesAmount; i++)
		{
			featuresByImage[i] = spDatabaseManagerLoad(imagePath, i, imgFeaturesAmount + i);
			if(featuresByImage[i] == NULL)
			{
				LOGGER_PRINT_ERROR("Failed to load image features from file");
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j <= i; j++)
					free(featuresByImage[j]);
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
			totalFeaturesAmount += imgFeaturesAmount[i];
		}
	}

	features = (SPPoint*) malloc(totalFeaturesAmount * sizeof(SPPoint));
	if(features == NULL)
	{
		LOGGER_PRINT_ERROR(ERR_MEM_ALLOCATION);
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imgProc;			
		for(i = 0; i <= imagesAmount; i++)
			free(featuresByImage[i]);
		free(featuresByImage);
		free(imgFeaturesAmount);
		return 1;
	}
	for(i = 0; i < imagesAmount; i++)
	{
		memcpy(features, featuresByImage[i], imgFeaturesAmount[i]);
		free(featuresByImage[i]);
	}
	free(imgFeaturesAmount);

	return 0;
}
