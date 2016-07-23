#ifndef SPDATABASEMANAGER_H_
#define SPDATABASEMANAGER_H_

#include <stdlib.h>
#include "SPPoint.h"
#include "SPConfig.h"

/*
 * Saves a .feats file that encodes the given image data
 *
 * @param config - the configuration file
 * @param index - the index of the image
 * @param featuresAmount - amounts of features to save
 * @param features - the features that will be wrriten
 * @return  true - on success
			false - if an error occured while saving
*/
bool spDatabaseManagerSave(SPConfig config, int index, int featuresAmount, SPPoint* features);

SPPoint* spDatabaseManagerLoad(SPConfig config, int index, int* featuresAmount);

#endif