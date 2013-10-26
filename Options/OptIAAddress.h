/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTIAADDRESS_H
#define OPTIAADDRESS_H

#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "IPv6Addr.h"

class TOptIAAddress : public TOpt
{
  public:
    TOptIAAddress(char* &addr, int& n, TMsg* parent);
    TOptIAAddress( SPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid, TMsg* parent);
    size_t getSize();

    char * storeSelf( char* buf);
    SPtr<TIPv6Addr> getAddr() const;
    unsigned long getPref() const;
    unsigned long getValid() const;
    void setPref(unsigned long pref);
    void setValid(unsigned long valid);
    bool isValid() const;

    virtual bool doDuties() { return true; } // does nothing on its own
 private:
    SPtr<TIPv6Addr> Addr_;
    unsigned long ValidLifetime_;
    unsigned long PrefLifetime_;
    bool Valid_;
};

#endif
