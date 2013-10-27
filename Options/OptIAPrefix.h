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
    size_t getSize();

    char * storeSelf( char* buf);
    SPtr<TIPv6Addr> getPrefix() const;
    uint8_t getPrefixLength() const;
    unsigned long getPref() const;
    unsigned long getValid() const;
    void setPref(unsigned long pref);
    void setValid(unsigned long valid);
    void setPrefixLenght(char prefix_length);
    virtual bool isValid() const;

 private:
    SPtr<TIPv6Addr> Prefix_; //
    unsigned long PrefLifetime_;
    unsigned long ValidLifetime_;
    char PrefixLength_; // this I am not sure (because prefix should be only 1 byte )
    bool Valid_;
};

#endif
