class TClntMsgDecline;
#ifndef CLNTMSGDECLINE_H
#define CLNTMSGDECLINE_H
#include "ClntMsg.h"

class TClntMsgDecline : public TClntMsg
{
  public:
    TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr> CfgMgr,
				 SmartPtr<TClntAddrMgr> AddrMgr,
				 int iface, SmartPtr<TIPv6Addr> addr,
				 TContainer< SmartPtr< TAddrIA> > IALst);

	TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
			SmartPtr<TClntTransMgr> TransMgr,
			SmartPtr<TClntCfgMgr> CfgMgr,
			SmartPtr<TClntAddrMgr> AddrMgr,
			int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);

	
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool  check();
    
    void  answer(SmartPtr<TMsg> Rep);
    void  doDuties();
    ~TClntMsgDecline();
};

#endif
