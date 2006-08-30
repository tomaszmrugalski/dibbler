/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.h,v 1.8 2006-08-30 01:10:38 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2006-03-02 00:57:46  thomson
 * FQDN support initial checkin.
 *
 * Revision 1.6  2005/01/08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
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
class TClntMsg;
class TClntIfaceIface;
#ifndef CLNTIFACEMGR_H
#define CLNTIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "ClntTransMgr.h"
#include "ClntIfaceIface.h"
#include "IPv6Addr.h"
#include "ClntMsg.h"

class TClntIfaceMgr : public TIfaceMgr
{
 public:
    TClntIfaceMgr(string xmlFile);
    ~TClntIfaceMgr();
    friend ostream & operator <<(ostream & strum, TClntIfaceMgr &x);
    void dump();
    
    bool sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SmartPtr<TClntMsg> select(unsigned int timeout);

    void setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
		  SmartPtr<TClntTransMgr> clntTransMgr,
		  SmartPtr<TClntCfgMgr>   clntCfgMgr,
		  SmartPtr<TClntAddrMgr>  clntAddrMgr);

    bool fqdnAdd(SmartPtr<TClntIfaceIface> iface, string domainname);
    bool fqdnDel(SmartPtr<TClntIfaceIface> iface, SmartPtr<TAddrIA> ia, string domainname);

    // --- option related ---
    void removeAllOpts();

    unsigned int getTimeout();
    
    bool doDuties();

  private:
    string XmlFile;
    SmartPtr<TClntCfgMgr> ClntCfgMgr;
    SmartPtr<TClntAddrMgr> ClntAddrMgr;
    SmartPtr<TClntTransMgr> ClntTransMgr;
    SmartPtr<TClntIfaceMgr> That;
};

#endif 
