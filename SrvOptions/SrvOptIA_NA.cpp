/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptIA_NA.cpp,v 1.30 2008-10-12 20:16:14 thomson Exp $
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
#include "SrvOptRemoteID.h"
#include "SrvCfgOptions.h"
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
    while (pos<bufsize)
    {
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=24))
        {
            if(allowOptInOpt(parent->getType(),OPTION_IA_NA,code)) {
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

/**
 * This constructor is used to create IA option as an aswer to a SOLICIT, SOLICIT (RAPID_COMMIT) or REQUEST
 *
 * @param addrMgr
 * @param cfgMgr
 * @param queryOpt
 * @param clntDuid
 * @param clntAddr
 * @param iface
 * @param msgType
 * @param parent
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

    // --- check if client already has binding
    if (renew(queryOpt, false)) {
      Log(Info) << "Previous binding for client " << ClntDuid->getPlain() << ", IA(iaid="
                << queryOpt->getIAID() << ") found and renewed." << LogEnd;
      return;
    }


    // --- Is this IA without IAADDR options? ---
    if (!queryOpt->countAddrs()) {
      Log(Notice) << "IA option (with IAADDR suboptions missing) received. Assigning one address."
                  << LogEnd;


      // is there any specific address reserved for this client? (exception mechanism)
      SPtr<TIPv6Addr> hint = getExceptionAddr();
      if (!hint) {
	  hint = new TIPv6Addr();
      } else {
	  Log(Notice) << "Reserved address " << hint->getPlain() << " for this client found, trying to assign." << LogEnd;
      }
      SmartPtr<TSrvOptStatusCode> ptrStatus;
      if (this->assignAddr(hint, DHCPV6_INFINITY, DHCPV6_INFINITY, quiet))
      {
	  // include status code
	  ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					    "1 address granted. You may include IAADDR in IA option, if you want to provide a hint.",
					    this->Parent);
      } else {
	  ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,
					    "No more addresses available. Sorry.",
					    this->Parent);
      }
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
	case OPTION_IAADDR:
	{
	    optAddr = (Ptr*) opt;
	    hint    = optAddr->getAddr();

	    if (getExceptionAddr()) {
		SPtr<TIPv6Addr> cliHint = hint;
		hint = getExceptionAddr();
		Log(Info) << "Client requested " << cliHint->getPlain();
		Log(Cont) << ", but there is address reserved for this client: " << hint->getPlain()
			  << " (client's hint ignored)." << LogEnd;
	    }

	    if (willAssign) {
		// we've got free addrs left, assign one of them
		// always register this address as used by this client
		// (if this is solicit, this addr will be released later)
		unsigned long pref  = optAddr->getPref();
		unsigned long valid = optAddr->getValid();
		this->assignAddr(hint, pref, valid, quiet);
		willAssign--;
		addrsAssigned++;

	    } else {
		ok = false;
	    }
	    break;
	}
	case OPTION_STATUS_CODE:
	{
	    SmartPtr<TOptStatusCode> ptrStatus = (Ptr*) opt;
	    Log(Notice) << "Receviced STATUS_CODE code="
			<<  ptrStatus->getCode() << ", message=(" << ptrStatus->getText()
			<< ")" << LogEnd;
	    break;
	}
	default:
	{
	    Log(Warning) << "Invalid suboption (" << opt->getOptType()
			 << ") in an OPTION_IA_NA option received. Option ignored." << LogEnd;
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
	char buf[60];
	sprintf(buf, "%lu addr(s) requested, but assigned only %lu.",addrsRequested, addrsAssigned);
	if (addrsAssigned) {
	    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,buf, this->Parent);
	} else {
	    ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,buf, this->Parent);
	}

    }
    SubOptions.append((Ptr*)ptrStatus);

    // if this is a ADVERTISE message, release those addresses in TSrvMsgAdvertise::answer() method
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
    if (!addr) {
	Log(Warning) << "There are no more addresses available." << LogEnd;
	return 0;
    }
    ptrClass = this->CfgMgr->getClassByAddr(this->Iface, addr);
    pref = ptrClass->getPref(pref);
    valid= ptrClass->getValid(valid);
    optAddr = new TSrvOptIAAddress(addr, pref, valid, this->Parent);
    if (ptrClass->getAddrParams()) {
	Log(Debug) << "Experimental: addr-params subotion added." << LogEnd;
	optAddr->addOption((Ptr*)ptrClass->getAddrParams());
    }
    SubOptions.append((Ptr*)optAddr);

    Log(Info) << "Client requested " << *hint <<", got " << *addr
	      << " (IAID=" << this->IAID << ", pref=" << pref << ",valid=" << valid << ")." << LogEnd;

    // configure this IA
    this->T1= ptrClass->getT1(this->T1);
    this->T2= ptrClass->getT2(this->T2);

    // register this address as used by this client
    this->AddrMgr->addClntAddr(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID,
			       this->T1, this->T2, addr, pref, valid, quiet);
    this->CfgMgr->addClntAddr(this->Iface, addr);

    return optAddr;
}

/**
 * tries to find address reserved for this particular client
 *
 * @return
 */
SPtr<TIPv6Addr> TSrvOptIA_NA::getExceptionAddr()
{
    SmartPtr<TSrvCfgIface> ptrIface=CfgMgr->getIfaceByID(Iface);
    if (!ptrIface) {
	return 0;
    }

    SPtr<TSrvOptRemoteID> remoteID;


    TSrvMsg * par = dynamic_cast<TSrvMsg*>(Parent);
    if (par) {
	remoteID = par->getRemoteID();
    } else {
    }

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(ClntDuid, remoteID, false/* false = verbose */);

    if (ex)
	return ex->getAddr();

    return 0;
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
        this->renew(queryOpt, true);
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
		     << "). Cannot generate OPTION_IA_NA."<< LogEnd;
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL,
						"Unknown message type.",this->Parent));
        break;
    }
    }
}

/**
 * generate OPTION_IA_NA based on OPTION_IA_NA received in RENEW message
 *
 * @param queryOpt - IA_NA option in the RENEW message
 * @param addrCount
 *
 * @return true - if binding was renewed, false - if not found or invalid
 */
bool TSrvOptIA_NA::renew(SmartPtr<TSrvOptIA_NA> queryOpt, bool complainIfMissing)
{
    // find that client in addrdb
    SmartPtr <TAddrClient> ptrClient;
    ptrClient = this->AddrMgr->getClient(this->ClntDuid);
    if (!ptrClient) {
      if (complainIfMissing) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",
                                                this->Parent));
        Log(Info) << "Unable to RENEW binding for IA(iaid=" << queryOpt->getIAID() << ", client="
                  << ClntDuid->getPlain() << ": No such client." << LogEnd;
      }
      return false;
    }

    // find that IA
    SmartPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(this->IAID);
    if (!ptrIA) {
      if (complainIfMissing) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",
                                                this->Parent ));
        Log(Info) << "Unable to RENEW binding for IA(iaid=" << queryOpt->getIAID() << ", client="
                  << ClntDuid->getPlain() << ": No such IA." << LogEnd;
      }
      return false;
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
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Address(es) renewed. Greetings from planet Earth",this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );

    return true;
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
    if (!ptrIA) {
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
    SmartPtr<TSrvOptIA_NA> ia = queryOpt;
    SmartPtr<TOpt> subOpt;
    bool NotOnLink = false;

    ia->firstOption();
    while ( subOpt = ia->getOption() ) {
	if (subOpt->getOptType() != OPTION_IAADDR)
	    continue;

        SmartPtr<TSrvOptIAAddress> optAddr = (Ptr*)subOpt;

	//FIXME: proper check if the addresses are valid or not should be performed
        SmartPtr<TSrvCfgAddrClass> ptrClass;
        ptrClass = CfgMgr->getClassByAddr(this->Iface, optAddr->getAddr());
	if (!ptrClass)
	{
	    NotOnLink = true;
	    break;
	}

	// set IA Address suboptions and IA
        optAddr->setPref( ptrClass->getPref(DHCPV6_INFINITY) );
        optAddr->setValid( ptrClass->getValid(DHCPV6_INFINITY) );

        this->setT1( ptrClass->getT1(DHCPV6_INFINITY) );
        this->setT2( ptrClass->getT2(DHCPV6_INFINITY) );

        SmartPtr<TOptIAAddress> myOptAddr;
        myOptAddr = new TSrvOptIAAddress(optAddr->getAddr(), optAddr->getPref(),
                                       optAddr->getValid(),this->Parent);
        SubOptions.append( (Ptr*)myOptAddr );
    }


    if (NotOnLink)
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOTONLINK,
						"Those addresses are not valid on this link.",this->Parent ));

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

    // Getting out the request Message
    
    SmartPtr<TSrvTransMgr> srvTrans =  ((TSrvMsg*)Parent)->SrvTransMgr;
    SmartPtr<TSrvMsg> requestMsg =  (Ptr*)(srvTrans->requestMsg);

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

	// For Supportting Client Classify
	// Should get clientMessage, instead of Parent message

	// If the Class are valid and support the Client (based on duid, addr, clientclass)
	if ( ptrClass && ptrClass->clntSupported(ClntDuid, ClntAddr, requestMsg ))
	{
		// best case: address belongs to supported class, and is free
		if ( AddrMgr->addrIsFree(hint) ) {
		    Log(Debug) << "Requested address (" << *hint << ") is free, great!" << LogEnd;
		    return hint;
		}

		// medium case: addess belongs to supported class, but is used
		// however the class pool is still free

		if (ptrClass->getAssignedCount()>=ptrClass->getClassMaxLease()) {
		Log(Debug) << "Requested address (" << *hint
			   << ") belongs to supported class, which has reached its limit ("
			   << ptrClass->getAssignedCount() << " assigned, "
			   << ptrClass->getClassMaxLease() << " max lease)." << LogEnd;
		} else {
		    Log(Debug) << "Requested address (" << *hint
			       << ") belongs to supported class, but is used." << LogEnd;
		    do {
			addr = ptrClass->getRandomAddr();
		    } while (!AddrMgr->addrIsFree(addr));
		    return addr;
		}
	}// If the Class are valid and support the Client (based on duid, addr, clientclass)
    } //  if ( !invalidAddr )

    // do we have a cached address for that client?
    if (addr = AddrMgr->getCachedAddr(this->ClntDuid)) {
	if (this->CfgMgr->getClassByAddr(this->Iface, addr)) {
	    Log(Info) << "Cache: Cached address " << *addr << " found. Welcome back." << LogEnd;
	    if (AddrMgr->addrIsFree(addr))
		return addr;
	    Log(Info) << "Unfortunately, " << addr->getPlain() << " is used." << LogEnd;
	    AddrMgr->delCachedAddr(addr);
	} else {
	    Log(Warning) << "Cache: Cached address " << *addr << " found, but it is no longer valid." << LogEnd;
	    AddrMgr->delCachedAddr(addr);
	}// else
    }// if have cached address for that client

    // worst case: address does not belong to supported class
    // or specified hint is invalid
    SmartPtr<TSrvCfgAddrClass> ptrClass;


    ptrClass = ptrIface->getRandomClass(this->ClntDuid, this->ClntAddr);

    if (!ptrClass || 
	!ptrClass->clntSupported(ClntDuid, ClntAddr, requestMsg ) ||  
	(ptrClass->getAssignedCount()>=ptrClass->getClassMaxLease()) ) {
	// random class in invalid, let's try to find another one

	ptrIface->firstAddrClass();
	while (ptrClass = ptrIface->getAddrClass()) {
	    if (!ptrClass->clntSupported(ClntDuid, ClntAddr, requestMsg))
		continue;
	    if (ptrClass->getAssignedCount()>=ptrClass->getClassMaxLease())
		continue;
	    break;
	}
    }

    if (!ptrClass) {
	Log(Warning) << "Unable to find any suitable (allowed, non-full) class for this client." << LogEnd;
	return 0;
    }

    do {
	addr = ptrClass->getRandomAddr();
    } while (!AddrMgr->addrIsFree(hint));
    return addr;
}
