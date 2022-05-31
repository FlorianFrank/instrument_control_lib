
 //
// Created by florianfrank on 29.12.20.
//

#include <memory.h>
#include <cstdio>
#include <cerrno>

#if __WIN32
#include <string.h>
#endif

#include "ErrorHandler.h"
#include "ErrorCodeDefines.h"



char errMsgBuff[512];

 const char* PIL_ErrorCodeToString(PIL_ERROR_CODE errorCode)
 {
     switch (errorCode)
     {
         case PIL_NO_ERROR:
             return "Success";
         case PIL_INVALID_ARGUMENTS:
             return "Invalid arguments";
         case PIL_TIMEOUT:
            return "Socket timeout";
         case PIL_ERRNO:
         {
             sprintf(errMsgBuff, "Errno %d (%s)", errno, strerror(errno));
             return errMsgBuff;
         }
         case PIL_INTERFACE_CLOSED:
             return "Socket is closed";
         case PIL_INVALID_BAUDRATE:
             return "Baudrate not supported";
         case PIL_INSUFFICIENT_RESOURCES:
             return "Insufficient resources";
         case PIL_DEADLOCK_DETECTED:
             return "Deadlock detected";
         case PIL_THREAD_NOT_JOINABLE:
             return "Thread not joinable";
         case PIL_THREAD_NOT_FOUND:
             return "Thread not found";
         case PIL_ONLY_PARTIALLY_READ_WRITTEN:
             return "File only partially written";
         case PIL_NO_SUCH_FILE:
             return "No such file or directory";
         case PIL_XML_PARSING_ERROR:
             return "Error while parsing XML file";
         default:
             return "Unknown error";
     }
     return "Unknown error";
 }

 bool PIL_ReturnErrorMessage(char *errorStr, PIL_ErrorHandle *socketStruct)
 {
    if(!errorStr)
        return false;

    if(!socketStruct)
    {
        strcpy(errorStr, "socketStruct == NULL");
        return false;
    }

    const char *errCodeStr = PIL_ErrorCodeToString(socketStruct->m_ErrorCode);
    if(socketStruct->m_ErrorCode == PIL_ERRNO)
    {
        sprintf(errorStr, "%s: %s", errCodeStr, strerror(socketStruct->m_ErrnoCode));
        return true;
    }
    if(strcmp(socketStruct->m_ErrorMessage, "") != 0)
    {
        sprintf(errorStr, "%s: %s", errCodeStr, socketStruct->m_ErrorMessage);
        return true;
    }
    strcpy(errorStr, errCodeStr);
    return true;
 }

 bool PIL_SetLastError(PIL_ErrorHandle *errStruct, PIL_ERROR_CODE errorCode)
 {
     errStruct->m_ErrorCode = errorCode;
    if(errorCode == PIL_ERRNO)
    {
        errStruct->m_ErrnoCode = errno;
    }
    strcpy(errStruct->m_ErrorMessage, "");

    if(errorCode == PIL_NO_ERROR)
        return true;
     return false;
 }


 const char* PIL_ReturnErrorMessage(PIL_ErrorHandle *errorHandle)
 {
     PIL_ReturnErrorMessage(errMsgBuff, errorHandle);
     return errMsgBuff;
 }

 bool PIL_SetLastErrorMsg(PIL_ErrorHandle *errStruct, PIL_ERROR_CODE errorCode, const char* errorMessage)
 {
    if(!errStruct)
        return false;

    if(strlen(errorMessage) > MAX_ERROR_MSG_LEN)
        return false;

    strcpy(errStruct->m_ErrorMessage, errorMessage);
     errStruct->m_ErrorCode = errorCode;

    return true;
 }

