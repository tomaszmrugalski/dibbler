#ifndef CONFIRM_H_HEADER_INCLUDED_C1125A67
#define CONFIRM_H_HEADER_INCLUDED_C1125A67
#include "ClntMsg.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgIA.h"

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
class TClntMsgConfirm : public TClntMsg
{
public:

   TClntMsgConfirm(SmartPtr<TClntIfaceMgr> IfaceMgr, 
	SmartPtr<TClntTransMgr> TransMgr, 
	SmartPtr<TClntCfgMgr>	CfgMgr, 
	SmartPtr<TClntAddrMgr> AddrMgr,
	unsigned int iface, 
    TContainer<SmartPtr<TAddrIA> > iaLst);
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

    void addrsAccepted();
    void addrsRejected();


    //##ModelId=3EC8AAC10027
    ~TClntMsgConfirm();

};
#endif /* CONFIRM_H_HEADER_INCLUDED_C1125A67 */
