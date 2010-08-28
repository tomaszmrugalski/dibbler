/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * $Id: Logger.h,v 1.19 2008-11-13 22:29:55 thomson Exp $
 *
 * Released under GNU GPL v2 only licence
 *
 */

#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>
#include "DHCPConst.h"

#define Log(X) logger :: log##X ()
#define LogEnd logger :: endl

#define LOGMODE_DEFAULT LOGMODE_FULL

namespace logger {

    enum Elogmode {
	LOGMODE_FULL,
	LOGMODE_SHORT,
	LOGMODE_PRECISE,
	LOGMODE_SYSLOG,  /* unix only */
	LOGMODE_EVENTLOG /* windows only */
    };

    using namespace std;
    ostream& logCont();
    ostream& logEmerg();
    ostream& logAlert();
    ostream& logCrit();
    ostream& logError();
    ostream& logWarning();
    ostream& logNotice();
    ostream& logInfo();
    ostream& logDebug();

    void Initialize(const char * file);
    void Terminate();
    void setLogName(string x);
    void setLogLevel(int x);
    void setLogMode(string x);
    void EchoOff();
    void EchoOn();
    void setColors(bool colors);
    string getLogName();
    int getLogLevel();
    
    ostream & endl (ostream & strum);

}

std::string StateToString(EState state);
std::string StatusCodeToString(int status);
std::string MsgTypeToString(int msgType);

// for debugging purposes
void PrintHex(std::string message, char *buf, unsigned len);

#endif
