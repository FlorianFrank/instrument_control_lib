#include "device.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <regex>


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
 * @param ip: the m_IPAddr address of the target device
 */
Device::Device(const char *ip) : m_IPAddr(ip), m_ErrorHandle({0}), m_IsOpen(false) {
    m_ErrorHandle.m_ErrorCode = PIL_NO_ERROR;
}

Device::~Device()
{
    disconnect();
}

/**
 * @brief connect to the device
 * @todo deal with connection timeout
 * @todo deal with connection warning(you have not connected or connect automatically)
 */
bool Device::connect() {
  std::cout << m_DeviceName + " is connecting...\n";
  struct sockaddr_in serv_addr = {0};
  if ((m_SocketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Could not open socket");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_Port);
  if (inet_pton(AF_INET, m_IPAddr, &serv_addr.sin_addr) <= 0)
      return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling inet_pton");

  if (::connect(m_SocketHandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling connect");

    m_IsOpen = true;
    return PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
}

bool Device::disconnect()
{
    m_IsOpen = false;
    if(close(m_SocketHandle) == -1)
        return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while closing socket.");
    return PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
}

bool Device::isOpen() const
{
    return m_IsOpen;
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
bool Device::exec(std::string message, char *result, bool br, int size) {
    if(!m_IsOpen)
        return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_INTERFACE_CLOSED, "Error interface is closed");
    if (br) {
    message += '\n';
  }
  char *command = const_cast<char *>(message.c_str());
  if (DEBUG) {
    std::cout << "Executing Command: " << command;
  }
  // TODO: add timeout
  // testcase: KST3000 k.exec("STATus? CHANnel2", buffer);
  if(send(m_SocketHandle, command, strlen(command), 0) == -1)
    return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling send");

  if (result) { // not all operation need a result
    if(read(m_SocketHandle, result, size) == -1)
      return PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling read");
  }
  if (DEBUG) {
    std::cout << "Executed Successfully.\n";
  }
    return PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
}

bool Device::exec_commands(std::string &commands) {
  std::stringstream s_commands(commands);
  std::string command;
  while (std::getline(s_commands, command)) {
    if(!exec(command))
        return false;
  }
    return PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
}

/**
 * @brief Print a message containing the device's name
 * */
std::string Device::what_am_i() {
  return "My name is: " + m_DeviceName;
}

std::string Device::return_error_message()
{
    return std::string(PIL_ReturnErrorMessage(&m_ErrorHandle));
}

std::string Device::getDeviceIdentifier()
{
    if(!m_IsOpen)
    {
        PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_INTERFACE_CLOSED, "Error device is closed");
        return std::string(PIL_ReturnErrorMessage(&m_ErrorHandle));
    }
    char buffer[512];
    if(!exec("*IDN?", buffer))
        return "Error while executing *IDN?";

    return std::regex_replace(buffer, std::regex("\n"), "");
}
