#ifndef REBIND_H_HEADER_INCLUDED_C1126D16
#define REBIND_H_HEADER_INCLUDED_C1126D16
#include "SrvMsg.h"

class TSrvMsgRebind : public TSrvMsg
{
  public:
    TSrvMsgRebind(SmartPtr<TSrvIfaceMgr>, 
		  SmartPtr<TSrvTransMgr>, 
		  SmartPtr<TSrvCfgMgr>, 
		  SmartPtr<TSrvAddrMgr> AddrMgr,
		  int iface,  SmartPtr<TIPv6Addr> addr);

    TSrvMsgRebind(SmartPtr<TSrvIfaceMgr>, 
		  SmartPtr<TSrvTransMgr>, 
		  SmartPtr<TSrvCfgMgr>, 
		  SmartPtr<TSrvAddrMgr> AddrMgr,
          int iface, SmartPtr<TIPv6Addr> addr,
		  char* buf, int bufSize);

    //## Odpowiada za reakcj� na otrzymanie wiadomo�ci. W prztpadku zako�czenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //## Funkcja odpowiada za transmisj� i retransmisj� danej wiadomo�ci z
    //## uwzglednienirem sta�ych czasowych.
    void doDuties();

    //## Zwraca timeout (wykorzystywane po stronie klienta) do okre�lenia czasu
    //## po kt�rym czasie powinna zosta� wykonana akcja po stronie klienta. 
    unsigned long getTimeout();

    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();
    ~TSrvMsgRebind();
};

#endif /* REBIND_H_HEADER_INCLUDED_C1126D16 */
