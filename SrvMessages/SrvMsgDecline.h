#ifndef DECLINE_H_HEADER_INCLUDED_C112140C
#define DECLINE_H_HEADER_INCLUDED_C112140C
#include "SrvMsg.h"

class TSrvMsgDecline : public TSrvMsg
{
  public:
    //##ModelId=3EC8AA8100F8
    TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr> CfgMgr,
		   SmartPtr<TSrvAddrMgr> AddrMgr,
		   int iface,  SmartPtr<TIPv6Addr> addr);
    
	TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		       SmartPtr<TSrvTransMgr> TransMgr,
		       SmartPtr<TSrvCfgMgr> CfgMgr,
		       SmartPtr<TSrvAddrMgr> AddrMgr,
		       int iface, SmartPtr<TIPv6Addr> addr,
		       char* buf, int bufSize);
	
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool  check();

    void  answer(SmartPtr<TMsg> Rep);

    void  doDuties();

    unsigned long  getTimeout();
	
    ~TSrvMsgDecline();
};



#endif /* DECLINE_H_HEADER_INCLUDED_C112140C */
