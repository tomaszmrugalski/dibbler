/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef RELIFACEMGR_H
#define RELIFACEMGR_H

#include "RelMsg.h"
#include "IfaceMgr.h"
#include "Iface.h"

#define RelIfaceMgr() (TRelIfaceMgr::instance())

class TRelIfaceMgr: public TIfaceMgr {
 public:
   static void instanceCreate(const std::string xmlFile);
   static TRelIfaceMgr& instance();
    ~TRelIfaceMgr();
    friend ostream & operator <<(ostream & strum, TRelIfaceMgr &x);

    SPtr<TRelMsg> decodeMsg(SPtr<TIfaceIface> iface, 
				SPtr<TIPv6Addr> peer, 
				char * buf, int bufsize);

    SPtr<TRelMsg> decodeRelayRepl(SPtr<TIfaceIface> iface, 
				      SPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    SPtr<TRelMsg> decodeRelayForw(SPtr<TIfaceIface> iface, 
				      SPtr<TIPv6Addr> peer, 
				      char * buf, int bufsize);
    SPtr<TRelMsg> decodeGeneric(SPtr<TIfaceIface> iface, 
				    SPtr<TIPv6Addr> peer, 
				    char * buf, int bufsize);
    void dump();
    
    // ---sends messages---
    bool send(int iface, char *data, int dataLen, SPtr<TIPv6Addr> addr, int port);
    
    // ---receives messages---
    SPtr<TRelMsg> select(unsigned long timeout);

  private:
    TRelIfaceMgr(string xmlFile);
    static TRelIfaceMgr * Instance;
};

#endif 
