//
// Created by florianfrank on 06.09.21.
//

#ifndef CE_DEVICE_COMMANDLINEINTERFACE_H
#define CE_DEVICE_COMMANDLINEINTERFACE_H

#include <map>
#include <vector>
#include <string>
#include "ConfigFileParser.h"

namespace PIL {
    class Logging;
}


enum CLI_Commands {
    CLI_HELP,
    CLI_CONNECT,
    CLI_SUPPORTED_DEVICES,
    CLI_DISCONNECT,
    CLI_SEND_CUSTOM_COMMAND,
    CLI_ACTIVE_DEVICES,
    CLI_SELECT_DEVICE,
    CLI_GET_DEVICE_IDENTIFIER,
    CLI_QUIT,
    CLI_COMMAND_NOT_FOUND,
    CUSTOM_COMMAND1,
    CUSTOM_COMMAND2,
    CUSTOM_COMMAND3,
};

enum {
    KST3000_DEVICE,
    KST33500_DEVICE,
    SPD1305_DEVICE
} typedef DeviceType;


class Device;

class CommandLineInterface
{

public:

    CommandLineInterface(PIL::Logging *logger);
    bool start();


    struct CLICommandStruct {

        CLI_Commands identifier;
        const char* command;
        const char* description;
        void (*func)(std::string&);
    } typedef CLICommandStruct;


    static void addCustomCommandLineOption(const char *identifier, const char *description, void (*func)(std::string&));
    static std::vector<std::string> splitArguments(std::string &args);


    CLI_Commands ParseCommand(std::string &string);

    static void printHelp(std::string &args);
    static void connect(std::string &args);
    static void sendCustomCommand(std::string &args);
    static void disconnect(std::string &args);
    static void quit(std::string &args);
    static void getSupportedDevices(std::string &args);
    static void activeDevices(std::string &args);
    static void selectDevice(std::string &args);
    static void getDeviceIdentifier(std::string &args);

    // Signal generator function
    static void setFrequency(std::string &args);

private:
    //Device m_Device;

    static std::vector<Device*> m_DeviceList;
    static int m_currentDevice;

    static void ctrl_c_handler(int signal);
    static volatile bool m_ExitCLI;

    static PIL::Logging *m_Logger;



    static std::map<CLI_Commands, CLICommandStruct> m_DescriptionMap;
    static std::map<std::string, std::string> m_SupportedDevices;
    static std::vector<std::string> m_DeviceNameList;
    static std::vector<std::string> m_DeviceIPList;

    static std::vector<DeviceDescription> m_DeviceDescriptions;

};


#endif //CE_DEVICE_COMMANDLINEINTERFACE_H
