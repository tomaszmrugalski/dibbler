#ifndef CLNTMSG_H_
#define CLNTMSG_H_

#include "Msg.h"
#include "ClntIfaceMgr.h"
#include "ClntTransMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "SmartPtr.h"
#include "Opt.h"

class TClntMsg : public TMsg
{
public:
    TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
	     SmartPtr<TClntTransMgr> TransMgr, 
	     SmartPtr<TClntCfgMgr> CfgMgr,
	     SmartPtr<TClntAddrMgr> AddrMgr,
	     int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
	     SmartPtr<TClntTransMgr> TransMgr, 
	     SmartPtr<TClntCfgMgr> CfgMgr,
	     SmartPtr<TClntAddrMgr> AddrMgr,
	     int iface, SmartPtr<TIPv6Addr> addr, int msgType);
    unsigned long getTimeout();
    void send();
    
    SmartPtr<TClntTransMgr>  getClntTransMgr();
    SmartPtr<TClntAddrMgr>   getClntAddrMgr();
    SmartPtr<TClntCfgMgr>    getClntCfgMgr();
    SmartPtr<TClntIfaceMgr>  getClntIfaceMgr();
    

    long IRT;             // Initial Retransmission Time
    long MRT;             // Maximum Retransmission Time
    long MRC;    // Maximum Retransmission Count
    long MRD;    // Maximum Retransmission Duration
    int RC;    //##iloœæ retransmisji (licznik zlicza do 0)
    int RT;    //## timeout retransmisji (w sek.)
    int FirstTimeStamp;    //##Stempel czasowy pierwszej transmiji wiadomoœci
    int LastTimeStamp;    //##Stempel czasowy ostatnio wyss³anej wiadomoœci

protected:
    SmartPtr<TClntTransMgr>  ClntTransMgr;
    SmartPtr<TClntAddrMgr>   ClntAddrMgr;
    SmartPtr<TClntCfgMgr>    ClntCfgMgr;
    SmartPtr<TClntIfaceMgr>  ClntIfaceMgr;
private:
    void setAttribs(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		    SmartPtr<TClntTransMgr> TransMgr, 
		    SmartPtr<TClntCfgMgr>   CfgMgr,
		    SmartPtr<TClntAddrMgr>  AddrMgr);
};

#endif
