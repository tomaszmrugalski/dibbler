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
    requestMsg = msg;

    if (!msg->check()) {
        // proper warnings will be printed in the check() method, if necessary.
        // Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }

    // Ask NodeClietSpecific to analyse the message
    NodeClientSpecific::analyseMessage(msg);
    //SrvCfgMgr().InClientClass(msg);

    /// @todo (or at least disable by default) answer buffering mechanism
    SPtr<TSrvMsg> answ;
    Log(Debug) << MsgLst.count() << " answers buffered.";

    MsgLst.first();
    while(answ=(Ptr*)MsgLst.get())
    {
        if (answ->getTransID()==msg->getTransID()) {
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
        if (msg->getOption(OPTION_RAPID_COMMIT) && !ptrCfgIface->getRapidCommit()) {
            Log(Info) << "SOLICIT with RAPID-COMMIT received, but RAPID-COMMIT is disabled on "
                      << ptrCfgIface->getName() << " interface." << LogEnd;
        }
        if (msg->getOption(OPTION_RAPID_COMMIT) && ptrCfgIface->getRapidCommit() )
        {
            SPtr<TSrvMsgSolicit> nmsg = (Ptr*)msg;
            SPtr<TSrvMsgReply> answRep = new TSrvMsgReply(nmsg);
            //if at least one IA has in reply message status success
            if (!answRep->isDone()) {
                SPtr<TOpt> ptrOpt;
                answRep->firstOption();
                bool found=false;
                while( (ptrOpt=answRep->getOption()) && (!found) ) {
                    if (ptrOpt->getOptType()==OPTION_IA_NA) {
                        SPtr<TSrvOptIA_NA> ptrIA=(Ptr*) ptrOpt;
                        SPtr<TOptStatusCode> ptrStat= (Ptr*)
                            ptrIA->getOption(OPTION_STATUS_CODE);
                        if(ptrStat&&(ptrStat->getCode()==STATUSCODE_SUCCESS))
                            found=true;
                    }
                }
                if(found) {
                    this->MsgLst.append((Ptr*)answRep);
                    a = (Ptr*)answRep;
                    break;
                }
                // else we didn't assign any address - this message sucks
                // it's better if advertise will be sent - maybe with better options
            }
        }
        //if there was no Rapid Commit option in solicit message or
        //construction of reply with rapid commit wasn't successful
        //Maybe it's possible to construct appropriate advertise message
        //and assign some "not rapid" addresses to this client
        SPtr<TSrvMsgAdvertise> x = new TSrvMsgAdvertise((Ptr*)msg);
        this->MsgLst.append((Ptr*)x);
        a = (Ptr*)x;
        break;
    }
    case REQUEST_MSG:
    {
        SPtr<TSrvMsgRequest> nmsg = (Ptr*)msg;
        answ = new TSrvMsgReply(nmsg);
        this->MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case CONFIRM_MSG:
    {
        SPtr<TSrvMsgConfirm> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply(nmsg);
        this->MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case RENEW_MSG:
    {
        SPtr<TSrvMsgRenew> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply(nmsg);
        this->MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case REBIND_MSG:
    {
        SPtr<TSrvMsgRebind> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply(nmsg);
        MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case DECLINE_MSG:
    {
        SPtr<TSrvMsgDecline> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply( nmsg);
        MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case RELEASE_MSG:
    {
        SPtr<TSrvMsgRelease> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply( nmsg);
        MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case INFORMATION_REQUEST_MSG :
    {
        SPtr<TSrvMsgInfRequest> nmsg=(Ptr*)msg;
        answ=new TSrvMsgReply( nmsg);
        MsgLst.append((Ptr*)answ);
        a = (Ptr*)answ;
        break;
    }
    case LEASEQUERY_MSG:
    {
        int iface = msg->getIface();
        if (!SrvCfgMgr().getIfaceByID(iface) || !SrvCfgMgr().getIfaceByID(iface)->leaseQuerySupport()) {
            Log(Error) << "LQ: LeaseQuery message received on " << iface
                       << " interface, but it is not supported there." << LogEnd;
            return;
        }
        Log(Debug) << "LQ: LeaseQuery received, preparing RQ_REPLY" << LogEnd;
        SPtr<TSrvMsgLeaseQuery> lq = (Ptr*)msg;
        answ = new TSrvMsgLeaseQueryReply(lq);
        MsgLst.append( (Ptr*) answ);
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

/** 
 * creates FQDN option and executes DNS Update procedure (if necessary)
 * 
 * @param requestFQDN  requested Fully Qualified Domain Name
 * @param clntDuid     client DUID
 * @param clntAddr     client address
 * @param hint         hint used to get name (it may or may not be used)
 * @param doRealUpdate - should the real update be performed (for example if response for
 *                       SOLICIT is prepare, this should be set to false)
 * 
 * @return FQDN option
 */
SPtr<TSrvOptFQDN> TSrvTransMgr::addFQDN(int iface, SPtr<TSrvOptFQDN> requestFQDN, SPtr<TDUID> clntDuid, 
                                        SPtr<TIPv6Addr> clntAddr, std::string hint, bool doRealUpdate) {
    SPtr<TSrvOptFQDN> optFQDN;
    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID(iface);
    if (!ptrIface) {
	Log(Crit) << "Msg received through not configured interface. "
	    "Somebody call an exorcist!" << LogEnd;
	this->IsDone = true;
	return 0;
    }
    // FQDN is chosen, "" if no name for this host is found.
    if (!ptrIface->supportFQDN()) {
	Log(Error) << "FQDN is not defined on " << ptrIface->getFullName() << " interface." << LogEnd;
	return 0;
    }
    
    if (!ptrIface->getExtraOption(OPTION_DNS_SERVERS)) {
	Log(Error) << "DNS server is not supported on " << ptrIface->getFullName() << " interface. DNS Update aborted." << LogEnd;
	return 0;
    }
    
    Log(Debug) << "Requesting FQDN for client with DUID=" << clntDuid->getPlain() << ", addr=" << clntAddr->getPlain() << LogEnd;
	
    SPtr<TFQDN> fqdn = ptrIface->getFQDNName(clntDuid,clntAddr, hint);
    if (!fqdn) {
	Log(Debug) << "Unable to find FQDN for this client." << LogEnd;
	return 0;
    } 

    optFQDN = new TSrvOptFQDN(fqdn->getName(), NULL);
    optFQDN->setSFlag(false);
    // Setting the O Flag correctly according to the difference between O flags
    optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
    string fqdnName = fqdn->getName();

    if (requestFQDN->getNFlag()) {
	      Log(Notice) << "FQDN: No DNS Update required." << LogEnd;
	      return optFQDN;
    }

    if (!doRealUpdate) {
	      Log(Debug) << "FQDN: Skipping update (probably a SOLICIT message)." << LogEnd;
	      return optFQDN;
    }

    fqdn->setUsed(true);

    int FQDNMode = ptrIface->getFQDNMode();
    Log(Debug) << "FQDN: Adding FQDN Option in REPLY message: " << fqdnName << ", FQDNMode=" << FQDNMode << LogEnd;

    if ( FQDNMode==1 || FQDNMode==2 ) {
	Log(Debug) << "FQDN: Server configuration allow DNS updates for " << clntDuid->getPlain() << LogEnd;
	
	if (FQDNMode == 1) 
	    optFQDN->setSFlag(false);
	else 
	    if (FQDNMode == 2) 
		optFQDN->setSFlag(true); // letting client update his AAAA
	// Setting the O Flag correctly according to the difference between O flags
	optFQDN->setOFlag(requestFQDN->getSFlag() /*xor 0*/);
	
	SPtr<TIPv6Addr> DNSAddr = SrvCfgMgr().getDDNSAddress(iface);
	if (!DNSAddr) {
	    Log(Error) << "DDNS: DNS Update aborted. DNS server address is not specified." << LogEnd;
	    return 0;
	}
      	
	SPtr<TAddrClient> ptrAddrClient = SrvAddrMgr().getClient(clntDuid);	
	if (!ptrAddrClient) { 
	    Log(Warning) << "Unable to find client."; 
	    return 0;
	}
	ptrAddrClient->firstIA();
	SPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
	if (!ptrAddrIA) { 
	    Log(Warning) << "Client does not have any addresses assigned." << LogEnd; 
	    return 0;
	}
	ptrAddrIA->firstAddr();
	SPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
	SPtr<TIPv6Addr> IPv6Addr = addr->get();
	
	Log(Notice) << "FQDN: About to perform DNS Update: DNS server=" << *DNSAddr << ", IP=" 
		    << *IPv6Addr << " and FQDN=" << fqdnName << LogEnd;
      	
	//Test for DNS update
	char zoneroot[128];
	doRevDnsZoneRoot(IPv6Addr->getAddr(), zoneroot, ptrIface->getRevDNSZoneRootLength());
#ifndef MOD_SRV_DISABLE_DNSUPDATE

	// that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
	// and that would include both poslib and Dibbler headers in one place. Universe would implode then.
	TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
	DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
	if (proto == TCfgMgr::DNSUPDATE_UDP)
	    proto2 = DNSUpdate::DNSUPDATE_UDP;
	if (proto == TCfgMgr::DNSUPDATE_ANY)
	    proto2 = DNSUpdate::DNSUPDATE_ANY;
	unsigned int timeout = SrvCfgMgr().getDDNSTimeout();
	
	if (FQDNMode==1){
	    /* add PTR only */
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), 
					   DNSUPDATE_PTR, proto2);
	    result = act->run(timeout);
	    act->showResult(result);
	    delete act;
	} // fqdnMode == 1
	else if (FQDNMode==2){
	    DnsUpdateResult result = DNSUPDATE_SKIP;
	    DNSUpdate *act = new DNSUpdate(DNSAddr->getPlain(), zoneroot, fqdnName, IPv6Addr->getPlain(), 
					   DNSUPDATE_PTR, proto2);
	    result = act->run(timeout);
	    act->showResult(result);
	    delete act;
      	    
	    DnsUpdateResult result2 = DNSUPDATE_SKIP;
	    DNSUpdate *act2 = new DNSUpdate(DNSAddr->getPlain(), "", fqdnName, 
                                            IPv6Addr->getPlain(),
					    DNSUPDATE_AAAA, proto2);
	    result2 = act2->run(timeout);
	    act2->showResult(result2);
	    delete act2;
	} // fqdnMode == 2
	
	// regardless of the result, store the info
	ptrAddrIA->setFQDN(fqdn);
	ptrAddrIA->setFQDNDnsServer(DNSAddr);
	
#else
      	Log(Error) << "This server is compiled without DNS Update support." << LogEnd;
#endif
    } else {
	Log(Debug) << "Server configuration does NOT allow DNS updates for " << clntDuid->getPlain() << LogEnd;
	optFQDN->setNFlag(true);
    }
    
    return optFQDN;

}

void TSrvTransMgr::removeFQDN(SPtr<TSrvCfgIface> ptrIface, SPtr<TAddrIA> ptrIA, SPtr<TFQDN> fqdn) {
#ifdef MOD_CLNT_DISABLE_DNSUPDATE
    Log(Error) << "This version is compiled without DNS Update support." << LogEnd;
    return;
#else

    string fqdnName = fqdn->getName();
    int FQDNMode = ptrIface->getFQDNMode();
    fqdn->setUsed(false);
    int result;

    SPtr<TIPv6Addr> dns = ptrIA->getFQDNDnsServer();
    if (!dns) {
	Log(Warning) << "Unable to find DNS Server for IA=" << ptrIA->getIAID() << LogEnd;
	return;
    }

    ptrIA->firstAddr();
    SPtr<TAddrAddr> addrAddr = ptrIA->getAddr();
    SPtr<TIPv6Addr> clntAddr;
    if (!addrAddr) {
	Log(Error) << "Client does not have any addresses asigned to IA (IAID=" << ptrIA->getIAID() << ")." << LogEnd;
	return;
    }
    clntAddr = addrAddr->get();

    char zoneroot[128];
    doRevDnsZoneRoot(clntAddr->getAddr(), zoneroot, ptrIface->getRevDNSZoneRootLength());


    // that's ugly but required. Otherwise we would have to include CfgMgr.h in DNSUpdate.h
    // and that would include both poslib and Dibbler headers in one place. Universe would implode then.
    TCfgMgr::DNSUpdateProtocol proto = SrvCfgMgr().getDDNSProtocol();
    DNSUpdate::DnsUpdateProtocol proto2 = DNSUpdate::DNSUPDATE_TCP;
    if (proto == TCfgMgr::DNSUPDATE_UDP)
        proto2 = DNSUpdate::DNSUPDATE_UDP;
    if (proto == TCfgMgr::DNSUPDATE_ANY)
        proto2 = DNSUpdate::DNSUPDATE_ANY;
    unsigned int timeout = SrvCfgMgr().getDDNSTimeout();

    if (FQDNMode == DNSUPDATE_MODE_PTR){
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), 
                                       DNSUPDATE_PTR_CLEANUP, proto2);
	result = act->run(timeout);
	act->showResult(result);
	delete act;
	
    } // fqdn mode 1 (PTR only)
    else if (FQDNMode == DNSUPDATE_MODE_BOTH){
	/* AAAA Cleanup */
	Log(Notice) << "FQDN: Attempting to clean up AAAA and PTR record in DNS Server " << * dns << ", IP = " 
		    << *clntAddr << " and FQDN=" << fqdn->getName() << LogEnd;
	
	DNSUpdate *act = new DNSUpdate(dns->getPlain(), "", fqdnName, clntAddr->getPlain(), 
                                       DNSUPDATE_AAAA_CLEANUP, proto2);
	result = act->run(timeout);
	act->showResult(result);
	delete act;
	
	/* PTR cleanup */
	Log(Notice) << "FQDN: Attempting to clean up PTR record in DNS Server " << * dns << ", IP = " << *clntAddr 
		    << " and FQDN=" << fqdn->getName() << LogEnd;
	DNSUpdate *act2 = new DNSUpdate(dns->getPlain(), zoneroot, fqdnName, clntAddr->getPlain(), 
                                        DNSUPDATE_PTR_CLEANUP, proto2);
	result = act2->run(timeout);
	act2->showResult(result);
	delete act2;
    } // fqdn mode 2 (AAAA and PTR)
#endif
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

/// TODO Remove this horrible workaround!
SPtr<TSrvMsg> TSrvTransMgr::getCurrentRequest()
{
    return requestMsg;
}

ostream & operator<<(ostream &s, TSrvTransMgr &x)
{
    s << "<TSrvTransMgr>" << endl;
    s << "<!-- SrvTransMgr dumps are not implemented yet -->" << endl;
    s << "</TSrvTransMgr>" << endl;
    return s;
}
