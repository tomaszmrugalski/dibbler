/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvOptIA_NA.cpp,v 1.9 2004-07-05 23:04:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.7  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 * Revision 1.6  2004/06/21 23:08:49  thomson
 * Minor fixes.
 *
 * Revision 1.5  2004/06/20 21:00:45  thomson
 * Various fixes.
 *
 * Revision 1.4  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.3  2004/06/17 23:53:55  thomson
 * Server Address Assignment rewritten.
 *
 * Revision 1.2  2004/06/06 22:31:44  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "SrvOptIA_NA.h"
#include "SrvOptIAAddress.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"

TSrvOptIA_NA::TSrvOptIA_NA( long IAID, long T1, long T2, TMsg* parent)
    :TOptIA_NA(IAID,T1,T2, parent) {
    
}

TSrvOptIA_NA::TSrvOptIA_NA( long IAID, long T1, long T2, int Code, string Text, TMsg* parent)
    :TOptIA_NA(IAID,T1,T2, parent) {
    SubOptions.append(new TSrvOptStatusCode(Code, Text, parent));
}

/*
 * Create IA_NA option based on receive buffer
 */
TSrvOptIA_NA::TSrvOptIA_NA( char * buf, int bufsize, TMsg* parent)
    :TOptIA_NA(buf,bufsize, parent) {
    int pos=0;
    while(pos<bufsize) 
    {
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(canBeOptInOpt(parent->getType(),OPTION_IA,code)) {
                SmartPtr<TOpt> opt;
		opt = SmartPtr<TOpt>(); /* NULL */
                switch (code)
                {
                case OPTION_IAADDR:
                    opt = (Ptr*)SmartPtr<TSrvOptIAAddress> 
			(new TSrvOptIAAddress(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    opt = (Ptr*)SmartPtr<TSrvOptStatusCode> 
			(new TSrvOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
		    Log(Warning) <<"Option " << code<< "not supported "
				 <<" in  message (type=" << parent->getType() 
				 <<") in this version of server." << LogEnd;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            }
            else {
		Log(Warning) << "Illegal option received (type=" << code 
			     << ") in an IA_NA option." << LogEnd;
	    }
        }
        else {
	    Log(Warning) << "Unknown option received (type=" << code
			 << ") in an IA_NA option." << LogEnd;
        };
        pos+=length;
    }
}

/*
 * Constructor used in aswers to:
 * - SOLICIT 
 * - SOLICIT (with RAPID_COMMIT)
 * - REQUEST
 */
TSrvOptIA_NA::TSrvOptIA_NA(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
			   SmartPtr<TSrvOptIA_NA> queryOpt,
			   SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
			   int iface, int msgType, TMsg* parent)
    :TOptIA_NA(queryOpt->getIAID(), DHCPV6_INFINITY, DHCPV6_INFINITY, parent) {
    
    this->AddrMgr   = addrMgr;
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;

    // FIXME: SOLICIT without RAPID COMMIT should set this to true
    bool quiet = false;

    // --- Is this IA without IAADDR options? ---
    if (!queryOpt->countAddrs()) {
	Log(Warning) << "IA option (with IAADDR suboptions missing) received. Assigning one address."
		     << LogEnd;
	
	SmartPtr<TIPv6Addr> anyaddr;
	assignAddr(anyaddr, DHCPV6_INFINITY, DHCPV6_INFINITY, quiet);
	
       	// include status code
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					  "1 addr granted. Next time include "
					  "IAADDR in IA option, please.",this->Parent);
        this->SubOptions.append((Ptr*)ptrStatus);
	
	return;
    }

    // --- check address counts, how many we've got, how many assigned etc. ---
    unsigned long addrsAssigned  = 0; // already assigned
    unsigned long addrsRequested = 0; // how many requested in this IA
    unsigned long addrsAvail     = 0; // how many are allowed for client?
    unsigned long addrsMax       = 0; // clnt-max-lease
    unsigned long willAssign     = 0; // how many will be assigned?

    addrsAssigned = addrMgr->getAddrCount(clntDuid);
    addrsRequested= queryOpt->countAddrs();
    addrsAvail    = cfgMgr->countAvailAddrs(clntDuid, clntAddr, iface);
    addrsMax      = cfgMgr->getIfaceByID(iface)->getClntMaxLease();

    willAssign = addrsRequested;

    if (willAssign > addrsMax - addrsAssigned) {
	Log(Notice) << "Client got " << addrsAssigned << " and requested " 
		    << addrsRequested << " more, but limit for a client is "
		    << addrsMax << LogEnd;
	willAssign = addrsMax - addrsAssigned;
    }

    if (willAssign > addrsAvail) {
	Log(Notice) << willAssign << " addrs would be assigned, but only" << addrsAssigned
		    << " is available." << LogEnd;
	willAssign = addrsAvail;
    }

    Log(Info) << "Client has " << addrsAssigned << " addrs, asks for " 
	      << addrsRequested << ", " << addrsAvail << " is available, limit for client is "
	      << addrsMax << ", " << willAssign << " will be assigned." << LogEnd;

    // --- ok, let's assign those damn addresses ---
    SmartPtr<TOpt> opt;
    SmartPtr<TIPv6Addr> hint;
    SmartPtr<TOptIAAddress> optAddr;
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    bool ok=true;
    
    queryOpt->firstOption();
    while ( opt = queryOpt->getOption() ) {
	switch ( opt->getOptType() ) {
	case OPTION_IAADDR: {
	    optAddr = (Ptr*) opt;
	    hint    = optAddr->getAddr();
	    
	    if (willAssign) {
		// we've got free addrs left, assign one of them
		// always register this address as used by this client
		// (if this is solicit, this addr will be released later)
		this->assignAddr(hint, optAddr->getPref(), optAddr->getValid(), quiet);
		willAssign--;
		addrsAssigned++;

	    } else {
		ok = false;
	    }
	    break;
	}
	case OPTION_STATUS_CODE: {
	    SmartPtr<TOptStatusCode> ptrStatus = (Ptr*) opt;
	    Log(Notice) << "Receviced STATUS_CODE code=" 
			<<  ptrStatus->getCode() << ", message=(" << ptrStatus->getText()
			<< ")" << LogEnd;
	    break;
	}
	default: {
	    Log(Warning) << "Invalid suboption (" << opt->getOptType() 
			 << ") in an OPTION_IA option received. Option ignored." << LogEnd;
	    break;
	}
	}
    }

    // --- now include STATUS CODE ---
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    if (ok) {
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					  "All addresses were assigned.",this->Parent);
	// FIXME: if this is solicit, place "all addrs would be assigned."
    } else {
	string tmp = addrsRequested+" addrs requested, but assigned only "+addrsAssigned;
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,tmp.c_str(), this->Parent);
    }
    SubOptions.append((Ptr*)ptrStatus);

    // --- release addresses if this reply for SOLICIT) ---
}

void TSrvOptIA_NA::releaseAllAddrs(bool quiet) {
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

SmartPtr<TSrvOptIAAddress> TSrvOptIA_NA::assignAddr(SmartPtr<TIPv6Addr> hint, unsigned long pref,
						    unsigned long valid,
						    bool quiet) {

    // Assign one address
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<TSrvOptIAAddress> optAddr;
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    
    // get address
    addr = this->getFreeAddr(hint);
    ptrClass = this->CfgMgr->getClassByAddr(this->Iface, addr);
    pref = ptrClass->getPref(pref);
    valid= ptrClass->getPref(valid);
    optAddr = new TSrvOptIAAddress(addr, pref, valid, this->Parent);
    SubOptions.append((Ptr*)optAddr);

    Log(Info) << "Client requested " << *hint <<", got " << *addr 
	      << " (IAID= " << this->IAID << ")" << LogEnd;

    // configure this IA
    this->T1= ptrClass->getT1(this->T1);
    this->T2= ptrClass->getT2(this->T2);
    
    // register this address as used by this client
    this->AddrMgr->addClntAddr(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID, 
			       this->T1, this->T2, addr, pref, valid, quiet);
    this->CfgMgr->addClntAddr(this->Iface, addr);
    
    return optAddr;
}


// constructor used only in RENEW, REBIND, DECLINE and RELEASE
TSrvOptIA_NA::TSrvOptIA_NA( SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr, SmartPtr<TDUID> clntDuid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent)
    :TOptIA_NA(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    this->AddrMgr   = addrMgr;
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;

    this->IAID = queryOpt->getIAID();

    switch (msgType) {
    case SOLICIT_MSG:
        //this->solicit(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID,iface, addrCount);
        break;
    case REQUEST_MSG:
        //this->request(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    
    case RENEW_MSG:
        this->renew(queryOpt, addrCount);
        break;
    case REBIND_MSG:
        this->rebind(queryOpt, addrCount);
        break;
    case RELEASE_MSG:
        this->release(queryOpt, addrCount);
        break;
    case CONFIRM_MSG:
        this->confirm(queryOpt, addrCount);
        break;
    case DECLINE_MSG:
        this->decline(queryOpt, addrCount);
        break;
    default: {
	Log(Warning) << "Unknown message type (" << msgType 
		     << "). Cannot generate OPTION_IA."<< LogEnd;
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL,
						"Unknown message type.",this->Parent));
        break;
    }
    }
}

// generate OPTION_IA based on OPTION_IA received in RENEW message
void TSrvOptIA_NA::renew(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount)
{
    // find that client in addrdb
    SmartPtr <TAddrClient> ptrClient;
    ptrClient = this->AddrMgr->getClient(this->ClntDuid);
    if (!ptrClient) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",
						this->Parent));
        return;
    }

    // find that IA
    SmartPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(this->IAID);
    if (!ptrIA) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",
						this->Parent ));
        return;
    }

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    this->T1 = ptrIA->getT1();
    this->T2 = ptrIA->getT2();

    // send addr info to client
    SmartPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SmartPtr<TOptIAAddress> optAddr;
        ptrAddr->setTimestamp();
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),ptrAddr->getValid(),
				       this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Greetings from planet Earth",this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_NA::rebind(SmartPtr<TSrvOptIA_NA> queryOpt,
                          unsigned long &addrCount)
{
    // find that client in addrdb
    SmartPtr <TAddrClient> ptrClient;
    ptrClient = this->AddrMgr->getClient(this->ClntDuid);
    if (!ptrClient) {
        // hmmm, that's not our client
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,
						"Who are you? Do I know you?",this->Parent ));
        return;
    }

    // find that IA
    SmartPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(this->IAID);
    if (!ptrClient) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,
						"I see this IAID first time.",this->Parent ));
        return;
    }

    // FIXME: 18.2.4 par. 3 (check if addrs are appropriate for this link)

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    this->T1 = ptrIA->getT1();
    this->T2 = ptrIA->getT2();

    // send addr info to client
    SmartPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SmartPtr<TOptIAAddress> optAddr;
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),
				       ptrAddr->getValid(),this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Greetings from planet Earth",
				      this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_NA::release(SmartPtr<TSrvOptIA_NA> queryOpt,
                           unsigned long &addrCount)
{
}

void TSrvOptIA_NA::confirm(SmartPtr<TSrvOptIA_NA> queryOpt,
                           unsigned long &addrCount)
{
}

void TSrvOptIA_NA::decline(SmartPtr<TSrvOptIA_NA> queryOpt,
                           unsigned long &addrCount)
{
}

bool TSrvOptIA_NA::doDuties()
{
    return true;
}

/*
 * gets free address for a client
 */
SmartPtr<TIPv6Addr> TSrvOptIA_NA::getFreeAddr(SmartPtr<TIPv6Addr> hint) {
    
    bool invalidAddr = false;
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TIPv6Addr>    addr;
    ptrIface = this->CfgMgr->getIfaceByID(this->Iface);
    if (!ptrIface) {
	Log(Error) << "Trying to find free address on non-existent interface (id=%d)\n" 
		   << this->Iface << LogEnd;
	return 0; // NULL
    }

    // check if this address is ok

    // is it anyaddress (::)?
    SmartPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
    if (*anyaddr==*hint) {
	Log(Debug) << "Client requested unspecified (" << *hint 
		   << ") address. Hint ignored." << LogEnd;
	invalidAddr = true;
    }

    // is it multicast address (ff...)?
    if ((*(hint->getAddr()))==0xff) {
	Log(Debug) << "Client requested multicast (" << *hint 
		   << ") address. Hint ignored." << LogEnd;
	invalidAddr = true;
    }

    // is it link-local address (fe80::...)?
    char linklocal[]={0xfe, 0x80};
    if (!memcmp(hint->getAddr(),linklocal,2)) {
	Log(Debug) << "Client requested link-local (" << *hint << ") address. Hint ignored." << LogEnd;
	invalidAddr = true;
    }

    if ( !invalidAddr ) {
	// hint is valid, try to use it

	SmartPtr<TSrvCfgAddrClass> ptrClass;
	ptrClass = this->CfgMgr->getClassByAddr(this->Iface, hint);

	// best case: address belongs to supported class, and is free
	if ( ptrClass && AddrMgr->addrIsFree(hint) ) {
	    Log(Debug) << "Requested address (" << *hint << ") is free, great!" << LogEnd;
	    return hint;
	} 

	// medium case: addess belongs to supported class, but is used
	if ( ptrClass ) {
	    Log(Debug) << "Requested address (" << *hint << ") is used, found." << LogEnd;
	    // FIXME: loops if all addrs are used
	    do {
		addr = ptrClass->getRandomAddr();
	    } while (!AddrMgr->addrIsFree(hint));
	    return addr;
	}
    }

    // worst case: address does not belong to supported class
    // or specified hint is invalid

    SmartPtr<TSrvCfgAddrClass> ptrClass;
    ptrClass = ptrIface->getRandomClass(this->ClntDuid, this->ClntAddr);
    if (!ptrClass) {
	ptrIface->firstAddrClass();
	ptrClass = ptrIface->getAddrClass();
    }
    
    do {
	addr = ptrClass->getRandomAddr();
    } while (!AddrMgr->addrIsFree(hint));
    return addr;
}
