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
 * @brief setting DEBUG=1 environment variable to enable some debug information
 * */
int is_enable_debug() {
  if (getenv("DEBUG")) {
    return std::stoi(getenv("DEBUG"));
  }
  return 0;
}

int DEBUG = is_enable_debug();

/**
 * @brief Constructor of Device
 * @param ip: the ip address of the target device
 */
Device::Device(const char *ip) : ip(ip) {
}

/**
 * @brief connect to the device
 * @todo deal with connection timeout
 * @todo deal with connection warning(you have not connected or connect automatically)
 */
int Device::connect() {
  std::cout << name + " is connecting...\n";
  struct sockaddr_in serv_addr = {0};
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cout << "\n Socket creation error \n";
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    std::cout << "\n Invalid address / Address not supported \n" << std::endl;
    return -1;
  }

  if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    std::cout << "Connection Failed. \n";
    return -1;
  }

  std::cout << "Connection Succeed. \n";
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
int Device::exec(std::string message, char *result, bool br, int size) {
  if (br) {
    message += '\n';
  }
  char *command = const_cast<char *>(message.c_str());
  if (DEBUG) {
    std::cout << "Executing Command: " << command;
  }
  // TODO: add timeout
  // testcase: KST3000 k.exec("STATus? CHANnel2", buffer);
  send(sockfd, command, strlen(command), 0);
  if (result) { // not all operation need a result
    read(sockfd, result, size);
  }
  if (DEBUG) {
    std::cout << "Executed Successfully.\n";
  }
  return 0;
}

int Device::exec_commands(std::string &commands) {
  std::stringstream s_commands(commands);
  std::string command;
  while (std::getline(s_commands, command)) {
    exec(command);
  }
  return 0;
}

/**
 * @brief Print a message containing the device's name
 * */
void Device::what_am_i() {
  std::cout << "My name is: " << name << "\n";
}

/**
 * @brief Start a executing cli. Input a raw SCPI command and execute.
 *        To see if a command is valid or not.
 * */
void Device::cli() {
  char buffer[1024] = {0};
  while (true) { // TODO exit condition?
    std::cout << "Input a command: ";
    std::string commands;
    getline(std::cin, commands);
    if (commands == "f") {
      std::cout << "CLI finished.\n";
      break;
    }
    bool is_query = commands.find('?') != std::string::npos;
    if (is_query) {
      memset(buffer, 0, sizeof(buffer));
      exec(commands, buffer);
      std::cout << "Result: " << buffer << "\n";
    } else {
      exec(commands);
    }
  }
}
