/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 *
 */

#ifndef OPTIAPREFIX_H
#define OPTIAPREFIX_H

#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "IPv6Addr.h"

class TOptIAPrefix : public TOpt
{
  public:
    TOptIAPrefix( char * &addr, int &n, TMsg* parent);
    TOptIAPrefix( SPtr<TIPv6Addr> addr, char prefix_length,unsigned long pref, unsigned long valid, TMsg* parent);
    int getSize();
    
    char * storeSelf( char* buf);
    SPtr<TIPv6Addr> getPrefix();
    char getPrefixLength();
    unsigned long getPref();
    unsigned long getValid();
    void setPref(unsigned long pref);
    void setValid(unsigned long valid);
    void setPrefixLenght(char prefix_length);
    bool isValid();    

 private:
    SPtr<TIPv6Addr> Prefix; // 
    unsigned long Valid;
    unsigned long Pref;
    char PrefixLength; // this I am not sure (because prefix should be only 1 byte )
    bool ValidOpt;
};

#endif
