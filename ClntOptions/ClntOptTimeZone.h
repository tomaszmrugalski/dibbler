/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptTimeZone.h,v 1.3 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTOPTTIMEZONE_H
#define CLNTOPTTIMEZONE_H

#include "OptString.h"
#include "DUID.h"

class TClntOptTimeZone : public TOptString
{
 public:
    TClntOptTimeZone(string domain, TMsg* parent);
    TClntOptTimeZone(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
    bool isValid();
 private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
