/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.h,v 1.5 2004-12-07 00:45:41 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/12/01 20:55:17  thomson
 * Obsolete definitions removed.
 *
 * Revision 1.3  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.2  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 *
 */

class TClntIfaceMgr;
#ifndef CLNTIFACEMGR_H
#define CLNTIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "ClntTransMgr.h"
#include "ClntIfaceIface.h"
#include "IPv6Addr.h"
#include "Msg.h"

class TClntIfaceMgr : public TIfaceMgr
{
 public:
    TClntIfaceMgr(string xmlFile);
    ~TClntIfaceMgr();
    friend ostream & operator <<(ostream & strum, TClntIfaceMgr &x);
    void dump();
    
    bool sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SmartPtr<TMsg> select(unsigned int timeout);

    void setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
		  SmartPtr<TClntTransMgr> clntTransMgr,
		  SmartPtr<TClntCfgMgr>   clntCfgMgr,
		  SmartPtr<TClntAddrMgr>  clntAddrMgr);

    // --- option related ---
    void removeAllOpts();

    unsigned int getTimeout();

  private:
    string XmlFile;
    SmartPtr<TClntCfgMgr> ClntCfgMgr;
    SmartPtr<TClntAddrMgr> ClntAddrMgr;
    SmartPtr<TClntTransMgr> ClntTransMgr;
    SmartPtr<TClntIfaceMgr> That;
};

#endif 
