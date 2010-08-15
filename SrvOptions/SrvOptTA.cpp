/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptTA.cpp,v 1.8 2008-10-12 20:16:14 thomson Exp $
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
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"
#include "SrvTransMgr.h"

#define MAX_TA_RANDOM_TRIES 100

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

	SPtr<TOpt> opt;
	switch (code) {
	case OPTION_IAADDR:
	    opt = (Ptr*) SPtr<TSrvOptIAAddress>(new TSrvOptIAAddress(buf+pos,length,this->Parent));
	    SubOptions.append(opt);
	    break;
	case OPTION_STATUS_CODE:
	    opt = (Ptr*) SPtr<TSrvOptStatusCode>(new TSrvOptStatusCode(buf+pos,length,this->Parent));
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
}

/*
 * Constructor used in aswers to:
 * - SOLICIT
 * - SOLICIT (with RAPID_COMMIT)
 * - REQUEST
 */
TSrvOptTA::TSrvOptTA(SPtr<TSrvOptTA> queryOpt,
			   SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
			   int iface, int msgType, TMsg* parent)
    :TOptTA(queryOpt->getIAID(), parent) {
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;
    this->OrgMessage= msgType;
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
void TSrvOptTA::solicit(SPtr<TSrvOptTA> queryOpt) {
    this->solicitRequest(queryOpt, true);
}

void TSrvOptTA::solicitRequest(SPtr<TSrvOptTA> queryOpt, bool solicit) {

    // --- check address counts, how many we've got, how many assigned etc. ---
    unsigned long addrsAssigned  = 0; // already assigned
    unsigned long addrsAvail     = 0; // how many are allowed for client?
    unsigned long addrsMax       = 0; // clnt-max-lease
    unsigned long willAssign     = 1; // how many will be assigned? Just 1.

    addrsAssigned = SrvAddrMgr().getAddrCount(this->ClntDuid);
    addrsAvail    = SrvCfgMgr().countAvailAddrs(this->ClntDuid, this->ClntAddr, this->Iface);
    addrsMax      = SrvCfgMgr().getIfaceByID(this->Iface)->getClntMaxLease();

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

    Log(Info) << "Client has " << addrsAssigned << " addrs, " << addrsAvail
	      << " is available, limit for client is " << addrsMax << ", "
	      << willAssign << " will be assigned." << LogEnd;
    if (!willAssign) {
	SubOptions.append( (Ptr*) new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,
							"Sorry, buddy. No temporary addresses for you", this->Parent) );
	Log(Warning) << "No temporary addresses were assigned in TA (iaid="<< this->IAID << ")." << LogEnd;
	return;
    }

    // --- ok, let's assign those damn addresses ---
    SPtr<TSrvOptIAAddress> optAddr;

    optAddr = this->assignAddr();
    if (!optAddr) {
	Log(Error) << "No temporary address found. Server is NOT configured with TA option." << LogEnd;
	SPtr<TSrvOptStatusCode> ptrStatus;
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,
					  "Server support for temporary addresses is not enabled. Sorry buddy.",this->Parent);
        this->SubOptions.append((Ptr*)ptrStatus);
	return;
    }
    SubOptions.append((Ptr*) optAddr);

    // those addresses will be released in the TSrvMsgAdvertise::answer() method
}

void TSrvOptTA::request(SPtr<TSrvOptTA> queryOpt) {
    this->solicitRequest(queryOpt, false);
}

void TSrvOptTA::release(SPtr<TSrvOptTA> queryOpt) {
    Log(Crit) << "### Implement this: TA in RELEASE msg." << LogEnd;
}

void TSrvOptTA::confirm(SPtr<TSrvOptTA> queryOpt) {
    Log(Crit) << "### Implement this: TA in CONFIRM msg." << LogEnd;
}

void TSrvOptTA::releaseAllAddrs(bool quiet) {
    SPtr<TOpt> opt;
    SPtr<TIPv6Addr> addr;
    SPtr<TOptIAAddress> optAddr;
    this->firstOption();
    while ( opt = this->getOption() ) {
	if (opt->getOptType() != OPTION_IAADDR)
	    continue;
	optAddr = (Ptr*) opt;
	addr = optAddr->getAddr();
	SrvAddrMgr().delClntAddr(this->ClntDuid, this->IAID, addr, quiet);
	SrvCfgMgr().delClntAddr(this->Iface, addr);
    }
}

/**
 * this method finds a temp. address for this client, marks it as used and then
 * creates IAAADDR option containing this option.
 *
 *
 * @return
 */
SPtr<TSrvOptIAAddress> TSrvOptTA::assignAddr() {
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = SrvCfgMgr().getIfaceByID(this->Iface);
    if (!ptrIface) {
	Log(Error) << "Trying to find free address on non-existent interface (id=%d)\n"
		   << this->Iface << LogEnd;
	return 0; // NULL
    }

    SPtr<TSrvCfgTA> ta;
    ptrIface->firstTA();

    SPtr<TSrvMsg> requestMsg =  (Ptr*)SrvTransMgr().getCurrentRequest();

    while ( ta = ptrIface->getTA())
    {
    	if (!ta->clntSupported(ClntDuid, ClntAddr, requestMsg ))
    		continue;
    	break;
    }

    if (!ta)
    {
    	Log(Warning) << "Unable to find any suitable (allowed,non-full) TA for this client." << LogEnd;
    	return 0;
    }
    SPtr<TIPv6Addr> addr;
    int safety=0;

    while (safety<MAX_TA_RANDOM_TRIES) {
	addr = ta->getRandomAddr();
	if (SrvAddrMgr().taAddrIsFree(addr)) {
	    if ((this->OrgMessage == REQUEST_MSG)) {
		Log(Debug) << "Temporary address " << addr->getPlain() << " granted." << LogEnd;
		SrvAddrMgr().addTAAddr(this->ClntDuid, this->ClntAddr, this->Iface,
				   this->IAID, addr, ta->getPref(), ta->getValid());
		SrvCfgMgr().addTAAddr(this->Iface);
	    } else {
		Log(Debug) << "Temporary address " << addr->getPlain() << " generated (not granted)." << LogEnd;
	    }
	    return new TSrvOptIAAddress(addr, ta->getPref(), ta->getValid(), this->Parent);

	}
	safety++;
    }
    Log(Error) << "Unable to randomly choose address after " << MAX_TA_RANDOM_TRIES << " tries." << LogEnd;
    return 0;
}

bool TSrvOptTA::doDuties()
{
    return true;
}
