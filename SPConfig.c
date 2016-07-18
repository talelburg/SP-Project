#include "SPConfig.h"
#define MAX_LEN 1024
#define PRINT_ERROR(FILE, LINE, MSG) printf("File: FILE\nLine: LINE\nMessage: MSG\n");
#define ERR_MSG_INVALID_LINE "Invalid configuration line"
#define ERR_MSG_VALUE_CONSTRAINT "Invalid value - constraint not met"
#define ERR_MSG_PARAMETER(PARAM_NAME) "Parameter PARAM_NAME is not set"
typedef enum method {
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
} Method;

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
	Method spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char spLoggerFilename[MAX_LEN];
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	SPConfig config = NULL;
	FILE* file = NULL;
	char line[MAX_LEN];
	char numberChecker[MAX_LEN];
	char* varName;
	char* varValue;
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
	int lineNum; 
	int i;
	int varNameStart, varNameLen;
	int varValueStart, varValueLen;
	int numberValue;
	assert(msg != NULL);
	if (filename = NULL)
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
		lineNum++;
		while (line[i] == ' ' || line[i] == '\t') 
			i++;
		if (line[i] == '#')
		{
			continue;
		}
		varNameStart = i;
		while (line[i] != ' ' && line[i] != '\t' && line[i] != '=')
			i++;
		varNameLen = i - varNameStart + 1;
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (line[i] != '=') 
		{
			PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
			free(config);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		varValueStart = i;
		while (line[i] != ' ' && line[i] != '\t')
			i++;
		varValueLen = i - varValueStart + 1;
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (line[i] != '\n')
		{
			PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
			free(config);
			*msg = SP_CONFIG_INVALID_STRING;
			return NULL;
		}
		varName = (char*)malloc((varNameLen + 1) * sizeof(char));
		varValue = (char*)malloc((varValueLen + 1) * sizeof(char));
		if (varName == NULL || varValue == NULL) 
		{
			printf("Allocation failure");
			free(config);
			free(varName);
			free(varValue);
			*msg = SP_CONFIG_ALLOC_FAIL;
			return NULL;
		}
		strncpy(varName, line + varNameStart, varNameLen);
		varName[varNameLen] = '\0';
		strncpy(varValue, line + varValueStart, varNameLen);
		varValue[varValueLen] = '\0';
		if (strcmp(varName, "spImagesDirectory") == 0)
		{
			sprintf(config->spImagesDirectory, "%s", varValue);
			spImagesDirectoryInit = true;
		}
		else if (strcmp(varName, "spImagesPrefix") == 0)
		{
			sprintf(config->spImagesPrefix, "%s", varValue);
			spImagesPrefixInit = true;
		}
		else if (strcmp(varName, "spImagesSuffix") == 0)
		{
			if (strcmp(varValue, ".jpg") != 0 && strcmp(varValue, ".png") != 0 && strcmp(varValue, ".bmp") != 0 && strcmp(varValue, ".gif") != 0)
			{
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
		else if (strcmp(varName, "spNumOfImages") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 0)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spNumOfImages = numberValue;
			spNumOfImagesInit = true;
		}
		else if (strcmp(varName, "spPCADimension") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 9 || numberValue >= 29)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spPCADimension = numberValue;
			spPCADimensionInit = true;
		}
		else if (strcmp(varName, "spPCAFilename") == 0)
		{
			sprintf(config->spPCAFilename, "%s", varValue);
			spPCAFilenameInit = true;
		}
		else if (strcmp(varName, "spNumOfFeatures") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 0)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spNumOfFeatures = numberValue;
			spNumOfFeaturesInit = true;
		}
		else if (strcmp(varName, "spExtractionMode") == 0)
		{
			if (strcmp(varValue, "true") == 0)
			{
				config->spExtractionMode = true;
				spExtractionModeInit = true;
			}
			else if (strcmp(varValue, "false") == 0)
			{
				config->spExtractionMode = false;
				spExtractionModeInit = true;
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
		}
		else if (strcmp(varName, "spNumOfSimilarImages") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 0)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spNumOfSimilarImages = numberValue;
			spNumOfSimilarImagesInit = true;
		}
		else if (strcmp(varName, "spKDTreeSplitMethod") == 0)
		{
			if (strcmp(varValue, "RANDOM") == 0)
			{
				config->spKDTreeSplitMethod = RANDOM;
				spKDTreeSplitMethodInit = true;
			}
			else if (strcmp(varValue, "MAX_SPREAD") == 0)
			{
				config->spKDTreeSplitMethod = MAX_SPREAD;
				spKDTreeSplitMethodInit = true;
			}
			else if (strcmp(varValue, "INCREMENTAL") == 0)
			{
				config->spKDTreeSplitMethod = INCREMENTAL;
				spKDTreeSplitMethodInit = true;
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
		}
		else if (strcmp(varName, "spKNN") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 0)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spKNN = numberValue;
			spKNNInit = true;
		}
		else if (strcmp(varName, "spMinimalGUI") == 0)
		{
			if (strcmp(varValue, "true") == 0)
			{
				config->spMinimalGUI = true;
				spMinimalGUIInit = true;
			}
			else if (strcmp(varValue, "false") == 0)
			{
				config->spMinimalGUI = false;
				spMinimalGUIInit = true;
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
		}
		else if (strcmp(varName, "spLoggerLevel") == 0)
		{
			numberValue = atoi(varValue);
			sprintf(numberChecker, "%d", numberValue);
			if (strcmp(numberChecker, varValue) != 0 || numberValue <= 0 || numberValue >= 5)
			{
				PRINT_ERROR(filename, lineNum, ERR_MSG_VALUE_CONSTRAINT);
				free(config);
				free(varName);
				free(varValue);
				*msg = SP_CONFIG_INVALID_INTEGER;
				return NULL;
			}
			config->spLoggerLevel = numberValue;
			spLoggerLevelInit = true;
		}
		else if (strcmp(varName, "spLoggerFilename") == 0)
		{
			sprintf(config->spLoggerFilename, "%s", varValue);
			spLoggerFilenameInit = true;
		}
		else 
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
	}
	if (!feof(file))
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_INVALID_LINE);
		free(config);
		*msg = SP_CONFIG_INVALID_STRING;
		return NULL;
	}
	if (!spImagesDirectoryInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER("spImagesDirctory"));
		free(config);
		*msg = SP_CONFIG_MISSING_DIR;
		return NULL;
	}
	if (!spImagesPrefixInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER("spImagesPrefix"));
		free(config);
		*msg = SP_CONFIG_MISSING_PREFIX;
		return NULL;
	}
	if (!spImagesSuffixInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER("spImagesSuffix"));
		free(config);
		*msg = SP_CONFIG_MISSING_SUFFIX;
		return NULL;
	}
	if (!spNumOfImagesInit)
	{
		PRINT_ERROR(filename, lineNum, ERR_MSG_PARAMETER("spNumOfImages"));
		free(config);
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		return NULL;
	}
	if (!spPCADimensionInit)
		config->spPCADimension = 20;
	if (!spPCAFilenameInit)
		sprintf(config->spPCAFilename, "pca.yml");
	if (!spNumOfFeaturesInit)
		config->spNumOfFeatures = 100;
	if (!spExtractionModeInit)
		config->spExtractionMode = true;
	if (!spNumOfSimilarImagesInit)
		config->spNumOfSimilarImages = 1;
	if (!spKDTreeSplitMethodInit)
		config->spKDTreeSplitMethod = MAX_SPREAD;
	if (!spKNNInit)
		config->spKNN = 1;
	if (!spMinimalGUIInit)
		config->spMinimalGUI = false;
	if (!spLoggerLevelInit)
		config->spLoggerLevel = 3;
	if (!spLoggerFilenameInit)
		sprintf(config->spLoggerFilename, "stdout");
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

bool spConfigMinialGui(const SPConfig config, SP_CONFIG_MSG* msg)
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

Method spConfigGetKDTreeSplitMethod(SPConfig config, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return MAX_SPREAD;
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
	sprintf(loggerFilename, "%s%s", config->spImagesDirectory, config->spLoggerFilename);
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
	sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spImagesPrefix);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config)
{
	if (config != NULL)
		free(config);
}
