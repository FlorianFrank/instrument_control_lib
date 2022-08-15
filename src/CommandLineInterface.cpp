#include "CommandLineInterface.h"
#include "Device.h"
#include "ConfigFileParser.h"
extern "C" {
#include "ctlib/ErrorHandler.h"
};

#include <csignal> // std::signal to capture CTR + C
#include <iostream> // std::cout
#include <cstring> // strlen
#include <iomanip> // setw, std::isstringstream
#include <iterator> // std::istream_iterator
#if __APPLE__
#include <sstream>
#endif // __APPLE__

/** Macro indicates that currently no device is selected. */
#define NO_DEVICE_SELECTED (-1)

/*static*/ volatile bool CommandLineInterface::m_ExitCLI;
/*static*/ std::vector<Device *> CommandLineInterface::m_DeviceList;
/*static*/ int CommandLineInterface::m_currentDevice = NO_DEVICE_SELECTED;
/*static*/ std::vector<std::string> CommandLineInterface::m_DeviceNameList;
/*static*/ std::vector<std::string> CommandLineInterface::m_DeviceIPList;
/*static*/ std::vector<DeviceDescription> CommandLineInterface::m_DeviceDescriptions;
/*static*/ PIL::Logging *CommandLineInterface::m_Logger;

/*static*/ std::map<CLI_Commands, CLICommandStruct> CommandLineInterface::m_DescriptionMap = {
        {{CLI_HELP, {CLI_HELP, "help", "print list of commands", CommandLineInterface::PrintHelp}},
         {CLI_SUPPORTED_DEVICES, {CLI_SUPPORTED_DEVICES, "supported_devices", "returns a list of supported devices", CommandLineInterface::GetSupportedDevices}},
        {CLI_CONNECT, {CLI_CONNECT, "connect", "(Connect <ip>:<port>) establish connection with a device", CommandLineInterface::Connect}},
        {CLI_DISCONNECT, {CLI_DISCONNECT, "disconnect", "(Disconnect <id>|all) disconnects the device connection specified by id or the connection of all devices.", CommandLineInterface::Disconnect}},
        {CLI_SEND_CUSTOM_COMMAND, {CLI_SEND_CUSTOM_COMMAND, "custom_command", "custom_command <id> <command> <parameters>", CommandLineInterface::SendCustomCommand}},
        {CLI_QUIT, {CLI_QUIT, "quit", "Closes the current session", CommandLineInterface::Quit}},
        {CLI_ACTIVE_DEVICES, {CLI_ACTIVE_DEVICES, "active_devices", "(active_devices <id>) Displays all active devices with its corresponding ID.", CommandLineInterface::ActiveDevices}},
        {CLI_SELECT_DEVICE, {CLI_SELECT_DEVICE, "select_device", "(select_device <id>) Select device on which all following actions are executed.", CommandLineInterface::SelectDevice}},
        {CLI_GET_DEVICE_IDENTITY, {CLI_GET_DEVICE_IDENTITY, "get_device_identity", "(get_device_identity <id>) Return the identity of a device in the network", CommandLineInterface::GetDeviceIdentifier}}}
};

CommandLineInterface::CommandLineInterface(PIL::Logging *logger)
{
    CommandLineInterface::m_ExitCLI = false;
    CommandLineInterface::m_Logger = logger;
}

CommandLineInterface::CommandLineInterface(): CommandLineInterface(nullptr)
{
}

/**
 * @brief Start executing the command line interface. Input a raw SCPI command and execute.
 *        To see if a command is valid or not.
 * @return true if CLI was successfully exited, else return false
 * */
bool CommandLineInterface::Start()
{
    std::signal(SIGINT, CtrlCHandler);

    std::cout << STARTUP_TEXT;

    std::string configFile = DEVICE_CONFIG_FILE;
    std::cout << DEVICE_CONFIG_FILE << std::endl;
    PIL_ERROR_CODE errorCode = ConfigFileParser::ParseSupportedDevices(configFile, m_DeviceDescriptions);
    if(errorCode != PIL_NO_ERROR)
    {
        std::cout << PIL_ErrorCodeToString(errorCode);
        return false;
    }

    // Start look until CTRL + C is pressed or an error occurs
    while (!m_ExitCLI)
    {
        std::cout << PROMPT_SYMBOL;
        std::string commands;
        std::getline(std::cin, commands);

        CLI_Commands command = ParseCommand(commands);
        if(command == CLI_COMMAND_NOT_FOUND)
        {
            std::cout << RESPONSE_INVALID_COMMAND;
        }
    }
    std::cout << MESSAGE_EXIT_CLI;
    return true;
}


/**
 * @brief This method prints an overview of available commands on the command line interface.
 * @param args Arguments are ignored.
 */
/*static*/ void CommandLineInterface::PrintHelp(std::string &args)
{
    std::cout << "    Available commands: " << std::endl;
    for (const auto &[key, value]: m_DescriptionMap)
    {
        std::cout << "        " << value.command << ": "
                  << std::setw(static_cast<int>(HELP_MESSAGE_INDENTATION - strlen(value.command))) << std::setfill(' ')
                  << value.description << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief Connects to a device based on a certain IP address and adds it to the list of active devices.
 * @param args only argument args[0] is chosen as IP addr.
 */
/*static*/ void CommandLineInterface::Connect(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "    Error the argument <ip> must be specified" << std::endl;
        return;
    }

    // TODO specifiy certain class of device
    auto *device = new Device(argumentList[0].c_str(), TIMEOUT_IN_MS, m_Logger);
    std::cout << "    Connect to Device " <<  "IP: " << argumentList[0] << " ID (" << m_DeviceList.size()-1 << ")" << std::endl;
    if(!device->Connect())
    {
        std::cout << device->ReturnErrorMessage() << std::endl;
    }

    m_DeviceList.push_back(device);
    std::string deviceName = device->GetDeviceIdentifier();
    m_DeviceNameList.push_back(deviceName);
    m_DeviceIPList.push_back(argumentList[0]);
    std::cout << "    Device identified as " << deviceName << std::endl;



    // TODO assert
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

/*static*/ void
CommandLineInterface::addCustomCommandLineOption(const char *identifier, const char *description,
                                                 void (*func)(std::string &))
{
    auto it = m_DescriptionMap.begin();
    m_DescriptionMap.insert(it, std::pair<CLI_Commands, CLICommandStruct>(CUSTOM_COMMAND1,
                                                                          {CUSTOM_COMMAND1, identifier, description,
                                                                           func}));  // max efficiency inserting
}

/*static*/ void CommandLineInterface::CtrlCHandler(int signal)
{
    if(signal == SIGINT)
    {
        m_ExitCLI = true;
        std::cout << "Exit command line interface" << std::endl;
    }
}


std::vector<std::string> CommandLineInterface::splitArguments(std::string &args)
{
    std::istringstream iss(args);
    std::vector<std::string> argumentList((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    return argumentList;
}

void CommandLineInterface::Disconnect(std::string &args)
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

/*static*/ void CommandLineInterface::SendCustomCommand(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
    std::cout << "Please specify an ID or select all to Disconnect all devices" << std::endl;
    return;
    }
    // TODO m_currentDevice
    int deviceIdx = atoi(argumentList[0].c_str());
    Device *d = m_DeviceList.at(deviceIdx);
    bool Exec(std::string message, char *result = nullptr, bool br = true, int size = 1024);

    std::string output;
    for (int i = 1; i < argumentList.size(); i++)
    {
        if (i < argumentList.size() - 1)
            output += argumentList[i] + " ";
        output += argumentList[i];
    }


    std::cout << output;

    char retBuff[1024];
    d->Exec(output.c_str(), nullptr, retBuff);

    std::cout << "Returns: " << retBuff << std::endl;
}


/*static*/ void CommandLineInterface::GetSupportedDevices(std::string &args)
{
    // TODO register devices somewhere else
    std::cout << "    List of supported devices" << std::endl;
    for(const auto& deviceDescription: m_DeviceDescriptions)
    {
        std::cout << "        " << deviceDescription.m_DeviceName << " - " << deviceDescription.m_DeviceDescription << std::endl;
    }
}

void CommandLineInterface::Quit(std::string &args)
{
    std::string closeAll = "all";
    Disconnect(closeAll);
    m_ExitCLI = true;
}

void CommandLineInterface::ActiveDevices(std::string &args)
{
    std::cout << "    List active devices: " << std::endl;
    for(int i = 0; i < m_DeviceList.size(); i++)
    {
        std::cout << "         ID: " << i << " " << m_DeviceNameList[i] << " IP: " << m_DeviceIPList[i] << " " << (m_DeviceList[i]->IsOpen() ? "Connected" : "Disconnected") << std::endl;
    }
}

void CommandLineInterface::SelectDevice(std::string &args)
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

void CommandLineInterface::GetDeviceIdentifier(std::string &args)
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



