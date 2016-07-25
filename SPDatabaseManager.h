#ifndef SPDATABASEMANAGER_H_
#define SPDATABASEMANAGER_H_

#include <stdlib.h>
#include "SPPoint.h"
#include "SPConfig.h"

/*
 * Saves a .feats file that encodes the given image features
 * The .feats file path is spConfigGetFeatsPath for the given image index
 *
 * @param config - the configuration file
 * @param index - the index of the image
 * @param featuresAmount - the length of 'features'
 * @param features - the features that will be written
 * @return  true - on success
			false - if an error occurred
*/
bool spDatabaseManagerSave(SPConfig config, int index, int featuresAmount, SPPoint* features);

/*
 * Loads image features from a .feats file
 * The .feats file path is spConfigGetFeatsPath for the given image index
 *
 * @param config - the configuration file
 * @param index - the index of the image
 * @param featuresAmount - the amount of features that were read
 * @return  The decoded features - on success
			NULL - if an error occurred
*/
SPPoint* spDatabaseManagerLoad(SPConfig config, int index, int* featuresAmount);

#endif
