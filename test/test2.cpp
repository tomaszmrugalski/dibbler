#include <Logger.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main() {
    cout << "Hello, fucking openwrt world" << endl;

    string x;

    string * logname = new string("Init");
    ofstream f;
    ostringstream * buffer = new ostringstream();


   Log(Emerg) << "Hello, Log(Emerg) " << LogEnd;

#if 1
    ostringstream strum;
    strum << "dupa";

    logger::setLogLevel(6);
    logger::Initialize("foo.log");
    Log(Debug) << "Entry Debug: str=" << LogEnd;
    Log(Info) << "Entry Info: str=" << LogEnd;    
    Log(Notice) << "Entry Notice: str=" << strum.str() << LogEnd;
    Log(Warning) << "Entry Warning: str=" << strum.str() << LogEnd;
    Log(Error) << "Entry Error: str=" << strum.str() << LogEnd;
    Log(Crit) << "Entry Crit: str=" << strum.str() << LogEnd;
    Log(Alert) << "Entry Alert: str=" << strum.str() << LogEnd;
    Log(Emerg) << "Entry Emerg: str=" << strum.str() << LogEnd;
    logger::Terminate();
    
#endif


    return 0;
}
