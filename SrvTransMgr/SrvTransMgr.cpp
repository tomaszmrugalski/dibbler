/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Petr Pisar <petr.pisar(at)atlas(dot)cz>
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <sstream>
#include <limits.h>
#include "SrvTransMgr.h"
#include "SmartPtr.h"
#include "SrvCfgIface.h"
#include "Iface.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgReply.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRelease.h"
#include "SrvMsgLeaseQuery.h"
#include "SrvMsgLeaseQueryReply.h"
#include "SrvOptIA_NA.h"
#include "OptStatusCode.h"
#include "NodeClientSpecific.h"

#ifndef MOD_SRV_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

using namespace std;

TSrvTransMgr * TSrvTransMgr::Instance = 0;

TSrvTransMgr::TSrvTransMgr(const std::string xmlFile)
: XmlFile(xmlFile), IsDone(false)
{
    // TransMgr is certainly not done yet. We're just getting started

    // for each interface in CfgMgr, create socket (in IfaceMgr)
    SPtr<TSrvCfgIface> confIface;
    SrvCfgMgr().firstIface();
    while (confIface = SrvCfgMgr().getIface()) {
            if (!this->openSocket(confIface)) {
                  this->IsDone = true;
                  break;
              }
    }

    SrvAddrMgr().setCacheSize(SrvCfgMgr().getCacheSize());
}

/*
 * opens proper (multicast or unicast) socket on interface
 */
bool TSrvTransMgr::openSocket(SPtr<TSrvCfgIface> confIface) {

    SPtr<TSrvIfaceIface> iface = (Ptr*)SrvIfaceMgr().getIfaceByID(confIface->getID());
    SPtr<TIPv6Addr> unicast = confIface->getUnicast();

    if (confIface->isRelay()) {
        while (iface->getUnderlaying()) {
            iface = iface->getUnderlaying();
        }
        if (!iface->countSocket())
            Log(Notice) << "Relay init: Creating socket on the underlaying interface: " << iface->getName()
                        << "/" << iface->getID() << "." << LogEnd;
    }

    if (unicast) {
        /* unicast */
        Log(Notice) << "Creating unicast (" << *unicast << ") socket on " << confIface->getName()
                    << "/" << confIface->getID() << " interface." << LogEnd;
        if (!iface->addSocket( unicast, DHCPSERVER_PORT, true, false)) {
            Log(Crit) << "Proper socket creation failed." << LogEnd;
            return false;
        }
    }

    char srvAddr[16];
    if (!confIface->isRelay()) {
        inet_pton6(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,srvAddr);
    } else {
        inet_pton6(ALL_DHCP_SERVERS,srvAddr);
    }

    SPtr<TIPv6Addr> ipAddr(new TIPv6Addr(srvAddr));
    Log(Notice) << "Creating multicast (" << ipAddr->getPlain() << ") socket on " << confIface->getName()
                << "/" << confIface->getID() << " (" << iface->getName() << "/"
                << iface->getID() << ") interface." << LogEnd;
    if (iface->getSocketByAddr(ipAddr)) {
        Log(Notice) << "Address " << ipAddr->getPlain() << " is already bound on the "
                    << iface->getName() << "." << LogEnd;
        return true;
    }

    if (!iface->addSocket(ipAddr, DHCPSERVER_PORT, true, false)) {
        Log(Crit) << "Proper socket creation failed." << LogEnd;
        return false;
    }

#if 1
    if (!iface->countLLAddress()) {
        Log(Crit) << "There is no link-local address on " << iface->getFullName() << " defined." << LogEnd;
        return false;
    }
    memcpy(srvAddr, iface->firstLLAddress(), 16);
    SPtr<TIPv6Addr> llAddr = new TIPv6Addr(iface->firstLLAddress());
    if (iface->getSocketByAddr(llAddr)) {
        Log(Notice) << "Address " << llAddr->getPlain() << " is already bound on the "
            << iface->getName() << "." << LogEnd;
        return true;
    } else {
        Log(Notice) << "Creating link-local (" << llAddr->getPlain() << ") socket on " << iface->getFullName()
                    << " interface." << LogEnd;
        if (!iface->addSocket(llAddr, DHCPSERVER_PORT, true, false)) {
            Log(Crit) << "Failed to create link-local socket on " << iface->getFullName() << " interface." << LogEnd;
            return false;
        }
    }
#endif

    return true;
}

/**
 * Computes number of seconds when next event is expected or a job is
 * supposted to be proceeded.
 *
 * @returns Number of seconds when something should happend
 */
long TSrvTransMgr::getTimeout()
{
    unsigned long min = 0xffffffff;
    unsigned long ifaceRecheckPeriod = 10;
    unsigned long addrTimeout = 0xffffffff;
    SPtr<TSrvMsg> ptrMsg;
    MsgLst.first();
    while (ptrMsg = MsgLst.get() )
    {
        if (ptrMsg->getTimeout() < min)
            min = ptrMsg->getTimeout();
    }
    if (SrvCfgMgr().inactiveIfacesCnt() && ifaceRecheckPeriod<min)
        min = ifaceRecheckPeriod;
    addrTimeout = SrvAddrMgr().getValidTimeout();
    return min<addrTimeout?min:addrTimeout;
}

void TSrvTransMgr::relayMsg(SPtr<TSrvMsg> msg)
{
    if (!msg->check()) {
        // proper warnings will be printed in the check() method, if necessary.
        // Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }

    // LEASE ASSIGN STEP 1: Evaluate defined expressions (client classification)
    // Ask NodeClietSpecific to analyse the message
    NodeClientSpecific::analyseMessage(msg);

    // LEASE ASSIGN STEP 2: Is this client supported?
    // is this client supported? (white-list, black-list)
    if (!SrvCfgMgr().isClntSupported(msg)) {
        return;
    }

    /// @todo remove (or at least disable by default) answer buffering mechanism
    SPtr<TSrvMsg> answ;
    Log(Debug) << MsgLst.count() << " answers buffered.";

    MsgLst.first();
    while(answ=(Ptr*)MsgLst.get())
    {
        if (answ->getTransID()==msg->getTransID() && msg->getType() != RELEASE_MSG ) {
            Log(Cont) << " Old reply with transID=" << hex << msg->getTransID()
                      << dec << " found. Sending old reply." << LogEnd;
            answ->send();
            return;
        }
    }
    Log(Cont) << " Old reply for transID=" << hex << msg->getTransID()
              << " not found. Generating new answer." << dec << LogEnd;

    SPtr<TMsg> q, a; // question and answer

    q = (Ptr*) msg;

    switch(msg->getType()) {
    case SOLICIT_MSG: {
        SPtr<TSrvCfgIface> ptrCfgIface = SrvCfgMgr().getIfaceByID(msg->getIface());
        if (msg->getOption(OPTION_RAPID_COMMIT)) {
            if (!ptrCfgIface->getRapidCommit()) {
                Log(Info) << "SOLICIT with RAPID-COMMIT received, but RAPID-COMMIT is disabled on "
                          << ptrCfgIface->getName() << " interface." << LogEnd;
                a = new TSrvMsgAdvertise((Ptr*)msg);
            } else {
                SPtr<TSrvMsgSolicit> nmsg = (Ptr*)msg;
                a = new TSrvMsgReply(nmsg);
            }
        } else {
            a = new TSrvMsgAdvertise( (Ptr*) msg);
        }
        break;
    }
    case REQUEST_MSG: {
        SPtr<TSrvMsgRequest> nmsg = (Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case CONFIRM_MSG: {
        SPtr<TSrvMsgConfirm> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case RENEW_MSG: {
        SPtr<TSrvMsgRenew> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case REBIND_MSG: {
        SPtr<TSrvMsgRebind> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case DECLINE_MSG: {
        SPtr<TSrvMsgDecline> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case RELEASE_MSG: {
        SPtr<TSrvMsgRelease> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case INFORMATION_REQUEST_MSG : {
        SPtr<TSrvMsgInfRequest> nmsg=(Ptr*)msg;
        a = new TSrvMsgReply(nmsg);
        break;
    }
    case LEASEQUERY_MSG: {
        int iface = msg->getIface();
        if (!SrvCfgMgr().getIfaceByID(iface) || !SrvCfgMgr().getIfaceByID(iface)->leaseQuerySupport()) {
            Log(Error) << "LQ: LeaseQuery message received on " << iface
                       << " interface, but it is not supported there." << LogEnd;
            return;
        }
        Log(Debug) << "LQ: LeaseQuery received, preparing RQ_REPLY" << LogEnd;
        SPtr<TSrvMsgLeaseQuery> lq = (Ptr*)msg;
        a = new TSrvMsgLeaseQueryReply(lq);
        break;
    }
    case RECONFIGURE_MSG:
    case ADVERTISE_MSG:
    case REPLY_MSG:
    {
        Log(Warning) << "Invalid message type received: " << msg->getType()
                     << LogEnd;
        break;
    }
    case RELAY_FORW_MSG: // They should be decapsulated earlier
    case RELAY_REPL_MSG:
    default:
    {
        Log(Warning)<< "Message type " << msg->getType()
                    << " not supported." << LogEnd;
        break;
    }
    }

    if (a) {
        /// @todo: messages should not call send() in their ctors, send should be done here
        MsgLst.append((Ptr*)a);
        SrvIfaceMgr().notifyScripts(SrvCfgMgr().getScriptName(), q, a);
    }

    // save DB state regardless of action taken
    SrvAddrMgr().dump();
    SrvCfgMgr().dump();
}

void TSrvTransMgr::doDuties()
{
    int deletedCnt = 0;
    // are there any outdated addresses?
    std::vector<TSrvAddrMgr::TExpiredInfo> addrLst;
    std::vector<TSrvAddrMgr::TExpiredInfo> tempAddrLst;
    std::vector<TSrvAddrMgr::TExpiredInfo> prefixLst;

    if (!SrvAddrMgr().getValidTimeout()) {
        SrvAddrMgr().doDuties(addrLst, tempAddrLst, prefixLst);
        removeExpired(addrLst, tempAddrLst, prefixLst);
    }

    // for each message on list, let it do its duties, if timeout is reached
    SPtr<TSrvMsg> msg;
    MsgLst.first();
    while (msg=MsgLst.get())
        if ( (!msg->getTimeout()) && (!msg->isDone()) )
            msg->doDuties();

    // now delete messages marked as done
    MsgLst.first();
    while (msg = MsgLst.get() )
    {
        if (msg->isDone())
        {
            MsgLst.del();
            deletedCnt++;
        }
    }
    if (deletedCnt) {
        Log(Debug) << deletedCnt << " message(s) were removed from cache." << LogEnd;
    }

    // Open socket on interface which becames ready during server run
    if (SrvCfgMgr().inactiveMode())
    {
        SPtr<TSrvCfgIface> x;
        x = SrvCfgMgr().checkInactiveIfaces();
        if (x)
            openSocket(x);
    }

}


/// @brief Generates parameters for notify script based on expired lease information
///
/// @param params Notify parameters (all available info will be set here)
/// @param exp expired lease details
/// @param type type of lease (IA, TA or PD)
void TSrvTransMgr::notifyExpireInfo(TNotifyScriptParams& params, const TSrvAddrMgr::TExpiredInfo& exp,
                                    TAddrIA::TIAType type) {
    stringstream tmp;
    tmp << exp.ia->getIface();
    params.addParam("IFINDEX", tmp.str());

    SPtr<TIfaceIface> iface = SrvIfaceMgr().getIfaceByID(exp.ia->getIface());
    if (iface)
        params.addParam("IFACE", iface->getName());

    if (exp.ia->getSrvAddr())
        params.addParam("REMOTE_ADDR", exp.ia->getSrvAddr()->getPlain());
    switch (type) {
    case TAddrIA::TYPE_IA:
    case TAddrIA::TYPE_TA:
    {
        params.addAddr(exp.addr, 0, 0, "");
        break;
    }
    case TAddrIA::TYPE_PD:
    {
        params.addPrefix(exp.addr, exp.prefixLen, 0, 0);
    }
    break;
    }

    tmp.str("");
    tmp << exp.ia->getIAID();
    params.addParam("IAID", tmp.str());
    params.addParam("SRV_OPTION1", exp.client->getDUID()->getPlain()); // set client-id
}

/// @brief Removes expired leases and calls notify script
///
/// @param addrLst list of expired address leases
/// @param tempAddrLst list of expired temporary addresses leases
/// @param prefixLst list of expired prefix delegation leases
void TSrvTransMgr::removeExpired(std::vector<TSrvAddrMgr::TExpiredInfo>& addrLst,
                                 std::vector<TSrvAddrMgr::TExpiredInfo>& tempAddrLst,
                                 std::vector<TSrvAddrMgr::TExpiredInfo>& prefixLst) {
    for (vector<TSrvAddrMgr::TExpiredInfo>::iterator addr = addrLst.begin();
         addr != addrLst.end(); ++addr) {
        // delete this address
        Log(Notice) << "Address " << *(addr->addr) << " in IA (IAID="
                    << addr->ia->getIAID() << ") in client (DUID=\""
                    << addr->client->getDUID()->getPlain()
                    << "\") has expired." << LogEnd;

        SrvAddrMgr().delClntAddr(addr->client->getDUID(),
                                 addr->ia->getIAID(),
                                 addr->addr, false);

        TNotifyScriptParams params;
        notifyExpireInfo(params, *addr, TAddrIA::TYPE_IA);
        SrvIfaceMgr().notifyScript(SrvCfgMgr().getScriptName(), "expire", params);
    }

    for (vector<TSrvAddrMgr::TExpiredInfo>::iterator addr = tempAddrLst.begin();
         addr != tempAddrLst.end(); ++addr) {
        // delete this address
        Log(Notice) << "Temp. address " << *(addr->addr) << " in IA (IAID="
                    << addr->ia->getIAID() << ") in client (DUID=\""
                    << addr->client->getDUID()->getPlain()
                    << "\") has expired." << LogEnd;

        SrvAddrMgr().delTAAddr(addr->client->getDUID(),
                               addr->ia->getIAID(),
                               addr->addr, false);

        TNotifyScriptParams params;
        notifyExpireInfo(params, *addr, TAddrIA::TYPE_TA);
        SrvIfaceMgr().notifyScript(SrvCfgMgr().getScriptName(), "expire", params);
    }

    for (vector<TSrvAddrMgr::TExpiredInfo>::iterator prefix = prefixLst.begin();
         prefix != prefixLst.end(); ++prefix) {
                // delete this prefix
        Log(Notice) << "Prefix " << prefix->addr->getPlain() << " in IAID="
                    << prefix->ia->getIAID() << " for client (DUID="
                    << prefix->client->getDUID()->getPlain()
                    << ") has expired." << LogEnd;

        SrvAddrMgr().delPrefix(prefix->client->getDUID(),
                               prefix->ia->getIAID(),
                               prefix->addr, false);

        TNotifyScriptParams params;
        notifyExpireInfo(params, *prefix, TAddrIA::TYPE_PD);
        SrvIfaceMgr().notifyScript(SrvCfgMgr().getScriptName(), "expire", params);
    }

    SrvAddrMgr().dump();
}

void TSrvTransMgr::shutdown()
{
    SrvAddrMgr().dump();
    IsDone = true;
}

bool TSrvTransMgr::isDone()
{
    return IsDone;
}

char* TSrvTransMgr::getCtrlAddr() {
        return this->ctrlAddr;
}

int  TSrvTransMgr::getCtrlIface() {
        return this->ctrlIface;
}

void TSrvTransMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

TSrvTransMgr::~TSrvTransMgr() {
    Log(Debug) << "SrvTransMgr cleanup." << LogEnd;
}

void TSrvTransMgr::instanceCreate( const std::string config )
{
  if (!Instance)
    Instance = new TSrvTransMgr(config);
  else
    Log(Crit) << "Attempt to create another Transmission Manager. One instance already present!" << LogEnd;
}

TSrvTransMgr & TSrvTransMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "TransMgr not created yet. Application error. Emergency shutdown." << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}

ostream & operator<<(ostream &s, TSrvTransMgr &x)
{
    s << "<TSrvTransMgr>" << endl;
    s << "<!-- SrvTransMgr dumps are not implemented yet -->" << endl;
    s << "</TSrvTransMgr>" << endl;
    return s;
}
