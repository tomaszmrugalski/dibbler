#ifndef ADVERTISE_H_HEADER_INCLUDED_C1121E50
#define ADVERTISE_H_HEADER_INCLUDED_C1121E50

#include "SrvMsg.h"
#include "SrvMsgSolicit.h"
//##ModelId=3EC75CE40057
class TSrvMsgAdvertise : public TSrvMsg
{
  public:
    //## Tworzy obiekt Advertise na podstawie bufora
    TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		     SmartPtr<TSrvTransMgr> TransMgr,
		     SmartPtr<TSrvCfgMgr> CfgMgr,
		     SmartPtr<TSrvAddrMgr> AddrMgr,
		     int iface, SmartPtr<TIPv6Addr> addr);

	TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			 SmartPtr<TSrvTransMgr> TransMgr,
			 SmartPtr<TSrvCfgMgr> CfgMgr,
			 SmartPtr<TSrvAddrMgr> AddrMgr,
			 SmartPtr<TSrvMsgSolicit> question);

	TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
		SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		unsigned int iface, SmartPtr<TIPv6Addr> addr,
		unsigned char* buf, unsigned int bufSize);

    //##ModelId=3EC959B9032B
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    //##ModelId=3EEE4B4A0166
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3EEE4B4A018E
    void doDuties();

    //##ModelId=3EEE4B4A01CA
    unsigned long getTimeout();

	//##ModelId=3EC8AA72002E
    ~TSrvMsgAdvertise();
};

#endif /* ADVERTISE_H_HEADER_INCLUDED_C1121E50 */
