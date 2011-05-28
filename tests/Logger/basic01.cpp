#include "Logger.h"
#include <sstream>

using namespace std;

int main() {

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

    return 0;
}
