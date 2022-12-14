/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Alexander Braml, Florian Frank
 * @copyright University of Passau
 */
#include "Device.h"

#include <cstring>
#include <regex>
#include <utility>

#include "ctlib/Socket.hpp"
#include "ctlib/Logging.hpp"
#include "ctlib/Exception.h"

extern "C" {
#include "ctlib/ErrorHandler.h"
}

#ifdef __APPLE__
#include <sstream>
#endif // __APPLE__

Device::Device(std::string ipAddress, int timeoutInMs) : Device(std::move(ipAddress), timeoutInMs, nullptr){
}


/**
 * @brief Constructor of Device
 * @param ip: the m_IPAddr address of the target device
 */
Device::Device(std::string ipAddress, int timeoutInMs, PIL::Logging *logger) : m_IPAddr(std::move(ipAddress)), m_ErrorHandle(){
    m_SocketHandle = new PIL::Socket(TCP, IPv4, m_IPAddr, m_Port, timeoutInMs);
    m_ErrorHandle.m_ErrorCode = PIL_NO_ERROR;
    m_Logger = logger;
}

Device::~Device(){
    Disconnect();
    delete m_SocketHandle;
}

PIL_ERROR_CODE Device::handleErrorsAndLogging(PIL_ERROR_CODE errorCode, bool throwException,
                                              PIL::Level logLevel, const char *fileName, int line, const char* formatStr, ...)
{
    va_list va;
    va_start(va, formatStr);
    PIL_SetLastErrorMsg(&m_ErrorHandle, errorCode, "");
    if(m_Logger)
        m_Logger->LogMessage(logLevel, fileName, line, formatStr, va);

    if (!throwException){
        auto exception = PIL::Exception(errorCode, fileName, line, formatStr, va);
        va_end(va);
        throw exception;
    }
    va_end(va);
    return errorCode;
}

/**
 * @brief Establish a connection to the device.
 * @return if the connection is established return PIL_NO_ERROR otherwise return error code.
 */
PIL_ERROR_CODE Device::Connect()
{
    m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Device %s is connecting.", m_DeviceName.c_str());

    if (m_SocketHandle->Connect(m_IPAddr, m_Port) != PIL_NO_ERROR)
        return Device::handleErrorsAndLogging(m_SocketHandle->GetLastError(), true, PIL::ERROR, __FILENAME__, __LINE__, "");

    m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Connection to device %s:%d established!", m_IPAddr.c_str(), m_Port);
    return PIL_NO_ERROR;
}

PIL_ERROR_CODE Device::Disconnect(){
    if (!m_SocketHandle->IsOpen())
        return Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, true, PIL::ERROR, __FILENAME__, __LINE__, "");

    auto errCode = m_SocketHandle->Close();
    if(errCode != PIL_NO_ERROR)
        return Device::handleErrorsAndLogging(errCode, true, PIL::ERROR, __FILENAME__, __LINE__, "");

    m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__,"Socket disconnected");
    return PIL_NO_ERROR;
}

bool Device::IsOpen() const{
    return m_SocketHandle->IsOpen();
}


std::string Device::GetDeviceIdentifier()
{
    if(!IsOpen())
        return PIL_ErrorCodeToString(Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, true, PIL::ERROR, __FILENAME__, __LINE__, ""));

    char buffer[512];

    SubArg arg("IDN", "*", "?");
    ExecArgs args;
    args.AddArgument(arg, "");

    if(Exec("", &args, buffer) != PIL_NO_ERROR)
        return "Error while executing *IDN?";

    return std::regex_replace(buffer, std::regex("\n"), "");
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
    if(!m_SocketHandle->IsOpen())
        return Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, true, PIL::ERROR, __FILENAME__, __LINE__, "Error interface is closed");

    if (br) {
    message += '\n';
  }
  char *messageString = const_cast<char *>(message.c_str());

  // TODO: add timeout
  int commandLen = static_cast<int>(strlen(messageString));
  if(m_SocketHandle->Send(reinterpret_cast<uint8_t*>(messageString), &commandLen) != PIL_NO_ERROR)
      return Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, true, PIL::ERROR, __FILENAME__, __LINE__, "Error while calling send");

  m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Command %s successfully executed", messageString);

  if (result) { // not all operation need a result
      uint32_t len = 2048;
      memset(m_recvBuff, '\0', 2048);
    if(m_SocketHandle->Receive(m_recvBuff, &len) != PIL_NO_ERROR)
    {
        if(m_Logger)
            m_Logger->LogMessage(PIL::ERROR, __FILENAME__, __LINE__,
                             PIL_ErrorCodeToString(m_SocketHandle->GetLastError()));
        PIL_SetLastErrorMsg(&m_ErrorHandle, PIL_ERRNO, "Error while calling read");
        return PIL_ERRNO;
    }
    int cpySize = strlen((const char*)m_recvBuff);
    if(cpySize > size)
        return PIL_INSUFFICIENT_RESOURCES;
      snprintf(result, size, "%s", m_recvBuff);
  }
    if(m_Logger)
        m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__,
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

