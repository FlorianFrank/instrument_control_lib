#include "iostream"
#include "test.h"


int main(int argc, char *argv[]) {

    std::cout << "Connect to SMU with IP: " << argv[1] << std::endl;
    // int result = testSPD();
    std::cout << "Start SMU test" << std::endl;
    int result = testSMU(std::string(argv[1]), 0.001);
    if (result == 0) {
        std::cout << "Tests PASSED!" << std::endl;
    } else {
        std::cout << "Test finished with result: " << result << endl;
    }
    return 0;
}
