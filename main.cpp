#include <cstdlib>
#include <stdio.h>
extern "C"
{
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPDatabaseManager.h"
#include "SPKDTree.h"
#include "SPQuerySolver.h"
}
#include "SPImageProc.h"

using namespace sp;

#define PRINT_ERROR(HUMAN_MSG) printf("Error: " HUMAN_MSG)
#define LOGGER_PRINT_ERROR(HUMAN_MSG, FILE, FUNCTION, LINE) spLoggerPrintError("Error: " HUMAN_MSG, FILE, FUNCTION, LINE)
#define ERR_LOGGER_OUT_OF_MEMORY "Logger is out of memory"
#define ERR_LOGGER_CANNOT_OPEN_FILE "Logger failed to open file"
#define ERR_LOGGER_DEFINED "Logger is already defined"
#define ERR_MEM_ALLOCATION "blabla"
#define ERR_GET_IMG_FEATS "blabla"
#define ERR_IMG_PATH_INDEX "blabla"
#define ASK_FOR_QUERY "Please enter an image path:\n"
#define MSG_BEST_CANDIDATES "Best candidates for - %s - are:\n"
#define MSG_EXIT "Exiting..."

#include <string>

int main()
{
	// ** Variables decleration ** 

	// Index variables
	int i = 0, j = 0, k = 0;

	// Config and Logger init variables
	char configFileName[1024] = "Temp";
	char loggerFileName[1024];
	SP_CONFIG_MSG configMsg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG loggerMsg = SP_LOGGER_SUCCESS;
	SPConfig config;

	// Config data variables
	int loggerLevel = 0;
	bool isExtractionMode = 0;
	int imagesAmount = 0;
	char imagePath[1024];
	int knn;
	int numOfSimilarImages;
	bool minimalGui;
	SP_KDTREE_SPLIT_METHOD kdTreeSplitMethod;
	int pcaDim;

	// Features extraction variables
	ImageProc *imgProc;
	SPPoint** featuresByImage;
	int* imgFeaturesAmount;
	int totalFeaturesAmount = 0;

	// Main datastructure variables
	SPKDTreeNode kdTreeRoot;
	SP_KDTREE_MSG kdTreeMsg = SP_KDTREE_SUCCESS;
	SPPoint* features;

	// Query variables
	int* similarImages;
	char userInput[1024];
	SPPoint* queryFeatures;
	int queryFeaturesAmount;
	char resImagePath[1024];

	// ** Config and Logger initialization **

	config = spConfigCreate(configFileName, &configMsg);

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
		LOGGER_PRINT_ERROR("Could not get IsExtractionMode from config", __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		return 1;
	}

	// ** Features extraction **

	imagesAmount = spConfigGetNumOfImages(config, &configMsg);
	
	imgProc = new ImageProc(config);

	featuresByImage = (SPPoint**) malloc(imagesAmount * sizeof(SPPoint*));
	imgFeaturesAmount = (int*) malloc(imagesAmount * sizeof(int));
	if(featuresByImage == NULL || imgFeaturesAmount == NULL)
	{
		LOGGER_PRINT_ERROR(ERR_MEM_ALLOCATION, __FILE__, __func__, __LINE__);
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
			configMsg = spConfigGetImagePath(imagePath, config, i);
			if(configMsg != SP_CONFIG_SUCCESS)
			{
				LOGGER_PRINT_ERROR("Failed to extract image features", __FILE__, __func__, __LINE__);
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j < i; j++)
				{
					for(k = 0; k < imgFeaturesAmount[j]; k++)
						spPointDestroy(featuresByImage[j][k]);
					free(featuresByImage[j]);
				}
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
			featuresByImage[i] = imgProc->getImageFeatures(imagePath, i, imgFeaturesAmount + i);
			if(featuresByImage[i] == NULL)
			{
				LOGGER_PRINT_ERROR("Failed to extract image features", __FILE__, __func__, __LINE__);
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j < i; j++)
				{
					for(k = 0; k < imgFeaturesAmount[j]; k++)
						spPointDestroy(featuresByImage[j][k]);
					free(featuresByImage[j]);
				}
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
			totalFeaturesAmount += imgFeaturesAmount[i];

			// Save
			if(!spDatabaseManagerSave(config, i, imgFeaturesAmount[i], featuresByImage[i]))
			{				
				LOGGER_PRINT_ERROR("Failed to save features to database", __FILE__, __func__, __LINE__);
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j <= i; j++)
				{
					for(k = 0; k < imgFeaturesAmount[j]; k++)
						spPointDestroy(featuresByImage[j][k]);
					free(featuresByImage[j]);
				}
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
		}
	}
	else // Extraction from files
	{		
		totalFeaturesAmount = 0;
		for(i = 0; i < imagesAmount; i++)
		{
			featuresByImage[i] = spDatabaseManagerLoad(config, i, imgFeaturesAmount + i);
			if(featuresByImage[i] == NULL)
			{
				LOGGER_PRINT_ERROR("Failed to load image features from file", __FILE__, __func__, __LINE__);
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imgProc;
				for(j = 0; j < i; j++)
				{					
					for(k = 0; k < imgFeaturesAmount[j]; k++)
						spPointDestroy(featuresByImage[j][k]);
					free(featuresByImage[j]);
				}
				free(featuresByImage);
				free(imgFeaturesAmount);
				return 1;
			}
			totalFeaturesAmount += imgFeaturesAmount[i];
		}
	}

	// ** Main datastructure initilization **

	features = (SPPoint*) malloc(totalFeaturesAmount * sizeof(SPPoint));
	if(features == NULL)
	{
		LOGGER_PRINT_ERROR(ERR_MEM_ALLOCATION, __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imgProc;			
		for(i = 0; i <= imagesAmount; i++)
		{			
			for(j = 0; j < imgFeaturesAmount[i]; j++)
				spPointDestroy(featuresByImage[i][j]);
			free(featuresByImage[i]);
		}
		free(featuresByImage);
		free(imgFeaturesAmount);
		return 1;
	}
	for(i = 0; i < imagesAmount; i++)
	{
		memcpy(features, featuresByImage[i], imgFeaturesAmount[i]);
		for(j = 0; j < imgFeaturesAmount[i]; j++)
			spPointDestroy(featuresByImage[i][j]);
		free(featuresByImage[i]);
	}
	free(imgFeaturesAmount);

	kdTreeSplitMethod = spConfigGetKDTreeSplitMethod(config, &configMsg);
	pcaDim = spConfigGetPCADim(config, &configMsg);

	kdTreeRoot = SPKDTreeInit(features, totalFeaturesAmount, pcaDim, kdTreeSplitMethod, &kdTreeMsg);

	// ** Queries handeling routine **

	knn = spConfigGetKNN(config, &configMsg);
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &configMsg);
	minimalGui = spConfigMinialGui(config, &configMsg);

	printf(ASK_FOR_QUERY);
	scanf("%s", userInput);
	// TODO: replace with checking for 'not a query'
	if(!userInput) // Terminate
	{
		spLoggerPrintInfo(MSG_EXIT);
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imgProc;
		for(i = 0; i < totalFeaturesAmount; i++)
			spPointDestroy(features[i]);
		free(features);
		SPKDTreeDestroy(kdTreeRoot);
		return 0;
	}
	queryFeatures = imgProc->getImageFeatures(userInput, 0, &queryFeaturesAmount);
	if(queryFeatures == NULL)
	{		
		LOGGER_PRINT_ERROR(ERR_GET_IMG_FEATS, __FILE__, __func__, __LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imgProc;
		for(i = 0; i < totalFeaturesAmount; i++)
			spPointDestroy(features[i]);
		free(features);
		SPKDTreeDestroy(kdTreeRoot);
		return 1;
	}

	while(1)
	{
		similarImages = SPQuerySolverSolve(&kdTreeRoot, queryFeatures, queryFeaturesAmount, knn, numOfSimilarImages);

		if(minimalGui) // minimal gui
		{
			for(i = 0; i < numOfSimilarImages; i++)
			{
				configMsg = spConfigGetImagePath(resImagePath, config, i);
				if(configMsg != SP_CONFIG_SUCCESS)
				{
					LOGGER_PRINT_ERROR(ERR_IMG_PATH_INDEX, __FILE__, __func__, __LINE__);
					spConfigDestroy(config);
					spLoggerDestroy();
					delete imgProc;
					for(j = 0; j < totalFeaturesAmount; j++)
						spPointDestroy(features[j]);
					free(features);
					SPKDTreeDestroy(kdTreeRoot);
					free(similarImages);
					for(j = 0; j < queryFeaturesAmount; j++)
						spPointDestroy(queryFeatures[j]);
					free(queryFeatures);
					return 1;
				}
				imgProc->showImage(resImagePath);
			}
		}
		else
		{
			printf(MSG_BEST_CANDIDATES, userInput);
			for(i = 0; i < numOfSimilarImages; i++)
			{
				configMsg = spConfigGetImagePath(resImagePath, config, i);
				if(configMsg != SP_CONFIG_SUCCESS)
				{					
					LOGGER_PRINT_ERROR(ERR_IMG_PATH_INDEX, __FILE__, __func__, __LINE__);
					spConfigDestroy(config);
					spLoggerDestroy();
					delete imgProc;
					for(j = 0; j < totalFeaturesAmount; j++)
						spPointDestroy(features[j]);
					free(features);
					SPKDTreeDestroy(kdTreeRoot);
					free(similarImages);
					for(j = 0; j < queryFeaturesAmount; j++)
						spPointDestroy(queryFeatures[j]);
					free(queryFeatures);
					return 1;
				}
				printf("%s\n", resImagePath);
			}
		}

		free(queryFeatures);
		free(similarImages);
		
		printf(ASK_FOR_QUERY);
		scanf("%s", userInput);
		// TODO: replace
		if(!userInput) // Terminate
		{
			spLoggerPrintInfo(MSG_EXIT);
			spConfigDestroy(config);
			spLoggerDestroy();
			delete imgProc;
			for(i = 0; i < totalFeaturesAmount; i++)
				spPointDestroy(features[i]);
			free(features);
			SPKDTreeDestroy(kdTreeRoot);
			return 0;
		}
		queryFeatures = imgProc->getImageFeatures(userInput, 0, &queryFeaturesAmount);
		if(queryFeatures == NULL)
		{			
			LOGGER_PRINT_ERROR(ERR_GET_IMG_FEATS, __FILE__, __func__, __LINE__);
			spConfigDestroy(config);
			spLoggerDestroy();
			delete imgProc;
			for(i = 0; i < totalFeaturesAmount; i++)
				spPointDestroy(features[i]);
			free(features);
			SPKDTreeDestroy(kdTreeRoot);
			return 1;
		}
	}

	return 0;
}
