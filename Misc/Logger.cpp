#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>
#include "Logger.h"
#include "Portable.h"

namespace logger {

    std::streambuf* orig;
    int logLevel=8;
    bool mute = false;
    string logname="Init";
    Elogmode logmode = FULL;
    ofstream logfile;            // file where wanted msgs are stored
    ofstream nullfile(NULLFILE); // file where unwanted msgs are dumped
    bool fileMode = false;

    ostream & endl (ostream & strum) {
	if (!mute)
	    strum<<std::endl;
	return strum;
    }
    
    ostream & logCommon(ostream & strum,int x) {
	static char lv[][10]= {"Emergency",
			       "Alert    ",
			       "Critical ",
			       "Error    ",
			       "Warning  ",
			       "Notice   ",
			       "Info     ",
			       "Debug    "
	};

	if ( x==0 )
	    return strum;

	if ( x > logger::logLevel) {
	    // ignore this entry
	    strum.rdbuf(logger::nullfile.rdbuf());
	    mute = 1;
	    return strum;
	} else {
	    // log this entry
	    if (fileMode) {
		strum.rdbuf(logger::logfile.rdbuf());
	    } else {
		strum.rdbuf(std::cerr.rdbuf());
	    }
	    mute = 0;
	}

	time_t teraz;
	teraz = time(NULL);
	struct tm * czas = localtime( &teraz );
	if (logmode!=SHORT) {
	    strum << (1900+czas->tm_year) << ".";
	    strum.width(2); strum.fill('0'); strum << czas->tm_mon+1 << ".";
	    strum.width(2); strum.fill('0'); strum << czas->tm_mday  << " ";
	    strum.width(2); strum.fill('0'); strum << czas->tm_hour  << ":";
	}
	strum.width(2);	strum.fill('0'); strum << czas->tm_min   << ":";
	strum.width(2);	strum.fill('0'); strum << czas->tm_sec;
	if (logmode!=SHORT) {
	    strum << ' ' << logger::logname ;
	}
	strum << ' ' << lv[x-1] << " ";
	return strum;
    }

    ostream& logCont(ostream & strum)    { return logger::logCommon(strum,0); }
    ostream& logEmerg(ostream & strum)   { return logger::logCommon(strum,1); }
    ostream& logAlert(ostream & strum)   { return logger::logCommon(strum,2); }
    ostream& logCrit(ostream & strum)    { return logger::logCommon(strum,3); }
    ostream& logError(ostream & strum)   { return logger::logCommon(strum,4); }
    ostream& logWarning(ostream & strum) { return logger::logCommon(strum,5); }
    ostream& logNotice(ostream & strum)  { return logger::logCommon(strum,6); }
    ostream& logInfo(ostream & strum)    { return logger::logCommon(strum,7); }
    ostream& logDebug(ostream & strum)   { return logger::logCommon(strum,8); }

    void Initialize(char * file) {
	fileMode = true;
	logger::orig = std::clog.rdbuf();

	logger::logfile.open(file);
  	std::clog.rdbuf(logger::logfile.rdbuf());

    }

    void Terminate() {
	std::clog.rdbuf(logger::orig);
	fileMode = false;
    }

    void setLogLevel(int x) {
	if (x>8 || x<1) 
	    return;
	logger::logLevel = x;
    }

    void setLogName(string x) {
	logger::logname = x;
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
