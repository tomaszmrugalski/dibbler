#ifndef RENEW_H_HEADER_INCLUDED_C1120438
#define RENEW_H_HEADER_INCLUDED_C1120438
#include "SrvMsg.h"

//##ModelId=3EC75CE4001C
class TSrvMsgRenew : public TSrvMsg
{
  public:
    TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		 SmartPtr<TSrvTransMgr> TransMgr,
		 SmartPtr<TSrvCfgMgr> CfgMgr,
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, SmartPtr<TIPv6Addr> addr);

    TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr> CfgMgr,SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, SmartPtr<TIPv6Addr> addr,
		 char* buf, int bufSize);
	
    //## Odpowiada za reakcjê na otrzymanie wiadomoœci. W prztpadku zakoñczenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //## Funkcja odpowiada za transmisjê i retransmisjê danej wiadomoœci z
    //## uwzglednienirem sta³ych czasowych.
    void doDuties();

    unsigned long getTimeout();

    bool check();
    ~TSrvMsgRenew();
};

#endif /* RENEW_H_HEADER_INCLUDED_C1120438 */
