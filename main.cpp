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
#include <string>

using namespace sp;

#define PRINT_ERROR(HUMAN_MSG) printf("Error: " HUMAN_MSG)
#define LOGGER_PRINT_ERROR(HUMAN_MSG, FILE, FUNCTION, LINE) spLoggerPrintError("Error: " HUMAN_MSG, FILE, FUNCTION, LINE)

#define ARG_CONFIG "-c"
#define DEF_CONFIG_FILE "spcbir.config"
#define STDOUT_NAME "stdout"

#define ERR_INV_CMD_LINE "Invalid command line : use -c <config_filename>\n"
#define ERR_OPEN_CONFIG "The configuration file %s couldn't be open\n"
#define ERR_OPEN_DEF_CONFIG "The default configuration file spcbir.config couldn't be open\n"
#define ERR_LOGGER_OUT_OF_MEMORY "Logger is out of memory\n"
#define ERR_LOGGER_CANNOT_OPEN_FILE "Logger failed to open file\n"
#define ERR_LOGGER_DEFINED "Logger is already defined\n"
#define ERR_MEM_ALLOCATION "Memory allocation failed\n"
#define ERR_GET_IMG_FEATS "Failed to get image features\n"
#define ERR_GET_IMG_PATH "Failed to get image path\n"
#define ERR_SAVE_FAILED "Failed to save features to database\n"
#define ERR_EXTRACT_FAILED "Failed to extract image features\n"
#define ERR_LOAD_FAILED "Failed to load image features from file\n"
#define ERR_QUERY_FAILED "Failed to solve query\n"

#define MSG_ASK_FOR_QUERY "Please enter an image path:\n"
#define MSG_BEST_CANDIDATES "Best candidates for - %s - are:\n"
#define MSG_EXIT "Exiting..."

#define EXIT_INPUT "<>"
#define STRING_LEN (1024)

int main(int argc, char** argv)
{
	// ** Variables deceleration **

	// Index variables
	int i = 0, j = 0, k = 0;

	// Config and Logger init variables
	char loggerFileName[STRING_LEN];
	SP_CONFIG_MSG configMsg = SP_CONFIG_SUCCESS;
	SP_LOGGER_MSG loggerMsg = SP_LOGGER_SUCCESS;
	SPConfig config;

	// Config data variables
	int loggerLevel = 0;
	bool isExtractionMode = 0;
	int imagesAmount = 0;
	char imagePath[STRING_LEN];
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

	// Main data structure variables
	SPKDTreeNode kdTreeRoot;
	SP_KDTREE_MSG kdTreeMsg = SP_KDTREE_SUCCESS;
	SPPoint* features;

	// Query variables
	int* similarImages;
	char userInput[STRING_LEN];
	SPPoint* queryFeatures;
	int queryFeaturesAmount;
	char resImagePath[STRING_LEN];

	// ** Config and Logger initialization **

	if(argc == 1) // No arguments
	{
		config = spConfigCreate(DEF_CONFIG_FILE, &configMsg);
	}
	else if(argc == 3) // Possibly in the format: -c <config_filename>
	{
		if(strcmp(argv[1], ARG_CONFIG) != 0)
		{
			printf(ERR_INV_CMD_LINE);
			return 1;
		}
		config = spConfigCreate(argv[2], &configMsg);
	}
	else // Invalid command line
	{
		printf(ERR_INV_CMD_LINE);
		return 1;
	}

	if(configMsg != SP_CONFIG_SUCCESS)
	{
		if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
		{
			if(argc == 1) // If no arguments
				printf(ERR_OPEN_DEF_CONFIG);
			else // If -c was used to give a configuration file
				printf(ERR_OPEN_CONFIG, argv[2]);
		}
		spConfigDestroy(config);
		return 1;
	}

	configMsg = spConfigGetLoggerFilename(loggerFileName, config);
	loggerLevel = spConfigGetLoggerLevel(config, &configMsg);

	if(strcmp(loggerFileName, STDOUT_NAME) == 0)
		loggerMsg = spLoggerCreate(NULL, (SP_LOGGER_LEVEL)loggerLevel);
	else
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
				LOGGER_PRINT_ERROR(ERR_EXTRACT_FAILED, __FILE__, __func__, __LINE__);
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
				LOGGER_PRINT_ERROR(ERR_EXTRACT_FAILED, __FILE__, __func__, __LINE__);
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
				LOGGER_PRINT_ERROR(ERR_SAVE_FAILED, __FILE__, __func__, __LINE__);
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
				LOGGER_PRINT_ERROR(ERR_LOAD_FAILED, __FILE__, __func__, __LINE__);
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

	// ** Main data structure initialization **

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

	k = 0;
	for(i = 0; i < imagesAmount; i++)
	{
		memcpy(features + k, featuresByImage[i], imgFeaturesAmount[i] * sizeof(SPPoint));
		k += imgFeaturesAmount[i];
		free(featuresByImage[i]);
	}
	free(featuresByImage);
	free(imgFeaturesAmount);

	kdTreeSplitMethod = spConfigGetKDTreeSplitMethod(config, &configMsg);
	pcaDim = spConfigGetPCADim(config, &configMsg);

	kdTreeRoot = SPKDTreeInit(features, totalFeaturesAmount, pcaDim, kdTreeSplitMethod, &kdTreeMsg);

	// ** Queries handling routine **

	knn = spConfigGetKNN(config, &configMsg);
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &configMsg);
	minimalGui = spConfigMinimalGui(config, &configMsg);

	printf(MSG_ASK_FOR_QUERY);
	scanf("%s", userInput);
	if(strcmp(userInput, EXIT_INPUT) == 0) // Clean exit
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
		similarImages = SPQuerySolverSolve(kdTreeRoot, queryFeatures, queryFeaturesAmount, knn, numOfSimilarImages, imagesAmount);
		if(similarImages == NULL)
		{
			LOGGER_PRINT_ERROR(ERR_QUERY_FAILED, __FILE__, __func__, __LINE__);
			spConfigDestroy(config);
			spLoggerDestroy();
			delete imgProc;
			for(i = 0; i < totalFeaturesAmount; i++)
				spPointDestroy(features[i]);
			free(features);
			SPKDTreeDestroy(kdTreeRoot);
			for(i = 0; i < queryFeaturesAmount; i++)
				spPointDestroy(queryFeatures[i]);
			free(queryFeatures);
			return 1;
		}

		if(minimalGui) // Minimal GUI
		{
			for(i = 0; i < numOfSimilarImages; i++)
			{
				configMsg = spConfigGetImagePath(resImagePath, config, similarImages[i]);
				if(configMsg != SP_CONFIG_SUCCESS)
				{
					LOGGER_PRINT_ERROR(ERR_GET_IMG_PATH, __FILE__, __func__, __LINE__);
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
		else // No minimal GUI
		{
			printf(MSG_BEST_CANDIDATES, userInput);
			for(i = 0; i < numOfSimilarImages; i++)
			{
				configMsg = spConfigGetImagePath(resImagePath, config, similarImages[i]);
				if(configMsg != SP_CONFIG_SUCCESS)
				{					
					LOGGER_PRINT_ERROR(ERR_GET_IMG_PATH, __FILE__, __func__, __LINE__);
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

		for(i = 0; i < queryFeaturesAmount; i++)
			spPointDestroy(queryFeatures[i]);
		free(queryFeatures);
		free(similarImages);
		
		// Read next query

		printf(MSG_ASK_FOR_QUERY);
		scanf("%s", userInput);
		if(strcmp(userInput, EXIT_INPUT) == 0) // Clean exit
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
