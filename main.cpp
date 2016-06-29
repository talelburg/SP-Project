#include <cstdlib>
extern "C"
{
#include "SPConfig.h"
#include "SPPoint.h"
#include "SPLogger.h"
}
#include "SPImageProc.h"

#define PRINT_ERROR(FILE, LINE, MSG) printf("File: FILE\nLine: LINE\nMessage: MSG");
#define ERR_MSG_INVALID_LINE "Invalid configuration line"
#define ERR_MSG_VALUE_CONSTRAINT "Invalid value - constraint not met"
#define ERR_MSG_PARAMETER(PARAM_NAME) "Parameter PARAM_NAME is not set"

int main()
{
	char* configFileName = "Temp";
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;

	SPConfig config = spConfigCreate(configFileName, &msg);

	return 0;
}