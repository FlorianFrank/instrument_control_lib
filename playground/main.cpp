#include "iostream"
#include "test.h"

using namespace std;

int main(int argc, char* argv[]) {

    cout << "Connect to SMU with IP: " << argv[1] << std::endl;
//    int result = testSPD();
    cout << "Start SMU test" << std::endl;
    int result = testSMU(std::string());
    if (result == 0) {
        cout << "Tests PASSED!" << std::endl;
    }
    return 0;
}
