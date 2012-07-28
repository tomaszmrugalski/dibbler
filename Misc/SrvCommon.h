/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: SrvCommon.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */
#ifndef SRVCOMMON_H
#define SRVCOMMON_H

#include "SmartPtr.h"
#include "SrvIfaceMgr.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"

struct Ctx_s {
    TSrvIfaceMgr * IfaceMgr;
    TSrvAddrMgr *  AddrMgr;
    TSrvCfgMgr  *  CfgMgr;
    TSrvTransMgr * TransMgr;
};
typedef struct Ctx_s Ctx;

#endif
