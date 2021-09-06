//
// Created by florianfrank on 06.09.21.
//

#include <csignal>
#include <iostream>
#include <cstring>
#include <device.h>
#include <iomanip>
#include <iterator>
#include "command_line_interface.h"

// TODO new abstraction layer
#include "kst3000.h"
#include "spd1305.h"
#include "kst33500.h"

/*static*/ volatile bool command_line_interface::m_ExitCLI;
/*static*/ std::vector<Device*> command_line_interface::m_DeviceList;
/*static*/ int command_line_interface::m_currentDevice = 0;


/*static*/ const std::map<CLI_Commands, command_line_interface::CLICommandStruct> command_line_interface::m_DescriptionMap = {
        {{CLI_HELP, {CLI_HELP, "help", "print list of commands", command_line_interface::printHelp}},
         {CLI_SUPPORTED_DEVICES, {CLI_SUPPORTED_DEVICES, "supported_devices", "returns a list of supported devices", command_line_interface::getSupportedDevices}},
        {CLI_CONNECT, {CLI_CONNECT, "connect", "(connect <ip>:<port>) establish connection with a device", command_line_interface::connect}},
        {CLI_DISCONNECT, {CLI_DISCONNECT, "disconnect", "(disconnect <id>|all) disconnects the device connection specified by id or the connection of all devices.", command_line_interface::disconnect}},
        {CLI_QUIT, {CLI_QUIT, "quit", "Closes the current session", command_line_interface::quit}},
        {CLI_ACTIVE_DEVICES, {CLI_ACTIVE_DEVICES, "active_devices", "(active_devices <id>) Displays all active devices with its corresponding ID.", command_line_interface::activeDevices}},
        {CLI_SELECT_DEVICE, {CLI_SELECT_DEVICE, "select_device", "(select_device <id>) Select device on which all following actions are executed.", command_line_interface::selectDevice}},
        {CLI_GET_DEVICE_IDENTIFIER, {CLI_GET_DEVICE_IDENTIFIER, "get_device_identity", "(get_device_identity <id>) Return the identity of a device in the network", command_line_interface::getDeviceIdentifier}}}

};

/*static*/ const std::map<std::string, std::string> command_line_interface::m_SupportedDevices {
                {"KST3000", "Keysight Oscilloscope"},
                {"SPD1305", "Digilent DC Powersupply"},
                {"KST33500", "Keysight Function Generator"},
        };



command_line_interface::command_line_interface()
{
    command_line_interface::m_ExitCLI = false;
}


/*static*/ void command_line_interface::ctrl_c_handler(int signal)
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
bool command_line_interface::start()
{
    char buffer[1024] = {0};
    std::signal(SIGINT, ctrl_c_handler);

    std::cout << "Instrument control lib command line..." << std::endl;
    std::cout << "Type 'help' to get a list of available commands" << std::endl;


    while (!m_ExitCLI)
    {
        std::cout << "$ ";
        std::basic_string<char> commands;
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



CLI_Commands command_line_interface::ParseCommand(std::string &string)
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



/*static*/ void command_line_interface::printHelp(std::string&)
{
    std::cout << "    Available commands: " << std::endl;
    for(const auto& [key, value]: m_DescriptionMap)
    {
        std::cout << "        " << value.command << ": " << std::setw(10  - strlen(value.command)) << std::setfill(' ') << value.description << std::endl;
    }
    std::cout << std::endl;
}

std::vector<std::string> splitArguments(std::string &args)
{
    std::istringstream iss(args);
    std::vector<std::string> argumentList((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());

    return argumentList;
}

/*static*/ void command_line_interface::connect(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "    Error the argument <ip> must be specified" << std::endl;
        return;
    }

    Device device(argumentList[0].c_str());


    std::cout << "    Connect to Device " <<  "IP: " << argumentList[0] << "ID (" << m_DeviceList.size()-1 << ")" << std::endl;
    if(!device.connect())
    {
        std::cout << device.return_error_message() << std::endl;
    }
    std::cout << "    Device identified as " << device.getDeviceIdentifier() << std::endl;



    // TODO assert
}

void command_line_interface::disconnect(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(argumentList.empty())
    {
        std::cout << "Please specify an ID or select all to disconnect all devices" << std::endl;
        return;
    }

    if(argumentList[0] == "all")
    {
        for(auto device: m_DeviceList)
        {
            if(!device->disconnect())
                std::cout << device->return_error_message() << std::endl;
            else
                std::cout << "Device " + device->what_am_i() + " closed successfully" << std::endl;
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
            if(!m_DeviceList[id]->disconnect())
                std::cout << m_DeviceList[id]->return_error_message() << std::endl;
            else
                std::cout << "Device " + m_DeviceList[id]->what_am_i() + " closed successfully" << std::endl;        }
    }

}

void command_line_interface::setFrequency(std::string &args)
{
    auto argumentList = splitArguments(args);
    if(m_SupportedDevices.count(argumentList[0]) == 0)
    {
        std::cout << "Error specified device not within the supported devices list" << std::endl;
        return;
    }
}


/*static*/ void command_line_interface::getSupportedDevices(std::string &args)
{
    // TODO register devices somewhere else
    std::cout << "    List of supported devices" << std::endl;
    for(const auto& [key, value]: m_SupportedDevices)
    {
        std::cout << "        " << key << " - " << value << std::endl;
    }
}

void command_line_interface::quit(std::string &args)
{
    std::string closeAll = "all";
    disconnect(closeAll);
    m_ExitCLI = true;
}

void command_line_interface::activeDevices(std::string &args)
{
    std::cout << "    List active devices: " << std::endl;
    for(int i = 0; i < m_DeviceList.size(); i++)
    {
        std::cout << "         ID: " << i << " " << m_DeviceList[i]->what_am_i() << " " << (m_DeviceList[i]->isOpen() ? "Connected" : "Disconnected") << std::endl;
    }
}

void command_line_interface::selectDevice(std::string &args)
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

void command_line_interface::getDeviceIdentifier(std::string &args)
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
    std::cout << "    " << m_DeviceList[index]->getDeviceIdentifier();
}



