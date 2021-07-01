#include "device.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>

/**
 * @class Device
 * @brief Basic device class
 * This class contains all basic method of all devices, like connect, exec, what_am_i etc.
 */


/**
 * @brief Constructor of Device
 * @param ip: the ip address of the target device
 */
Device::Device(char *ip) : ip(ip) {
}

/**
 * @brief connect to the device
 * @todo deal with connection timeout
 */
int Device::connect() {
    cout << name + " is connecting...\n";
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        cout << "\n Invalid address / Address not supported \n" << endl;
        return -1;
    }

    if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed. \n";
        return -1;
    }

    cout << "Connection Succeed. \n";
    return 0;
}

/**
 * @brief execute a (SCPI) command
 * @param message: a (SCPI) command
 * @param result[out]: The string will contain the result message(Max length: 1024)
 * @param br: whether add a '\\n' at the end of the command
 * @note  Different companies, devices may have different levels of support for SCPI.
 *          Some device may require '\\n', some may not. That is why here is a br param.
 *
 * @todo  Deal with execution timeout;
 *        Support for dynamic length of result;
 * @warning Some commands may timeout
 *      @code{.c}
 *      KST3000 k.exec("RSTater?", buffer);
 *      @endcode
 * */
int Device::exec(string message, char* result, bool br, int size) {
    if (br) {
        message += '\n';
    }
    char* command = const_cast<char *>(message.c_str());
    cout << "Executing Command: " << command;
    // TODO: add timeout
    // testcase: KST3000 k.exec("STATus? CHANnel2", buffer);
    send(sockfd, command, strlen(command), 0);
    if (result) {
        read(sockfd, result, size);
    }
    cout << "Executed Successfully.\n";
    return 0;
}

int Device::exec_commands(string commands) {
    stringstream s_commands(commands);
    string command;
    while (std::getline(s_commands, command)) {
        exec(command);
//        cout << command << endl;
    }
}

/**
 * @brief Print a message containing the device's name
 * */
void Device::what_am_i() {
    cout << "My name is: " << name << "\n";
}

/**
 * @brief Start a executing cli. Input a raw SCPI command and execute.
 *        To see if a command is valid or not.
 * */
void Device::cli() {
    char buffer[1024] = {0};
    while(1) {
        cout << "Input a command: ";
        string commands = "";
        getline(cin, commands);
        if (commands.compare("f") == 0) {
            cout << "CLI finished.\n";
            break;
        }
        bool is_query = commands.find("?") != string::npos;
        if (is_query) {
            memset(buffer, 0, sizeof(buffer));
            exec(commands, buffer);
            cout << "Result: " << buffer << "\n";
        } else {
            exec(commands);
        }
    }
}