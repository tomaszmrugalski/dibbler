/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqTransMgr.h,v 1.5 2008-01-01 20:21:14 thomson Exp $
 */

#ifndef REQIFACEMGR_H
#define REQIFACEMGR_H

#include "IfaceMgr.h"
#include "ReqCfgMgr.h"
#include "SmartPtr.h"
#include "OptVendorSpecInfo.h"
#include "ReqOpt.h"


class ReqTransMgr {
public:
    ReqTransMgr(TIfaceMgr * ifaceMgr);
    void SetParams(ReqCfgMgr * cfgMgr);
    bool BindSockets();
    bool CreateNewTCPSocket(char *dstAddr);
    bool SendMsg();
    bool SendTcpMsg();
    bool WaitForRsp();
    bool RetryConnection(); //if server doesn't answer retry attemption
    void TerminateTcpConn();
    bool ValidateMsg(char * msgBuf);
	int GetQueryType();

private:
    void PrintRsp(char * buf, int bufLen);
    bool ParseOpts(int msgType, int recurseLevel, char * buf, int bufLen);
    std::string BinToString(char * buf, int bufLen);
    TIfaceMgr * IfaceMgr;
    SPtr<TIfaceIface> Iface;
    ReqCfgMgr * CfgMgr;
    SPtr<TIfaceSocket> Socket;
};

#endif
