#ifndef SRVSOLICIT_H_HEADER_INCLUDED_C1126259
#define SRVSOLICIT_H_HEADER_INCLUDED_C1126259
#include "SrvMsg.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvAddrMgr.h"

#include "SmartPtr.h"

//## Powinien zostaæ wywo³any, przy:
//## 1. Restarcie systemu
//## 2. Przy zwolnieniu, któregoœ z IA
class TSrvMsgSolicit : public TSrvMsg
{
 public:
    TSrvMsgSolicit(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
		   SmartPtr<TSrvTransMgr>			transMgr,
		   SmartPtr<TSrvCfgMgr>			cfgMgr,
		   SmartPtr<TSrvAddrMgr>			addrMgr,
		   int iface, SmartPtr<TIPv6Addr> addr,
		   char* buf, int bufSzie);

    //## Odbiera wiadomoœci TAdvertise i wstawia jest do listy Answers.
    void answer(SmartPtr<TMsg> msg);

    //## Wysy³a wiadomoœæ
    //## Retransmituje wiadomoœæ
    //## Po zakoñczeniu okresu zbierania SOLICITOW sortuje liste wg preferencji
    //## okreœlonych w konfigu (+ odrzucanie niepo¿¹danych serwerów),
    //##  wywo³uje metode sendRequest TransMgr(lista serwerow) i ustawia pole
    //## IsDone na true.
    void doDuties();

    // sort answers
    void sortAnswers();

    //##ModelId=3EFCBFC9021B
    //##Documentation
    //## Zwraca timeout (wykorzystywane po stronie klienta) do okreœlenia
    //## czasu, po którym powinna zostaæ wykonana jakaœ akcja (retransmisja,
    //## koniec transakcji itp.) wykonywana przez metodê do Duties 
    //## po stronie klienta. 
    unsigned long getTimeout();

    bool check();

	void send();
    
	//##ModelId=3EC75CE40027
    ~TSrvMsgSolicit();

private:
	void setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
	   SmartPtr<TSrvTransMgr> TransMgr, 
	   SmartPtr<TSrvCfgMgr> CfgMgr,
	   SmartPtr<TSrvAddrMgr> AddrMgr,
	    int iface, 
	    char* addr, 
	    int msgType);
};
#endif /* SOLICIT_H_HEADER_INCLUDED_C1126259 */
