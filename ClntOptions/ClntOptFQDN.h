/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptFQDN.h,v 1.2 2006-03-02 00:59:22 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 *
 */

#ifndef CLNTOPTFQDN_H
#define CLNTOPTFQDN_H

#include "OptFQDN.h"
#include "DUID.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"


// void *updateDNS(void *IfaceMgr);

class TClntOptFQDN : public TOptFQDN
{
 public:
    TClntOptFQDN(string fqdn, TMsg* parent);
    TClntOptFQDN(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};



#endif
