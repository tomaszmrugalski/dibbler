/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqTransMgr.h,v 1.4 2007-12-05 10:20:20 thomson Exp $
 */

#ifndef REQIFACEMGR_H
#define REQIFACEMGR_H

#include "IfaceMgr.h"
#include "ReqCfgMgr.h"

class ReqTransMgr {
public:
    ReqTransMgr(TIfaceMgr * ifaceMgr);
    void SetParams(ReqCfgMgr * cfgMgr);
    bool BindSockets();
    bool SendMsg();
    bool WaitForRsp();

private:
    bool PrintRsp(char * buf, int bufLen);
    string ParseLQCLientData(char * buf, int bufLen);
    TIfaceMgr * IfaceMgr;
    SPtr<TIfaceIface> Iface;
    ReqCfgMgr * CfgMgr;
    SPtr<TIfaceSocket> Socket;
};

#endif
