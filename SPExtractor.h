#ifndef SPEXTRACTOR_H_
#define SPEXTRACTOR_H_

#include "SPConfig.h"
#include "SPPoint.h"
#include "SPImageProc.h"

/**

*/
SPPoint* spExtractorExtract(SPConfig config, int* featuresAmount);

SPPoint* spExtractorFromImages(SPConfig config, int* featuresAmount);

SPPoint* spExtractorFromFiles(SPConfig config, int* featuresAmount);

#endif