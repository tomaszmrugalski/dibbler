/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvTransMgr.cpp,v 1.21 2004-12-07 00:45:10 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.20  2004/11/29 23:25:13  thomson
 * Server now properly retransmits messages.
 *
 * Revision 1.19  2004/11/02 01:37:09  thomson
 * *** empty log message ***
 *
 * Revision 1.18  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.17  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.16  2004/09/07 15:37:44  thomson
 * Socket handling changes.
 *
 * Revision 1.15  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.14  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 */

#include <limits.h>
#include "SrvTransMgr.h"
#include "SmartPtr.h"
#include "SrvCfgIface.h"
#include "Iface.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "AddrClient.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgReply.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRelease.h"
#include "SrvOptIA_NA.h"
#include "SrvOptStatusCode.h"

TSrvTransMgr::TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
			   SmartPtr<TSrvAddrMgr> addrMgr,
			   SmartPtr<TSrvCfgMgr> cfgMgr,
                           string xmlFile)
{
    // remember IfaceMgr and create remaining managers
    this->IfaceMgr = ifaceMgr;
    this->AddrMgr  = addrMgr;
    this->CfgMgr   = cfgMgr;
    
    // TransMgr is certainly not done yet. We're just getting started
    IsDone = false;
    
    // for each interface in CfgMgr, create socket (in IfaceMgr)
    SmartPtr<TSrvCfgIface> confIface;
    CfgMgr->firstIface();
    while (confIface=CfgMgr->getIface()) {
	if (!this->openSocket(confIface)) {
	    this->IsDone = true;
	    break;
	}
    }
}

/*
 * opens proper (multicast or unicast) socket on interface 
 */
bool TSrvTransMgr::openSocket(SmartPtr<TSrvCfgIface> confIface) {
    char srvAddr[16];
    inet_pton6(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,srvAddr);
    SmartPtr<TIPv6Addr> ipAddr(new TIPv6Addr(srvAddr));
    
    SmartPtr<TIfaceIface> iface=IfaceMgr->getIfaceByID(confIface->getID());
    SmartPtr<TIPv6Addr> unicast = confIface->getUnicast();

	if (unicast) {
	/* unicast */
	Log(Notice) << "Creating unicast (" << *unicast << ") socket on " << confIface->getName() 
		    << "/" << confIface->getID() << " interface." << LogEnd;
	if (!iface->addSocket( unicast, DHCPSERVER_PORT, true, false)) {
	    Log(Crit) << "Proper socket creation failed." << LogEnd;
	    return false;
	}
    } 
    
    /* multicast */
    Log(Notice) << "Creating multicast (ff02::1:2) socket on " << confIface->getName() 
		<< "/" << confIface->getID() << " interface." << LogEnd;
    if (!iface->addSocket( ipAddr, DHCPSERVER_PORT, true, false)) {
	Log(Crit) << "Proper socket creation failed." << LogEnd;
	return false;
    }
    return true;
}

long TSrvTransMgr::getTimeout()
{
    unsigned long min = 0xffffffff;
    unsigned long addrTimeout = 0xffffffff;
    SmartPtr<TMsg> ptrMsg;
    MsgLst.first();
    while (ptrMsg = MsgLst.get() ) 
    {
        if (ptrMsg->getTimeout() < min) 
            min = ptrMsg->getTimeout();
    }
    addrTimeout = AddrMgr->getTimeout();
    return min<addrTimeout?min:addrTimeout;
}

void TSrvTransMgr::relayMsg(SmartPtr<TMsg> msg)
{	
    if (!msg->check())
    {
	Log(Warning) << "Invalid message received." << LogEnd;
        return;
    }
    // Do we have ready answer for this?
    SmartPtr<TMsg> answ;
    Log(Debug) << MsgLst.count() << " answers buffered.";

    MsgLst.first();
    while(answ=MsgLst.get()) 
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

    switch(msg->getType()) {
    case SOLICIT_MSG: {
	SmartPtr<TSrvCfgIface> ptrCfgIface = CfgMgr->getIfaceByID(msg->getIface());
	if (msg->getOption(OPTION_RAPID_COMMIT) && !ptrCfgIface->getRapidCommit()) {
	    Log(Info) << "SOLICIT with RAPID-COMMIT received, but RAPID-COMMIT is disabled on "
		      << ptrCfgIface->getName() << " interface." << LogEnd;
	}
	if (msg->getOption(OPTION_RAPID_COMMIT) && ptrCfgIface->getRapidCommit() )
	{
	    SmartPtr<TSrvMsgSolicit> nmsg = (Ptr*)msg;
	    SmartPtr<TSrvMsgReply> answRep=new TSrvMsgReply(IfaceMgr, That, CfgMgr, AddrMgr, nmsg);
	    //if at least one IA has in reply message status success
	    if (!answRep->isDone()) {
		SmartPtr<TOpt> ptrOpt;
		answRep->firstOption();
		bool found=false;
		while( (ptrOpt=answRep->getOption()) && (!found) ) {
		    if (ptrOpt->getOptType()==OPTION_IA) {
			SmartPtr<TSrvOptIA_NA> ptrIA=(Ptr*) ptrOpt;
			SmartPtr<TSrvOptStatusCode> ptrStat= (Ptr*)
			    ptrIA->getOption(OPTION_STATUS_CODE);
			if(ptrStat&&(ptrStat->getCode()==STATUSCODE_SUCCESS))
			    found=true;
		    }
		}
		if(found) {
		    this->MsgLst.append((Ptr*)answRep);
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
	SmartPtr<TSrvMsgAdvertise> x = new TSrvMsgAdvertise(IfaceMgr,That,CfgMgr,AddrMgr,(Ptr*)msg);
	this->MsgLst.append((Ptr*)x);
	break;
    }
    case REQUEST_MSG: 
    {
	SmartPtr<TSrvMsgRequest> nmsg = (Ptr*)msg;
	answ=new TSrvMsgReply(IfaceMgr, That, CfgMgr, AddrMgr, nmsg);
	this->MsgLst.append((Ptr*)answ);
	break;
    }
    case CONFIRM_MSG: 
    {
	SmartPtr<TSrvMsgConfirm> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply(IfaceMgr,That,CfgMgr,AddrMgr,nmsg);
	this->MsgLst.append((Ptr*)answ);
	break;
    }
    case RENEW_MSG: 
    {
	SmartPtr<TSrvMsgRenew> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply(IfaceMgr, That,CfgMgr,AddrMgr,nmsg);
	this->MsgLst.append((Ptr*)answ);
	break;
    }
    case REBIND_MSG: 
    {
	SmartPtr<TSrvMsgRebind> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
	MsgLst.append((Ptr*)answ);
	break;
    }
    case DECLINE_MSG: 
    {
	SmartPtr<TSrvMsgDecline> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
	MsgLst.append((Ptr*)answ);
	break;
    }
    case RELEASE_MSG: 
    {
	SmartPtr<TSrvMsgRelease> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
	MsgLst.append((Ptr*)answ);
	break;
    }
    case INFORMATION_REQUEST_MSG : 
    {
	SmartPtr<TSrvMsgInfRequest> nmsg=(Ptr*)msg;
	answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
	MsgLst.append((Ptr*)answ);
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
    case RELAY_FORW:
    case RELAY_REPL:
    default:
    {
	Log(Warning)<< "Message type " << msg->getType() 
		    << " not supported." << LogEnd;
	break;
    }
    }
    
    // save DB state regardless of action taken
    AddrMgr->dump();
    CfgMgr->dump();
}	

void TSrvTransMgr::doDuties()
{
    // are there any outdated addresses? 
    if (!AddrMgr->getTimeout())
        AddrMgr->doDuties();

    // for each message on list, let it do its duties, if timeout is reached
    SmartPtr<TMsg> msg;
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
        }
    }
}

void TSrvTransMgr::shutdown()
{
    AddrMgr->dump();
    IsDone = true;
}

bool TSrvTransMgr::isDone()
{
    return IsDone;
}

void TSrvTransMgr::setThat(SmartPtr<TSrvTransMgr> that)
{
    this->That=that;
    IfaceMgr->setThats(IfaceMgr,That,CfgMgr,AddrMgr);
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


ostream & operator<<(ostream &s, TSrvTransMgr &x)
{
    s << "<TSrvTransMgr>" << endl;
    s << "<!-- SrvTransMgr dumps are not implemented yet -->" << endl;
    s << "</TSrvTransMgr>" << endl;
    return s;
}
