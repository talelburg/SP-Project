#ifndef SPDATABASEMANAGER_H_
#define SPDATABASEMANAGER_H_

#include <stdlib.h>
#include "SPPoint.h"
#include "SPConfig.h"


bool spDatabaseManagerSave(SPConfig config, int index, int featuresAmount, SPPoint* features);

SPPoint* spDatabaseManagerLoad(SPConfig config, int index, int* featuresAmount);

#endif