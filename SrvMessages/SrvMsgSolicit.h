#ifndef SRVSOLICIT_H_HEADER_INCLUDED_C1126259
#define SRVSOLICIT_H_HEADER_INCLUDED_C1126259
#include "SrvMsg.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvAddrMgr.h"

#include "SmartPtr.h"

//## Powinien zosta� wywo�any, przy:
//## 1. Restarcie systemu
//## 2. Przy zwolnieniu, kt�rego� z IA
class TSrvMsgSolicit : public TSrvMsg
{
 public:
    TSrvMsgSolicit(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
		   SmartPtr<TSrvTransMgr>			transMgr,
		   SmartPtr<TSrvCfgMgr>			cfgMgr,
		   SmartPtr<TSrvAddrMgr>			addrMgr,
		   int iface, SmartPtr<TIPv6Addr> addr,
		   char* buf, int bufSzie);

    //## Odbiera wiadomo�ci TAdvertise i wstawia jest do listy Answers.
    void answer(SmartPtr<TMsg> msg);

    //## Wysy�a wiadomo��
    //## Retransmituje wiadomo��
    //## Po zako�czeniu okresu zbierania SOLICITOW sortuje liste wg preferencji
    //## okre�lonych w konfigu (+ odrzucanie niepo��danych serwer�w),
    //##  wywo�uje metode sendRequest TransMgr(lista serwerow) i ustawia pole
    //## IsDone na true.
    void doDuties();

    // sort answers
    void sortAnswers();

    //##ModelId=3EFCBFC9021B
    //##Documentation
    //## Zwraca timeout (wykorzystywane po stronie klienta) do okre�lenia
    //## czasu, po kt�rym powinna zosta� wykonana jaka� akcja (retransmisja,
    //## koniec transakcji itp.) wykonywana przez metod� do Duties 
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
