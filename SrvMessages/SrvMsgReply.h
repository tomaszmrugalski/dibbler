#ifndef REPLY_H_HEADER_INCLUDED_C1125C05
#define REPLY_H_HEADER_INCLUDED_C1125C05
#include "SrvMsg.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRelease.h"
#include "SrvMsgSolicit.h"
#include "SrvMsgInfRequest.h"

class TSrvMsgReply : public TSrvMsg
{
  public:
/*  Server does not receive REPLY
    TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		 SmartPtr<TSrvTransMgr>, 
		 SmartPtr<TSrvCfgMgr>, 
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface,  char* addr);
    
    TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		 SmartPtr<TSrvTransMgr>, 
		 SmartPtr<TSrvCfgMgr>, 
		 SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, 
		 char* addr,
		 char* buf,
		 int bufSize); */
    
	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
		     SmartPtr<TSrvTransMgr>, 
		     SmartPtr<TSrvCfgMgr>, 
		     SmartPtr<TSrvAddrMgr> AddrMgr,
		     SmartPtr<TSrvMsgConfirm> question);
	
	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgDecline> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRebind> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRelease> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRenew> question);	

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgRequest> question);

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
		  SmartPtr<TSrvMsgSolicit> question);

	TSrvMsgReply(SmartPtr<TSrvIfaceMgr>, 
	      SmartPtr<TSrvTransMgr>, 
	      SmartPtr<TSrvCfgMgr>, 
	      SmartPtr<TSrvAddrMgr> AddrMgr,
          SmartPtr<TSrvMsgInfRequest> question);

    //##ModelId=3ECA8702037E
    //##Documentation
    //## Odpowiada za reakcj� na otrzymanie wiadomo�ci. W prztpadku zako�czenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3ECA870203A6
    //##Documentation
    //## Funkcja odpowiada za transmisj� i retransmisj� danej wiadomo�ci z
    //## uwzglednienirem sta�ych czasowych.
    void doDuties();

    //## Zwraca timeout (wykorzystywane po stronie klienta) do okre�lenia
    //## czasu, po kt�rym powinna zosta� wykonana jaka� akcja (retransmisja,
    //## koniec transakcji itp.) wykonywana przez metod� do Duties 
    //## po stronie klienta. 
    unsigned long getTimeout();
	
    bool check();
    
    
    ~TSrvMsgReply();
private:
    SmartPtr<TSrvOptOptionRequest> reqOpts;
    SmartPtr<TSrvOptClientIdentifier> duidOpt;

    void appendDefaultOption(SmartPtr<TOpt> ptrOpt);

    void setOptionsReqOptClntDUID(SmartPtr<TMsg> msg);


};


#endif /* REPLY_H_HEADER_INCLUDED_C1125C05 */
