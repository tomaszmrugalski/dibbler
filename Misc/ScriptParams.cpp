/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <stdio.h>
#include <sstream>
#include "ScriptParams.h"
#include "Logger.h"
#include <string.h>
#include "Portable.h"

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

void TNotifyScriptParams::addParam(const std::string& name, const std::string& value)
{
    if (envCnt>=MAX_PARAMS) {
        Log(Error) << "Too many parameter for script: " << envCnt << LogEnd;
        return;
    }

    // +2, because = and \n have to be added.
    size_t len = name.length() + value.length() + 2;
    char * tmp = new char[len];
    snprintf(tmp, len, "%s=%s", name.c_str(), value.c_str());
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
    stringstream name, value;
    name << "ADDR" << ipCnt;
    addParam(name.str().c_str(), addr->getPlain());
    name.str("");

    name << "ADDR" << ipCnt << "PREF";
    value << prefered;
    addParam(name.str().c_str(), value.str().c_str());
    name.str("");
    value.str("");

    name << "ADDR" << ipCnt << "VALID";
    value << valid;
    addParam(name.str().c_str(), value.str().c_str());
    ipCnt++;
}

void TNotifyScriptParams::addPrefix(SPtr<TIPv6Addr> prefix, unsigned short length, unsigned int prefered, 
                                    unsigned int valid, std::string txt /*= std::string("") */ ) {
    stringstream name, value;
    name << "PREFIX" << pdCnt;
    addParam(name.str().c_str(), prefix->getPlain());
    name.str("");

    name << "PREFIX" << pdCnt << "LEN";
    value << length;
    addParam(name.str().c_str(), value.str().c_str());
    name.str("");
    value.str("");

    name << "PREFIX" << pdCnt << "PREF";
    value << prefered;
    addParam(name.str().c_str(), value.str().c_str());
    name.str("");
    value.str("");

    name << "PREFIX" << pdCnt << "VALID";
    value << valid;
    addParam(name.str().c_str(), value.str().c_str());
    name.str("");
    value.str("");

    pdCnt++;
}
