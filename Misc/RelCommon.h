/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: RelCommon.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

class TRelIfaceMgr;
class TRelCfgMgr;
class TRelTransMgr;
#ifndef RELCOMMON_H
#define RELCOMMON_H

//#include "RelIfaceMgr.h"
//#include "RelCfgMgr.h"
//#include "RelTransMgr.h"

struct Ctx_s {
    TRelIfaceMgr * IfaceMgr;
    TRelCfgMgr   * CfgMgr;
    TRelTransMgr * TransMgr;
};
typedef struct Ctx_s TCtx;

#endif
