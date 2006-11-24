/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Micha³ Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: Logger.cpp,v 1.19 2006-11-24 01:24:16 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2006-07-04 21:19:51  thomson
 * Win32 compilation problems resolved.
 *
 * Revision 1.17  2006-07-03 17:56:58  thomson
 * Precise logmode added.
 *
 * Revision 1.16  2006/03/05 21:39:19  thomson
 * TA support merged.
 *
 * Revision 1.15.2.1  2006/02/05 23:38:08  thomson
 * Devel branch with Temporary addresses support added.
 *
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
    Elogmode logmode = LOGMODE_DEFAULT; /* default logmode */
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
	struct tm * now = localtime( &teraz );
	switch(logmode) {
	case LOGMODE_FULL:
	    buffer << (1900+now->tm_year) << ".";
	    buffer.width(2); buffer.fill('0'); buffer << now->tm_mon+1 << ".";
	    buffer.width(2); buffer.fill('0'); buffer << now->tm_mday  << " ";
	    buffer.width(2);    buffer.fill('0'); buffer << now->tm_hour  << ":";
	    buffer.width(2);	buffer.fill('0'); buffer << now->tm_min   << ":";
	    buffer.width(2);	buffer.fill('0'); buffer << now->tm_sec;
	    break;
	case LOGMODE_SHORT:
	    buffer.width(2);	buffer.fill('0'); buffer << now->tm_min   << ":";
	    buffer.width(2);	buffer.fill('0'); buffer << now->tm_sec;
	    break;
	case LOGMODE_PRECISE:
		int sec, usec;
#ifndef WIN32
		/* get time, Unix style */
		struct timeval preciseTime;
	    gettimeofday(&preciseTime, NULL);
		sec  = preciseTime.tv_sec%3600;
		usec = preciseTime.tv_usec; 
#else
		/* get time, Windws style */
		SYSTEMTIME now;
		GetSystemTime(&now);
		sec = now.wMinute*60 + now.wSecond;
		usec= now.wMilliseconds*1000;
#endif
		buffer.width(4); buffer.fill('0'); buffer << sec  << "s,";
	    buffer.width(6); buffer.fill('0'); buffer << usec << "us ";
	    break;
	case LOGMODE_SYSLOG:
	    buffer << "SYSLOG logging mode not supported yet.";
	    break;
	case LOGMODE_EVENTLOG:
	    buffer << "SYSLOG logging mode not supported yet.";
	    break;
	}
	buffer << ' ' << logger::logname ;
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

    void Initialize(const char * file) {
	logger::logFileMode = true;
	logger::logFile.open(file, ofstream::out | ofstream::app);
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
	    logger::logmode = LOGMODE_SHORT;
	}
	if (x=="full") {
	    logger::logmode = LOGMODE_FULL;
	}
	if (x=="precise") {
	    logger::logmode = LOGMODE_PRECISE;
	}

#ifdef LINUX
	if (x=="syslog") {
	    logger::logmode = LOGMODE_SYSLOG;
	}
#endif 
#ifdef WIN32
	if (x=="eventlog") {
	    logger::logmode = LOGMODE_EVENTLOG;
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
