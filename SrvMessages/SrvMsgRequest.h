#ifndef SRVREQUEST_H_HEADER_INCLUDED_C1127952
#define SRVREQUEST_H_HEADER_INCLUDED_C1127952

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "IPv6Addr.h"

class TSrvMsgRequest : public TSrvMsg
{
  public:
    //## Po wybraniu pierwszego serwera z listy serwer�w, tworzona jest
    //## wiadomo�� REQUEST.
    TSrvMsgRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface);

    TSrvMsgRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
	
    //## O ile odpowied� zawiera IA:
    //##   1. Tworzymy nowa Transakcj� VerifyIA na podstawie otryzmanego IA w
    //## odpowiedzi
    //##   2. informuujew IfaceMgr'a ze ma przypisac otrzymany adres
    //##   3. Ustawia IsDone na true.
    //## Je�li nie zawiera IA w�wczas Request powinien rozpocz�� now�
    //## transakcj� dla kolejnego Advertise, 
    //## Je�li nie ma ju� Advertisow, dla wszystkich IA, kt�re probowano
    //## skonfigurowa�
    void answer(SmartPtr<TMsg> msg);

    //## 1. wysy�a wiadomo��
    //## 2. retransmituje wiadomo��
    //## 3. w przypadku przekroczenia czasu timeout�w, zwalnia pami��
    //## pierwszego serwera na li�cie, i rozpoczyna transmisj� do nast�pnego
    //## 4. je�eli lista serwer�w jest pusta, ustawia IsDone na true i zwraca
    //## FATAL.
    //## 5. Jezeli otrzymana zostanie odpowiedz wywo�uje AddrMgr i dodaje
    //## wszystkie IA dla kt�rych uda�o si� uzyska� odpowiedzi. (!UWAGA!
    //## ADVERTISE musi ��czy� IA, o kt�re si� ubiegamy z IA z ConfMgr)
    //## 
    void doDuties();

    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();
    unsigned long getTimeout();
    ~TSrvMsgRequest();


  private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    TContainer< SmartPtr<TMsg> > BackupSrvLst;
};


#endif /* REQUEST_H_HEADER_INCLUDED_C1127952 */
