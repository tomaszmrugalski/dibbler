/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 */

class TSrvMsgReply;
#ifndef SRVMSGREPLY_H
#define SRVMSGREPLY_H
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
#include "SrvOptIAAddress.h"

#include "OptDUID.h"

class TSrvMsgReply : public TSrvMsg
{
  public:
    TSrvMsgReply(SPtr<TSrvMsgConfirm> question);
    TSrvMsgReply(SPtr<TSrvMsgDecline> question);	
    TSrvMsgReply(SPtr<TSrvMsgRebind> question);	
    TSrvMsgReply(SPtr<TSrvMsgRelease> question);	
    TSrvMsgReply(SPtr<TSrvMsgRenew> question);	
    TSrvMsgReply(SPtr<TSrvMsgRequest> question);
    TSrvMsgReply(SPtr<TSrvMsgSolicit> question);
    TSrvMsgReply(SPtr<TSrvMsgInfRequest> question);
    TSrvMsgReply(SPtr<TSrvMsg> msg, TOptList& options);

    void doDuties();
    unsigned long getTimeout();
    bool check();
    std::string getName() const;
    
    ~TSrvMsgReply();
private:

    bool handleSolicitOptions(TOptList& options);
    bool handleRequestOptions(TOptList& options);
    bool handleRenewOptions(TOptList& options);
    bool handleRebindOptions(TOptList& options);
    bool handleReleaseOptions(TOptList& options);
    bool handleDeclineOptions(TOptList& options);
    bool handleConfirmOptions(TOptList& options);
    bool handleInfRequestOptions(TOptList& options);
    EAddrStatus confirmAddress(TIAType type, SPtr<TIPv6Addr> addr);
};


#endif /* SRVMSGREPLY_H */
