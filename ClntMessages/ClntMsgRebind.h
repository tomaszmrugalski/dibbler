#ifndef REBIND_H
#define REBIND_H

#include "ClntMsg.h"
#include "ClntOptIA_NA.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerUnicast.h"

class TClntMsgRebind : public TClntMsg
{
  public:

/*    TClntMsgRebind(SmartPtr<TClntIfaceMgr>, 
		   SmartPtr<TClntTransMgr>, 
		   SmartPtr<TClntCfgMgr>, 
		   SmartPtr<TClntAddrMgr> AddrMgr,
		   int iface,  char* addr);
    
    TClntMsgRebind(SmartPtr<TClntIfaceMgr>, 
		   SmartPtr<TClntTransMgr>, 
		   SmartPtr<TClntCfgMgr>, 
		   SmartPtr<TClntAddrMgr> AddrMgr,
		   int iface, char* addr, char* buf, int bufSize); */

/*    TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		   SmartPtr<TClntTransMgr> TransMgr, 
		   SmartPtr<TClntCfgMgr> CfgMgr, 
		   SmartPtr<TClntAddrMgr> AddrMgr,
		   SmartPtr<TAddrIA> ptrIA);
           */

    TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
			       SmartPtr<TClntTransMgr> TransMgr, 
			       SmartPtr<TClntCfgMgr> CfgMgr, 
			       SmartPtr<TClntAddrMgr> AddrMgr,
			       TContainer<SmartPtr<TOpt> > ptrOpts, int iface);

    //## Odpowiada za reakcjê na otrzymanie wiadomoœci. W prztpadku zakoñczenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3EC9301B00AA
    //##Documentation
    //## Funkcja odpowiada za transmisjê i retransmisjê danej wiadomoœci z
    //## uwzglednienirem sta³ych czasowych.
    void doDuties();

    //##ModelId=3EC9304B0302
    //##Documentation
    //## Funkcja sprawdza, czy komunikat DHCP jest poprawny tj. czy zawiera
    //## odpowiednie opcje jak podano w RFC.
    bool check();

    ~TClntMsgRebind();
 private:
    void updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA,
		  SmartPtr<TClntOptServerIdentifier> optSrvDUID, 
		  SmartPtr<TClntOptServerUnicast> optUnicast);
    void releaseIA(int IAID);


};

#endif /* REBIND_H_HEADER_INCLUDED_C1126D16 */
