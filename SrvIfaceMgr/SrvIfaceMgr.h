/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Petr Pisar <petr.pisar(at)atlas(dot)cz>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVIFACEMGR_H
#define SRVIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "Iface.h"
#include "SrvMsg.h"

#define SrvIfaceMgr() (TSrvIfaceMgr::instance())

class TSrvIfaceMgr :public TIfaceMgr {
 public:
   static void instanceCreate(const std::string& xmlDumpFile);
   static TSrvIfaceMgr &instance();

   ~TSrvIfaceMgr();
   friend std::ostream & operator <<(std::ostream & strum, TSrvIfaceMgr &x);

   SPtr<TSrvMsg> decodeMsg(int ifindex,
                           SPtr<TIPv6Addr> peer,
                           char * buf, int bufsize);

   SPtr<TSrvMsg> decodeRelayForw(SPtr<TIfaceIface> physicalIface,
                                 SPtr<TIPv6Addr> peer,
                                 char * buf, int bufsize);

   //bool setupRelay(std::string name, int ifindex, int underIfindex,
   //                SPtr<TSrvOptInterfaceID> interfaceID);
   void dump();

   // --- transmission/reception methods ---
   virtual bool send(int iface, char *msg, int size, SPtr<TIPv6Addr> addr, int port);
   virtual int receive(unsigned long timeout, char* buf, int& bufsize,
                       SPtr<TIPv6Addr> peer, SPtr<TIPv6Addr> myaddr);

   // ---receives messages---
   SPtr<TSrvMsg> select(unsigned long timeout);

   bool addFQDN(int iface, SPtr<TIPv6Addr> dnsAddr, SPtr<TIPv6Addr> addr,
                const std::string& domainname);

   bool delFQDN(int iface, SPtr<TIPv6Addr> dnsAddr, SPtr<TIPv6Addr> addr,
                const std::string& domainname);

   virtual void notifyScripts(const std::string& scriptName,
                              SPtr<TMsg> question, SPtr<TMsg> answer);

   void redetectIfaces();

protected:
   TSrvIfaceMgr(const std::string& xmlFile);
   static TSrvIfaceMgr * Instance;

   std::string XmlFile;
};

#endif
