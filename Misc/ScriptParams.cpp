/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <sstream>
#include "ScriptParams.h"
#include "Logger.h"
#include <string.h>

using namespace std;

TNotifyScriptParams::TNotifyScriptParams() 
    :envCnt(0), ipCnt(1), pdCnt(1) {
    for (int i=0; i<512; i++) {
        env[i]=0;
    }
}

/// adds parameter to parameters list
///
/// @param value value to be copied
///
/// @return next unused offset
///
void TNotifyScriptParams::addParam(const char * value)
{
    if (envCnt>=MAX_PARAMS) {
        Log(Error) << "Too many parameter for script: " << envCnt << LogEnd;
        return;
    }

    char * tmp = new char[strlen(value)+1];
    strncpy(tmp, value, strlen(value)+1);
    env[envCnt] = tmp;
    envCnt++;
}

TNotifyScriptParams::~TNotifyScriptParams()
{
    int offset = 0;
    while (env[offset] != NULL) {
        delete [] env[offset];
        env[offset] = 0;
        offset++;
    }
}


void TNotifyScriptParams::addAddr(SPtr<TIPv6Addr> addr, unsigned int prefered, unsigned int valid, 
                                  std::string txt /*= std::string("")*/ ) {
    stringstream tmp;
    tmp << "ADDR" << ipCnt << "=" << addr->getPlain();
    addParam(tmp.str().c_str());
    tmp.str("");
    tmp << "ADDR" << ipCnt << "PREF=" << prefered;
    addParam(tmp.str().c_str());
    tmp.str("");
    tmp << "ADDR" << ipCnt << "VALID=" << valid;
    addParam(tmp.str().c_str());
    ipCnt++;
}

void TNotifyScriptParams::addPrefix(SPtr<TIPv6Addr> prefix, unsigned short length, unsigned int prefered, 
                                    unsigned int valid, std::string txt /*= std::string("") */ ) {
    stringstream tmp;
    tmp << "PREFIX" << pdCnt << "=" << prefix->getPlain();
    addParam(tmp.str().c_str());
    tmp.str("");

    tmp << "PREFIX" << pdCnt << "LEN=" << length;
    addParam(tmp.str().c_str());
    tmp.str("");

    tmp << "PREFIX" << pdCnt << "PREF=" << prefered;
    addParam(tmp.str().c_str());
    tmp.str("");

    tmp << "PREFIX" << pdCnt << "VALID=" << valid;
    addParam(tmp.str().c_str());
    tmp.str("");

    pdCnt++;
}
