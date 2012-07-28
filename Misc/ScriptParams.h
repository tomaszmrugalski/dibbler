/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef NOTIFYSCRIPTPARAMS
#define NOTIFYSCRIPTPARAMS

#include <iostream>
#include "IPv6Addr.h"

class TNotifyScriptParams {
public:
    static const int MAX_PARAMS = 512;
    const char * env[MAX_PARAMS];
    std::string params;
    int envCnt;
    int ipCnt;
    int pdCnt;
    TNotifyScriptParams();
    ~TNotifyScriptParams();
    void addParam(const std::string& name, const std::string& value);
    void addAddr(SPtr<TIPv6Addr> addr, unsigned int prefered, unsigned int valid, 
                 std::string txt = std::string("") );
    void addPrefix(SPtr<TIPv6Addr> prefix, unsigned short length, unsigned int prefered, 
                   unsigned int valid, std::string txt = std::string(""));
};

#endif
