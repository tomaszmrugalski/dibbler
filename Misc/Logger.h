/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * Released under GNU GPL v2 only licence
 *
 */

#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>
#include "DHCPConst.h"
#include <stdint.h>

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

    std::ostream& logCont();
    std::ostream& logEmerg();
    std::ostream& logAlert();
    std::ostream& logCrit();
    std::ostream& logError();
    std::ostream& logWarning();
    std::ostream& logNotice();
    std::ostream& logInfo();
    std::ostream& logDebug();
    std::ostream & endl (std::ostream & strum);

    void Initialize(const char * file);
    void Terminate();
    void setLogName(const std::string x);
    void setLogLevel(int x);
    void setLogMode(const std::string x);
    void EchoOff();
    void EchoOn();
    void setColors(bool colors);
    std::string getLogName();
    int getLogLevel();
}

std::string StateToString(EState state);
std::string StatusCodeToString(int status);
std::string MsgTypeToString(int msgType);

// for debugging purposes
void PrintHex(const std::string& message, const uint8_t *buf, unsigned len);

#endif
