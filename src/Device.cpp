/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Florian Frank
 * @copyright University of Passau
 */
#include "Device.h"

#include <cstring>
#include <regex>
#include <iostream>

#include "ctlib/Socket.hpp"
#include "ctlib/Logging.hpp"
extern "C" {
#include "ctlib/ErrorHandler.h"
}

#ifdef __APPLE__
#include <sstream>
#endif // __APPLE__

// TODO add further comments and cleanup some code

Device::Device(const char *ip, int timeoutInMs) : Device(ip, timeoutInMs, nullptr)
{
}


/**
 * @brief Constructor of Device
 * @param ip: the m_IPAddr address of the target device
 */
Device::Device(const char *ip, int timeoutInMs, PIL::Logging *logger) : m_IPAddr(ip), m_ErrorHandle()
{
    m_SocketHandle = new PIL::Socket(TCP, IPv4, ip, m_Port, timeoutInMs);
    m_ErrorHandle.m_ErrorCode = PIL_NO_ERROR;
    m_Logger = logger;
}

Device::~Device()
{
    Disconnect();
    delete m_SocketHandle;
}

/**
 * @brief Connect to the device
 * @todo deal with connection timeout
 * @todo deal with connection warning(you have not connected or Connect automatically)
 */
PIL_ERROR_CODE Device::Connect()
{
    if (m_Logger)
        m_Logger->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Device %s is connecting.", m_DeviceName.c_str());
    if (m_SocketHandle->GetLastError() != PIL_NO_ERROR)
    {
        if(m_Logger)
            m_Logger->LogMessage(ERROR_LVL, __FILENAME__, __LINE__,
                                 PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
        PIL_SetLastErrorMsg(&m_ErrorHandle, m_SocketHandle->GetLastError(), "Could not open socket");
        return m_SocketHandle->GetLastError();
    }

    if (m_SocketHandle->Connect(m_IPAddr, m_Port) != PIL_NO_ERROR)
    {
        if(m_Logger)
            m_Logger->LogMessage(ERROR_LVL, __FILENAME__, __LINE__, PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
        return m_SocketHandle->GetLastError();
    }
    else
    {
        if(m_Logger)
            m_Logger->LogMessage(INFO_LVL, __FILENAME__, __LINE__, "Connection to device %s:%d established!",
                             m_IPAddr.c_str(), m_Port);
    }

    PIL_SetLastError(&m_ErrorHandle, m_SocketHandle->GetLastError());
    return m_SocketHandle->GetLastError();
}

PIL_ERROR_CODE Device::Disconnect()
{
    if (m_SocketHandle->IsOpen())
    {
        if(m_SocketHandle->Close() != PIL_NO_ERROR)
        {
            if(m_Logger)
                m_Logger->LogMessage(ERROR_LVL, __FILENAME__, __LINE__,
                                 PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
            PIL_SetLastError(&m_ErrorHandle, m_SocketHandle->GetLastError());
            return m_SocketHandle->GetLastError();
        }
        if(m_Logger)
            m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"Socket disconnected");
        return PIL_NO_ERROR;
    }
    if(m_Logger)
        m_Logger->LogMessage(DEBUG_LVL, __FILENAME__, __LINE__,"Socket already closed");
    return PIL_INTERFACE_CLOSED;
}

bool Device::IsOpen() const
{
    return m_SocketHandle->IsOpen();
}

/**
 * @brief Print a message containing the device's name
 * */
// TODO: this function should be removed
std::string Device::WhatAmI() {
    return "My name is: " + m_DeviceName;
}

const char* Device::GetDeviceIdentifier()
{
    if(!IsOpen())
    {
        PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_INTERFACE_CLOSED, "Error device is closed");
        return PIL_ReturnErrorMessageAsString(&m_ErrorHandle);
    }
    char buffer[512];

    SubArg arg("IDN", "*", "?");

    ExecArgs args;
    args.AddArgument(arg, "");

    if(!Exec("", &args, buffer))
        return "Error while executing *IDN?";

    return std::regex_replace(buffer, std::regex("\n"), "").c_str();
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
 *      KST3000 k.Exec("RSTater?", buffer);
 *      @endcode
 * */
PIL_ERROR_CODE Device::Exec(std::string command, ExecArgs *args, char *result, bool br, int size)
{

    std::string message = command;
    if(args)
        message += args->GetArgumentsAsString();
    if(!m_SocketHandle->IsOpen()){
        PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_INTERFACE_CLOSED, "Error interface is closed");
        return PIL_INTERFACE_CLOSED;
    }

    if (br) {
    message += '\n';
  }
  char *c = const_cast<char *>(message.c_str());

  // TODO: add timeout
  // testcase: KST3000 k.exec("STATus? CHANnel2", buffer);
  int commandLen = static_cast<int>(strlen(c));
  if(m_SocketHandle->Send(reinterpret_cast<uint8_t*>(c), &commandLen) != PIL_NO_ERROR)
  {
      if(m_Logger)
          m_Logger->LogMessage(ERROR_LVL, __FILENAME__, __LINE__,
                               PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
      PIL_SetLastErrorMsg(&m_ErrorHandle, m_SocketHandle->GetLastError(), "Error while calling send");
      return m_SocketHandle->GetLastError();
  }

    if(m_Logger)
        m_Logger->LogMessage(INFO_LVL, __FILENAME__, __LINE__,
                             "Command %s successfully executed", c);

  if (result) { // not all operation need a result
    if(m_SocketHandle->Receive(reinterpret_cast<uint8_t*>(result), reinterpret_cast<uint32_t*>(&size)) != PIL_NO_ERROR)
    {
        if(m_Logger)
            m_Logger->LogMessage(ERROR_LVL, __FILENAME__, __LINE__,
                             PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
        PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling read");
        return PIL_ERRNO;
    }
  }
    if(m_Logger)
        m_Logger->LogMessage(INFO_LVL, __FILENAME__, __LINE__,
                             "Receive result: %s", result);

    PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
    return PIL_NO_ERROR;
}

/***
 * @brief
 * @param commands
 * @return
 */
PIL_ERROR_CODE Device::ExecCommands(std::string &commands) {
  std::stringstream s_commands(commands);
  std::string command;
  while (std::getline(s_commands, command)) {
      auto execRet = Exec(command);
    if(execRet != PIL_NO_ERROR)
        return execRet;
  }
    PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
    return PIL_NO_ERROR;
}

std::string Device::ReturnErrorMessage()
{
    return PIL_ReturnErrorMessageAsString(&m_ErrorHandle);
}

