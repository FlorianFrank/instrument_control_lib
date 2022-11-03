//
// Created by florianfrank on 28.10.22.
//
#include <iostream>

#include <netinet/in.h>
#include "ctlib/Socket.hpp"

#include <sstream>
#include <csignal>
#include <ctlib/Threading.hpp>
#include <cstring>

#include "ctlib/Socket.h"
#include "Device.h"
#include "DeviceDiscovery.h"

#define SOCKET_TIMEOUT 1000
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

std::vector<uint8_t> split(const std::string &string, char delimiter)
{
    std::vector<uint8_t> result;
    std::stringstream stringStream(string);
    std::string element;

    while (getline(stringStream, element, delimiter))
    {
        result.push_back(atoi(element.c_str()));
    }

    return result;
}

PIL_ERROR_CODE DeviceDiscovery::setDeviceList()
{
    std::string addr = "localhost";
    PIL::Socket socket(UDP, IPv4, addr, TEST_PORT, SOCKET_TIMEOUT);
    auto errCode = socket.GetInterfaceInfos(&m_DeviceList);
    if (errCode != PIL_NO_ERROR)
        return errCode;

    if (m_Logging)
    {
        for (uint32_t i = 0; i < m_DeviceList.availableInterfaces; i++)
        {
            m_Logging->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Interface: %s, IP: %s, Mask: %s",
                                  m_DeviceList.interfaces[i].m_InterfaceName, m_DeviceList.interfaces[i].m_IPAddr,
                                  m_DeviceList.interfaces[i].m_NetMask);
        }
    }
    return PIL_NO_ERROR;
}


std::vector<Device> DeviceDiscovery::startDiscovery()
{
    // TODO avoid SIGPIPE
    signal(SIGPIPE, handler);

    auto errCode = setDeviceList();
    if (errCode != PIL_NO_ERROR)
        return {};

    for (uint32_t i = 0; i < m_DeviceList.availableInterfaces; i++)
    {
        if (m_InterfaceName == "all" || m_DeviceList.interfaces[i].m_InterfaceName == m_InterfaceName)
        {
            std::string ip = m_DeviceList.interfaces[i].m_IPAddr;
            std::string mask = m_DeviceList.interfaces[i].m_NetMask;
            auto ipRange = getAddressRange(ip, mask);
            if (m_Logging)
            {
                m_Logging->LogMessage(INFO_LVL, __FILENAME__, __LINE__,
                                      "If: %s, Start: %u.%u.%u.%u End: %u.%u.%u.%u",
                                      m_DeviceList.interfaces[i].m_InterfaceName, ipRange.m_startIPRange[0],
                                      ipRange.m_startIPRange[1], ipRange.m_startIPRange[2], ipRange.m_startIPRange[3],
                                      ipRange.m_stopIPRange[0], ipRange.m_stopIPRange[1], ipRange.m_stopIPRange[2],
                                      ipRange.m_stopIPRange[3]);
            }
            testIRange(ipRange);
        }
    }
    return {};
}


std::vector<Device> DeviceDiscovery::testIRange(IPRange& ipRange)
{
    std::vector<uint8_t> temporaryIP = ipRange.m_startIPRange;
    std::vector<PIL::Threading> threadingList;

    auto threading_function = [](void *ipPtr) -> void *
    {
        std::string ipStr;
        auto tip = reinterpret_cast<std::vector<uint8_t> *>(ipPtr);
        auto ip = [&tip, &ipStr]()
        {
            for (uint32_t i = 0; i < tip->size(); i++)
            {
                if (i < tip->size() - 1)
                    ipStr += std::to_string(tip->at(i)) + ".";
                else
                    ipStr += std::to_string(tip->at(i));
            }
            return ipStr;
        };

        ip();
        Device dev(ipStr.c_str(), 1000);
        auto ret = dev.Connect();
        if (ret == PIL_NO_ERROR)
        {
            std::string devIdentifier = dev.GetDeviceIdentifier();
            if (devIdentifier.find("Error") == std::string::npos)
            {
                std::cout << " DEVICE IDENTIFIER " << devIdentifier << std::endl << " IP: "
                          << std::to_string(tip->at(0)) << "." << std::to_string(tip->at(1)) << "."
                          << std::to_string(tip->at(2)) << "." << std::to_string(tip->at(3)) << std::endl;
            }
        }

        return (void *) nullptr;
    };
    std::vector<std::vector<uint8_t> *> temporaryIPList;
    int threadCtr = 0;
    while (temporaryIP[temporaryIP.size() - 1] < ipRange.m_stopIPRange[ipRange.m_stopIPRange.size() - 1])
    {
        auto t = new std::vector<uint8_t>{temporaryIP.at(0), temporaryIP.at(1), temporaryIP.at(2), temporaryIP.at(3)};
        temporaryIPList.push_back(t);
        PIL::Threading threading(threading_function, (void *) t);
        threadingList.push_back(threading);
        if (m_Logging)
            m_Logging->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Start Thread #%d for IP: %d.%d.%d.%d", threadCtr,
                                  temporaryIP[0], temporaryIP[1], temporaryIP[2], temporaryIP[3]);
        threading.Run();

        temporaryIP[temporaryIP.size() - 1]++;
        threadCtr++;
    }

    for (uint32_t i = 0; i < temporaryIPList.size(); i++)
    {
        if (m_Logging)
            m_Logging->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Join Thread #%d", i);
        threadingList[i].Join();
        delete temporaryIPList[i];
    }
    if(m_Logging)
        m_Logging->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Found 10 devices");
    return {};
}


IPRange DeviceDiscovery::getAddressRange(std::string &ip, std::string &mask)
{
    auto ipAddrList = split(ip, '.');
    auto maskList = split(mask, '.');

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

