#ifndef CONFIRM_H_HEADER_INCLUDED_C1125A67
#define CONFIRM_H_HEADER_INCLUDED_C1125A67
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

//##ModelId=3EC75CE30311
//##Documentation
//## Klient wysy�a komunikat CONFIRM do serwera, aby okre�li�, czy adresy,
//## kt�re zosta�y mu przypisane s� nadal prwid�owe dla ��cza, do kt�rego jest
//## do��czony. 
//## Mo�e si� to sta�:
//## 1. w momencie restartu systemu, po za�adowaniu danych z bazy danych dla
//## ka�dego adresu, dla kt�rego adres wa�no�ci si� nie sko�czy�
//## 2. w momencie zmainy, kt�regokolwiek z ��czy, do kt�rego klient jest
//## do��czony, wysy�any jest do wszystkich serwer�w 
class TSrvMsgConfirm : public TSrvMsg
{
  public:
    TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			       SmartPtr<TSrvTransMgr> TransMgr, 
			       SmartPtr<TSrvCfgMgr> CfgMgr, 
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize);

	//##ModelId=3EC8AABB02F0
    TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			       SmartPtr<TSrvTransMgr> TransMgr, 
			       SmartPtr<TSrvCfgMgr>	CfgMgr, 
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr);

    //##ModelId=3EC92E8601FC
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    //##ModelId=3EEE4B930324
    void answer(SmartPtr<TMsg> Rep);


    //##ModelId=3EEE4B93034C
    void doDuties();

    //##ModelId=3EEE4B93037E
    unsigned long getTimeout();

    //##ModelId=3EC8AAC10027
    ~TSrvMsgConfirm();

};
#endif /* CONFIRM_H_HEADER_INCLUDED_C1125A67 */
