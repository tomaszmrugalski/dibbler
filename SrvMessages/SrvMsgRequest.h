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
    //## Po wybraniu pierwszego serwera z listy serwerów, tworzona jest
    //## wiadomoœæ REQUEST.
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
	
    //## O ile odpowiedŸ zawiera IA:
    //##   1. Tworzymy nowa Transakcjê VerifyIA na podstawie otryzmanego IA w
    //## odpowiedzi
    //##   2. informuujew IfaceMgr'a ze ma przypisac otrzymany adres
    //##   3. Ustawia IsDone na true.
    //## Jeœli nie zawiera IA wówczas Request powinien rozpocz¹æ now¹
    //## transakcjê dla kolejnego Advertise, 
    //## Jeœli nie ma ju¿ Advertisow, dla wszystkich IA, które probowano
    //## skonfigurowaæ
    void answer(SmartPtr<TMsg> msg);

    //## 1. wysy³a wiadomoœæ
    //## 2. retransmituje wiadomoœæ
    //## 3. w przypadku przekroczenia czasu timeoutów, zwalnia pamiêæ
    //## pierwszego serwera na liœcie, i rozpoczyna transmisjê do nastêpnego
    //## 4. je¿eli lista serwerów jest pusta, ustawia IsDone na true i zwraca
    //## FATAL.
    //## 5. Jezeli otrzymana zostanie odpowiedz wywo³uje AddrMgr i dodaje
    //## wszystkie IA dla których uda³o siê uzyskaæ odpowiedzi. (!UWAGA!
    //## ADVERTISE musi ³¹czyæ IA, o które siê ubiegamy z IA z ConfMgr)
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
