class TSrvMsgRelease;
#ifndef RELEASE_H_HEADER_INCLUDED_C1123867
#define RELEASE_H_HEADER_INCLUDED_C1123867

#include "SrvMsg.h"

//##ModelId=3EC75CE40043
//##Documentation
//## Transakcja Release tworzona jest w przypadku:
//## 1. zatrzymywanie us�ugi (Tworzona w metodzie stop())
class TSrvMsgRelease : public TSrvMsg
{
  public:
    TSrvMsgRelease(SmartPtr<TSrvIfaceMgr> IfMgr, 
		SmartPtr<TSrvTransMgr> TransMgr,
		SmartPtr<TSrvCfgMgr>   ConfMgr, 
		SmartPtr<TSrvAddrMgr>  AddrMgr, 
		 int iface, 
		 SmartPtr<TIPv6Addr> addr,
		 char* buf,
		 int bufSize);
	
    //##ModelId=3ECA86C001AD
    //##Documentation
    //## Odpowiada za reakcj� na otrzymanie wiadomo�ci. W prztpadku zako�czenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3ECA86C001CB
    //##Documentation
    //## Funkcja odpowiada za transmisj� i retransmisj� danej wiadomo�ci z
    //## uwzglednienirem sta�ych czasowych.
    void doDuties();

    //##ModelId=3EFF0A95001F
    //##Documentation
    //## Zwraca timeout (wykorzystywane po stronie klienta) do okre�lenia
    //## czasu, po kt�rym powinna zosta� wykonana jaka� akcja (retransmisja,
    //## koniec transakcji itp.) wykonywana przez metod� do Duties 
    //## po stronie klienta. 
    unsigned long getTimeout();

    //##ModelId=3ECA86C001FD
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    //##ModelId=3EC8AA180101
    ~TSrvMsgRelease();
};
#endif /* RELEASE_H_HEADER_INCLUDED_C1123867 */
