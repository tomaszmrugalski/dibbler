/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                          
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "SrvOptIA_PD.h"
#include "SrvOptIAPrefix.h" 
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"

TSrvOptIA_PD::TSrvOptIA_PD( long IAID, long T1, long T2, TMsg* parent)
    :TOptIA_PD(IAID,T1,T2, parent) {
    this->IAID=IAID;
    this->T1=T1;
    this->T2=T2;
}

TSrvOptIA_PD::TSrvOptIA_PD( long IAID, long T1, long T2, int Code, string Text, TMsg* parent)
    :TOptIA_PD(IAID,T1,T2, parent) {
    SubOptions.append(new TSrvOptStatusCode(Code, Text, parent));
}

/*
 * Create IA_PD option based on receive buffer
 */
TSrvOptIA_PD::TSrvOptIA_PD( char * buf, int bufsize, TMsg* parent)
    :TOptIA_PD(buf,bufsize, parent) {
    int pos=0;


	
    while(pos<bufsize) 
    {
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=26)) // was 24
        { 
	             
            if(allowOptInOpt(parent->getType(),OPTION_IA_PD,code)) {
                
		SmartPtr<TOpt> opt;
		opt = SmartPtr<TOpt>(); /* NULL */
                switch (code)
                {
                case OPTION_IAPREFIX:
		    opt = (Ptr*)SmartPtr<TSrvOptIAPrefix> 
			(new TSrvOptIAPrefix(buf+pos,length,this->Parent));
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
			     << ") in an IA_PD option." << LogEnd;
	    }
        }
        else {
	    Log(Warning) << "Unknown option received (type=" << code
			 << ") in an IA_PD option." << LogEnd;
        };
        pos+=length;
    }
	
}

/** 
 * This constructor is used to create PD option as an aswer to a SOLICIT, SOLICIT (RAPID_COMMIT) or REQUEST
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
TSrvOptIA_PD::TSrvOptIA_PD(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
			   SmartPtr<TSrvOptIA_PD> queryOpt,
			   SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
			   int iface, int msgType, TMsg* parent)
    :TOptIA_PD(queryOpt->getIAID(), queryOpt->getT1(), queryOpt->getT2(), parent) {
    
    this->AddrMgr   = addrMgr; // in next version this should be PrefixMgr // TODO 
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;
	
    bool fake  = false; // is this assignment for real?

    if (msgType == SOLICIT_MSG)    // FIXME: SOLICIT without RAPID COMMIT should set this to true
	fake = true;

    // --- Is this PD without IAPREFIX options? ---
    if (!queryOpt->countPrefixes()) {
	Log(Warning) << "PD option (with IAPREFIX suboptions missing) received. Assigning one prefix."
		     << LogEnd;
	
	SmartPtr<TIPv6Addr> anyaddr = new TIPv6Addr(); 
	this->assignPrefix(anyaddr,64 ,DHCPV6_INFINITY, DHCPV6_INFINITY, fake); // this should be checked 
	
       	// include status code
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					  "1 prefix granted. Next time you might consider including "
					  "IAPREFIX in IA_PD option.",this->Parent);
        this->SubOptions.append((Ptr*)ptrStatus);

	return;
    }
	
    // --- check prefixes counts, how many we've got, how many assigned etc. ---
    unsigned long prefixesAssigned  = 0; // already assigned
    unsigned long prefixesRequested = 0; // how many requested in this IA
    unsigned long prefixesAvail     = 0; // how many are allowed for client?
    unsigned long prefixesMax       = 0; // clnt-max-lease
    unsigned long willAssign     = 0; // how many will be assigned?

    prefixesAssigned = 0;/*addrMgr->getAddrCount(clntDuid);*/
    prefixesRequested= 0; /*queryOpt->countAddrs();*/
    prefixesAvail    = cfgMgr->countAvailAddrs(clntDuid, clntAddr, iface);
    prefixesMax      = cfgMgr->getIfaceByID(iface)->getClntMaxLease();
    willAssign = prefixesRequested;

    if (willAssign > prefixesMax - prefixesAssigned) {
	Log(Notice) << "Client got " << prefixesAssigned << " and requested " 
		    << prefixesRequested << " more, but limit for a client is "
		    << prefixesMax << LogEnd;
	willAssign = prefixesMax - prefixesAssigned;
    }

    if (willAssign > prefixesAvail) {
	Log(Notice) << willAssign << " addrs would be assigned, but only" << prefixesAssigned
		    << " is available." << LogEnd;
	willAssign = prefixesAvail;
    }

    Log(Info) << "Client has " << prefixesAssigned << " addrs, asks for " 
	      << prefixesRequested << ", " << prefixesAvail << " is available, limit for client is "
	      << prefixesMax << ", " << willAssign << " will be assigned." << LogEnd;
	
    // --- ok, let's assign those damn addresses ---
   /* SmartPtr<TOpt> opt;
    SmartPtr<TIPv6Addr> hint = new TIPv6Addr(addr, false);
    SmartPtr<TOptIAPrefix> optPrefix; // was TOptIAAddress   and optAddr
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    bool ok=true; */
   // queryOpt->firstOption();
   // while ( opt = queryOpt->getOption() ) {
//	switch ( opt->getOptType() ) {
//	case OPTION_IAPREFIX: {
//	    optPrefix = (Ptr*) opt;
//	    hint    = optPrefix->getPrefix();
//	    cout << hint;
	    //if (willAssign) { // was commented 
		// we've got free addrs left, assign one of them
		// always register this address as used by this client
		// (if this is solicit, this addr will be released later)
		//unsigned long pref  = optPrefix->getPref();
		//unsigned long valid = optPrefix->getValid();
		//this->assignPrefix(hint, 64,40000, 50000, quiet);
	//this->assignPrefix(hint, 64,pref, valid, quiet);
		
	//willAssign--;
		//prefixesAssigned++;

	    //} else {               WAS UNCOMMENTED 
	//	ok = false;
	 //   }
	    //break; WAS UNCOMENTED ALSO 
	/*} WAS ALSO UNCOMMENTED 
	case OPTION_STATUS_CODE: {
	    SmartPtr<TOptStatusCode> ptrStatus = (Ptr*) opt;
	    Log(Notice) << "Receviced STATUS_CODE code=" 
			<<  ptrStatus->getCode() << ", message=(" << ptrStatus->getText()
			<< ")" << LogEnd;
	    break;
	}
	default: {
	    Log(Warning) << "Invalid suboption (" << opt->getOptType() 
			 << ") in an OPTION_PD option received. Option ignored." << LogEnd;
	    break;
	}
	}
    }*/

    // --- now include STATUS CODE ---
    /*
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    if (ok) {
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					  "All prefixes were assigned.",this->Parent);
	// FIXME: if this is solicit, place "all addrs would be assigned."
    } else {
	string tmp = prefixesRequested+" prefixes requested, but assigned only "+prefixesAssigned;
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOPREFAVAIL,tmp.c_str(), this->Parent);
    }
    SubOptions.append((Ptr*)ptrStatus);
	*/ 
    // if this is a ADVERTISE message, release those addresses in TSrvMsgAdvertise::answer() method
}

void TSrvOptIA_PD::releaseAllPrefixes(bool quiet) {
    SmartPtr<TOpt> opt;
    SmartPtr<TIPv6Addr> prefix;
    SmartPtr<TOptIAPrefix> optPrefix;
    this->firstOption();
    while ( opt = this->getOption() ) {
	if (opt->getOptType() != OPTION_IAPREFIX)
	    continue;
	optPrefix = (Ptr*) opt;
	prefix = optPrefix->getPrefix();
	this->AddrMgr->delClntAddr(this->ClntDuid, this->IAID, prefix, quiet); // not sure about that 
	this->CfgMgr->delClntAddr(this->Iface, prefix);
    }
}

SmartPtr<TSrvOptIAPrefix> TSrvOptIA_PD::assignPrefix(SmartPtr<TIPv6Addr> hint,int length, unsigned long pref,
						    unsigned long valid,
						    bool fake) {
	
    // Assign one prefix
    SmartPtr<TIPv6Addr> prefix;
    SmartPtr<TSrvOptIAPrefix> optPrefix;
    SmartPtr<TSrvCfgPD> ptrPD;
    
    // get address
    prefix = this->getFreePrefix(hint);
    ptrPD = this->CfgMgr->getClassByPrefix(this->Iface, prefix);
    Log(Debug) << "Assigned prefix: "<< prefix->getPlain() << LogEnd;
    //pref = ptrClass->getPref(pref);
    //valid= ptrClass->getValid(valid);
   pref = 60000;
   valid = 120000;
	 //length=ptrClass->getLength(length);

   // remember assigned prefix in addrMgr
   if (!fake)
       AddrMgr->addPrefix(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID, this->T1, this->T2,
			  prefix, pref, valid, length, false);

   optPrefix = new TSrvOptIAPrefix( prefix,length, pref, valid, this->Parent); // 64 is a prefix length
   SubOptions.append((Ptr*)optPrefix);
   
   Log(Info) << "Client requested " << *hint <<", got " << *prefix 
	     << " (PDID=" << this->IAID << ", pref=" << pref << ",valid=" << valid << " length of prefix is "<< length << LogEnd;
   


    // configure this PD
    this->T1= ptrPD->getPD_T1(this->T1);
    this->T2= ptrPD->getPD_T2(this->T2);
    
    // register this address as used by this client
    //this->AddrMgr->addClntAddr(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID, // not sure about that 
			      // this->T1, this->T2, prefix, pref, valid, quiet); // we need to change this also 
    //this->CfgMgr->addClntAddr(this->Iface, prefix);
    
    return optPrefix;
}

// so far it is enought here 
// constructor used only in RENEW, REBIND, DECLINE and RELEASE
TSrvOptIA_PD::TSrvOptIA_PD( SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_PD> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr, SmartPtr<TDUID> clntDuid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent)
    :TOptIA_PD(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    this->AddrMgr   = addrMgr;
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;

    this->IAID = queryOpt->getIAID();

    switch (msgType) {
    case SOLICIT_MSG:
        //this->solicit(cfgMgr, addrMgr, queryOpt, clntAddr, clntDuid,iface, addrCount);
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
		     << "). Cannot generate OPTION_PD."<< LogEnd;
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL,
						"Unknown message type.",this->Parent));
        break;
    }
    }
}

// 
/** 
 * generate OPTION_PD based on OPTION_PD received in RENEW message
 * 
 * @param queryOpt - IA_NA option in the RENEW message
 * @param addrCount 
 */
void TSrvOptIA_PD::renew(SmartPtr<TSrvOptIA_PD> queryOpt, unsigned long &addrCount)
{
    // find that client in addrdb
   /* SmartPtr <TAddrClient> ptrClient;
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
    SubOptions.append( (Ptr*)ptrStatus );*/
}

void TSrvOptIA_PD::rebind(SmartPtr<TSrvOptIA_PD> queryOpt,
                          unsigned long &addrCount)
{
    // find that client in addrdb
  /*  SmartPtr <TAddrClient> ptrClient;
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
    SubOptions.append( (Ptr*)ptrStatus );*/
}

void TSrvOptIA_PD::release(SmartPtr<TSrvOptIA_PD> queryOpt,
                           unsigned long &addrCount)
{
}

void TSrvOptIA_PD::confirm(SmartPtr<TSrvOptIA_PD> queryOpt,
                           unsigned long &addrCount)
{
}

void TSrvOptIA_PD::decline(SmartPtr<TSrvOptIA_PD> queryOpt,
                           unsigned long &addrCount)
{
}

bool TSrvOptIA_PD::doDuties()
{
    return true;
}

/*
 * gets free address for a client
 */
SmartPtr<TIPv6Addr> TSrvOptIA_PD::getFreePrefix(SmartPtr<TIPv6Addr> hint) {
    
    bool invalidPrefix = false;
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TIPv6Addr>   prefix;
    ptrIface = this->CfgMgr->getIfaceByID(this->Iface);
    if (!ptrIface) {
	Log(Error) << "Trying to find free prefix on non-existent interface (id=%d)\n" 
		   << this->Iface << LogEnd;
	return 0; // NULL
    }
    
    // check if this prefix is ok
    
    // is it anyaddress (::)?
    SmartPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
    if (*anyaddr==*hint) {
	Log(Debug) << "Client requested unspecified (" << *hint 
		   << ") prefix. Hint ignored." << LogEnd;
	invalidPrefix = true;
    }
    
    // is it multicast address (ff...)?
    if ((*(hint->getAddr()))==0xff) {
	Log(Debug) << "Client requested multicast (" << *hint 
		   << ") prefix. Hint ignored." << LogEnd;
	invalidPrefix = true;
    }

    // is it link-local address (fe80::...)?
    char linklocal[]={0xfe, 0x80};
    if (!memcmp(hint->getAddr(),linklocal,2)) {
	Log(Debug) << "Client requested link-local (" << *hint << ") prefix. Hint ignored." << LogEnd;
	invalidPrefix = true;
    }
 
    if ( !invalidPrefix ) {
	// hint is valid, try to use it
	
	SmartPtr<TSrvCfgPD> ptrPD;
	ptrPD = this->CfgMgr->getClassByPrefix(this->Iface, hint);

	// best case: address belongs to supported class, and is free
	if ( ptrPD && AddrMgr->prefixIsFree(hint) ) {
	    Log(Debug) << "Requested prefix (" << *hint << ") is free, great!" << LogEnd;
	    return hint;
	} 
/*
	// medium case: addess belongs to supported class, but is used
	if ( ptrClass ) {
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
	}
    }

    // do we have a cached address for that client?
    if (addr = AddrMgr->getCachedAddr(this->ClntDuid)) {
	if (this->CfgMgr->getClassByAddr(this->Iface, addr)) {
	    Log(Info) << "Cache: Cached address " << *addr << " found. Welcome back." << LogEnd;
	    return addr;
	} else {
	    Log(Warning) << "Cache: Cached address " << *addr << " found, but it is no longer valid." << LogEnd;
	    AddrMgr->delCachedAddr(addr);
	}
    }
    */

    // worst case: address does not belong to supported class
    // or specified hint is invalid
	}
        SmartPtr<TSrvCfgPD> ptrPD;
        ptrPD = ptrIface->getPD();
        if (!ptrPD) {
	     ptrIface->firstPD();
	     ptrPD = ptrIface->getPD();
    }
    
	do {
	    prefix=ptrPD->getRandomPrefix();
	} while (!this->AddrMgr->prefixIsFree(prefix));

    return prefix;

}
