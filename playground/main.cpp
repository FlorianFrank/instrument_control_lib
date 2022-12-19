#include <iostream>
#include "Device.h"
#include "devices/KEI2600.h"
#include "ctlib/Logging.hpp"
#include "devices/types/SMU.h"

int sweep() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr, Device::BUFFER_ENABLED);
    smu->Connect();

    double value = 0.0;

    smu->turnOn(SMU::SMU_CHANNEL::CHANNEL_A, false);
    smu->setLimit(SMU::VOLTAGE, SMU::SMU_CHANNEL::CHANNEL_A, 3, false);

    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 1, false);
    std::cout << smu->delay(1) << std::endl;
    smu->setLevel(SMU::VOLTAGE, SMU::CHANNEL_A, 2, false);
    smu->delay(1);
    smu->turnOff(SMU::SMU_CHANNEL::CHANNEL_A, false);

    // smu->measure(SMU::VOLTAGE, SMU::CHANNEL_A, &value, false);

    std::cout << smu->performLinearVoltageSweep(SMU::SMU_CHANNEL::CHANNEL_A, 0.0, 0.55, 18, 0.01, false) << std::endl;

    std::cout << "Buffered Script: " + smu->getBufferedScript() << std::endl;

    smu->changeSendMode(SMU::DIREKT_SEND);
    std::cout << smu->GetDeviceIdentifier() << std::endl;
    smu->changeSendMode(SMU::BUFFER_ENABLED);

    smu->executeBufferedScript(false);

    smu->Disconnect();

    return 0;
}

std::vector<std::string> split (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void readBuffer(KEI2600 *smu) {
    char bufferN[20];

    smu->Exec("print(b.n)", nullptr, bufferN);
    int n = (int) std::stod(bufferN);
    std::cout << std::fixed << "b.n: " << n << std::endl;

    int batchSize = 1024 / 15;
    int batches = n / batchSize;
    int remaining = n % batchSize;

    double results[n];

    char bufferPrint[15 * batchSize];

    for (int i = 0; i < batches; ++i) {
        int offset = i * batchSize;
        std::string toPrint = "b[" + std::to_string(1 + offset) + "]";
        for (int j = 2; j <= batchSize; ++j) {
            toPrint += ", b[" + std::to_string(j + offset) + "]";
        }

        std::string command = "print(" + toPrint + ")";
        std::cout << command << std::endl;
        smu->Exec(command, nullptr, bufferPrint);

        std::vector<std::string> bufferValues = split(std::string(bufferPrint), "\t");

        for (int j = 0; j < bufferValues.size(); ++j) {
            results[offset + j] = std::stod(bufferValues[j]);
        }
    }

    if (remaining > 0) {
        int offset = batches * batchSize;
        std::string toPrint = "b[" + std::to_string(offset + 1) + "]";

        for (int j = 2; j <= remaining; ++j) {
            toPrint += ", b[" + std::to_string(j + offset) + "]";
        }

        std::string command = "print(" + toPrint + ")";
        std::cout << command << std::endl;
        smu->Exec(command, nullptr, bufferPrint);

        std::vector<std::string> bufferValues = split(std::string(bufferPrint), "\t");

        for (int j = 0; j < bufferValues.size(); ++j) {
            results[offset + j] = std::stod(bufferValues[j]);
        }
    }
}

int testBuffer() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr, Device::DIREKT_SEND);
    smu->Connect();

    std::cout << smu->GetDeviceIdentifier() << std::endl;


    char buffer[65536];
    SubArg subArgPrint("");
    ExecArgs execArgs;

    // getBufferN
    // createBuffer
    // clearBuffer
    // measure -> use buffer
    // readBuffer

    subArgPrint = SubArg("");
    execArgs = ExecArgs();
    subArgPrint.AddElem("smua.makebuffer(1000)", "", "");
    execArgs.AddArgument(subArgPrint, "");
    smu->Exec("b = ", &execArgs, nullptr);
    printf("%s", buffer);

    smu->Exec("b.appendmode = 1", nullptr, nullptr, true, 0);
    printf("%s", buffer);

    smu->Exec("b.clear()", nullptr, nullptr, true, 0);
    printf("%s", buffer);

    smu->Exec("for i = 1, 100 do smua.measure.v(b) end", nullptr, nullptr, true, 0);
    printf("%s", buffer);

    readBuffer(smu);

    smu->Disconnect();

    return 0;
}

int testBufferImplemented() {
    std::string ip = "132.231.14.168";
    auto *smu = new KEI2600(ip.c_str(), 0, nullptr, Device::DIREKT_SEND);
    smu->Connect();
    std::cout << smu->GetDeviceIdentifier() << std::endl;

    std::cout << smu->createBuffer(SMU::CHANNEL_A, "b", 100, false);
    std::cout << smu->clearBuffer("b", false);
    int buffersize;
    std::cout << smu->getBufferSize("b", &buffersize, false);
    double buffer[buffersize * 15];
    std::cout << smu->readBuffer("b", buffer, false);

    smu->Disconnect();

    return 0;
}

int main(int argc, char* argv[]) {
    return testBufferImplemented();
}
