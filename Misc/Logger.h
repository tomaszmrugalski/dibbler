/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: Logger.h,v 1.8 2004-10-27 22:07:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

#define Log(X) std::clog << logger:: log##X
#define LogEnd logger::endl

namespace logger {

    enum Elogmode {
	FULL,
	SHORT,
	SYSLOG,
	EVENTLOG
    };

    using namespace std;
    ostream& logCont(ostream & strum);
    ostream& logEmerg(ostream & strum);
    ostream& logAlert(ostream & strum);
    ostream& logCrit(ostream & strum);
    ostream& logError(ostream & strum);
    ostream& logWarning(ostream & strum);
    ostream& logNotice(ostream & strum);
    ostream& logInfo(ostream & strum);
    ostream& logDebug(ostream & strum);

    void Initialize(char * file);
    void Terminate();
    void setLogName(string x);
    void setLogLevel(int x);
    void setLogMode(string x);
    
    ostream & endl (ostream & strum);

}

std::string StateToString(EState state);


#endif
