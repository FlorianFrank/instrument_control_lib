//
// Created by florianfrank on 28.10.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_DEVICEDISCOVERY_HPP
#define INSTRUMENT_CONTROL_LIB_DEVICEDISCOVERY_HPP


#include <string>
#include <vector>
#include "ctlib/SocketDefines.h"
#include "ctlib/Logging.hpp"
#include "Device.h"

struct IPRange{
    std::vector<uint8_t> m_startIPRange;
    std::vector<uint8_t> m_stopIPRange;
} typedef IPRange;

class DeviceDiscovery
{
public:
    DeviceDiscovery(std::string &interface, PIL::Logging *logging = nullptr);
    ~DeviceDiscovery();
    std::vector<Device> startDiscovery();

private:
    IPRange getAddressRange(std::string &ip, std::string &mask);
    PIL_ERROR_CODE setDeviceList();
    std::vector<Device> testIRange(IPRange& ipRange);


    InterfaceInfoList m_DeviceList{};
    std::string m_InterfaceName;
    PIL::Logging *m_Logging;
};


#endif //INSTRUMENT_CONTROL_LIB_DEVICEDISCOVERY_HPP
