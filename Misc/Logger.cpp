#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>
#include "Logger.h"

namespace logger {

    std::streambuf* orig;
    int logLevel=8;
    bool mute = false;
    string logname="Unknown";
    ofstream logfile;
    ofstream tmp;

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

	// FIXME:
/*	if ( x >= logger::logLevel) {
	    mute = 1;
	    strum.clear(ios::failbit);
	    return strum;
	}
	strum.clear();
	mute=0; */

	time_t teraz;
	teraz = time(NULL);
	struct tm * czas = localtime( &teraz );
	strum << (1900+czas->tm_year) << ".";
	strum.width(2); strum.fill('0'); strum << czas->tm_mon+1 << ".";
	strum.width(2);	strum.fill('0'); strum << czas->tm_mday  << " ";
	strum.width(2);	strum.fill('0'); strum << czas->tm_hour  << ":";
	strum.width(2);	strum.fill('0'); strum << czas->tm_min   << ":";
	strum.width(2);	strum.fill('0'); strum << czas->tm_sec;
	strum << ' ' << logger::logname << ' ';
	strum << lv[x-1] << " ";
	return strum;
    }

    ostream& logEmerg(ostream & strum)   { return logger::logCommon(strum,1); }
    ostream& logAlert(ostream & strum)   { return logger::logCommon(strum,2); }
    ostream& logCrit(ostream & strum)    { return logger::logCommon(strum,3); }
    ostream& logError(ostream & strum)   { return logger::logCommon(strum,4); }
    ostream& logWarning(ostream & strum) { return logger::logCommon(strum,5); }
    ostream& logNotice(ostream & strum)  { return logger::logCommon(strum,6); }
    ostream& logInfo(ostream & strum)    { return logger::logCommon(strum,7); }
    ostream& logDebug(ostream & strum)   { return logger::logCommon(strum,8); }

    void setLogname(string x) {
	logger::logname = x;
    }

    void Initialize(char * file) {
	logger::tmp.clear(ios::failbit);
	
	logger::orig = std::clog.rdbuf();

	logger::logfile.open(file);
  	std::clog.rdbuf(logger::logfile.rdbuf());

    }

    void Terminate() {
	std::clog.rdbuf(logger::orig);
    }

    void setLogLevel(int x) {
	if (x>8 || x<1) 
	    return;
	logger::logLevel = x;
    }
}
