/**
 * @brief This file contains the code to discover measurement devices in the network.
 * @author Florian Frank
 * @copyright University of Passau
 */
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
    explicit DeviceDiscovery(std::string &interface, PIL::Logging *logging = nullptr);
    ~DeviceDiscovery();

    std::vector<Device *> startDiscovery();

private:
    static IPRange getAddressRange(std::string &ip, std::string &mask);
    PIL_ERROR_CODE setInterfaceList();
    void testIPRange(IPRange& ipRange, std::vector<Device*>* deviceList);
    static Device * createDeviceFromDeviceString(std::string &deviceStr, std::vector<uint8_t> &ip);
    static std::vector<uint8_t> splitIpAddr(const std::string &string);

    InterfaceInfoList m_InterfaceList{};
    std::string m_InterfaceName;
    PIL::Logging *m_Logging;
};
#endif //INSTRUMENT_CONTROL_LIB_DEVICEDISCOVERY_HPP