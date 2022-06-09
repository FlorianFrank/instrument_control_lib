#include "iostream"
#include "SPD1305.h"

using namespace std;

int main() {
    SPD1305 *spd = new SPD1305("132.231.14.176", 0);
    spd->Connect();
    double current = spd->get_current();
    cout << current << endl;
    return 0;
}
