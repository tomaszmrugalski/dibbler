/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * $Id: ClntOptFQDN.h,v 1.3 2006-10-06 00:42:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2006-03-02 00:59:22  thomson
 * ClntOptFQDN implemented for real.
 *
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
    void setSrvDuid(SPtr<TDUID> duid);
private:
    SPtr<TDUID> SrvDUID;
};



#endif
