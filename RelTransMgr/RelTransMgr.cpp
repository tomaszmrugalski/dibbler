/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelTransMgr.cpp,v 1.1 2005-01-11 22:53:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "RelTransMgr.h"
#include "RelCfgMgr.h"
#include "RelIfaceMgr.h"
#include "Logger.h"

TRelTransMgr::TRelTransMgr(TCtx * ctx, string xmlFile)
{
    // remember context
    this->Ctx = ctx;
    this->IsDone = false; // TransMgr is certainly not done yet. We're just getting started
    
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
    SmartPtr<TIPv6Addr> multicast;

    if (cfgIface->getServerMulticast()) {
    }

    if (srvUnicast) {

    }

    if (cfgIface->getClientMulticast()) {
	multicast = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
	Log(Notice) << "Creating multicast (" << multicast->getPlain() << ") socket on the "
		    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
	if (!iface->addSocket(multicast, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    }

    if (clntUnicast) {
	multicast = new TIPv6Addr(ALL_DHCP_RELAY_AGENTS_AND_SERVERS, true);
	Log(Notice) << "Creating unicast (" << clntUnicast->getPlain() << ") socket on the "
		    << iface->getName() << "/" << iface->getID() << " interface." << LogEnd;
	if (!iface->addSocket(clntUnicast, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    }

    return true;
}

void TRelTransMgr::relayMsg(SmartPtr<TRelMsg> msg)
{	
    if (!msg->check()) {
	Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }
    
    // FIXME:
    
    // save DB state regardless of action taken
    this->Ctx->CfgMgr->dump();
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
