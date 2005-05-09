/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelTransMgr.cpp,v 1.12 2005-05-09 23:17:50 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/05/09 23:16:08  thomson
 * Interface-ID option length check improved.
 *
 * Revision 1.10  2005/05/03 15:36:01  thomson
 * Relay now binds global (or site scoped) address instead of link-local
 * for srv unicast communication. (bug #113)
 *
 * Revision 1.9  2005/05/02 21:52:39  thomson
 * Logging cleanup.
 *
 * Revision 1.8  2005/05/02 21:20:34  thomson
 * Issues with invalid port usage have been fixed.
 *
 * Revision 1.7  2005/05/02 20:58:13  thomson
 * Support for multiple relays added. (bug #107)
 *
 * Revision 1.6  2005/04/28 21:20:52  thomson
 * Support for multiple relays added.
 *
 * Revision 1.5  2005/04/25 00:19:20  thomson
 * Changes in progress.
 *
 * Revision 1.4  2005/01/24 00:42:57  thomson
 * no message
 *
 * Revision 1.3  2005/01/23 23:17:53  thomson
 * Relay/global address support related improvements.
 *
 * Revision 1.2  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.1  2005/01/11 22:53:36  thomson
 * Relay skeleton implemented.
 *
 */

#define MAX_PACKET_LEN 1452
#define RELAY_FORW_MSG_LEN 36

#include "RelTransMgr.h"
#include "RelCfgMgr.h"
#include "RelIfaceMgr.h"
#include "RelOptInterfaceID.h"
#include "Logger.h"

TRelTransMgr::TRelTransMgr(TCtx * ctx, string xmlFile)
{
    // remember context
    this->Ctx = ctx;
    this->IsDone = false; // TransMgr is certainly not done yet. We're just getting started
    this->XmlFile = xmlFile;

    // for each interface in CfgMgr, create socket (in IfaceMgr)
    SmartPtr<TRelCfgIface> confIface;
    this->Ctx->CfgMgr->firstIface();
    while (confIface=this->Ctx->CfgMgr->getIface()) {
	if (!this->openSocket(confIface)) {
	    this->IsDone = true;
	    break;
	}
    }
}

/*
 * opens proper (multicast or unicast) socket on interface 
 */
bool TRelTransMgr::openSocket(SmartPtr<TRelCfgIface> cfgIface) {

    SmartPtr<TIfaceIface> iface = this->Ctx->IfaceMgr->getIfaceByID(cfgIface->getID());
    if (!iface) {
	Log(Crit) << "Unable to find " << cfgIface->getName() << "/" << cfgIface->getID()
		  << " interface in the IfaceMgr." << LogEnd;
	return false;
    }

    SmartPtr<TIPv6Addr> srvUnicast = cfgIface->getServerUnicast();
    SmartPtr<TIPv6Addr> clntUnicast = cfgIface->getClientUnicast();
    SmartPtr<TIPv6Addr> addr;

    if (cfgIface->getServerMulticast() || srvUnicast) {

	addr = iface->getGlobalAddr();
	if (!addr) {
	    Log(Warning) << "No global address defined on the " << iface->getFullName() << " interface."
			 << " Trying to bind link local address, but expect troubles with relaying." << LogEnd;
	    iface->firstLLAddress();
	    addr = new TIPv6Addr(iface->getLLAddress());
	}
	Log(Notice) << "Creating srv unicast (" << addr->getPlain() << ") socket on the "
		    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
	if (!iface->addSocket(addr, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    }

    if (cfgIface->getClientMulticast()) {
	addr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
	Log(Notice) << "Creating clnt multicast (" << addr->getPlain() << ") socket on the "
		    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
	if (!iface->addSocket(addr, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    }

    if (clntUnicast) {
	addr = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
	Log(Notice) << "Creating clnt unicast (" << clntUnicast->getPlain() << ") socket on the "
		    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
	if (!iface->addSocket(clntUnicast, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    }

    return true;
}


/**
 * relays normal (i.e. not server replies) messages to defined servers
 */
void TRelTransMgr::relayMsg(SmartPtr<TRelMsg> msg)
{	
    static char buf[MAX_PACKET_LEN];
    int offset = 0;
    int bufLen;
    int hopCount = 0;
    if (!msg->check()) {
	Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }
    
    if (msg->getDestAddr()) {
	this->relayMsgRepl(msg);
	return;
    }

    if (msg->getType() == RELAY_FORW_MSG) {
	hopCount = msg->getHopCount()+1;
    }

    // prepare message
    SmartPtr<TIfaceIface> iface = this->Ctx->IfaceMgr->getIfaceByID(msg->getIface());
    SmartPtr<TIPv6Addr> addr;

    // store header
    buf[offset++] = RELAY_FORW_MSG;
    buf[offset++] = hopCount;

    // store link-addr 
    addr = iface->getGlobalAddr();
    if (!addr) {
	Log(Warning) << "Interface " << iface->getFullName() << " does not have global address." << LogEnd;
	addr = new TIPv6Addr("::", true);
    }
    addr->storeSelf(buf+offset);
    offset += 16;

    // store peer-addr
    addr = msg->getAddr();
    addr->storeSelf(buf+offset);
    offset += 16;

    SmartPtr<TRelCfgIface> cfgIface;
    cfgIface = this->Ctx->CfgMgr->getIfaceByID(msg->getIface());
    TRelOptInterfaceID ifaceID(cfgIface->getInterfaceID(), 0);
    
#if 1
    {
	// store InterfaceID option
	ifaceID.storeSelf(buf + offset);
	offset += ifaceID.getSize();
    }
#endif

    // store relay msg option
    *(short*)(buf+offset) = htons(OPTION_RELAY_MSG);
    offset+=2;
    *(short*)(buf+offset) = htons(msg->getSize());
    offset+=2;
    bufLen = msg->storeSelf(buf+offset);
    offset += bufLen;

#if 0
    {
	// store InterfaceID option
	ifaceID.storeSelf(buf + offset);
	offset += ifaceID.getSize();
    }
#endif

    this->Ctx->CfgMgr->firstIface();
    while (cfgIface = this->Ctx->CfgMgr->getIface()) {
	if (cfgIface->getServerUnicast()) {
	    Log(Notice) << "Relaying message on the " << cfgIface->getName() << "/" << cfgIface->getID()
			<< " interface to unicast (" << cfgIface->getServerUnicast()->getPlain() << ") address." << LogEnd;

	    if (!this->Ctx->IfaceMgr->send(cfgIface->getID(), buf, offset, cfgIface->getServerUnicast(), DHCPSERVER_PORT)) {
		Log(Error) << "Failed to send data." << LogEnd;
	    }
					   
	}
	if (cfgIface->getServerMulticast()) {
	    addr = new TIPv6Addr(ALL_DHCP_SERVERS, true);
	    Log(Notice) << "Relaying message on the " << cfgIface->getName() << "/" << cfgIface->getID()
			<< " interface to multicast (" << addr->getPlain() << ") address." << LogEnd;
	    if (!this->Ctx->IfaceMgr->send(cfgIface->getID(), buf, offset, addr, DHCPSERVER_PORT)) {
		Log(Error) << "Failed to send data." << LogEnd;
	    }
	}
    }

    // save DB state regardless of action taken
    this->Ctx->CfgMgr->dump();
}	

void TRelTransMgr::relayMsgRepl(SmartPtr<TRelMsg> msg) {
    int port;
    SmartPtr<TRelCfgIface> cfgIface = this->Ctx->CfgMgr->getIfaceByInterfaceID(msg->getDestIface());
    if (!cfgIface) {
	Log(Error) << "Unable to relay message: Invalid interfaceID value:" << msg->getDestIface() << LogEnd;
	return;
    }

    SmartPtr<TIfaceIface> iface = this->Ctx->IfaceMgr->getIfaceByID(cfgIface->getID());
    SmartPtr<TIPv6Addr> addr = msg->getDestAddr();
    static char buf[MAX_PACKET_LEN];
    int bufLen;

    if (!iface) { 
	Log(Warning) << "Unable to find interface with interfaceID=" << msg->getDestIface()
		     << LogEnd;
	return;
    }

    bufLen = msg->storeSelf(buf);
    if (msg->getType() == RELAY_REPL_MSG)
	port = DHCPSERVER_PORT;
    else
	port = DHCPCLIENT_PORT;
    Log(Notice) << "Relaying " << msg->getName() << " message on the " << iface->getFullName()
		<< " interface to the " << addr->getPlain() << ", port " << port << "." << LogEnd;

    if (!this->Ctx->IfaceMgr->send(iface->getID(), buf, bufLen, addr, DHCPCLIENT_PORT)) {
	Log(Error) << "Failed to send data." << LogEnd;
    }
    
}

void TRelTransMgr::shutdown() {
    IsDone = true;
}

bool TRelTransMgr::isDone() {
    return this->IsDone;
}

bool TRelTransMgr::doDuties() {
    return 0;
}

char* TRelTransMgr::getCtrlAddr() {
	return this->ctrlAddr;
}

int  TRelTransMgr::getCtrlIface() {
	return this->ctrlIface;
}

void TRelTransMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

TRelTransMgr::~TRelTransMgr() {
    Log(Debug) << "RelTransMgr cleanup." << LogEnd;
}


ostream & operator<<(ostream &s, TRelTransMgr &x)
{
    s << "<TRelTransMgr>" << endl;
    s << "<!-- RelTransMgr dumps are not implemented yet -->" << endl;
    s << "</TRelTransMgr>" << endl;
    return s;
}
