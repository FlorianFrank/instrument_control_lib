//
// Created by florianfrank on 06.09.21.
//

#ifndef CE_DEVICE_COMMAND_LINE_INTERFACE_H
#define CE_DEVICE_COMMAND_LINE_INTERFACE_H

#include <map>
#include <vector>


enum CLI_Commands {
    CLI_HELP,
    CLI_CONNECT,
    CLI_SUPPORTED_DEVICES,
    CLI_DISCONNECT,
    CLI_ACTIVE_DEVICES,
    CLI_SELECT_DEVICE,
    CLI_GET_DEVICE_IDENTIFIER,
    CLI_QUIT,
    CLI_COMMAND_NOT_FOUND
};

enum {
    KST3000,
    KST33500,
    SPD1305
} typedef DeviceType;


class Device;

class command_line_interface
{

public:
    command_line_interface();
    bool start();
    CLI_Commands ParseCommand(std::string &string);

    static void printHelp(std::string &args);
    static void connect(std::string &args);
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

    struct CLICommandStruct {

        CLI_Commands identifier;
        const char* command;
        const char* description;
        void (*func)(std::string&);
    } typedef CLICommandStruct;



    static const std::map<CLI_Commands, CLICommandStruct> m_DescriptionMap;
    static const std::map<std::string, std::string> m_SupportedDevices;

};


#endif //CE_DEVICE_COMMAND_LINE_INTERFACE_H
