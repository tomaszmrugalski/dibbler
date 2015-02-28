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
#include <map>
#include <limits.h>
#include "SrvTransMgr.h"
#include "SmartPtr.h"
#include "SrvCfgIface.h"
#include "IfaceMgr.h"
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
#include "SrvMsgReconfigure.h"
#include "SrvMsg.h"
#include "SrvMsgLeaseQuery.h"
#include "SrvMsgLeaseQueryReply.h"
#include "SrvOptIA_NA.h"
#include "OptStatusCode.h"
#include "NodeClientSpecific.h"

using namespace std;

TSrvTransMgr * TSrvTransMgr::Instance = 0;

TSrvTransMgr::TSrvTransMgr(const std::string xmlFile, int port)
    : XmlFile(xmlFile), IsDone(false), port_(port)
{
    // TransMgr is certainly not done yet. We're just getting started

    // for each interface in CfgMgr, create socket (in IfaceMgr)
    SPtr<TSrvCfgIface> confIface;
    SrvCfgMgr().firstIface();
    while (confIface = SrvCfgMgr().getIface()) {
        if (!this->openSocket(confIface, port)) {
            this->IsDone = true;
            break;
        }
    }
    
    if (SrvCfgMgr().getReconfigureSupport()) {
        int clients = checkReconfigures();
        Log(Info) << "Sent Reconfigure to " << clients << " client(s)." << LogEnd;
    } else {
        Log(Info) << "Reconfigure support was not enabled." << LogEnd;
    }

    SrvAddrMgr().setCacheSize(SrvCfgMgr().getCacheSize());
}

/// @brief Checks loaded database and sends RECONFIGURE to some clients.
///
/// Checks loaded database against current configuration and finds addresses
/// that are outdated (do not match current configuration). RECONFIGURE message
/// is sent to clients that currently hold those addresses.
///
/// @return number of clients that were reconfigured
int TSrvTransMgr::checkReconfigures() {

    int clients = 0; // how many client did we reconfigure?
    int iface;
    bool PD;
    bool check;
    SPtr<TAddrClient> cli;
    SPtr<TDUID> ptrDUID;
    SrvAddrMgr().firstClient();
    unsigned long IAID;

    Log(Debug) << "Checking which clients need RECONFIGURE." << LogEnd;

    while (cli = SrvAddrMgr().getClient() ) 
    {
        /// @todo clean up this shit
        check=true;
        ptrDUID=cli->getDUID();
        SPtr<TAddrIA> ia;
        cli->firstIA();
        while ( (ia = cli->getIA()) && check) 
        {
            IAID=ia->getIAID();	
            iface=ia->getIfindex();
            SPtr<TIfaceIface> ptrIface = SrvIfaceMgr().getIfaceByID(iface);
            SPtr<TIPv6Addr> unicast=ia->getSrvAddr();
            SPtr<TAddrAddr> adr;
            SPtr<TIPv6Addr> addra;
            ia->firstAddr();
            while( (adr=ia->getAddr()) && check ) 
            {
                PD=false;
                if(ClientInPool1(adr->get(),iface,PD))
                {
                    Log(Debug) << "Client " << cli->getDUID()->getPlain()
                               << " doesn't need to reconfigure IA (iaid=" << ia->getIAID() << ")." << LogEnd;
                }
                else 
                {
                    Log(Info) << "Client " << cli->getDUID()->getPlain()
                              << " uses outdated info. Sending RECONFIGURE." << LogEnd;
                    if (!unicast) {
                        Log(Warning) << "Unable to send RECONFIGURE to client " << cli->getDUID()->getPlain()
                                  << ": no unicast address recorded." << LogEnd;
                        check = false;
                        // Can't do anything here if we don't have unicast address of the client.
                        // (That's odd. Unicast should be in the database. Are we dealing with a
                        // broken database here?)
                        break;
                    }

                    sendReconfigure(unicast, iface, RENEW_MSG, ptrDUID);
                    clients++;
                    if (SrvAddrMgr().delClntAddr(cli->getDUID(), ia->getIAID(), adr->get(),false)) {
                        Log(Debug) << "Outdated " << *adr->get() << " address deleted." << LogEnd;
                   }
                    check = false;
                    break; // break inner while loop
                }
            }
        } 
        SPtr<TAddrIA> pd;
        cli->firstPD();
        while ( (pd = cli->getPD()) && check ) 
        {
            IAID=pd->getIAID();
            iface=pd->getIfindex();
            SPtr<TAddrPrefix> prefix;
            SPtr<TIPv6Addr> addra;
            SPtr<TIfaceIface> ptrIface = SrvIfaceMgr().getIfaceByID(iface);
            SPtr<TIPv6Addr> unicast=pd->getSrvAddr();
            pd->firstPrefix();
            while(prefix=pd->getPrefix() ) 
            {
                PD=true;
                if(ClientInPool1(prefix->get(),iface,PD))
                {
                    Log(Debug) << "Client " << cli->getDUID()->getPlain() << " doesn't need to reconfigure PD (iaid="
                               << pd->getIAID() << ")." << LogEnd;
                }
                else
                {
                    if (!unicast) {
                        Log(Warning) << "Unable to send RECONFIGURE to client " << cli->getDUID()->getPlain()
                                  << ": no unicast address recorded." << LogEnd;
                        check = false;
                        // Can't do anything here if we don't have unicast address of the client.
                        // (That's odd. Unicast should be in the database. Are we dealing with a
                        // broken database here?)
                        break;
                    }

                    Log(Info) << "Client " << cli->getDUID()->getPlain()
                              << "uses outdated info. Sending RECONFIGURE." << LogEnd;

                    sendReconfigure(unicast, iface, RENEW_MSG, ptrDUID);
                    clients++;
                    check=false;
                    if(SrvAddrMgr().delPrefix(ptrDUID, IAID, prefix->get(),true)) {
                        Log(Debug) << "Outdated " << *prefix->get() << " prefix for client "
                                   << cli->getDUID()->getPlain() << " deleted." << LogEnd;
                    }
                    break;
                }
            }
        }
    }

    return clients;
}

/*
 * opens proper (multicast or unicast) socket on interface
 */
bool TSrvTransMgr::openSocket(SPtr<TSrvCfgIface> confIface, int port) {

    int ifindex = -1;
    if (confIface->isRelay()) {
      ifindex = confIface->getRelayID();
    } else {
      ifindex = confIface->getID();
    }
    SPtr<TIfaceIface> iface = SrvIfaceMgr().getIfaceByID(ifindex);
    SPtr<TIPv6Addr> unicast = confIface->getUnicast();
    if (!iface) {
        Log(Crit) << "Unable to find interface with ifindex=" << ifindex << LogEnd;
        return false;
    }

    if (confIface->isRelay()) {
        Log(Info) << "Relay init: Creating socket on the underlaying interface: "
		  << iface->getFullName() << "." << LogEnd;
    }

    if (unicast) {
        /* unicast */
        Log(Notice) << "Creating unicast (" << *unicast << ") socket on "
		    << confIface->getFullName() << " interface." << LogEnd;
        if (!iface->addSocket(unicast, port, true, false)) {
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
    Log(Notice) << "Creating multicast (" << ipAddr->getPlain() << ") socket on "
                << confIface->getFullName() << " (" << iface->getFullName()
                << ") interface." << LogEnd;
    if (iface->getSocketByAddr(ipAddr)) {
        Log(Notice) << "Address " << ipAddr->getPlain() << " is already bound on the "
                    << iface->getName() << "." << LogEnd;
        return true;
    }

    if (!iface->addSocket(ipAddr, port, true, false)) {
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
        if (!iface->addSocket(llAddr, port, true, false)) {
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
    if (SrvCfgMgr().inactiveIfacesCnt() && ifaceRecheckPeriod<min) {
        min = ifaceRecheckPeriod;
    }
    addrTimeout = SrvAddrMgr().getValidTimeout();
    if (min < addrTimeout) {
        return min;
    } else {
        return addrTimeout;
    }
}

void TSrvTransMgr::relayMsg(SPtr<TSrvMsg> msg)
{
    if (!msg->check()) {
        // proper warnings will be printed in the check() method, if necessary.
        // Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }

    // If unicast is disabled and the client sent it to unicast,
    // send back status code with UseMulticast and be done with it.
    if (!unicastCheck(msg)) {
        Log(Warning) << "Message was dropped, because it was sent to unicast and "
                     << "unicast traffic is not allowed." << LogEnd;
        return;
    }

    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(msg->getIface());
    if (!cfgIface) {
        Log(Error) << "Received message on unknown interface (ifindex="
                   << msg->getIface() << LogEnd;
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

    SPtr<TMsg> q, a; // question and answer
    q = (Ptr*) msg;

    switch(msg->getType()) {
    case SOLICIT_MSG: {
        if (msg->getOption(OPTION_RAPID_COMMIT)) {
            if (!cfgIface->getRapidCommit()) {
                Log(Info) << "SOLICIT with RAPID-COMMIT received, but RAPID-COMMIT is disabled on "
                          << cfgIface->getName() << " interface." << LogEnd;
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
        if (!SrvCfgMgr().getIfaceByID(iface) ||
            !SrvCfgMgr().getIfaceByID(iface)->leaseQuerySupport()) {
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

    if (a && !a->isDone()) {
        SPtr<TSrvMsg> answ = (Ptr*)a;

        // Send the packet
        sendPacket(answ);

        // Call notify script
        SrvIfaceMgr().notifyScripts(SrvCfgMgr().getScriptName(), q, a);
    }

    // save DB state regardless of action taken
    SrvAddrMgr().dump();
    SrvCfgMgr().dump();
}

void TSrvTransMgr::sendPacket(SPtr<TSrvMsg> msg) {
    if (!msg) {
        return;
    }

    msg->send();
}

bool TSrvTransMgr::unicastCheck(SPtr<TSrvMsg> msg) {

    // If it's relayed message, then it's ok
    if (!msg->RelayInfo_.empty())
        return true;

    // Ok, we don't know what address it was received on.
    // This must be one weird OS we're running on. Anyway, let's
    // pretend it was not unicast.
    if (!msg->getLocalAddr())
        return true;

    // All good, received on multicast
    if (msg->getLocalAddr()->multicast())
        return true;

    // Received on unicast and it's not relayed. Do we have
    // unicast support enabled on this interface?
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(msg->getIface());

    // That's weird! We don't have a configuration for this interface?
    if (!cfgIface)
        return true;

    // Do we have unicast enabled on this interface? Yes => all is good.
    if (cfgIface->getUnicast())
        return true;

    // Ok, so we've got a problem here. Unicast is forbidden and we
    // received unicast traffic.
    if (!SrvCfgMgr().dropUnicast()) {
        Log(Warning) << "Received message on address " << msg->getLocalAddr()->getPlain()
                     << " on interface " << cfgIface->getFullName()
                     << ", but unicast is not allowed on this interface."
                     << LogEnd;
        return true;
    }

    TOptList options;

    SPtr<TOpt> status(new TOptStatusCode(STATUSCODE_USEMULTICAST,
                                         string("Please send your message to multicast, not to ")
                                         + msg->getLocalAddr()->getPlain(), NULL));
    options.push_back(status);

    // Message will be sent in the constructor
    TSrvMsgReply(msg, options);

    return false;
}

void TSrvTransMgr::doDuties()
{
    // are there any outdated addresses?
    std::vector<TSrvAddrMgr::TExpiredInfo> addrLst;
    std::vector<TSrvAddrMgr::TExpiredInfo> tempAddrLst;
    std::vector<TSrvAddrMgr::TExpiredInfo> prefixLst;

    if (!SrvAddrMgr().getValidTimeout()) {
        SrvAddrMgr().doDuties(addrLst, tempAddrLst, prefixLst);
        removeExpired(addrLst, tempAddrLst, prefixLst);
    }

    // Open socket on interface which becames ready during server run
    if (SrvCfgMgr().inactiveMode())
    {
        SPtr<TSrvCfgIface> x;
        x = SrvCfgMgr().checkInactiveIfaces();
        if (x)
            openSocket(x, port_);
    }

}


/// @brief Generates parameters for notify script based on expired lease information
///
/// @param params Notify parameters (all available info will be set here)
/// @param exp expired lease details
/// @param type type of lease (IA, TA or PD)
void TSrvTransMgr::notifyExpireInfo(TNotifyScriptParams& params,
                                    const TSrvAddrMgr::TExpiredInfo& exp,
                                    TIAType type) {
    stringstream tmp;
    tmp << exp.ia->getIfindex();
    params.addParam("IFINDEX", tmp.str());

    SPtr<TIfaceIface> iface = SrvIfaceMgr().getIfaceByID(exp.ia->getIfindex());
    if (iface)
        params.addParam("IFACE", iface->getName());

    if (exp.ia->getSrvAddr())
        params.addParam("REMOTE_ADDR", exp.ia->getSrvAddr()->getPlain());
    switch (type) {
    case IATYPE_IA:
    case IATYPE_TA:
    {
        params.addAddr(exp.addr, 0, 0, "");
        break;
    }
    case IATYPE_PD:
    {
        params.addPrefix(exp.addr, exp.prefixLen, 0, 0);
	break;
    }
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

	// FQDN - remove
	SPtr<TIPv6Addr> dnsAddr = addr->ia->getFQDNDnsServer();
	SPtr<TFQDN> fqdn = addr->ia->getFQDN();
        if (dnsAddr && fqdn) {
	    SrvIfaceMgr().delFQDN(addr->ia->getIfindex(), dnsAddr,
				  addr->addr, fqdn->getName());
	    /// @todo: remove this FQDN from the list of used names
        }

        SrvAddrMgr().delClntAddr(addr->client->getDUID(),
                                 addr->ia->getIAID(),
                                 addr->addr, false);

        SrvCfgMgr().delClntAddr(addr->ia->getIfindex(), addr->addr);

        TNotifyScriptParams params;
        notifyExpireInfo(params, *addr, IATYPE_IA);
		std::string scriptName = SrvCfgMgr().getScriptName();
		if( !scriptName.empty() )
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
        notifyExpireInfo(params, *addr, IATYPE_TA);
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

        SrvCfgMgr().decrPrefixCount(prefix->ia->getIfindex(), prefix->addr);

        TNotifyScriptParams params;
        notifyExpireInfo(params, *prefix, IATYPE_PD);
        SrvIfaceMgr().notifyScript(SrvCfgMgr().getScriptName(), "expire", params);
    }

    SrvAddrMgr().dump();
    SrvCfgMgr().dump();
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

void TSrvTransMgr::instanceCreate(const std::string& config, int port)
{
  if (!Instance)
      Instance = new TSrvTransMgr(config, port);
  else
    Log(Crit) << "Attempt to create another Transmission Manager. "
              << "One instance already present!" << LogEnd;
}

TSrvTransMgr & TSrvTransMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "TransMgr not created yet. Application error. "
                  << "Emergency shutdown." << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}

/// @brief checks/updates loaded database (regarding interface names/indexes)
///
///
/// @return true if sanitization was successful, false if it failed
bool TSrvTransMgr::sanitizeAddrDB() {

    // Those two maps will hold current interface names/ifindexes
    TAddrMgr::NameToIndexMapping currentNameToIndex;
    TAddrMgr::IndexToNameMapping currentIndexToName;

    // Let's get name->index and index->name maps first
    SrvCfgMgr().firstIface();
    while (SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIface()) {
        currentNameToIndex.insert(make_pair(iface->getName(), iface->getID()));
        currentIndexToName.insert(make_pair(iface->getID(), iface->getName()));
    }

    // Ok, let's iterate over all loaded entries in Ifa

    return SrvAddrMgr().updateInterfacesInfo(currentNameToIndex,
                                             currentIndexToName);
}

ostream & operator<<(ostream &s, TSrvTransMgr &x)
{
    s << "<TSrvTransMgr>" << endl;
    s << "<!-- SrvTransMgr dumps are not implemented yet -->" << endl;
    s << "</TSrvTransMgr>" << endl;
    return s;
}

/// @brief Checks if client's address or prefix is in current pool
///
/// This method is used after configuration and old database is loaded.
/// We need to check if client's leases are still within current configuration.
///
/// @param addr address
/// @param iface interface 
/// @param PD is this PD?
///
/// @return 
bool TSrvTransMgr::ClientInPool1(SPtr<TIPv6Addr> addr, int iface, bool PD) {

    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID(iface);
    if (!ptrIface)
	return false;
    
    if(PD) {
        // checking prefix delegation
        ptrIface->firstPD();
        SPtr<TSrvCfgPD> PDClass;
        while (PDClass = ptrIface->getPD()) {
            if (PDClass->prefixInPool(addr)){
                return true;
            }
        }
        return false;
    }
    else {
        // checking addresses
    	ptrIface->firstAddrClass();
    	SPtr<TSrvCfgAddrClass> addrClass;
    	while (addrClass = ptrIface->getAddrClass()) {
            if (addrClass->addrInPool(addr)){
                return true;
            }
    	}
    	return false;
   }
}

bool TSrvTransMgr::sendReconfigure(SPtr<TIPv6Addr> addr, int iface,
                                   int msgType, SPtr<TDUID> ptrDUID)
{
    SPtr<TSrvMsg> reconfigure;
    reconfigure = new TSrvMsgReconfigure(iface, addr, msgType, ptrDUID);
    //reconfigure->send(); // not needed (message will send itself in constructor)
    return true;
}

// vim:ts=4 expandtab
