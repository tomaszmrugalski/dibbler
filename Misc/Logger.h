/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: Logger.h,v 1.2 2004-03-29 22:06:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>

namespace logger {
    using namespace std;
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
    void setLogname(string x);
    void setLogLevel(int x);
    
    ostream & endl (ostream & strum);

}


#endif
