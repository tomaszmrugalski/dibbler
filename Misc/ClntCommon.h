/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ClntCommon.h,v 1.2 2006-03-05 21:39:19 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.1  2006/02/05 23:44:22  thomson
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

class TClntIfaceMgr;
class TClntCfgMgr;
class TClntTransMgr;
class TClntAddrMgr;
#ifndef CLNTCOMMON_H
#define CLNTCOMMON_H

struct Ctx_s {
    TClntIfaceMgr * IfaceMgr;
    TClntCfgMgr   * CfgMgr;
    TClntTransMgr * TransMgr;
};
typedef struct Ctx_s TCtx;

#endif /* CLNTCOMMON_H */
