/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ClntParsAddrOpt.h,v 1.2 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTPARSADDROPT_H
#define CLNTPARSADDROPT_H

#include "DHCPConst.h"

class TClntParsAddrOpt
{
 public:
    TClntParsAddrOpt();
    long getPref();
    void setPref(long pref);

    long getValid();
    void setValid(long valid);

private:
    long Pref;
    long Valid;
};

#endif
