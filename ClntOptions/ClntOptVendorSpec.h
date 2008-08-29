/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptVendorSpec.h,v 1.4 2008-08-29 00:07:29 thomson Exp $
 */

class TClntOptVendorSpec;
#ifndef CLNTOPTVENDORSPEC_H
#define CLNTOPTVENDORSPEC_H

#include "ClntIfaceMgr.h"
#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

class TClntOptVendorSpec : public TOptVendorSpecInfo
{
public:
    TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent);
    TClntOptVendorSpec(char * buf,  int n, TMsg* parent);
    bool doDuties();
    void setIfaceMgr(SPtr<TClntIfaceMgr> ifaceMgr);
 private:
    SPtr<TClntIfaceMgr> IfaceMgr;
};

#endif /* CLNTOPTVENDORSPECINFO_H */
