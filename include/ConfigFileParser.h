//
// Created by florianfrank on 30.05.22.
//

#ifndef CE_DEVICE_CONFIGFILEPARSER_H
#define CE_DEVICE_CONFIGFILEPARSER_H

#include "ErrorHandler.h"
#include "ConfigFileDefines.h"
#include <vector>


class ConfigFileParser
{
public:
    static PIL_ERROR_CODE ParseSupportedDevices(std::string &configFile, std::vector<DeviceDescription> &configFiles);

};


#endif //CE_DEVICE_CONFIGFILEPARSER_H
