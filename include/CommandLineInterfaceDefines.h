/**
 * @file This file contains all required defines required by the command line interface.
 * @authors Florian Frank, Wuhao Liu
 */
#pragma once

#include <string> // std::string

/** Text shown when starting the command line interface. */
#define STARTUP_TEXT             "Instrument control lib command line..." << std::endl << "Type 'help' to get a list of available commands" << std::endl;
/** Prompt symbol which should be displayed. */
#define PROMPT_SYMBOL            "$ "
/** Response message when invalid command was passed. */
#define RESPONSE_INVALID_COMMAND "    Command not found. Type 'help' to get list of available commands." << std::endl;
/** Message when exiting the CLI. E.g. when pressing CTRL + C. */
#define MESSAGE_EXIT_CLI         "Exit command line interface";
/** Defines the indentation for the help message */
#define HELP_MESSAGE_INDENTATION 10

#define TIMEOUT_IN_MS   2000

enum CLI_Commands
{
    /** <help> Print function overview on the command line interface. */
    CLI_HELP,
    /** <Connect ip> Connect to a device. Capture its identifier and puts him to the list of connected devices. */
    CLI_CONNECT,
    /** <Disconnect ID> closes the connection with a device of a certain ID. */
    CLI_DISCONNECT,
    /** <supported_devices> prints the list of supported devices on the command line. */
    CLI_SUPPORTED_DEVICES,
    /** <active_devices> prints a list with all active devices and its corresponding ID. */
    CLI_ACTIVE_DEVICES,
    /** <select_device ID> Selects a device according to its ID. All further commands are executed based on that device. */
    CLI_SELECT_DEVICE,
    /** <custom_command ID command args> Sends a custom VISA command to a device. */
    CLI_SEND_CUSTOM_COMMAND,
    /** <get_device_identity ID> Return the device identity as string representation. */
    CLI_GET_DEVICE_IDENTITY,
    /** <Quit> Closes the session. */
    CLI_QUIT,
    /** Dummy define to react on unknown commands. */
    CLI_COMMAND_NOT_FOUND,
    /** Dummy define to add custom commands. */
    CUSTOM_COMMAND1,
};

struct CLICommandStruct
{
    CLI_Commands identifier;
    const char *command;
    const char *description;

    void (*func)(std::string &);
} typedef CLICommandStruct;