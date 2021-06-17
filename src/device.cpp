#include "device.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Device::Device(char *ip) : ip(ip) {
}

int Device::test() {
    char* command = "OUTPut CH1,ON";
    int cont = send(sockfd, command, strlen(command), 0);
    cout << cont;
}

int Device::connect() {
    // TODO: add connection timeout
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

int Device::exec(string message, char* result, bool br) {
    if (br) {
        // Some commands for some devices \n can cause failure, seems a bug to me
        message += '\n';
    }
    char* command = const_cast<char *>(message.c_str());
    cout << "Executing Command: " << command;
    // TODO: add timeout
    // testcase: KST3000 k.exec("STATus? CHANnel2", buffer);
    send(sockfd, command, strlen(command), 0);
    if (result) {
        // TODO: dynamic length - 1024
        // TODO: add timeout
        read(sockfd, result, 1024);
    }
    cout << "Executed Successfully.\n";
    return 0;
}

void Device::what_am_i() {
    cout << "My name is: " << name << "\n";
}

void Device::cli() {
    char buffer[1024] = {0};
    while(1) {
        cout << "Input a command: ";
        string commands = "";
        getline(cin, commands);
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