#include "SPConfig.h"

#define MAX_LEN 1024

// Macros for printing errors
#define PRINT_ERROR(FILE, LINE, MSG) printf("File:%s\nLine:%d\nMessage:%s\n", FILE, LINE, MSG);
#define ERR_MSG_INVALID_LINE "Invalid configuration line"
#define ERR_MSG_VALUE_CONSTRAINT "Invalid value - constraint not met"
#define ERR_MSG_PARAMETER(PARAM_NAME) "Parameter PARAM_NAME is not set"

// Variable names
#define IMAGES_DIRECTORY "spImagesDirectory"
#define IMAGES_PREFIX "spImagesPrefix"
#define IMAGES_SUFFIX "spImagesSuffix"
#define NUM_IMAGES "spNumOfImages"
#define PCA_DIM "spPCADimension"
#define PCA_FILENAME "spPCAFilename"
#define NUM_FEATURES "spNumOfFeatures"
#define EXTRACTION_MODE "spExtractionMode"
#define NUM_SIMILAR_IMAGES "spNumOfSimilarImages"
#define KDTREE_SPLIT_METHOD "spKDTreeSplitMethod"
#define KNN "spKNN"
#define MINIMAL_GUI "spMinimalGUI"
#define LOGGER_LEVEL "spLoggerLevel"
#define LOGGER_FILENAME "spLoggerFilename"

#define IS_VALID_SUFFIX(STRING) (strcmp(STRING, ".jpg") == 0 || strcmp(STRING, ".png") == 0 \
		|| strcmp(STRING, ".bmp") == 0 || strcmp(STRING, ".gif") == 0)

// Values
#define TRUE_STRING "true"
#define FALSE_STRING "false"

#define KDTREE_SPLIT_RANDOM "RANDOM"
#define KDTREE_SPLIT_MAX_SPREAD "MAX_SPREAD"
#define KDTREE_SPLIT_INCREMENTAL "INCREMENTAL"

// Constraints
#define MIN_DIM 10
#define MAX_DIM 28

// Default values
#define DEF_PCA_DIM 20
#define DEF_PCA_FILENAME "pca.yml"
#define DEF_NUM_FEATURES 100
#define DEF_EXTRACTION_MODE true
#define DEF_NUM_SIMILAR_IMAGES 1
#define DEF_KDTREE_SPLIT_METHOD SP_KDTREE_MAX_SPREAD
#define DEF_KNN 1
#define DEF_MINIMAL_GUI false
#define DEF_LOGGER_LEVEL 3
#define DEF_LOGGER_FILENAME "stdout"

// A struct representing the configuration
struct sp_config_t 
{
	char spImagesDirectory[MAX_LEN];
	char spImagesPrefix[MAX_LEN];
	char spImagesSuffix[MAX_LEN];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[MAX_LEN];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KDTREE_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char spLoggerFilename[MAX_LEN];
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	// To be returned
	SPConfig config = NULL;
	
	// To be used during run
	FILE* file = NULL; 
	char line[MAX_LEN];
	char *varName, *varValue;
	int lineNum; 
	int i;
	int varNameStart, varNameLen;
	int varValueStart, varValueLen;
	int numberValue;
	
	// These help track what is initialized
	bool spImagesDirectoryInit = false;
	bool spImagesPrefixInit = false;
	bool spImagesSuffixInit = false;
	bool spNumOfImagesInit = false;
	bool spPCADimensionInit = false;
	bool spPCAFilenameInit = false;
	bool spNumOfFeaturesInit = false;
	bool spExtractionModeInit = false;
	bool spNumOfSimilarImagesInit = false;
	bool spKDTreeSplitMethodInit = false;
	bool spKNNInit = false;
	bool spMinimalGUIInit = false;
	bool spLoggerLevelInit = false;
	bool spLoggerFilenameInit = false;
	
	assert(msg != NULL);
	if (filename == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	file = fopen(filename, "r");
	if (file == NULL)
	{
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	config = (SPConfig)malloc(sizeof(*config));
	if (config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	
	lineNum = 0;
	while (fgets(line, MAX_LEN, file))
	{
		i = 0;
		lineNum++;
		while (line[i] == ' ' || line[i] == '\t') // Ignore whitespace before variable name
			i++;
		if (line[i] == '#' || line[i] == '\n' || (line[i] == '\r' && line[i+1] == '\n')) // Either comment or empty line
			continue;
		varNameStart = i;
		while (line[i] != ' ' && line[i] != '\t' && line[i] != '=') // Read variable name
		{
			if (line[i] == '\n' || (line[i] == '\r' && line[i+1] == '\n')) // If line ends amidst variable name
			{
				printf("1the line: {%s}\n{%d}\n", line, i);
				printf("The first: {%d}\n", (int)line[0]);
				PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
				free(config);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			i++;
		}
		varNameLen = i - varNameStart;
		while (line[i] == ' ' || line[i] == '\t') // Ignore whitespace after variable name
			i++;
		if (line[i] != '=') // Next non-whitespace character must be equals
		{			
			printf("2the line: {%s}\n", line);
			PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
			free(config);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		i++;
		while (line[i] == ' ' || line[i] == '\t') // Ignore whitespace before variable value
			i++;
		varValueStart = i;
		while (line[i] != ' ' && line[i] != '\t' && line[i] != '\n' && (line[i] != '\r' || line[i+1] != '\n')) // Read variable value
			i++;
		varValueLen = i - varValueStart;
		while (line[i] == ' ' || line[i] == '\t') // Ignore whitespace after variable value
			i++;
		if (line[i] != '\n'  && (line[i] != '\r' || line[i+1] != '\n')) // Next non-whitespace character must be new line
		{
			printf("3the line: {%s}\n", line);
			PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
			free(config);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		varName = (char*)malloc((varNameLen + 1) * sizeof(char));
		varValue = (char*)malloc((varValueLen + 1) * sizeof(char));
		if (varName == NULL || varValue == NULL) 
		{
			free(config);
			free(varName);
			free(varValue);
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}
		strncpy(varName, line + varNameStart, varNameLen); // Copy variable name
		varName[varNameLen] = '\0'; // Add null terminator
		strncpy(varValue, line + varValueStart, varValueLen); // Copy variable value
		varValue[varValueLen] = '\0'; // Add null terminator
		
		// Check which variable the line declares and set its value
		if (strcmp(varName, IMAGES_DIRECTORY) == 0)
		{
			sprintf(config->spImagesDirectory, "%s", varValue); // just copy the value - same is done for all strings
			spImagesDirectoryInit = true;
		}
		else if (strcmp(varName, IMAGES_PREFIX) == 0)
		{
			sprintf(config->spImagesPrefix, "%s", varValue);
			spImagesPrefixInit = true;
		}
		else if (strcmp(varName, IMAGES_SUFFIX) == 0)
		{
			if (!IS_VALID_SUFFIX(varValue)) // check suffix is one of the options
			{
				printf("suf value: {%s}\n", varValue);
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			sprintf(config->spImagesSuffix, "%s", varValue);
			spImagesSuffixInit = true;
		}
		else if (strcmp(varName, NUM_IMAGES) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++) // check all chars of the value are digits - same is done for all integers
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spNumOfImages = numberValue;
			spNumOfImagesInit = true;
		}
		else if (strcmp(varName, PCA_DIM) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++)
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spPCADimension = numberValue;
			spPCADimensionInit = true;
		}
		else if (strcmp(varName, PCA_FILENAME) == 0)
		{
			sprintf(config->spPCAFilename, "%s", varValue);
			spPCAFilenameInit = true;
		}
		else if (strcmp(varName, NUM_FEATURES) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++) 
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spNumOfFeatures = numberValue;
			spNumOfFeaturesInit = true;
		}
		else if (strcmp(varName, EXTRACTION_MODE) == 0)
		{
			if (strcmp(varValue, TRUE_STRING) == 0) // check value is boolean - same is done for all booleans
			{
				config->spExtractionMode = true;
			}
			else if (strcmp(varValue, FALSE_STRING) == 0)
			{
				config->spExtractionMode = false;
			}
			else
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			spExtractionModeInit = true;
		}
		else if (strcmp(varName, NUM_SIMILAR_IMAGES) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++) 
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spNumOfSimilarImages = numberValue;
			spNumOfSimilarImagesInit = true;
		}
		else if (strcmp(varName, KDTREE_SPLIT_METHOD) == 0)
		{
			if (strcmp(varValue, KDTREE_SPLIT_RANDOM) == 0) // check value is one of the options
			{
				config->spKDTreeSplitMethod = SP_KDTREE_RANDOM;
			}
			else if (strcmp(varValue, KDTREE_SPLIT_MAX_SPREAD) == 0)
			{
				config->spKDTreeSplitMethod = SP_KDTREE_MAX_SPREAD;
			}
			else if (strcmp(varValue, KDTREE_SPLIT_INCREMENTAL) == 0)
			{
				config->spKDTreeSplitMethod = SP_KDTREE_INCREMENTAL;
			}
			else
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			spKDTreeSplitMethodInit = true;
		}
		else if (strcmp(varName, KNN) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++) 
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spKNN = numberValue;
			spKNNInit = true;
		}
		else if (strcmp(varName, MINIMAL_GUI) == 0)
		{
			if (strcmp(varValue, TRUE_STRING) == 0)
			{
				config->spMinimalGUI = true;
			}
			else if (strcmp(varValue, FALSE_STRING) == 0)
			{
				config->spMinimalGUI = false;
			}
			else
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_STRING;
				return NULL;
			}
			spMinimalGUIInit = true;
		}
		else if (strcmp(varName, LOGGER_LEVEL) == 0)
		{
			for (i = 0; i < (int)strlen(varValue); i++) 
			{
				if (!isdigit(varValue[i]))
				{
					PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
					free(config);
					free(varName);
					free(varValue);
					*msg = SP_CONFIG_INVALID_INTEGER;
					return NULL;
				}
			}
			numberValue = atoi(varValue);
			config->spLoggerLevel = numberValue;
			spLoggerLevelInit = true;
		}
		else if (strcmp(varName, LOGGER_FILENAME) == 0)
		{
			sprintf(config->spLoggerFilename, "%s", varValue);
			spLoggerFilenameInit = true;
		}
		else // line declares an illegal variable
		{
			PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
			free(config);
			free(varName);
			free(varValue);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		free(varName);
		free(varValue);
	} // Now done reading the file
	
	if (!feof(file))
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
		free(config);
		*msg = SP_CONFIG_INVALID_STRING;
		return NULL;
	}
	
	// Check all non-default variables were initialized
	if (!spImagesDirectoryInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER(IMAGES_DIRECTORY));
		free(config);
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	if (!spImagesPrefixInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER(IMAGES_PREFIX));
		free(config);
		*msg = SP_CONFIG_MISSING_PREFIX;
		return NULL;
	}
	if (!spImagesSuffixInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER(IMAGES_SUFFIX));
		free(config);
		*msg = SP_CONFIG_MISSING_SUFFIX;
		return NULL;
	}
	if (!spNumOfImagesInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER(NUM_IMAGES));
		free(config);
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		return NULL;
	}
	
	// If other variables were not initialized, set to default value
	if (!spPCADimensionInit)
		config->spPCADimension = DEF_PCA_DIM;
	if (!spPCAFilenameInit)
		sprintf(config->spPCAFilename, DEF_PCA_FILENAME);
	if (!spNumOfFeaturesInit)
		config->spNumOfFeatures = DEF_NUM_FEATURES;
	if (!spExtractionModeInit)
		config->spExtractionMode = DEF_EXTRACTION_MODE;
	if (!spNumOfSimilarImagesInit)
		config->spNumOfSimilarImages = DEF_NUM_SIMILAR_IMAGES;
	if (!spKDTreeSplitMethodInit)
		config->spKDTreeSplitMethod = DEF_KDTREE_SPLIT_METHOD;
	if (!spKNNInit)
		config->spKNN = DEF_KNN;
	if (!spMinimalGUIInit)
		config->spMinimalGUI = DEF_MINIMAL_GUI;
	if (!spLoggerLevelInit)
		config->spLoggerLevel = DEF_LOGGER_LEVEL;
	if (!spLoggerFilenameInit)
		sprintf(config->spLoggerFilename, DEF_LOGGER_FILENAME);
	
	// All done
	*msg = SP_CONFIG_SUCCESS;
	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL) 
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;
}

SP_KDTREE_SPLIT_METHOD spConfigGetKDTreeSplitMethod(SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return SP_KDTREE_MAX_SPREAD;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKDTreeSplitMethod;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

int spConfigGetLoggerLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spLoggerLevel;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG * msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

int spConfigGetKNN(const SPConfig config, SP_CONFIG_MSG * msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

SP_CONFIG_MSG spConfigGetLoggerFilename(char* loggerFilename, const SPConfig config)
{
	if (config == NULL || loggerFilename == NULL)
		return SP_CONFIG_INVALID_ARGUMENT;
	sprintf(loggerFilename, "%s", config->spLoggerFilename);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index)
{
	if (config == NULL || imagePath == NULL || index < 0)
		return SP_CONFIG_INVALID_ARGUMENT;
	if (index >= config->spNumOfImages)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory, config->spImagesPrefix, index, config->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetFeatsPath(char * featsPath, const SPConfig config, int index)
{
	if (config == NULL || featsPath == NULL || index < 0)
		return SP_CONFIG_INVALID_ARGUMENT;
	if (index >= config->spNumOfImages)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	sprintf(featsPath, "%s%s%d.feats", config->spImagesDirectory, config->spImagesPrefix, index);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	if (config == NULL || pcaPath == NULL)
		return SP_CONFIG_INVALID_ARGUMENT;
	sprintf(pcaPath, "%s", config->spPCAFilename);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config)
{
	if (config != NULL)
		free(config);
}
