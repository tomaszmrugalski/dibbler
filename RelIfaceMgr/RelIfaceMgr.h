/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelIfaceMgr.h,v 1.3 2005-04-25 00:19:20 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/01/11 23:35:22  thomson
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TSrvIfaceMgr;
#ifndef RELIFACEMGR_H
#define RELIFACEMGR_H

#include "RelCommon.h"
#include "RelMsg.h"
#include "IfaceMgr.h"
#include "Iface.h"

class TRelIfaceMgr: public TIfaceMgr {
 public:
    TRelIfaceMgr(string xmlFile);
    ~TRelIfaceMgr();
    friend ostream & operator <<(ostream & strum, TRelIfaceMgr &x);

    SmartPtr<TRelMsg> decodeMsg(SmartPtr<TIfaceIface> iface, 
				SmartPtr<TIPv6Addr> peer, 
				char * buf, int bufsize);

    SmartPtr<TRelMsg> decodeRelayRepl(SmartPtr<TIfaceIface> iface, 
				      SmartPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    SmartPtr<TRelMsg> decodeRelayForw(SmartPtr<TIfaceIface> iface, 
				      SmartPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    SmartPtr<TRelMsg> decodeGeneric(SmartPtr<TIfaceIface> iface, 
				    SmartPtr<TIPv6Addr> peer, 
				    char * buf, int bufsize);
    void dump();
    
    // ---sends messages---
    bool send(int iface, char *data, int dataLen, SmartPtr<TIPv6Addr> addr, int port);
    
    // ---receives messages---
    SmartPtr<TRelMsg> select(unsigned long timeout);

  private:
    TCtx * Ctx;
};

#endif 
