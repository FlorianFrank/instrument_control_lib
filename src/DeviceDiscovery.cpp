#if __linux__
#include <iostream>

#include "ctlib/Socket.hpp"

#include <sstream>
#include <csignal>
#include <ctlib/Threading.hpp>
#include <cstring>

#include "ctlib/Socket.h"
#include "Device.h"
#include "DeviceDiscovery.h"
#include "devices/KST33500.h"
#include "devices/KEI2600.h"

#define SOCKET_TIMEOUT 2000
#define TEST_PORT      65535

DeviceDiscovery::DeviceDiscovery(std::string &interface, PIL::Logging *logging) : m_InterfaceName(interface),
                                                                                  m_Logging(logging)
{
}

DeviceDiscovery::~DeviceDiscovery() = default;

// TODO remove this function about SIGPIPE
void handler(int)
{
    //std::cout << "HANDLER " << std::endl;
}

/**
 * @brief Starts the discovery protocol. Starts threads for each ip in the ip range, specified by the netmask retrieved
 * from each network interface. If interfaceName = 'all' search in range of each interface.
 * @return List of found devices.
 */
std::vector<Device*> DeviceDiscovery::startDiscovery()
{
    // TODO avoid SIGPIPE
    signal(SIGPIPE, handler);

    std::vector<Device*> deviceList;

    auto errCode = setInterfaceList();
    if (errCode != PIL_NO_ERROR)
        return {};

    for (uint32_t i = 0; i < m_InterfaceList.availableInterfaces; i++)
    {
        if ((m_InterfaceName == "all" || m_InterfaceList.interfaces[i].m_InterfaceName == m_InterfaceName) &&
            m_InterfaceName != "lo")
        {
            std::string ip = m_InterfaceList.interfaces[i].m_IPAddr;
            std::string mask = m_InterfaceList.interfaces[i].m_NetMask;
            auto ipRange = getAddressRange(ip, mask);
            if (m_Logging)
            {
                m_Logging->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "If: %s, Start: %u.%u.%u.%u End: %u.%u.%u.%u",
                                      m_InterfaceList.interfaces[i].m_InterfaceName, ipRange.m_startIPRange[0],
                                      ipRange.m_startIPRange[1], ipRange.m_startIPRange[2], ipRange.m_startIPRange[3],
                                      ipRange.m_stopIPRange[0], ipRange.m_stopIPRange[1], ipRange.m_stopIPRange[2],
                                      ipRange.m_stopIPRange[3]);
            }
            testIPRange(ipRange, &deviceList);
        }
    }
    return deviceList;
}

/**
 * @brief This function retrieves the name: e.g. wlan0, the ip and netmask of all local interfaces and stores
 * the information in m_InterfaceList.
 * @return PIL_NO_ERROR if no error occurred. Otherwise return and error code.
 */
PIL_ERROR_CODE DeviceDiscovery::setInterfaceList()
{
    std::string addr = "localhost";
    PIL::Socket socket(UDP, IPv4, addr, TEST_PORT, SOCKET_TIMEOUT);
    auto errCode = socket.GetInterfaceInfos(&m_InterfaceList);
    if (errCode != PIL_NO_ERROR)
        return errCode;

    if (m_Logging)
    {
        for (uint32_t i = 0; i < m_InterfaceList.availableInterfaces; i++)
        {
            m_Logging->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Interface: %s, IP: %s, Mask: %s",
                                  m_InterfaceList.interfaces[i].m_InterfaceName, m_InterfaceList.interfaces[i].m_IPAddr,
                                  m_InterfaceList.interfaces[i].m_NetMask);
        }
    }
    return PIL_NO_ERROR;
}

/**
 * @brief Get Address range specified by an IP and network mask. If an IP e.g. '132.231.14.50' is passed
 * and the network mask is '255.255.255.0' then a range of 132.231.14.1 - 132.231.14.254 is returned.
 * @param ip IP-address of a local IP interface, to specify the part where the netmask contains binary 1's.
 * @param mask Mask used to to retrieve the ip-range.
 * @return IPRange struct containing the start and end address of the range.
 */
/*static*/ IPRange DeviceDiscovery::getAddressRange(std::string &ip, std::string &mask)
{
    auto ipAddrList = splitIpAddr(ip);
    auto maskList = splitIpAddr(mask);

    std::vector<uint8_t> startRange;
    std::vector<uint8_t> endRange;

    for (uint32_t i = 0; i < maskList.size(); i++)
    {
        if ((maskList[i] ^ 255) == 0)
        {
            startRange.push_back(ipAddrList[i]);
            endRange.push_back(ipAddrList[i]);

        } else
        {
            startRange.push_back((ipAddrList[i] & maskList[i]) + 1 /* do not include net address*/);
            endRange.push_back(((ipAddrList[i] & maskList[i]) | ~maskList[i]) - 1 /* do not include broadcast*/);
        }
    }

    IPRange ipRange = {startRange, endRange};

    return ipRange;
}

/**
 * @brief This function opens a connection for each IP-address in the address range and test if the *IDN? command
 * returns any information. Each socket is executed in a dedicated thread, allowing for a highly parallel execution
 * of the discovery. It parses the string returned by *IDN? and generates a device specific object, which can further
 * be used to execute tests.
 * @param ipRange IP range containing the start and end address.
 * @param deviceList List of devices which are returned by this function.
 */
void DeviceDiscovery::testIPRange(IPRange& ipRange, std::vector<Device*> *deviceList)
{
    std::vector<uint8_t> temporaryIP = ipRange.m_startIPRange;
    std::vector<PIL::Threading*> threadingList;

    auto threading_function = [](void *ipPtr) -> void *
    {
        std::string ipStr;
        auto devTuple = reinterpret_cast<std::tuple<std::vector<uint8_t>, std::string>*>(ipPtr);
        auto tip = std::get<0>(*devTuple);
        auto ip = [&tip, &ipStr]()
        {
            for (uint32_t i = 0; i < tip.size(); i++)
            {
                if (i < tip.size() - 1)
                    ipStr += std::to_string(tip.at(i)) + ".";
                else
                    ipStr += std::to_string(tip.at(i));
            }
            return ipStr;
        };

        ip();
        Device dev(ipStr.c_str(), SOCKET_TIMEOUT);
        auto ret = dev.Connect();
        if (ret == PIL_NO_ERROR)
        {
            std::string devIdentifier = dev.GetDeviceIdentifier();
            std::cout << devIdentifier << std::endl;
            if (devIdentifier.find("Error") == std::string::npos)
            {
                std::get<1>(*devTuple) = devIdentifier;
            }
        }
        dev.Disconnect();
        return (void *) nullptr;
    };
    std::vector<std::tuple<std::vector<uint8_t>, std::string>*> temporaryIPList;
    int threadCtr = 0;
    while (temporaryIP[temporaryIP.size() - 1] < ipRange.m_stopIPRange[ipRange.m_stopIPRange.size() - 1])
    {
        auto t = new std::tuple<std::vector<uint8_t>, std::string>{{temporaryIP.at(0), temporaryIP.at(1), temporaryIP.at(2), temporaryIP.at(3)}, ""};
        temporaryIPList.push_back(t);
        auto *threading = new PIL::Threading(threading_function, (void *) t);
        threadingList.push_back(threading);
        if (m_Logging)
            m_Logging->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Start Thread #%d for IP: %d.%d.%d.%d", threadCtr,
                                  temporaryIP[0], temporaryIP[1], temporaryIP[2], temporaryIP[3]);
        threading->Run();

        temporaryIP[temporaryIP.size() - 1]++;
        threadCtr++;
    }

    for (uint32_t i = 0; i < temporaryIPList.size(); i++)
    {
        if (m_Logging)
            m_Logging->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Join Thread #%d", i);
        threadingList[i]->Join();
        delete threadingList[i];
        std::string identifier = std::get<1>(*temporaryIPList[i]);
        if (!identifier.empty())
        {
            auto dev = createDeviceFromDeviceString(std::get<1>(*temporaryIPList[i]), std::get<0>(*temporaryIPList[i]));
            deviceList->push_back(dev);
        }
        delete temporaryIPList[i];
    }
    if (m_Logging)
        m_Logging->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Found 10 devices");
}

/**
 * @brief Helper function to split an ip-address string into single integer values.
 * @param string ip address e.g. "192.168.0.2" to parse. Currently only IPv4 addresses are supported.
 * @return
 */
std::vector<uint8_t> DeviceDiscovery::splitIpAddr(const std::string &string)
{
    std::vector<uint8_t> result;
    std::stringstream stringStream(string);
    std::string element;

    while (getline(stringStream, element, '.'))
    {
        result.push_back(atoi(element.c_str()));
    }

    return result;
}

/**
 * @brief Creates a device specific object based on the string returned by *IDN?.
 * @param deviceStr device string to pass.
 * @param ip IP used for the device creation.
 * @return Pointer to a device. TODO: Device is allocated on heap, to avoid slicing problem.
 */
/*static*/ Device* DeviceDiscovery::createDeviceFromDeviceString(std::string &deviceStr, std::vector<uint8_t> &ip)
{
    std::string ipAddrStr =
            std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." +
            std::to_string(ip[3]);
    if(deviceStr.find("Agilent Technologies,33522B") != std::string::npos)
        return new KST33500(ipAddrStr.c_str(), SOCKET_TIMEOUT);
    if(deviceStr.find("Keithley Instruments Inc., Model 26") != std::string::npos)
        return new KEI2600(ipAddrStr.c_str(), SOCKET_TIMEOUT);
    return new Device(nullptr, 0);
}

#endif // __linux