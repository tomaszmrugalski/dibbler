#ifndef CONFIRM_H_HEADER_INCLUDED_C1125A67
#define CONFIRM_H_HEADER_INCLUDED_C1125A67
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

//##ModelId=3EC75CE30311
//##Documentation
//## Klient wysy³a komunikat CONFIRM do serwera, aby okreœliæ, czy adresy,
//## które zosta³y mu przypisane s¹ nadal prwid³owe dla ³¹cza, do którego jest
//## do³¹czony. 
//## Mo¿e siê to staæ:
//## 1. w momencie restartu systemu, po za³adowaniu danych z bazy danych dla
//## ka¿dego adresu, dla którego adres wa¿noœci siê nie skoñczy³
//## 2. w momencie zmainy, któregokolwiek z ³¹czy, do którego klient jest
//## do³¹czony, wysy³any jest do wszystkich serwerów 
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
