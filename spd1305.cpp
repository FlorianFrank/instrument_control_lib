//
// Created by liuwuhao on 20.05.21.
//
#include <iostream>
#include "spd1305.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

SPD1305::SPD1305(char* ip) : Device(ip, port) {
    this->ip = ip;
}

int SPD1305::connect() {
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        std::cout << "\n Invalid address / Address not supported \n" << std::endl;
        return -1;
    }

    if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed. \n";
        return -1;
    }

    cout << "Connection Succeed. \n";
    return 0;
}

int SPD1305::exec(char* message, char* result) {
    send(sockfd, message, strlen(message), 0);
    if (result) {
        // TODO: dynamic length - 1024
        read(sockfd, result, 1024);
        return 0;
    }
    return 0;
}

int SPD1305::setCurrent(double current, int channel) {
    string str = "CH" + to_string(channel) + ":CURRent " + to_string(current);
    char command[str.length() + 1];
    strcpy(command, str.c_str());
    exec(command);
}

double SPD1305::getCurrent(int channel) {
    string str = "CH" + to_string(channel) + ":CURRent?";
    char command[str.length() + 1];
    strcpy(command, str.c_str());
    // TODO: 20 ?
    char result[20] = {0};
    exec(command, result);
    return atof(result);
}
