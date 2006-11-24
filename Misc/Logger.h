/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Micha³ Kowalczuk <michal@kowalczuk.eu>
 *
 * $Id: Logger.h,v 1.14 2006-11-24 01:24:16 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2006-07-03 17:56:58  thomson
 * Precise logmode added.
 *
 * Revision 1.12  2006/03/05 21:39:19  thomson
 * TA support merged.
 *
 * Revision 1.11.2.1  2006/02/05 23:38:08  thomson
 * Devel branch with Temporary addresses support added.
 *
 * Revision 1.11  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.10  2004/12/08 00:17:59  thomson
 * StatusCodeToString function added.
 *
 * Revision 1.9  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.8  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.7  2004/09/03 20:58:36  thomson
 * *** empty log message ***
 *
 * Revision 1.6  2004/06/04 21:02:45  thomson
 * *** empty log message ***
 *
 * Revision 1.5  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.4  2004/05/23 19:12:34  thomson
 * *** empty log message ***
 *
 * Revision 1.3  2004/04/10 12:18:01  thomson
 * Numerous fixes: LogName, LogMode options added, dns-servers changed to
 * dns-server, '' around strings are no longer needed.
 *
 * Revision 1.2  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 *
 *
 * Released under GNU GPL v2 licence
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

#endif
