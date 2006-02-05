/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptTA.cpp,v 1.1.2.1 2006-02-05 23:41:13 thomson Exp $
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "SrvOptTA.h"
#include "SrvOptIAAddress.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"

/*
 * Create IA_TA option based on receive buffer
 */
TSrvOptTA::TSrvOptTA( char * buf, int bufsize, TMsg* parent)
    :TOptTA(buf,bufsize, parent) {
    int pos=0;
    while(pos<bufsize) {
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;

	SmartPtr<TOpt> opt;
	switch (code) {
	case OPTION_IAADDR:
	    opt = (Ptr*) new TSrvOptIAAddress(buf+pos,length,this->Parent);
	    SubOptions.append(opt);
	    break;
	case OPTION_STATUS_CODE:
	    opt = (Ptr*) new TSrvOptStatusCode(buf+pos,length,this->Parent);
	    SubOptions.append(opt);
	    break;
	default:
	    Log(Warning) <<"Option " << code << "not supported "
			 <<" in message (type=" << parent->getType() 
			 <<") in this version of server." << LogEnd;
	    break;
	}
        pos+=length;
    }

    Log(Debug) << "### TA option (iaid=" << this->IAID << ") received." << LogEnd;
}

/*
 * Constructor used in aswers to:
 * - SOLICIT 
 * - SOLICIT (with RAPID_COMMIT)
 * - REQUEST
 */
TSrvOptTA::TSrvOptTA(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
			   SmartPtr<TSrvOptTA> queryOpt,
			   SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
			   int iface, int msgType, TMsg* parent)
    :TOptTA(queryOpt->getIAID(), parent) {
    
    this->AddrMgr   = addrMgr;
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;

    switch (msgType) {
    case SOLICIT_MSG:
	this->solicit(queryOpt);
	break;
    case REQUEST_MSG:
	this->request(queryOpt);
	break;
    case RELEASE_MSG:
	this->release(queryOpt);
	break;
    case CONFIRM_MSG:
	this->confirm(queryOpt);
	break;
    default:
	Log(Warning) << "Option TA in not supported in message " << MsgTypeToString(msgType) << "." << LogEnd;
	return;
    }
}

TSrvOptTA::TSrvOptTA(int iaid, int statusCode, string txt, TMsg* parent)
    :TOptTA(iaid, parent) {
    SubOptions.append(new TSrvOptStatusCode(statusCode, txt, parent));
}

/** 
 * used in response to SOLICIT message
 * 
 * @param queryOpt 
 */
void TSrvOptTA::solicit(SmartPtr<TSrvOptTA> queryOpt) {
    this->solicitRequest(queryOpt, true);
}

void TSrvOptTA::solicitRequest(SmartPtr<TSrvOptTA> queryOpt, bool solicit) {

    // --- check address counts, how many we've got, how many assigned etc. ---
    unsigned long addrsAssigned  = 0; // already assigned
    unsigned long addrsAvail     = 0; // how many are allowed for client?
    unsigned long addrsMax       = 0; // clnt-max-lease
    unsigned long willAssign     = 1; // how many will be assigned? Just 1.

    addrsAssigned = AddrMgr->getAddrCount(this->ClntDuid);
    addrsAvail    = CfgMgr->countAvailAddrs(this->ClntDuid, this->ClntAddr, this->Iface);
    addrsMax      = CfgMgr->getIfaceByID(this->Iface)->getClntMaxLease();

    if (willAssign > addrsMax - addrsAssigned) {
	Log(Notice) << "Client got " << addrsAssigned << " and requested " 
		    << willAssign << " more, but limit for a client is "
		    << addrsMax << LogEnd;
	willAssign = addrsMax - addrsAssigned;
    }

    if (willAssign > addrsAvail) {
	Log(Notice) << willAssign << " addrs " << (solicit?"would":"will") << " be assigned, but only" 
		    << addrsAssigned << " is available." << LogEnd;
	willAssign = addrsAvail;
    }

    Log(Info) << "Client has " << addrsAssigned << " addrs," << ", " << addrsAvail 
	      << " is available, limit for client is " << addrsMax << ", " 
	      << willAssign << " will be assigned." << LogEnd;
    if (!willAssign) {
	SubOptions.append( (Ptr*) new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL, 
							"Sorry, buddy. No temporary addresses for you", this->Parent) );
	Log(Warning) << "No temporary addresses were assigned in TA (iaid="<< this->IAID << ")." << LogEnd;
	return;
    } 

    // --- ok, let's assign those damn addresses ---
    SmartPtr<TSrvOptIAAddress> optAddr;

    optAddr = this->assignAddr();
    if (!optAddr) {
	Log(Error) << "No temporary address found. Probably bug in the server logic." << LogEnd;
	return;
    }
    SubOptions.append((Ptr*) optAddr);

    // those addresses will be released in the TSrvMsgAdvertise::answer() method
}

void TSrvOptTA::request(SmartPtr<TSrvOptTA> queryOpt) {
    this->solicitRequest(queryOpt, false);
}

void TSrvOptTA::release(SmartPtr<TSrvOptTA> queryOpt) {
    Log(Crit) << "### Implement this: TA in RELEASE msg." << LogEnd;
}

void TSrvOptTA::confirm(SmartPtr<TSrvOptTA> queryOpt) {
    Log(Crit) << "### Implement this: TA in CONFIRM msg." << LogEnd;
}

void TSrvOptTA::releaseAllAddrs(bool quiet) {
    SmartPtr<TOpt> opt;
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<TOptIAAddress> optAddr;
    this->firstOption();
    while ( opt = this->getOption() ) {
	if (opt->getOptType() != OPTION_IAADDR)
	    continue;
	optAddr = (Ptr*) opt;
	addr = optAddr->getAddr();
	this->AddrMgr->delClntAddr(this->ClntDuid, this->IAID, addr, quiet);
	this->CfgMgr->delClntAddr(this->Iface, addr);
    }
}

/** 
 * this method finds a temp. address for this client, marks it as used and then
 * creates IAAADDR option containing this option.
 * 
 * 
 * @return 
 */
SmartPtr<TSrvOptIAAddress> TSrvOptTA::assignAddr() {
    SmartPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->CfgMgr->getIfaceByID(this->Iface);
    if (!ptrIface) {
	Log(Error) << "Trying to find free address on non-existent interface (id=%d)\n" 
		   << this->Iface << LogEnd;
	return 0; // NULL
    }

    SmartPtr<TSrvCfgTA> ta;
    ptrIface->firstTA();
    ta = ptrIface->getTA();
    if (!ta) {
	Log(Warning) << "TA option (temporary addresses) is not configured on the " 
		     << ptrIface->getFullName() << LogEnd;
    }

    SmartPtr<TIPv6Addr> addr;
    int safety=0;

    while (safety<100) {
	addr = ta->getRandomAddr();
	Log(Debug) << "Temporary address " << addr->getPlain() << " generated." << LogEnd;
	if (AddrMgr->taAddrIsFree(addr)) {
	    AddrMgr->addTAAddr(this->ClntDuid, this->ClntAddr, this->Iface, 
			       this->IAID, addr, ta->getPref(), ta->getValid());
	    CfgMgr->addTAAddr(this->Iface);
	    return new TSrvOptIAAddress(addr, ta->getPref(), ta->getValid(), this->Parent);

	}
	safety++;
    }
    Log(Error) << "Unable to randomly choose address after 100 tries." << LogEnd;
    return 0;
}



bool TSrvOptTA::doDuties()
{
    return true;
}


/*
 * $Log: not supported by cvs2svn $
 */
