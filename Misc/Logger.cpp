/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 only licence                                
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

#ifdef LINUX
#include <sys/time.h>
#include <syslog.h>
#endif

#ifdef MACOS
#include <sys/time.h>
#include <syslog.h>
#endif

namespace logger {

    string logname="Init";  // Application ID in the log
    int logLevel=8;  // Don't log messages with lower priority (=higher number)
    Elogmode logmode = LOGMODE_DEFAULT; /* default logmode */
    ofstream logFile;    // file where wanted msgs are stored
    string logFileName;
    bool logFileMode = false;	// loging into file is active
    bool echo = true;		// copy log on tty
    int curLogEntry = 8;	// Log level of currently constructed message
    bool color = false;
#ifdef LINUX
    string syslogname="DibblerInit";	// logname for syslog
    int curSyslogEntry = LOG_NOTICE;	// curLogEntry for syslog
#endif

    ostringstream buffer;	// buffer for currently constructed message

    // LogEnd;
    ostream & endl (ostream & strum) {
	if (curLogEntry <= logLevel) {

	    if (color)
		buffer << "\033[0m";
	    // log on the console
	    if (echo)
		std::cout << buffer.str() << std::endl;

	    // log to the file
	    if (logFileMode)
		logger::logFile << buffer.str() << std::endl;
#ifdef LINUX
	    // POSIX syslog
	    if (logmode == LOGMODE_SYSLOG) 
		syslog(curSyslogEntry, "%s", buffer.str().c_str());
#endif
	}

	buffer.str(std::string());
	buffer.clear();

	return strum;
    }

    void setColors(bool colorLogs) {
	Log(Debug) << "Color logs " << (colorLogs?"enabled.":"disabled.") << LogEnd;
	color = colorLogs;
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

	static char colors[][10] = { "\033[31m",
				     "\033[31m",
				     "\033[31m",
				     "\033[31m",
				     "\033[33m",
				     "\033[30m",
				     "\033[30m",
				     "\033[37m" };

	logger::curLogEntry = x;

#ifdef LINUX
	static int syslogLevel[]= {LOG_EMERG,
				   LOG_ALERT,
				   LOG_CRIT,
				   LOG_ERR,
			           LOG_WARNING,
				   LOG_NOTICE,
				   LOG_INFO,
			           LOG_DEBUG};
	logger::curSyslogEntry = syslogLevel[logger::curLogEntry - 1];
#endif

	time_t teraz;
	teraz = time(NULL);
	struct tm * now = localtime( &teraz );
	if (color && (logmode==LOGMODE_FULL || logmode==LOGMODE_SHORT) )
	{
	    buffer << colors[x-1];
	}

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
	    return buffer;
	    break;
	case LOGMODE_EVENTLOG:
	    buffer << "EVENTLOG logging mode not supported yet.";
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

    /**
     * Prepare logging backend specified in logger::logmode for logging.
     */
    static void openLog() {
	switch (logger::logmode) {
	    case LOGMODE_FULL:
	    case LOGMODE_SHORT:
	    case LOGMODE_PRECISE:
		logger::logFileMode = true;
		logger::logFile.open(logFileName.c_str(),
			ofstream::out | ofstream::app);
		break;
	    case LOGMODE_SYSLOG:
#ifdef LINUX
		openlog(syslogname.c_str(), LOG_PID, LOG_DAEMON);
#endif
		break;
	    case LOGMODE_EVENTLOG:
#ifdef WIN32
#endif
		break;
	}
    }

    /**
     * Change logging mode, possibly backend too.
     * Some modes log into common backend, some modes have unique backends.
     *
     * @param newMode New logging mode
     * */
    static void changeLogMode(Elogmode newMode) {
	if (newMode != logger::logmode) {
	    if (logger::logFileMode &&
		    ((newMode == LOGMODE_FULL) || (newMode == LOGMODE_SHORT) ||
		     (newMode == LOGMODE_PRECISE))) 
		logger::logmode = newMode;
	    else {
		Terminate();
		logger::logmode = newMode;
		openLog();
	    }
	}
    }

    /**
     * Initialize logging.
     * 
     * @param file File suitable for logging into
     */
    void Initialize(const char * file) {
	logger::logFileName = std::string(file);
	openLog();
    }

    /**
     * Close loging backend.
     */
    void Terminate() {
	switch (logger::logmode) {
	    case LOGMODE_FULL:
	    case LOGMODE_SHORT:
	    case LOGMODE_PRECISE:
		logger::logFileMode = false;
		logger::logFile.close();
		break;
	    case LOGMODE_SYSLOG:
#ifdef LINUX
		closelog();
#endif
		break;
	    case LOGMODE_EVENTLOG:
#ifdef WIN32
#endif
		break;
	}
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
#ifdef LINUX
	logger::syslogname = std::string("Dibbler").append(logger::logname);
#endif
    }

    string getLogName() {
        return logger::logname;
    }

    int getLogLevel() {
        return logger::logLevel;
    }

    void setLogMode(string x) {
	if (x=="short") {
	    changeLogMode(LOGMODE_SHORT);
	}
	if (x=="full") {
	    changeLogMode(LOGMODE_FULL);
	}
	if (x=="precise") {
	    changeLogMode(LOGMODE_PRECISE);
	}

#ifdef LINUX
	if (x=="syslog") {
	    changeLogMode(LOGMODE_SYSLOG);
	}
#endif 
#ifdef WIN32
	if (x=="eventlog") {
	    changeLogMode(LOGMODE_EVENTLOG);
	}
#endif
    }
}


std::string StateToString(EState state) {
    std::stringstream tmp;

    switch (state) {
    case STATE_NOTCONFIGURED:
	return "NOTCONFIGURED";
    case STATE_INPROCESS:
	return "INPROCESS";
    case STATE_CONFIGURED:
	return "CONFIGURED";
    case STATE_FAILED:
	return "FAILED";
    case STATE_DISABLED:
	return "DISABLED";
    case STATE_TENTATIVECHECK:
	return "TENTATIVECHECK";
    case STATE_CONFIRMME:
	return "CONFIRMME";
    case STATE_TENTATIVE:
	return "TENTATIVE";
    default:
	tmp << "UNKNOWN(" << state << ")";
	return tmp.str();
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
    case STATUSCODE_NOPREFIXAVAIL:
	return "No prefix available";
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
