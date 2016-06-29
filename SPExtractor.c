#include "SPExtractor.h"

SPPoint* spExtractorExtract(SPConfig config, int* featuresAmount)
{
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	bool isExtractionMode = spConfigIsExtractionMode(config, &msg);
	if(msg != SP_CONFIG_SUCCESS)
	{
		// Return error!
		return NULL;
	}
	if(isExtractionMode)
		return spExtractorFromImages(config, featuresAmount);
	else
		return spExtractorFromFiles(config, featuresAmount);
}

SPPoint* spExtractorFromImages(SPConfig config, int* featuresAmount)
{

}

SPPoint* spExtractorFromFiles(SPConfig config, int* featuresAmount)
{
}