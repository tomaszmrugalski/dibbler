/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
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
    TClntMsg(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    TClntMsg(int iface, SPtr<TIPv6Addr> addr, int msgType);
    ~TClntMsg();
    unsigned long getTimeout();
    void send();
    SPtr<TOpt> parseExtraOption(const char *buf, unsigned int code, unsigned int length);

    //answer for a specific message
    virtual void doDuties() = 0;
    virtual bool check() = 0;
    void setIface(int iface); // used to override when we have received msg via loopback interface.

    void copyAAASPI(SPtr<TClntMsg> q);
    void appendTAOptions(bool switchToInProcess); // append all TAs, which are currently in the NOTCONFIGURED state
//    void appendPDOptions(bool switchToInProcess); // append all PDs, which are currently in the NOTCONFIGURED state

    void appendAuthenticationOption();
    void appendElapsedOption();
    void appendRequestedOptions();

    bool validateReplayDetection();

    virtual string getName() = 0;
    virtual void answer(SPtr<TClntMsg> reply);

 protected:
    bool check(bool clntIDmandatory, bool srvIDmandatory);
    bool appendClientID();

    long IRT;           // Initial Retransmission Time
    long MRT;           // Maximum Retransmission Time
    long MRC;           // Maximum Retransmission Count
    long MRD;           // Maximum Retransmission Duration
    int RC;             // Retransmission counter (counts to 0)
    int RT;             // Retransmission timeout (in seconds)
    int FirstTimeStamp; // timestamp of the first transmission
    int LastTimeStamp;  // timestamp of the last transmission

 private:
    void setDefaults();
    void invalidAllowOptInMsg(int msg, int opt);
    void invalidAllowOptInOpt(int msg, int parentOpt, int childOpt);
};

#endif
