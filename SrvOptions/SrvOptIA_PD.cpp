/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
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
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"
#include "Msg.h"
#include "SrvCfgMgr.h"
#include "SrvTransMgr.h"

TSrvOptIA_PD::TSrvOptIA_PD(uint32_t iaid, uint32_t t1, uint32_t t2, int Code, string Text, TMsg* parent)
    :TOptIA_PD(iaid, t1, t2, parent)
{
    SubOptions.append(new TSrvOptStatusCode(Code, Text, parent));
}

/*
 * Create IA_PD option based on receive buffer
 */
TSrvOptIA_PD::TSrvOptIA_PD( char * buf, int bufsize, TMsg* parent)
    :TOptIA_PD(buf, bufsize, parent)
{
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

                SPtr<TOpt> opt;
                opt = SPtr<TOpt>(); /* NULL */
                switch (code)
                {
                case OPTION_IAPREFIX:
                    opt = (Ptr*)SPtr<TSrvOptIAPrefix>
                        (new TSrvOptIAPrefix(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    opt = (Ptr*)SPtr<TSrvOptStatusCode>
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

void TSrvOptIA_PD::releaseAllPrefixes(bool quiet) {
    SPtr<TOpt> opt;
    SPtr<TIPv6Addr> prefix;
    SPtr<TOptIAPrefix> optPrefix;
    this->firstOption();
    while ( opt = this->getOption() ) {
        if (opt->getOptType() != OPTION_IAPREFIX)
            continue;
        optPrefix = (Ptr*) opt;
        prefix = optPrefix->getPrefix();
        SrvAddrMgr().delPrefix(ClntDuid, IAID, prefix, quiet);
        SrvCfgMgr().decrPrefixCount(this->Iface, prefix);
    }
}

/// @brief checks if there are existing leases (and assigns them)
///
/// @return true, if existing lease(s) are found
bool TSrvOptIA_PD::existingLease() {
    SPtr<TAddrClient> client = SrvAddrMgr().getClient(ClntDuid);
    if (!client)
        return false;
    SPtr<TAddrIA> pd = client->getPD(IAID);
    if (!pd)
        return false;
    if (!pd->countPrefix())
        return false;

    SPtr<TAddrPrefix> prefix;
    pd->firstPrefix();
    while (prefix = pd->getPrefix()) {
        Log(Debug) << "Assinging existing lease: prefix=" << prefix->get()->getPlain() << "/" << prefix->getLength()
                   << ", pref=" << prefix->getPref() << ", valid=" << prefix->getValid() << LogEnd;
        SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(prefix->get(), prefix->getLength(), 
                                                   prefix->getPref(), prefix->getValid(), this->Parent);
        SubOptions.append((Ptr*)optPrefix);
    }

    T1 = pd->getT1();
    T2 = pd->getT2();

    stringstream status;
    status << "Here's your existing lease: " << countPrefixes() << " prefix(es).";
    SubOptions.append(new TSrvOptStatusCode(STATUSCODE_SUCCESS, status.str(),  this->Parent) );
    return true;
}

// @brief gets one (or more) prefix for requested
//
// @param clientMsg client message
// @param hint proposed prefix
// @param fake should the prefix be really assigned or not (used in SOLICIT processing)
//
// @return true, if something was assigned
bool TSrvOptIA_PD::assignPrefix(SPtr<TSrvMsg> clientMsg, SPtr<TIPv6Addr> hint, bool fake) {
    SPtr<TIPv6Addr> prefix;
    SPtr<TSrvOptIAPrefix> optPrefix;
    SPtr<TSrvCfgPD> ptrPD;
    List(TIPv6Addr) prefixLst;
    SPtr<TIPv6Addr> cached;

    if (hint->getPlain()==string("::") ) {
        cached = SrvAddrMgr().getCachedEntry(ClntDuid, TAddrIA::TYPE_PD);
        if (cached)
            hint = cached;
    }

    // get address
    prefixLst.clear();
    prefixLst = getFreePrefixes(clientMsg, hint);
    ostringstream buf;
    prefixLst.first();
    while (prefix = prefixLst.get()) {
        buf << prefix->getPlain() << "/" << this->PDLength << " ";
        optPrefix = new TSrvOptIAPrefix(prefix, (char)this->PDLength, this->Prefered, this->Valid, this->Parent);
        SubOptions.append((Ptr*)optPrefix);
        
        // every prefix has to be remembered in AddrMgr, e.g. when there are 2 pools defined,
        // prefixLst contains entries from each pool, so 2 prefixes has to be remembered
        SrvAddrMgr().addPrefix(this->ClntDuid, this->ClntAddr, this->Iface, this->IAID, this->T1, this->T2,
                               prefix, this->Prefered, this->Valid, this->PDLength, false);
        
        // but CfgMgr has to increase usage only once. Don't ask my why :)
        SrvCfgMgr().incrPrefixCount(Iface, prefix);
    }
    Log(Info) << "PD:" << (fake?"(would be)":"") << " assigned prefix(es):" << buf.str() << LogEnd;

    if (prefixLst.count()) {
        stringstream tmp; 
        tmp << "Assigned " << prefixLst.count() << " prefix(es).";
        SubOptions.append( new TSrvOptStatusCode(STATUSCODE_SUCCESS, tmp.str(), Parent) );
        return true;
    } 

    SubOptions.append( new TSrvOptStatusCode(STATUSCODE_NOPREFIXAVAIL,
                                             "Unable to provide any prefixes. Sorry.", this->Parent) );
    return false;
}

/// @brief constructor used in replies to SOLICIT, REQUEST, RENEW, REBIND, DECLINE and RELEASE 
///
/// @param clientMsg client message that server responds to
/// @param queryOpt IA_PD option from client message
/// @param msgType message type of client request
/// @param parent 
TSrvOptIA_PD::TSrvOptIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt, TMsg* parent)
    :TOptIA_PD(queryOpt->getIAID(), DHCPV6_INFINITY, DHCPV6_INFINITY, parent)
{
    int msgType = clientMsg->getType();
    ClntDuid  = clientMsg->getClientDUID();
    ClntAddr  = clientMsg->getAddr();
    Iface     = clientMsg->getIface();

    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID(Iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex="
                   << Iface << ". Something is wrong, VERY wrong." << LogEnd;
        return;
    }

    // is the prefix delegation supported?
    if ( !ptrIface->supportPrefixDelegation() ) {
        SPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOPREFIXAVAIL,
                                          "Server support for prefix delegation is not enabled. Sorry buddy.",
                                          Parent);
        this->SubOptions.append((Ptr*)ptrStatus);
        return;
    }

    bool fake  = false; // is this assignment for real?
    if (msgType == SOLICIT_MSG)
        fake = true;
    if (parent->getOption(OPTION_RAPID_COMMIT))
        fake = false;

    switch (msgType) {
    case SOLICIT_MSG:
        solicitRequest(clientMsg, queryOpt, ptrIface, fake);
        break;
    case REQUEST_MSG:
        solicitRequest(clientMsg, queryOpt, ptrIface, fake);
        break;
    case RENEW_MSG:
        renew(queryOpt, ptrIface);
        break;
    case REBIND_MSG:
        rebind(queryOpt, ptrIface);
        break;
    case RELEASE_MSG:
        release(queryOpt, ptrIface);
        break;
    case CONFIRM_MSG:
        confirm(queryOpt, ptrIface);
        break;
    case DECLINE_MSG:
        decline(queryOpt, ptrIface);
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

/**
 * this method is used to prepare response to IA_PD received in SOLICIT and REQUEST messages
 * (i.e. it tries to assign prefix as requested by client)
 *
 * @param queryOpt
 * @param ptrIface
 * @param fake
 */
void TSrvOptIA_PD::solicitRequest(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> ptrIface, bool fake) {

    // --- Is this PD without IAPREFIX options? ---
    SPtr<TIPv6Addr> hint = 0;
    if (!queryOpt->countPrefixes()) {
        Log(Info) << "PD option (with IAPREFIX suboptions missing) received. " << LogEnd;
        hint = new TIPv6Addr(); /* :: - any address */
        this->Prefered = DHCPV6_INFINITY;
        this->Valid    = DHCPV6_INFINITY;
    } else {
        SPtr<TSrvOptIAPrefix> hintPrefix = (Ptr*) queryOpt->getOption(OPTION_IAPREFIX);
        hint  = hintPrefix->getPrefix();
        Log(Info) << "PD: PD option with " << hint->getPlain() << " as a hint received." << LogEnd;
        this->Prefered  = hintPrefix->getPref();
        this->Valid     = hintPrefix->getValid();
    }

    // --- LEASE ASSIGN STEP 3: check if client already has binding
    if (existingLease()) {
        return;
    }

    // --- LEASE ASSIGN STEP 4: Try to find fixed lease
    if (assignFixedLease(queryOpt)) {
        return;
    }

    // --- LEASE ASSIGN STEP 5: Count available addresses ---
    unsigned long leaseAssigned  = SrvAddrMgr().getLeaseCount(ClntDuid); // already assigned
    unsigned long leaseMax       = SrvCfgMgr().getIfaceByID(Iface)->getClntMaxLease(); // clnt-max-lease

    if (leaseAssigned >= leaseMax) {
        Log(Notice) << "Client got " << leaseAssigned << " lease(s) and requested 1 more, but limit for a client is "
                  << leaseMax << LogEnd;
        stringstream tmp;
        tmp << "Sorry. You already have " << leaseAssigned << " lease(es) and you can't have more.";
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOPREFIXAVAIL,tmp.str(), Parent));
        return;
    }

    // --- LEASE ASSIGN STEP 6: Cached address? ---
    // --- LEASE ASSIGN STEP 7: client's hint ---
    // --- LEASE ASSIGN STEP 8: get new random address --
    assignPrefix(clientMsg, hint, fake);
}

/**
 * generate OPTION_PD based on OPTION_PD received in RENEW message
 *
 * @param queryOpt IA_PD option received from client in the RENEW message
 * @param iface interface on which client communicated
 */
void TSrvOptIA_PD::renew(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface) {
    SPtr <TAddrClient> ptrClient;
    ptrClient = SrvAddrMgr().getClient(this->ClntDuid);
    if (!ptrClient) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",
                                                this->Parent));
        return;
    }

    // find that IA
    SPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getPD(this->IAID);
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
    SPtr<TAddrPrefix> prefix;
    ptrIA->firstPrefix();
    while ( prefix = ptrIA->getPrefix() ) {
        SPtr<TSrvOptIAPrefix> optPrefix;
        prefix->setTimestamp();
        optPrefix = new TSrvOptIAPrefix(prefix->get(), prefix->getLength(), prefix->getPref(),
                                        prefix->getValid(), this->Parent);
        SubOptions.append( (Ptr*)optPrefix );
    }

    // finally send greetings and happy OK status code
    SPtr<TSrvOptStatusCode> ptrStatus;
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Prefix(es) renewed.", this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_PD::rebind(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface) {
    /// @todo: implement PD support in REBIND message
}

void TSrvOptIA_PD::release(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface) {
    /// @todo: implement PD support in RELEASE message
}

void TSrvOptIA_PD::confirm(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface) {
    /// @todo: implement PD support in CONFIRM message
}

void TSrvOptIA_PD::decline(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface) {
    SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOPREFIXAVAIL, "You tried to decline a prefix. Are you crazy?", Parent));
}

bool TSrvOptIA_PD::doDuties() {
    return true;
}

/// @brief tries to find a fixed lease for a client
///
/// @param req IA_PD sent by client
///
/// @return true, if fixed lease is assigned
bool TSrvOptIA_PD::assignFixedLease(SPtr<TSrvOptIA_PD> req) {
    
    // is there any specific address reserved for this client? (exception mechanism)
    SPtr<TSrvCfgIface> ptrIface=SrvCfgMgr().getIfaceByID(Iface);
    if (!ptrIface) {
	return 0;
    }

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(ClntDuid, Parent, false/* false = verbose */);
    if (!ex)
        return false;

    SPtr<TIPv6Addr> reservedPrefix = ex->getPrefix();
    if (!reservedPrefix) {
        // there's no reserved prefix for this lad
        return false;
    } 
    Log(Debug) << "Assigning fixed lease" << LogEnd;
    
    // we've got fixed prefix, yay! Let's try to calculate its preferred and valid lifetimes
    SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIfaceByID(Iface);
    if (!iface) {
        // this should never happen
        Log(Error) << "Unable to find interface with ifindex=" << Iface << " in SrvCfgMgr." << LogEnd;
        return false;
    }
    
    // if the lease is not within normal range, treat it as fixed, infinite one
    uint32_t pref = DHCPV6_INFINITY;
    uint32_t valid = DHCPV6_INFINITY;
    
    SPtr<TSrvCfgPD> pool;
    iface->firstPD();
    while (pool = iface->getPD()) {
        // This is not the pool you are looking for.
        if (!pool->prefixInPool(reservedPrefix))
            continue;

        // we found matching pool! Yay!
        T1 = pool->getT1(req->getT1());
        T2 = pool->getT2(req->getT2());

        SPtr<TSrvOptIAPrefix> hint = (Ptr*) req->getOption(OPTION_IAPREFIX);
        if (hint) {
            pref = hint->getPref();
            valid = hint->getValid();
        }
        
        pref = pool->getPrefered(pref);
        valid = pool->getValid(valid);
        
        Log(Info) << "Reserved in-pool prefix " << reservedPrefix->getPlain() << "/" 
                  << static_cast<unsigned int>(ex->getPrefixLen()) << " for this client found, assigning." << LogEnd;
        SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(reservedPrefix, ex->getPrefixLen(), pref, valid, Parent);
        SubOptions.append(optPrefix);

        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed in-pool prefix.", Parent));

        SrvAddrMgr().addPrefix(ClntDuid, this->ClntAddr, Iface, this->IAID, T1, T2, reservedPrefix, pref, valid, ex->getPrefixLen(), false);

        // but CfgMgr has to increase usage only once. Don't ask my why :)
        SrvCfgMgr().incrPrefixCount(Iface, reservedPrefix);
        
        return true;
    }
    
    // This address does not belong to any pool. Assign it anyway
    Log(Info) << "Reserved out-of-pool address " << reservedPrefix->getPlain()
              << static_cast<unsigned int>(ex->getPrefixLen()) << " for this client found, assigning." << LogEnd;
    SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(reservedPrefix, ex->getPrefixLen(), pref, valid, Parent);
    SubOptions.append(optPrefix);
    
    SubOptions.append(new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed out-of-pool address.", Parent));

    SrvAddrMgr().addPrefix(ClntDuid, this->ClntAddr, Iface, this->IAID, T1, T2, reservedPrefix, pref, valid, ex->getPrefixLen(), false);
    
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
List(TIPv6Addr) TSrvOptIA_PD::getFreePrefixes(SPtr<TSrvMsg> clientMsg, SPtr<TIPv6Addr> hint) {

    SPtr<TSrvCfgIface> ptrIface;
    SPtr<TIPv6Addr>    prefix;
    SPtr<TSrvCfgPD>    ptrPD;
    bool validHint = true;
    List(TIPv6Addr) lst;

    lst.clear();
    ptrIface = SrvCfgMgr().getIfaceByID(this->Iface);
    if (!ptrIface) {
      Log(Error) << "PD: Trying to find free prefix on non-existent interface (ifindex=" << this->Iface << ")." << LogEnd;
      return lst; // empty list
    }

    if (!ptrIface->supportPrefixDelegation()) {
      // this method should not be called anyway
      Log(Error) << "PD: Prefix delegation is not supported on the " << ptrIface->getFullName() << "." << LogEnd;
      return lst; // empty list
    }

    ptrIface->firstPD();
    ptrPD = ptrIface->getPD();
    if (ptrPD->getAssignedCount() >= ptrPD->getTotalCount()) { // should be ==, asigned>total should never happen
      Log(Error) << "PD: Unable to grant any prefixes: Already asigned " << ptrPD->getAssignedCount()
                 << " out of " << ptrPD->getTotalCount() << "." << LogEnd;
      return lst; // empty list
    }


    // check if this prefix is ok

    // is it anyaddress (::)?
    SPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
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


    SPtr<TOptVendorData> remoteID;
    TSrvMsg * par = dynamic_cast<TSrvMsg*>(Parent);
    if (par) {
      remoteID = par->getRemoteID();
    }

    if ( validHint ) {
      // hint is valid, try to use it
      ptrPD = SrvCfgMgr().getClassByPrefix(this->Iface, hint);

      // if the PD allow the hint, based on DUID, Addr, and Msg from client
     if (ptrPD && ptrPD->clntSupported(ClntDuid, ClntAddr, clientMsg ) && !ptrIface->checkReservedPrefix(hint,ClntDuid,remoteID) )
         {
                  // case 2: address belongs to supported class, and is free
                  if ( ptrPD && SrvAddrMgr().prefixIsFree(hint) ) {
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
                        } while (!SrvAddrMgr().prefixIsFree(prefix));
                        lst.append(prefix);

                        this->PDLength = ptrPD->getPD_Length();
                        this->Prefered = ptrPD->getPrefered(this->Prefered);
                        this->Valid    = ptrPD->getValid(this->Valid);
                        this->T1       = ptrPD->getT1(this->T1);
                        this->T2       = ptrPD->getT2(this->T2);
                        return lst;
                  } // if hint is used
           } // if the PD support hint, based on Client Class
    } // if valid hint

    // case 1: no hint provided, assign one prefix from each pool
    // case 4: provided hint does not belong to supported class or is useless (multicast,link-local, ::)
    ptrIface->firstPD();

    while ( ptrPD = ptrIface->getPD())
    {
        if (!ptrPD->clntSupported(ClntDuid, ClntAddr, clientMsg ))
                continue;
        break;
    }

    if (!ptrPD)
    {
        Log(Warning) << "Unable to find any PD for this client." << LogEnd;
        return lst;  // return empty list
    }


    while (true) {
        List(TIPv6Addr) lst;
        lst = ptrPD->getRandomList();
        lst.first();
        bool allFree = true;
        while (prefix = lst.get()) {
            if (!SrvAddrMgr().prefixIsFree(prefix)) {
                allFree = false;
            }
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
