#include "IfaceMgr.h"

using namespace std;
int main() {
    TIfaceMgr mgr;

    cout << mgr << endl;
    ipaddr_add("eth0", 2, "2000::1234", 100, 200);
    ipaddr_add("eth0", 2, "2000::1234", 100, 200);
}
