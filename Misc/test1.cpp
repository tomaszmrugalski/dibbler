#include <iostream>
#include <fstream>
#include "Logger.h"

using namespace std;

ostream & logCommon(ostream & strum) {
    cout << "X";
    return strum;
}

int main() {
    ofstream tmp("/dev/null");
    
//    clog << logCommon << "111111111111" << endl;
////    clog.rdbuf(tmp.rdbuf());
//    clog << logCommon << "222222222222" << endl;
//    clog.rdbuf(cout.rdbuf());
//    clog << logCommon << "333333333333" << endl;
    //logger::Initialize("log1.txt");

    Log(logDebug)  << "To jest 1 DEBUG" << logger::endl;
    Log(logDebug)  << "To jest 2 DEBUG" << logger::endl;
    Log(logDebug)  << "To jest 3 DEBUG" << logger::endl;
    logger::setLogLevel(1);
    Log(logDebug)  << "To jest 4 DEBUG" << logger::endl;
    Log(logDebug)  << "To jest 5 DEBUG" << logger::endl;
    logger::setLogLevel(8);
    Log(logDebug)  << "To jest 6 DEBUG" << logger::endl;
    Log(logDebug)  << "To jest 7 DEBUG" << logger::endl;
//    Log(logNotice) << "To jest NOTICE" << logger::endl;
//    Log(logInfo)   << "To jest INFO" << logger::endl;
//    Log(logWarning)<< "To jest WARN" << logger::endl;
}
