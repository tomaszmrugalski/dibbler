/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#ifndef CLNTMSG_H
#define CLNTMSG_H

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

    //answer for a specific message
    virtual void answer(SmartPtr<TClntMsg> Rep) = 0;
    virtual void doDuties() = 0;
    virtual bool check() = 0;
    void setIface(int iface); // used to override when we have received msg via loopback interface.

    void appendTAOptions(bool switchToInProcess); // append all TAs, which are currently in the NOTCONFIGURED state
    void appendRequestedOptions();
    
    SmartPtr<TClntTransMgr>  getClntTransMgr();
    SmartPtr<TClntAddrMgr>   getClntAddrMgr();
    SmartPtr<TClntCfgMgr>    getClntCfgMgr();
    SmartPtr<TClntIfaceMgr>  getClntIfaceMgr();

    virtual string getName() = 0;

 protected:
    long IRT;           // Initial Retransmission Time
    long MRT;           // Maximum Retransmission Time
    long MRC;           // Maximum Retransmission Count
    long MRD;           // Maximum Retransmission Duration
    int RC;             // Retransmission counter (counts to 0)
    int RT;             // Retransmission timeout (in seconds)
    int FirstTimeStamp; // timestamp of the first transmission
    int LastTimeStamp;  // timestamp of the last transmission
    SmartPtr<TClntTransMgr>  ClntTransMgr;
    SmartPtr<TClntAddrMgr>   ClntAddrMgr;
    SmartPtr<TClntCfgMgr>    ClntCfgMgr;
    SmartPtr<TClntIfaceMgr>  ClntIfaceMgr;
 private:
    void setAttribs(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		    SmartPtr<TClntTransMgr> TransMgr, 
		    SmartPtr<TClntCfgMgr>   CfgMgr,
		    SmartPtr<TClntAddrMgr>  AddrMgr);
    void invalidAllowOptInMsg(int msg, int opt);
    void invalidAllowOptInOpt(int msg, int parentOpt, int childOpt);
};

#endif
