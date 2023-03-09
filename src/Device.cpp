/**
 * @brief Base class containing the base functionality of all devices.
 * And parameters to store the name connection type, etc.
 * @authors Wuhao Liu, Alexander Braml, Florian Frank
 * @copyright University of Passau
 */
#include "Device.h"
#include "HTTPRequest.h"

#include <regex> // std::regex_replace
#include <iostream> // std::cout
#include <utility>
#include <thread>
#include <chrono>

#include "ctlib/Socket.hpp"
#include "ctlib/Logging.hpp"

extern "C" {
#include "ctlib/ErrorHandler.h"
}

#ifdef __APPLE__
#include <sstream>
#endif // __APPLE__

Device::Device(std::string ipAddress, uint16_t srcPort, uint16_t destPort, int timeoutInMs, PIL::Logging *logger,
               SEND_METHOD mode,
               bool throwException) : m_IPAddr(std::move(ipAddress)), m_ErrorHandle(), m_destPort(destPort),
                                      m_srcPort(srcPort),
                                      m_EnableExceptions(throwException) {
    m_SocketHandle = new PIL::Socket(TCP, IPv4, m_IPAddr, m_destPort, timeoutInMs);
    m_ErrorHandle.m_ErrorCode = PIL_NO_ERROR;
    m_Logger = logger;
    m_SendMode = mode;
}


/**
 * @brief Constructor for the Device without passing a logging object.
 * @param ipAddress IP address of the device
 * @param timeoutInMs
 */
Device::Device(std::string ipAddress, int timeoutInMs, SEND_METHOD mode, bool throwException) : Device(
        std::move(ipAddress), timeoutInMs, nullptr, mode, throwException) {}


/**
 * @brief Constructor of Device generates a socket handle without connecting to it.
 * @param ipAddress: The ip address to connect to the device.
 * @param timeoutInMs: Timeout of the socket in milliseconds.
 * @param logger: Pass a logging object to configure what types of messages should be logged.
 * If nullptr is passed, logging is disabled.
 */
Device::Device(std::string ipAddress, int timeoutInMs, PIL::Logging *logger, SEND_METHOD mode, bool throwException) :
        Device(std::move(ipAddress), 5025, 5025, timeoutInMs, logger, mode, throwException) {}

Device::~Device() {
    Disconnect();
    delete m_SocketHandle;
}

/**
 * @brief Handle logging messages, logs it based on the previously passed logging object. If exceptions are enabled throw an exception
 * otherwise return the error code.
 * @param errorCode error code to identify the error.
 * @param throwException if true, throw an exception.
 * @param logLevel log level of the message.
 * @param fileName name of the file where the error occurred.
 * @param line line number where the error occurred.
 * @param formatStr format string for the error message.
 * @param ...   variable arguments for the format string.
 * @return error code.
 */
PIL_ERROR_CODE Device::handleErrorsAndLogging(PIL_ERROR_CODE errorCode, bool throwException,
                                              PIL::Level logLevel, const std::string &fileName, int line,
                                              const std::string formatStr, ...) {
    // Redirect std::cout shortly to a stringstream to get the error message without using fixed buffers.
    // Later replace with C++20 std::format.
    va_list args;
    va_start(args, formatStr);
    char buffer[1024];
    vsprintf(buffer, formatStr.c_str(), args);
    if (m_Logger)
        m_Logger->LogMessage(logLevel, fileName.c_str(), line, buffer);
    va_end(args);

    if (throwException)
        throw PIL::Exception(errorCode, fileName.c_str(), line, buffer);

    return errorCode;
}

/**
 * @brief Establish a connection to the device.
 * @return if the connection is established return PIL_NO_ERROR otherwise return error code.
 * @throw PIL::Exception if the connection could not established.
 */
PIL_ERROR_CODE Device::Connect() {
    m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Device is connecting.");

    auto ret = m_SocketHandle->Connect(m_IPAddr, m_destPort);
    if (ret != PIL_NO_ERROR)
        return Device::handleErrorsAndLogging(ret, m_EnableExceptions, PIL::ERROR, __FILENAME__, __LINE__, "");

    m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Connection to device %s:%d established!", m_IPAddr.c_str(),
                         m_destPort);
    return PIL_NO_ERROR;
}

/**
 * @brief Disconnect from the device.
 * @return if the connection is disconnected return PIL_NO_ERROR otherwise return error code.
 * @throw PIL::Exception if the connection could not disconnected.
 */
PIL_ERROR_CODE Device::Disconnect() {
    if (!m_SocketHandle->IsOpen()) {
        Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, false, PIL::ERROR, __FILENAME__,
                                       __LINE__, "");
        return PIL_NO_ERROR;
    }

    auto errCode = m_SocketHandle->Disconnect();
    if (errCode != PIL_NO_ERROR)
        return Device::handleErrorsAndLogging(errCode, m_EnableExceptions, PIL::ERROR, __FILENAME__, __LINE__, "");

    m_Logger->LogMessage(PIL::DEBUG, __FILENAME__, __LINE__, "Socket disconnected");
    return PIL_NO_ERROR;
}

/**
 * @brief Checks if the connection to the device is established.
 * @return true if connection is established, otherwise false.
 */
bool Device::IsOpen() const {
    return m_SocketHandle->IsOpen();
}

bool Device::IsBuffered() const {
    return m_SendMode == SEND_METHOD::BUFFER_ENABLED;
}

/**
 * @brief
 * @return
 */
std::string Device::GetDeviceIdentifier() {
    if (!IsBuffered() && !IsOpen())
        return PIL_ErrorCodeToString(
                Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, m_EnableExceptions, PIL::ERROR, __FILENAME__,
                                               __LINE__, ""));

    SubArg arg("IDN", "*", "?");
    ExecArgs args;
    args.AddArgument(arg, "");

    std::string ss;
    auto errCode = Exec("", &args, &ss, false);
    if (errCode != PIL_NO_ERROR)
        return PIL_ErrorCodeToString(
                Device::handleErrorsAndLogging(errCode, m_EnableExceptions, PIL::ERROR, __FILENAME__, __LINE__,
                                               "Error while executing Exec()"));

    return std::regex_replace(ss, std::regex("\n"), "");
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
 * @warning Some commands may timeout
 *      @code{.c}
 *      KST3000 k.Exec("RSTater?", buffer);
 *      @endcode
 * */
PIL_ERROR_CODE Device::Exec(const std::string &command, ExecArgs *args, char *result, bool br, int size) {
    std::string *res = nullptr;
    if (result)
        res = new std::string(result, size);
    auto ret = Exec(command, args, res, br);

    if (result && res->c_str())
        strcpy(result, res->c_str());
    return ret;
}

PIL_ERROR_CODE Device::Exec(const std::string &command, ExecArgs *args, std::string *result, bool br) {
    std::stringstream message;
    message << command;
    if (args)
        message << args->GetArgumentsAsString();
    if (!IsBuffered() && !m_SocketHandle->IsOpen())
        return Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, m_EnableExceptions, PIL::ERROR, __FILENAME__,
                                              __LINE__, "Error interface is closed");

    if (IsBuffered() || br)
        message << std::endl;

    auto strToSend = message.str();
    if (IsBuffered()) {
        m_BufferedScript += strToSend;
        return PIL_NO_ERROR;
    } else {
        if (m_SocketHandle->Send(strToSend) != PIL_NO_ERROR)
            return Device::handleErrorsAndLogging(PIL_INTERFACE_CLOSED, m_EnableExceptions, PIL::ERROR, __FILENAME__,
                                                  __LINE__,
                                                  "Error while calling send");

        m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Command %s successfully executed", strToSend.c_str());

        if (result) { // not all operation need a result
            auto ret = m_SocketHandle->Receive(*result);
            if (ret != PIL_NO_ERROR)
                return Device::handleErrorsAndLogging(ret, m_EnableExceptions, PIL::ERROR, __FILENAME__, __LINE__,
                                                      "Error while calling read");
            if (m_Logger)
                m_Logger->LogMessage(PIL::INFO, __FILENAME__, __LINE__, "Receive result: %s", result->c_str());
        }
        return PIL_NO_ERROR;
    }
}

/***
 * @brief Execute multiple commands seperated by newline (\\n).
 * @param commands commands seperated by newline (\\n).
 * @return PIL_NO_ERROR if no error occurs.
 * @throw PIL::Exception if an error occurs.
 */
PIL_ERROR_CODE Device::ExecCommands(std::string &commands) {
    std::stringstream s_commands(commands);
    std::string command;
    while (std::getline(s_commands, command)) {
        auto execRet = Exec(command);
        if (execRet != PIL_NO_ERROR)
            return Device::handleErrorsAndLogging(execRet, m_EnableExceptions, PIL::ERROR, __FILENAME__, __LINE__,
                                                  "Error while executing Exec()");
    }
    PIL_SetLastError(&m_ErrorHandle, PIL_NO_ERROR);
    return PIL_NO_ERROR;
}

std::string Device::ReturnErrorMessage() {
    return PIL_ReturnErrorMessageAsString(&m_ErrorHandle);
}

PIL_ERROR_CODE Device::delay(double delayTime) {
    if (IsBuffered()) {
        SubArg arg(std::to_string(delayTime), "delay(", ")");

        ExecArgs args;
        args.AddArgument(arg, "");
        return Exec("", &args, nullptr);
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds((int) delayTime * 1000));
        return PIL_NO_ERROR;
    }

}

std::string Device::getBufferedScript() {
    return m_BufferedScript;
}

void Device::changeSendMode(SEND_METHOD mode) {
    m_SendMode = mode;
}

/**
 * @brief Checks if a error occured given the error code.
 *
 * @param errorCode The error code to check.
 * @return true if and only if there is an error.
 */
/* static */ bool Device::errorOccured(PIL_ERROR_CODE errorCode) {
    return errorCode != PIL_NO_ERROR;
}

/**
 * @brief Sends a post request to the given url with the given payload.
 *
 * @param url The url to send the post request to.
 * @param payload The payload to send.
 */
/* static */ PIL_ERROR_CODE Device::postRequest(const std::string &url, std::string &payload) {
    try {
        http::Request request{url};
        const auto response = request.send("POST", payload, {
                {"Content-Type", "application/json"}
        });
        return PIL_NO_ERROR;
    } catch (const std::exception &e) {
        return PIL_ERRNO;  // TODO: Add exception for http requests
    }
}

/**
 * @brief Replaces all substrings in the string with the given replacement string.
 * @param str String to replace substrings in.
 * @param from The substring to replace.
 * @param to The string to replace all matching substrings with.
 * @return The processed string
 */
/* static */ std::string Device::replaceAllSubstrings(std::string str, const std::string &from, const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/**
 * @brief Splits a string by the given delimiter.
 *
 * @param toSplit The string to split.
 * @param delimiter The delimter to split the string by.
 * @return A vector of substrings of toSplit.
 */
/* static */ std::vector<std::string> Device::splitString(const std::string &toSplit, const std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = toSplit.find(delimiter, pos_start)) != std::string::npos) {
        token = toSplit.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(toSplit.substr(pos_start));
    return res;
}
