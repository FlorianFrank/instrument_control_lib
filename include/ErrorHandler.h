//
// Created by florianfrank on 29.12.20.
//

#ifndef PLATFORMINDEPENDENTLIB_ERRORHANDLER_H
#define PLATFORMINDEPENDENTLIB_ERRORHANDLER_H

#include "ErrorCodeDefines.h"
#include <stdint.h>

const char* PIL_ErrorCodeToString(PIL_ERROR_CODE errorCode);

bool PIL_SetLastError(PIL_ErrorHandle *socketStruct, PIL_ERROR_CODE errorCode);

bool PIL_SetLastErrorMsg(PIL_ErrorHandle *errStruct, PIL_ERROR_CODE errorCode, const char* errorMessage);

const char* PIL_ReturnErrorMessage(PIL_ErrorHandle *errorHandle);

#endif //PLATFORMINDEPENDENTLIB_ERRORHANDLER_H
