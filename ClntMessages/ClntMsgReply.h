#ifndef REPLY_H_HEADER_INCLUDED_C1125C05
#define REPLY_H_HEADER_INCLUDED_C1125C05
#include "ClntMsg.h"

//##ModelId=3EC75CE4006C
class TClntMsgReply : public TClntMsg
{
  public:
    TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		  SmartPtr<TClntTransMgr> TransMgr, 
		  SmartPtr<TClntCfgMgr>   CfgMgr, 
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  int iface, SmartPtr<TIPv6Addr> addr);

    TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		  SmartPtr<TClntTransMgr> TransMgr, 
		  SmartPtr<TClntCfgMgr>   CfgMgr, 
		  SmartPtr<TClntAddrMgr> AddrMgr,
		  int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
	
    //## Odpowiada za reakcjê na otrzymanie wiadomoœci. W prztpadku zakoñczenia
    //## transakcji ustawia pole IsDone na true
    void answer(SmartPtr<TMsg> Rep);

    //##ModelId=3ECA870203A6
    //##Documentation
    //## Funkcja odpowiada za transmisjê i retransmisjê danej wiadomoœci z
    //## uwzglednienirem sta³ych czasowych.
    void doDuties();

	
	bool check();
    
    //##ModelId=3EC8AA3A01C8
    ~TClntMsgReply();

};


#endif /* REPLY_H_HEADER_INCLUDED_C1125C05 */
