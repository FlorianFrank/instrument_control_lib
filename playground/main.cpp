#include "iostream"
#include "test.h"

using namespace std;

int main() {

//    int result = testSPD();

    int result = testSMU();
    if (result == 0) {
        cout << "Tests PASSED!\n";
    }
    return 0;
}
