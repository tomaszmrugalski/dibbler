#ifndef ADVERTISE_H
#define ADVERTISE_H

#include "ClntMsg.h"

class TClntMsgAdvertise : public TClntMsg
{
  public:
    //## Tworzy obiekt Advertise na podstawie bufora
    TClntMsgAdvertise(SmartPtr<TClntIfaceMgr> IfaceMgr,
		      SmartPtr<TClntTransMgr> TransMgr,
		      SmartPtr<TClntCfgMgr>   CfgMgr,
		      SmartPtr<TClntAddrMgr>  AddrMgr,
		      int iface, SmartPtr<TIPv6Addr> addr);
    
    TClntMsgAdvertise(SmartPtr<TClntIfaceMgr> IfaceMgr,
		      SmartPtr<TClntTransMgr> TransMgr,
		      SmartPtr<TClntCfgMgr> CfgMgr,
		      SmartPtr<TClntAddrMgr> AddrMgr,
		      int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    // returns preference value (default value is 0)
    int getPreference();

    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    void answer(SmartPtr<TMsg> Rep);

    void doDuties();

    ~TClntMsgAdvertise();
};

#endif 
