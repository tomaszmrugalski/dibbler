/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * $Id: Logger.h,v 1.17 2008-06-18 21:55:59 thomson Exp $
 *
 * Released under GNU GPL v2 or later licence
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
	LOGMODE_SYSLOG,
	LOGMODE_EVENTLOG, /* unix only */
	LOGMODE_PRECISE   /* windows only */
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
