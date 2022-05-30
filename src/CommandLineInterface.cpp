//
// Created by florianfrank on 06.09.21.
//

#include <csignal>
#include <iostream>
#include <cstring>
#include <Device.h>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <ConfigFileParser.h>
#include "CommandLineInterface.h"

// TODO new abstraction layer
#include "KST3000.h"
#include "SPD1305.h"
#include "KST33500.h"

/*static*/ volatile bool CommandLineInterface::m_ExitCLI;
/*static*/ std::vector<Device*> CommandLineInterface::m_DeviceList;
/*static*/ int CommandLineInterface::m_currentDevice = 0;
/*static*/ std::vector<std::string> CommandLineInterface::m_DeviceNameList;
/*static*/ std::vector<std::string> CommandLineInterface::m_DeviceIPList;
/*static*/ std::vector<DeviceDescription> CommandLineInterface::m_DeviceDescriptions;

/*static*/ std::map<CLI_Commands, CommandLineInterface::CLICommandStruct> CommandLineInterface::m_DescriptionMap = {
        {{CLI_HELP, {CLI_HELP, "help", "print list of commands", CommandLineInterface::printHelp}},
         {CLI_SUPPORTED_DEVICES, {CLI_SUPPORTED_DEVICES, "supported_devices", "returns a list of supported devices", CommandLineInterface::getSupportedDevices}},
        {CLI_CONNECT, {CLI_CONNECT, "connect", "(connect <ip>:<port>) establish connection with a device", CommandLineInterface::connect}},
        {CLI_DISCONNECT, {CLI_DISCONNECT, "disconnect", "(disconnect <id>|all) disconnects the device connection specified by id or the connection of all devices.", CommandLineInterface::disconnect}},
        {CLI_QUIT, {CLI_QUIT, "quit", "Closes the current session", CommandLineInterface::quit}},
        {CLI_ACTIVE_DEVICES, {CLI_ACTIVE_DEVICES, "active_devices", "(active_devices <id>) Displays all active devices with its corresponding ID.", CommandLineInterface::activeDevices}},
        {CLI_SELECT_DEVICE, {CLI_SELECT_DEVICE, "select_device", "(select_device <id>) Select device on which all following actions are executed.", CommandLineInterface::selectDevice}},
        {CLI_GET_DEVICE_IDENTIFIER, {CLI_GET_DEVICE_IDENTIFIER, "get_device_identity", "(get_device_identity <id>) Return the identity of a device in the network", CommandLineInterface::getDeviceIdentifier}}}
};

/*static*/ void
CommandLineInterface::addCustomCommandLineOption(const char *identifier, const char *description, void (*func)(std::string&))
{
    auto it = m_DescriptionMap.begin();
    m_DescriptionMap.insert(it, std::pair<CLI_Commands, CommandLineInterface::CLICommandStruct>(CUSTOM_COMMAND1,
                                                                                                {CUSTOM_COMMAND1,
                                                                                                 identifier,
                                                                                                 description,
                                                                                                 func}));  // max efficiency inserting
}



CommandLineInterface::CommandLineInterface()
{
    CommandLineInterface::m_ExitCLI = false;
}


/*static*/ void CommandLineInterface::ctrl_c_handler(int signal)
{
    if(signal == SIGINT)
    {
        m_ExitCLI = true;
        std::cout << "Exit command line interface" << std::endl;
    }
}

/**
 * @brief Start executing the command line interface. Input a raw SCPI command and execute.
 *        To see if a command is valid or not.
 * */
bool CommandLineInterface::start()
{
    std::signal(SIGINT, ctrl_c_handler);

    std::cout << "Instrument control lib command line..." << std::endl;
    std::cout << "Type 'help' to get a list of available commands" << std::endl;

    std::string configFile = DEVICE_CONFIG_FILE;
    std::cout << DEVICE_CONFIG_FILE << std::endl;
    PIL_ERROR_CODE errorCode = ConfigFileParser::ParseSupportedDevices(configFile, m_DeviceDescriptions);
    if(errorCode != PIL_NO_ERROR)
        std::cout << PIL_ErrorCodeToString(errorCode);

    while (!m_ExitCLI)
    {
        std::cout << "$ ";
        std::string commands;
        std::getline(std::cin, commands);
        if (commands == "f")
        {
            std::cout << "CLI finished.\n";
            break;
        }
        bool is_query = commands.find('?') != std::string::npos;
        CLI_Commands command = ParseCommand(commands);
        if(command == CLI_COMMAND_NOT_FOUND)
        {
            std::cout << "    Command not found. Type 'help' to get list of available commands." << std::endl;
        }
    }
    std::cout << "Exit command line interface";
    return true;
}



CLI_Commands CommandLineInterface::ParseCommand(std::string &string)
{
    for(const auto& [key, value]: m_DescriptionMap)
    {
        // first cut off the parameters
        int loc = string.find( ' ', 0 );
        std::string searchStr = string;
        if(loc != -1)
            searchStr = string.substr(0, loc);

        if(searchStr == value.command)
        {
            std::string args = string.substr(loc + 1, std::string::npos);
            value.func(args);
            return key;
        }
    }
    return CLI_COMMAND_NOT_FOUND;
}



/*static*/ void CommandLineInterface::printHelp(std::string&)
{
    std::cout << "    Available commands: " << std::endl;
    for(const auto& [key, value]: m_DescriptionMap)
    {
        std::cout << "        " << value.command << ": " << std::setw(10  - strlen(value.command)) << std::setfill(' ') << value.description << std::endl;
    }
    std::cout << std::endl;
}

std::vector<std::string> CommandLineInterface::splitArguments(std::string &args)
{
    std::istringstream iss(args);
    std::vector<std::string> argumentList((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    return argumentList;
}

/*static*/ void CommandLineInterface::connect(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "    Error the argument <ip> must be specified" << std::endl;
        return;
    }

    auto *device = new Device(argumentList[0].c_str());
    m_DeviceList.push_back(device);



    std::cout << "    Connect to Device " <<  "IP: " << argumentList[0] << " ID (" << m_DeviceList.size()-1 << ")" << std::endl;
    if(!device->Connect())
    {
        std::cout << device->ReturnErrorMessage() << std::endl;
    }
    std::string deviceName = device->GetDeviceIdentifier();
    m_DeviceNameList.push_back(deviceName);
    m_DeviceIPList.push_back(argumentList[0]);
    std::cout << "    Device identified as " << deviceName << std::endl;



    // TODO assert
}

void CommandLineInterface::disconnect(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "Please specify an ID or select all to Disconnect all devices" << std::endl;
        return;
    }

    if(argumentList[0] == "all")
    {
        for(auto device: m_DeviceList)
        {
            if(!device->Disconnect())
                std::cout << device->ReturnErrorMessage() << std::endl;
            else
                std::cout << "Device " + device->WhatAmI() + " closed successfully" << std::endl;
        }
    }
    else {
        int id = atoi(argumentList[0].c_str());
        if(m_DeviceList.empty() || id > m_DeviceList.size()-1)
        {
            std::cout << "Error ID not found. Execute command 'available_devices' to get list of available devices."
                      << std::endl;
            return;
        }
        else
        {
            if(!m_DeviceList[id]->Disconnect())
                std::cout << m_DeviceList[id]->ReturnErrorMessage() << std::endl;
            else
                std::cout << "Device " + m_DeviceList[id]->WhatAmI() + " closed successfully" << std::endl;        }
    }

}

void CommandLineInterface::setFrequency(std::string &args)
{
    auto argumentList = splitArguments(args);
//    if(m_SupportedDevices.count(argumentList[0]) == 0)
    {
        std::cout << "Error specified device not within the supported devices list" << std::endl;
        return;
    }
}


/*static*/ void CommandLineInterface::getSupportedDevices(std::string &args)
{
    // TODO register devices somewhere else
    std::cout << "    List of supported devices" << std::endl;
    for(const auto& deviceDescription: m_DeviceDescriptions)
    {
        std::cout << "        " << deviceDescription.m_DeviceName << " - " << deviceDescription.m_DeviceDescription << std::endl;
    }
}

void CommandLineInterface::quit(std::string &args)
{
    std::string closeAll = "all";
    disconnect(closeAll);
    m_ExitCLI = true;
}

void CommandLineInterface::activeDevices(std::string &args)
{
    std::cout << "    List active devices: " << std::endl;
    for(int i = 0; i < m_DeviceList.size(); i++)
    {
        std::cout << "         ID: " << i << " " << m_DeviceNameList[i] << " IP: " << m_DeviceIPList[i] << " " << (m_DeviceList[i]->IsOpen() ? "Connected" : "Disconnected") << std::endl;
    }
}

void CommandLineInterface::selectDevice(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "        Error a device number must be selected" << std::endl;
        return;
    }

    int index = atoi(argumentList[0].c_str());
    if(m_DeviceList.empty() || index > m_DeviceList.size() -1 )
    {
        std::cout << "    Error ID not found. Execute command 'available_devices' to get list of available devices." << std::endl;
        return;
    }
    std::cout << "    Device with ID " << index << " selected" << std::endl;
    m_currentDevice = index;
}

void CommandLineInterface::getDeviceIdentifier(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "    Error a device number must be selected" << std::endl;
        return;
    }

    int index = atoi(argumentList[0].c_str());
    if(m_DeviceList.empty() || index > m_DeviceList.size() -1 )
    {
        std::cout << "    Error ID not found. Execute command 'available_devices' to get list of available devices." << std::endl;
        return;
    }
    std::cout << "              Get Device Identifier:";
    std::cout << "    " << m_DeviceList[index]->GetDeviceIdentifier();
}



