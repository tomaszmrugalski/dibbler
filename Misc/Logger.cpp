/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: Logger.cpp,v 1.15.2.1 2006-02-05 23:38:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2005/07/17 21:09:52  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.14  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.13  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 * Revision 1.12  2004/12/08 00:17:59  thomson
 * StatusCodeToString function added.
 *
 * Revision 1.11  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.10  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.9  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include "Logger.h"
#include "Portable.h"
#include "DHCPConst.h"

namespace logger {

    string logname="Init";
    int logLevel=8;
    Elogmode logmode = SHORT;
    ofstream logFile;    // file where wanted msgs are stored
    bool logFileMode = false;
    bool echo = true;
    int curLogEntry = 8;

    ostringstream buffer;

    // LogEnd;
    ostream & endl (ostream & strum) {
	if (curLogEntry <= logLevel) {
	    // log on the console
	    if (echo)
		std::cout << buffer.str() << std::endl;

	    // log to the file
	    if (logFileMode)
		logger::logFile << buffer.str() << std::endl;
	}

	buffer.str(std::string());
	buffer.clear();

	return strum;
    }
    
    ostream & logCommon(int x) {
	static char lv[][10]= {"Emergency",
			       "Alert    ",
			       "Critical ",
			       "Error    ",
			       "Warning  ",
			       "Notice   ",
			       "Info     ",
			       "Debug    " };

	logger::curLogEntry = x;

	time_t teraz;
	teraz = time(NULL);
	struct tm * czas = localtime( &teraz );
	if (logmode!=SHORT) {
	    buffer << (1900+czas->tm_year) << ".";
	    buffer.width(2); buffer.fill('0'); buffer << czas->tm_mon+1 << ".";
	    buffer.width(2); buffer.fill('0'); buffer << czas->tm_mday  << " ";
	}
    buffer.width(2);    buffer.fill('0'); buffer << czas->tm_hour  << ":";
	buffer.width(2);	buffer.fill('0'); buffer << czas->tm_min   << ":";
	buffer.width(2);	buffer.fill('0'); buffer << czas->tm_sec;
	if (logmode!=SHORT) {
	    buffer << ' ' << logger::logname ;
	}
	buffer << ' ' << lv[x-1] << " ";
	return buffer;
    }

    ostream& logCont()    { return logger::buffer; }
    ostream& logEmerg()   { return logger::logCommon(1); }
    ostream& logAlert()   { return logger::logCommon(2); }
    ostream& logCrit()    { return logger::logCommon(3); }
    ostream& logError()   { return logger::logCommon(4); }
    ostream& logWarning() { return logger::logCommon(5); }
    ostream& logNotice()  { return logger::logCommon(6); }
    ostream& logInfo()    { return logger::logCommon(7); }
    ostream& logDebug()   { return logger::logCommon(8); }

    void Initialize(char * file) {
	logger::logFileMode = true;
	logger::logFile.open(file);
    }

    void Terminate() {
	logger::logFileMode = false;
	logger::logFile.close();
    }

    void EchoOn() {
	logger::echo = true;
    }
    
    void EchoOff() {
	logger::echo = false;
    }

    void setLogLevel(int x) {
	if (x>8 || x<1) 
	    return;
	logger::logLevel = x;
    }

    void setLogName(string x) {
	logger::logname = x;
    }

    string getLogName() {
        return logger::logname;
    }

    int getLogLevel() {
        return logger::logLevel;
    }

    void setLogMode(string x) {
	if (x=="short") {
	    logger::logmode = SHORT;
	}
	if (x=="full") {
	    logger::logmode = FULL;
	}

#ifdef LINUX
	if (x=="syslog") {
	    logger::logmode = SYSLOG;
	}
#endif 
#ifdef WIN32
	if (x=="eventlog") {
	    logger::logmode = EVENTLOG;
	}
#endif
    }
}


std::string StateToString(EState state) {
    switch (state) {
    case NOTCONFIGURED:
	return "NOTCONFIGURED";
	break;
    case INPROCESS:
	return "INPROCESS";
	break;
    case CONFIGURED:
	return "CONFIGURED";
	break;
    case FAILED:
	return "FAILED";
	break;
    case UNKNOWN:
	return "UNKNOWN";
	break;
    case TENTATIVECHECK:
	return "TENTATIVECHECK";
	break;
    case TENTATIVE:
	return "TENTATIVE";
	break;
    default:
	return "???";
    }
}

std::string StatusCodeToString(int status) {
    switch(status) {
    case STATUSCODE_SUCCESS:
	return "Success";
    case STATUSCODE_UNSPECFAIL:
	return "Unspecified failure";
    case STATUSCODE_NOADDRSAVAIL:
	return "No addresses available";
    case STATUSCODE_NOBINDING:
	return "No binding";
    case STATUSCODE_NOTONLINK:
	return "Not on link";
    case STATUSCODE_USEMULTICAST:
	return "Use multicast";
    }
    return "";
}

std::string MsgTypeToString(int msgType) {
    switch (msgType) {
    case SOLICIT_MSG:
	return "SOLICIT";
	break;
    case ADVERTISE_MSG:
	return "ADVERTISE";
    case REQUEST_MSG:
	return "REQUEST";
    case REPLY_MSG:
	return "REPLY";
    case RELEASE_MSG:
	return "RELEASE";
    case CONFIRM_MSG:
	return "CONFIRM";
    case DECLINE_MSG:
	return "DECLINE";
    case RENEW_MSG:
	return "RENEW";
    case REBIND_MSG:
	return "REBIND";
    case INFORMATION_REQUEST_MSG:
	return "INF-REQUEST";
    case RELAY_FORW_MSG:
	return "RELAY-FORW";
    case RELAY_REPL_MSG:
	return "RELAY-REPL";
	
    default:
	return "?";
    }
}
