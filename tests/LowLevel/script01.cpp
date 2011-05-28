#include "SmartPtr.h"
#include "Logger.h"
#include "IPv6Addr.h"
#include <iostream>

using namespace std;

int main() {


    SPtr<TIPv6Addr> prefix = new TIPv6Addr("2000::1",true);
    char buf[128];
    char cmd1[]="./mappingprefixadd";
    char cmd2[]="./mappingprefixdel";
    int returnCode = 0;
    sprintf(buf, "%s %s", "sh", cmd1);
    Log(Notice) << "Executing external command: [" << buf << "]" << LogEnd;
    returnCode = system(buf);
    Log(Notice) << "returnCode=" << returnCode << LogEnd;

    sprintf(buf, "sh /tmp/mappingprefixadd %s", prefix->getPlain());
    Log(Notice) << "Executing external command: [" << buf << "]" << LogEnd;
    returnCode = system(buf);
    Log(Notice) << "returnCode=" << returnCode << LogEnd;

#if 0
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

    cout << "Hello, fucking openwrt world" << endl;

    return 0;
}
