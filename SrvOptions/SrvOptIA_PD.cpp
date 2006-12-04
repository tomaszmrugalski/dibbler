/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                          
 *
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include <sstream>
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
    
    this->AddrMgr   = addrMgr;
    this->CfgMgr    = cfgMgr;
    this->ClntDuid  = clntDuid;
    this->ClntAddr  = clntAddr;
    this->Iface     = iface;
	
    bool fake  = false; // is this assignment for real?
    if (msgType == SOLICIT_MSG)
	fake = true;
    if (parent->getOption(OPTION_RAPID_COMMIT))
	fake = false;

    // --- Is the PD supported? ---
    SPtr<TSrvCfgIface> ptrIface = CfgMgr->getIfaceByID(iface);
    if (!iface) {
	Log(Error) << "Unable to find interface " << iface << ". Something is wrong, VERY wrong." << LogEnd;
	return;
    }

    // is the prefix delegation supported?
    if ( !ptrIface->supportPrefixDelegation() ) {
	SmartPtr<TSrvOptStatusCode> ptrStatus;
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOPREFIXAVAIL,
					  "Server support for prefix delegation is not enabled. Sorry buddy.",this->Parent);
        this->SubOptions.append((Ptr*)ptrStatus);
	return;
    }

    // --- Is this PD without IAPREFIX options? ---
    SPtr<TIPv6Addr> hint = 0;
    if (!queryOpt->countPrefixes()) {
	Log(Notice) << "PD option (with IAPREFIX suboptions missing) received. " << LogEnd;
	hint  = new TIPv6Addr(); /* :: - any address */
	this->Prefered = DHCPV6_INFINITY;
	this->Valid    = DHCPV6_INFINITY;
    } else {
	SPtr<TSrvOptIAPrefix> hintPrefix = (Ptr*) queryOpt->getOption(OPTION_IAPREFIX);
	Log(Info) << "PD: PD option with " << hint->getPlain() << " as a hint received." << LogEnd;
	hint  = hintPrefix->getPrefix();
	this->Prefered  = hintPrefix->getPref();
	this->Valid     = hintPrefix->getValid();
    }

    // assign prefixes
    int status = this->assignPrefix(hint, fake);
    
    // include status code
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    if (status) {
	ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,
					  "1 prefix granted. Next time you might consider including "
					  "IAPREFIX in IA_PD option.", this->Parent);
    } else {
	ptrStatus = new TSrvOptStatusCode(status,
					  "Unable to provide any prefixes. Sorry.", this->Parent);
    }
    this->SubOptions.append((Ptr*)ptrStatus);
    return;
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

/** 
 * gets one (or more) prefix for requested 
 * 
 * @param hint 
 * @param length 
 * @param pref 
 * @param valid 
 * @param fake 
 * 
 * @return status, if it was possible to assign something (STATUSCODE_SUCCESS)
 */
int TSrvOptIA_PD::assignPrefix(SmartPtr<TIPv6Addr> hint, bool fake) {
    SmartPtr<TIPv6Addr> prefix;
    SmartPtr<TSrvOptIAPrefix> optPrefix;
    SmartPtr<TSrvCfgPD> ptrPD;
    List(TIPv6Addr) prefixLst;
    
    // get address
    prefixLst.clear();
    prefixLst = this->getFreePrefixes(hint);
    prefixLst.first();
    ostringstream buf;
    while (prefix = prefixLst.get()) {
	buf << prefix->getPlain() << "/" << this->PDLength << " ";
	optPrefix = new TSrvOptIAPrefix(prefix, this->PDLength, this->Prefered, this->Valid, this->Parent);
	SubOptions.append((Ptr*)optPrefix);
	
	if (!fake)
	    AddrMgr->addPrefix(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID, this->T1, this->T2,
			       prefix, this->Prefered, this->Valid, this->PDLength, false);
    }
    Log(Info) << "PD:" << (fake?"(would be)":"") << " assigned prefix(es):" << buf.str() << LogEnd;

    if (prefixLst.count()) {
	return STATUSCODE_SUCCESS;
    } else
	return STATUSCODE_NOPREFIXAVAIL;
}

// so far it is enough here 
// constructor used only in RENEW, REBIND, DECLINE and RELEASE
TSrvOptIA_PD::TSrvOptIA_PD( SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_PD> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr, SmartPtr<TDUID> clntDuid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent)
    :TOptIA_PD(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    // FIXME: addrCount is obsolete

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
        this->renew(queryOpt);
        break;
    case REBIND_MSG:
        this->rebind(queryOpt);
        break;
    case RELEASE_MSG:
        this->release(queryOpt);
        break;
    case CONFIRM_MSG:
        this->confirm(queryOpt);
        break;
    case DECLINE_MSG:
        this->decline(queryOpt);
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
 * @param queryOpt - IA_PD option in the RENEW message
 * @param addrCount 
 */
void TSrvOptIA_PD::renew(SmartPtr<TSrvOptIA_PD> queryOpt) {
    // FIXME: support PD in RENEW message.
}

void TSrvOptIA_PD::rebind(SmartPtr<TSrvOptIA_PD> queryOpt) {
    // FIXME: implement PD support in REBIND message
}

void TSrvOptIA_PD::release(SmartPtr<TSrvOptIA_PD> queryOpt) {
    // FIXME: implement PD support in RELEASE message
}

void TSrvOptIA_PD::confirm(SmartPtr<TSrvOptIA_PD> queryOpt) {
    // FIXME: implement PD support in CONFIRM message
}

void TSrvOptIA_PD::decline(SmartPtr<TSrvOptIA_PD> queryOpt) {
    // FIXME: implement PD support in DECLINE message
}

bool TSrvOptIA_PD::doDuties() {
    return true;
}

/** 
 * return free prefixes for a client. There are several ways that method may work:
 * 1 - client didn't provide any hints: 
 *     => one prefix from each pool will be granted
 * 2 - client has provided hint and that is valid (supported and unused): 
 *     => requested prefix will be granted
 * 3 - client has provided hint, which belongs to supported pool, but this prefix is used: 
 *     => other prefix from that pool will be asigned
 * 4 - client has provided hint, but it is invalid (not beloninging to a supported pool,
 *     multicast or link-local): 
 *     => see 1
 * 
 * @param hint - hint provided by client (or ::)
 * 
 * @return - list of prefixes
 */
List(TIPv6Addr) TSrvOptIA_PD::getFreePrefixes(SmartPtr<TIPv6Addr> hint) {
    
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TIPv6Addr>    prefix;
    SmartPtr<TSrvCfgPD>    ptrPD;
    bool validHint = true;
    List(TIPv6Addr) lst;

    lst.clear();
    ptrIface = this->CfgMgr->getIfaceByID(this->Iface);
    if (!ptrIface) {
	Log(Error) << "PD: Trying to find free prefix on non-existent interface (ifindex=" << this->Iface << ")." << LogEnd;
	return lst;
    }

    if (!ptrIface->supportPrefixDelegation()) {
	// this method should not be called anyway
	Log(Error) << "PD: Prefix delegation is not supported on the " << ptrIface->getFullName() << "." << LogEnd;
	return lst;
    }
    
    // check if this prefix is ok
    
    // is it anyaddress (::)?
    SmartPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
    if (*anyaddr==*hint) {
	Log(Debug) << "PD: Client requested unspecified (" << *hint 
		   << ") prefix. Hint ignored." << LogEnd;
	validHint = false;
    }
    
    // is it multicast address (ff...)?
    if ((*(hint->getAddr()))==0xff) {
	Log(Debug) << "PD: Client requested multicast (" << *hint 
		   << ") prefix. Hint ignored." << LogEnd;
	validHint = false;
    }

    // is it link-local address (fe80::...)?
    char linklocal[]={0xfe, 0x80};
    if (!memcmp(hint->getAddr(),linklocal,2)) {
	Log(Debug) << "PD: Client requested link-local (" << *hint << ") prefix. Hint ignored." << LogEnd;
	validHint = false;
    }
 
    if ( validHint ) {
	// hint is valid, try to use it
	ptrPD = this->CfgMgr->getClassByPrefix(this->Iface, hint);
	
	// case 2: address belongs to supported class, and is free
	if ( ptrPD && AddrMgr->prefixIsFree(hint) ) {
	    Log(Debug) << "PD: Requested prefix (" << *hint << ") is free, great!" << LogEnd;
	    this->PDLength = ptrPD->getPD_Length();
	    this->Prefered = ptrPD->getPrefered(this->Prefered);
	    this->Valid    = ptrPD->getValid(this->Valid);
	    this->T1       = ptrPD->getT1(this->T1);
	    this->T2       = ptrPD->getT2(this->T2);
	    lst.append(hint);
	    return lst;
	} 
	
	// case 3: hint is used, but we can assign another prefix from the same pool
	if (ptrPD) {
	    do {
		prefix=ptrPD->getRandomPrefix();
	    } while (!this->AddrMgr->prefixIsFree(prefix));
	    lst.append(prefix);

	    this->PDLength = ptrPD->getPD_Length();
	    this->Prefered = ptrPD->getPrefered(this->Prefered);
	    this->Valid    = ptrPD->getValid(this->Valid);
	    this->T1       = ptrPD->getT1(this->T1);
	    this->T2       = ptrPD->getT2(this->T2);
	    return lst;
	}
    } 

    // case 1: no hint provided, assign one prefix from each pool
    // case 4: provided hint does not belong to supported class or is useless (multicast,link-local, ::)
    ptrIface->firstPD();
    ptrPD = ptrIface->getPD();
    
    while (true) {
	List(TIPv6Addr) lst;
	lst = ptrPD->getRandomList();
	lst.first();
	bool allFree = true;
	while (prefix = lst.get()) {
	    if (!AddrMgr->prefixIsFree(prefix))
		allFree = false;
	}
	if (allFree) {
	    this->PDLength = ptrPD->getPD_Length();
	    this->Prefered = ptrPD->getPrefered(this->Prefered);
	    this->Valid    = ptrPD->getValid(this->Valid);
	    this->T1       = ptrPD->getT1(this->T1);
	    this->T2       = ptrPD->getT2(this->T2);
	    return lst;
	}
    };
}
