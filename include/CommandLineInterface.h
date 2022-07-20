/**
 * @brief This class contains a implementation of a command line interface.
 * It supports various functions like connecting and disconnecting to a device or to send custom commands.
 * @authors Florian Frank, Wuhao Liu
 * @copyright University of Passau
 */
#pragma once

#include "CommandLineInterfaceDefines.h"
#include "ConfigFileParser.h"

#include <map> // std::map
#include <vector> // std::vector

// Forward declarations to avoid importing PIL headers
namespace PIL {
    class Logging;
}
class Device;

/**
 * @brief This class represents a command line interface to execute common functions of VISA devices.
 */
class CommandLineInterface
{
public:
    explicit CommandLineInterface(PIL::Logging *logger);
    explicit CommandLineInterface();
    bool Start();

    // Commands to be executed on the command line interface.
    static void PrintHelp(std::string &args);
    static void Connect(std::string &args);
    static void SendCustomCommand(std::string &args);
    static void Disconnect(std::string &args);
    static void Quit(std::string &args);
    static void GetSupportedDevices(std::string &args);
    static void ActiveDevices(std::string &args);
    static void SelectDevice(std::string &args);
    static void GetDeviceIdentifier(std::string &args);
    static void addCustomCommandLineOption(const char *identifier, const char *description, void (*func)(std::string&));

    static std::vector<std::string> splitArguments(std::string &args);

private:
    CLI_Commands ParseCommand(std::string &string);

    static void CtrlCHandler(int signal);

    // TODO: maybe combined to one data structure
    /** List of active devices. **/
    static std::vector<Device*> m_DeviceList;
    /** List of device names corresponding to the device list. **/
    static std::vector<std::string> m_DeviceNameList;
    /** List of ip addresses corresponding to the device list. **/
    static std::vector<std::string> m_DeviceIPList;
    /** List of device descriptions corresponding to the device list. **/
    static std::vector<DeviceDescription> m_DeviceDescriptions;

    /** Currently selected device. If variable is set to -1 and ID must be passed for each command. **/
    static int m_currentDevice;
    /** Variable indicating that the command line interface should be closed. **/
    static volatile bool m_ExitCLI;
    /** Logging handler. If set to nullptr, logging is disabled. **/
    static PIL::Logging *m_Logger;
    /** Map containing the commands, its descriptions as well as the function pointers to call the corresponding function. **/
    static std::map<CLI_Commands, CLICommandStruct> m_DescriptionMap;
    /** List of supported devices. Is load from a config XML file. **/
    static std::map<std::string, std::string> m_SupportedDevices;
};
