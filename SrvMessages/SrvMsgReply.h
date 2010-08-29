/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgReply.h,v 1.12 2009-03-24 01:16:53 thomson Exp $
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

#include "SrvOptClientIdentifier.h"

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

    void doDuties();
    unsigned long getTimeout();
    bool check();
    string getName();
    
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
};


#endif /* SRVMSGREPLY_H */
