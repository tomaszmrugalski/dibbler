class TClntIfaceMgr;
#ifndef CLNTIFACEMGR_H
#define CLNTIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "ClntTransMgr.h"
#include "IPv6Addr.h"
#include "Msg.h"

class TClntIfaceMgr : public TIfaceMgr
{
 public:
    TClntIfaceMgr();
    
    bool sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SmartPtr<TMsg> select(unsigned int timeout);

    void setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
		  SmartPtr<TClntTransMgr> clntTransMgr,
		  SmartPtr<TClntCfgMgr>   clntCfgMgr,
		  SmartPtr<TClntAddrMgr>  clntAddrMgr);

  private:
    SmartPtr<TClntCfgMgr> ClntCfgMgr;
    SmartPtr<TClntAddrMgr> ClntAddrMgr;
    SmartPtr<TClntTransMgr> ClntTransMgr;
    SmartPtr<TClntIfaceMgr> That;
};

#endif 
