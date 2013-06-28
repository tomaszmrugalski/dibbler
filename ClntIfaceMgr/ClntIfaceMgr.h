/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>

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
#include "ScriptParams.h"

#define ClntIfaceMgr() (TClntIfaceMgr::instance())

class TClntIfaceMgr : public TIfaceMgr
{
public:
    typedef enum {
        PREFIX_MODIFY_ADD,
        PREFIX_MODIFY_UPDATE,
        PREFIX_MODIFY_DEL
    } PrefixModifyMode;
    
private:
    TClntIfaceMgr(const std::string& xmlFile); // this is singleton
    
public:
    static void instanceCreate(const std::string& xmlFile);
    static TClntIfaceMgr& instance();

    ~TClntIfaceMgr();
    friend std::ostream & operator <<(std::ostream & strum, TClntIfaceMgr &x);
    void dump();
    
    bool sendUnicast(int iface, char *msg, int size, SPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SPtr<TClntMsg> select(unsigned int timeout);

#ifdef MOD_REMOTE_AUTOCONF
    bool notifyRemoteScripts(SPtr<TIPv6Addr> receivedAddr, SPtr<TIPv6Addr> serverAddr, int ifindex);
#endif    

    bool fqdnAdd(SPtr<TClntIfaceIface> iface, const std::string& domainname);
    bool fqdnDel(SPtr<TClntIfaceIface> iface, SPtr<TAddrIA> ia, const std::string& domainname);

    bool addPrefix   (int iface, SPtr<TIPv6Addr> prefix, int prefixLen,
                      unsigned int pref, unsigned int valid,
                      TNotifyScriptParams* params /*= NULL*/);
    bool updatePrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen,
                      unsigned int pref, unsigned int valid,
                      TNotifyScriptParams* params /*= NULL*/);
    bool delPrefix   (int iface, SPtr<TIPv6Addr> prefix, int prefixLen,
                      TNotifyScriptParams* params /*= NULL*/);

    // --- option related ---
    void removeAllOpts();

    unsigned int getTimeout();
    
    bool doDuties();

    void redetectIfaces();

  private:
    bool modifyPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen,
                      unsigned int pref, unsigned int valid, PrefixModifyMode mode,
                      TNotifyScriptParams* params /*= NULL*/);

    std::string XmlFile;

    static TClntIfaceMgr* Instance;
};

#endif 
