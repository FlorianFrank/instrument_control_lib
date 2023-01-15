#include <gtest/gtest.h> // google test
#include "Device.h"

#include <ctlib/Exception.h>
#include <thread>
#include "ctlib/Logging.h"
extern "C" {
#include "ctlib/Socket.h"
}
#include <ctlib/Socket.hpp>

TEST(DeviceTest, DeviceNotAvailableException)
{
    // TODO solve in common_tools_lib
    EXPECT_THROW({
                     try
                     {
                         PIL::Logging logger(PIL::INFO, nullptr);
                         Device device("invalid", 1000, &logger, true);
                         auto ret = device.Connect();
                         EXPECT_EQ(ret, PIL_ERRNO);

                     } catch (PIL::Exception &e)
                     {
                        EXPECT_STREQ("Exception (Socket.cpp:115): Errno 115 (Operation now in progress)", e.what());
                         throw;
                     }
                 }, PIL::Exception);
}

TEST(DeviceTest, TestUnableToConnectException)
{
    EXPECT_THROW({
                     try {
                         PIL::Logging logger(PIL::INFO, nullptr);
                         Device device("127.0.0.1", 0, &logger, true);
                         auto ret = device.Connect();
                     }
                     catch(PIL::Exception &e) {
                         EXPECT_STREQ( "Exception (Socket.cpp:115): Errno 111 (Connection refused)", e.what() );
                         throw;
                     }
                 }, PIL::Exception );
}

TEST(DeviceTest, TestUnableToConnectRetCode)
{
    EXPECT_THROW({
                     try
                     {
                         PIL::Logging logger(PIL::INFO, nullptr);
                         Device device("127.0.0.1", 0, &logger, false);
                         auto ret = device.Connect();
                         EXPECT_EQ(ret, PIL_ERRNO);
                     } catch (PIL::Exception &e)
                     {
                         EXPECT_STREQ("Exception (Socket.cpp:115): Errno 111 (Connection refused)", e.what());
                         throw;
                     }
                 }, PIL::Exception);
}

TEST(DeviceTest, TestDisconnect)
{
    PIL::Logging logger(PIL::INFO, nullptr);
    Device device("127.0.0.1", 0, &logger, false);
    auto ret = device.Disconnect();
    EXPECT_EQ(ret, PIL_NO_ERROR);
}

/*TEST(DeviceTest, IdentificationTest)
{
    std::string address = "127.0.0.1";
    PIL::Socket testSocket(TCP, IPv4, address, 5025, 1000);

    std::function<void(std::shared_ptr<PIL::Socket>&, std::string&)> receiveCallback([](std::shared_ptr<PIL::Socket> &socket, const std::string& buffer)
                                                                                     {
                                                                                         EXPECT_TRUE(std::string("*IDN?") ==  buffer);
                                                                                         std::string retMessage("Agilent Technologies,33522B\n");
                                                                                         fflush(stdout);
                                                                                         socket->Send(retMessage);
                                                                                     });
    PIL::Socket::ReceiveCallbackArg additionalArg(receiveCallback);


    std::function<void(std::shared_ptr<PIL::Socket>&)> acceptCallback([&additionalArg](const std::shared_ptr<PIL::Socket>& socket)
    {
           auto returnCode = socket->RegisterReceiveCallbackFunction(additionalArg);
        EXPECT_EQ(returnCode, PIL_NO_ERROR);
    });
    auto returnCode = testSocket.CreateServerSocket(acceptCallback);
    EXPECT_EQ(returnCode, PIL_NO_ERROR);


    PIL::Logging logger(PIL::INFO, nullptr);
    Device device("127.0.0.1", 5026, 5025, 1000, &logger, true);
    auto ret = device.Connect();
    EXPECT_EQ(ret, PIL_NO_ERROR);
    std::string deviceIdentifier = device.GetDeviceIdentifier();
    EXPECT_EQ("Agilent Technologies,33522B", deviceIdentifier);

    auto errorCode = testSocket.Disconnect();
    EXPECT_EQ(errorCode, PIL_NO_ERROR);
}*/

/*
TEST(DeviceTest, TestSimpleExecution)
{
    std::string address = "127.0.0.1";
    PIL::Socket testSocket(TCP, IPv4, address, 5025, 1000);


    auto acceptCallback = [](PIL_SOCKET socket, char *buff)
    {
        auto receiveCallback = [](PIL_SOCKET socket, uint8_t *buffer, uint32_t len)
        {
            char *sendBuff = "Ret\n";
            uint32_t sendLen = strlen(sendBuff);
            PIL_SOCKET_Send(&socket, reinterpret_cast<uint8_t*>(sendBuff), &sendLen);
        };

        auto returnCode = PIL_SOCKET_RegisterCallbackFunction(&socket, receiveCallback);
        EXPECT_EQ(returnCode, PIL_NO_ERROR);

    };
    auto returnCode = testSocket.CreateServerSocket(acceptCallback);
    EXPECT_EQ(returnCode, PIL_NO_ERROR);


    PIL::Logging logger(PIL::INFO, nullptr);
    Device device("127.0.0.1", 5024, 5025, 1000, &logger, true);
    auto ret = device.Connect();
    EXPECT_EQ(ret, PIL_NO_ERROR);
    std::string command = "TestCommand";
    std::string result;
    returnCode = device.Exec(command, nullptr, &result, false);
    EXPECT_EQ(returnCode, PIL_NO_ERROR);
    EXPECT_EQ("Ret\n", result);
}*/
