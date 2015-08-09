/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif

#include <sstream>
#include "SrvOptIA_NA.h"
#include "SrvOptIAAddress.h"
#include "OptStatusCode.h"
#include "OptVendorData.h"
#include "SrvCfgOptions.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"
#include "Msg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"

using namespace std;

/// @brief Creates an empty IA_NA (used for DECLINE and CONFIRM).
///
/// @param IAID iaid value to be used.
/// @param T1 T1 timer value to be used.
/// @param T2 T2 timer value to be used.
/// @param parent Pointer to parent message.
TSrvOptIA_NA::TSrvOptIA_NA(long IAID, long T1, long T2, TMsg* parent)
    :TOptIA_NA(IAID, T1, T2, parent), Iface(parent->getIface()) {
}

/// @brief Creates an IA_NA with option status code.
///
/// @param iaid iaid value to be used.
/// @param t1 T1 timer value to be used.
/// @param t2 T2 timer value to be used.
/// @param code Status Code (to be set in status code option)
/// @param text text to be used in status code option as description
/// @param parent Pointer to parent message.
TSrvOptIA_NA::TSrvOptIA_NA(long iaid, long t1, long t2, int code,
                           const std::string& text, TMsg* parent)
    :TOptIA_NA(iaid, t1, t2, parent), Iface(parent->getIface()) {
    SubOptions.append(new TOptStatusCode(code, text, parent));
}

/// @brief Create IA_NA option based on receive buffer. 
///
/// @param buf pointer to beginning of a buffer containing IA_NA (without type and option fields)
/// @param bufsize length of the option (value or already parsed option-len)
/// @param parent Pointer to parent message.
TSrvOptIA_NA::TSrvOptIA_NA(char * buf, int bufsize, TMsg* parent)
    :TOptIA_NA(buf,bufsize, parent), Iface(parent->getIface()) {
    int pos=0;
    /// @todo: implement unpack()
    while (pos < bufsize)
    {
        int code=buf[pos]*256 + buf[pos+1];
        pos += 2;
        int length=buf[pos]*256 + buf[pos+1];
        pos += 2;
        if ((code > 0) && (code <= 24))
        {
            if(allowOptInOpt(parent->getType(),OPTION_IA_NA,code)) {
                SPtr<TOpt> opt;
                opt = SPtr<TOpt>(); /* NULL */
                switch (code)
                {
                case OPTION_IAADDR:
                    opt = (Ptr*)SPtr<TSrvOptIAAddress>
                        (new TSrvOptIAAddress(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    opt = (Ptr*)SPtr<TOptStatusCode>
                        (new TOptStatusCode(buf+pos,length,this->Parent));
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

/// This constructor is used to create IA option as an aswer to a SOLICIT, SOLICIT (RAPID_COMMIT) or REQUEST.
///
/// @param queryOpt 
/// @param queryMsg 
/// @param parent 
TSrvOptIA_NA::TSrvOptIA_NA(SPtr<TSrvOptIA_NA> queryOpt, SPtr<TSrvMsg> queryMsg, TMsg* parent)
    :TOptIA_NA(queryOpt->getIAID(), queryOpt->getT1(), queryOpt->getT2(), parent) {

    Iface = parent->getIface();
    ClntAddr = queryMsg->getRemoteAddr();
    ClntDuid  = queryMsg->getClientDUID();

    // true for advertise, false for everything else
    bool quiet = (parent->getType()==ADVERTISE_MSG);

    // --- LEASE ASSIGN STEP 3: check if client already has binding
    if (renew(queryOpt, false)) {
      Log(Info) << "Previous binding for client " << ClntDuid->getPlain() << ", IA(iaid="
                << queryOpt->getIAID() << ") found and renewed." << LogEnd;
      return;
    }

    // --- LEASE ASSIGN STEP 4: Try to find fixed lease
    if (assignFixedLease(queryOpt, quiet)) {
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
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOADDRSAVAIL, tmp.str(), parent));
        return;
    }

    // --- LEASE ASSIGN STEP 6: Cached address? ---
    if (assignCachedAddr(quiet)) {
        return;
    }
    
    // --- LEASE ASSIGN STEP 7: client's hint ---
    if (assignRequestedAddr(queryMsg, queryOpt, quiet)) {
        return;
    }

    // --- LEASE ASSIGN STEP 8: get new random address --
    if (assignRandomAddr(queryMsg, quiet)) {
        return;
    }

    SubOptions.append(new TOptStatusCode(STATUSCODE_NOADDRSAVAIL,
                                         "No more addresses for you. Sorry.", Parent));
}

bool TSrvOptIA_NA::assignRequestedAddr(SPtr<TSrvMsg> queryMsg, SPtr<TSrvOptIA_NA> queryOpt,
                                       bool quiet) {
    SPtr<TOpt> opt;
    SPtr<TIPv6Addr> hint, candidate;
    SPtr<TOptIAAddress> optAddr;
    SPtr<TSrvCfgAddrClass> ptrClass;

    queryOpt->firstOption();
    while ( opt = queryOpt->getOption() ) {
	switch ( opt->getOptType() ) {
	case OPTION_IAADDR: {
	    optAddr = (Ptr*) opt;
	    hint    = optAddr->getAddr();

            if (SrvCfgMgr().addrReserved(hint)) {
                Log(Debug) << "Requested address " << hint->getPlain()
                           << " is reserved for someone else, sorry." << LogEnd;
                return false;
            }

            if (candidate = getAddressHint(queryMsg, hint)) {
                if (assignAddr(candidate, optAddr->getPref(), optAddr->getValid(), quiet))
                    return true;
            }
            continue;
        }
        default:
            continue;
        }
    }

    return false;
}

/// @brief Tries to get cached address for this client.
///
/// This method may delete entry from cache if it finds out that entry is used by someone else
/// or is no longer valid (i.e. updated config has different pool definitions).
/// That is step 6 of lease assignment policy.
///
/// @param quiet should the assignment messages be logged (it shouldn't for solicit)
///
/// @return true, if address was assigned
bool TSrvOptIA_NA::assignCachedAddr(bool quiet) {
    SPtr<TIPv6Addr> candidate;
    if (candidate = SrvAddrMgr().getCachedEntry(ClntDuid, IATYPE_IA)) {
        SPtr<TSrvCfgAddrClass> pool = SrvCfgMgr().getClassByAddr(Iface, candidate);
        if (pool) {
	    Log(Info) << "Cache: Cached address " << *candidate << " found. Welcome back." << LogEnd;

	    if (SrvAddrMgr().addrIsFree(candidate) && !SrvCfgMgr().addrReserved(candidate)) {
                if (assignAddr(candidate, pool->getPref(), pool->getValid(), quiet))
                    return true;
                // WTF? Address is free, buy we can't assign it?
                Log(Error) << "Failed to assign cached address that seems unused. Strange." << LogEnd;
		return false;
            }
	    Log(Info) << "Unfortunately, " << candidate->getPlain() << " is already used or reserved." << LogEnd;
	    SrvAddrMgr().delCachedEntry(candidate, IATYPE_IA);
            return false;
	} else {
	    Log(Warning) << "Cache: Cached address " << *candidate << " found, but it is no longer valid." << LogEnd;
	    SrvAddrMgr().delCachedEntry(candidate, IATYPE_IA);
            return false;
	}// else
    }

    return false;
}

/// @brief Tries to assign fixed (reserved) lease.
///
/// @param req client's IA_NA (used for trying to assign as close T1,T2,pref,valid values as possible)
///
/// @return true, if assignment was successful, false if there are no fixed-leases reserved
bool TSrvOptIA_NA::assignFixedLease(SPtr<TSrvOptIA_NA> req, bool quiet) {
    // is there any specific address reserved for this client? (exception mechanism)
    SPtr<TIPv6Addr> reservedAddr = getExceptionAddr();
    if (!reservedAddr) {
        // there's no reserved address for this pal
        return false;
    } 
    
    // we've got fixed address, yay! Let's try to calculate its preferred and valid lifetimes
    SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIfaceByID(Iface);
    if (!iface) {
        // this should never happen
          Log(Error) << "Unable to find interface with ifindex=" << Iface << " in SrvCfgMgr." << LogEnd;
          return false;
    }
    
    // if the lease is not within normal range, treat it as fixed, infinite one
    uint32_t pref = DHCPV6_INFINITY;
    uint32_t valid = DHCPV6_INFINITY;

    SPtr<TSrvOptIAAddress> hint = (Ptr*) req->getOption(OPTION_IAADDR);
    if (hint) {
        pref = hint->getPref();
        valid = hint->getValid();
    }

    SPtr<TSrvCfgAddrClass> pool;
    iface->firstAddrClass();
    while (pool = iface->getAddrClass()) {
        // This is not the pool you are looking for.
        if (!pool->addrInPool(reservedAddr))
            continue;

        T1_ = pool->getT1(req->getT1());
        T2_ = pool->getT2(req->getT2());

        pref = pool->getPref(pref);
        valid = pool->getValid(valid);

        Log(Info) << "Reserved in-pool address " << reservedAddr->getPlain() << " for this client found, assigning." << LogEnd;
        SPtr<TOpt> optAddr = new TSrvOptIAAddress(reservedAddr, pref, valid, Parent);
        SubOptions.append(optAddr);
        
        SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed address.", Parent));
        
        SrvAddrMgr().addClntAddr(ClntDuid, ClntAddr, Iface, IAID_, T1_, T2_, reservedAddr, pref, valid, quiet);
        SrvCfgMgr().addClntAddr(this->Iface, reservedAddr);

        return true;
    }
    
    // This address does not belong to any pool. Assign it anyway
    T1_ = iface->getT1(req->getT1());
    T2_ = iface->getT2(req->getT2());
    pref = iface->getPref(pref);
    valid = iface->getValid(valid);
    Log(Info) << "Reserved out-of-pool address " << reservedAddr->getPlain() << " for this client found, assigning." << LogEnd;
    SPtr<TOpt> optAddr = new TSrvOptIAAddress(reservedAddr, pref, valid, Parent);
    SubOptions.append(optAddr);
    
    SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed address.", Parent));
    SrvAddrMgr().addClntAddr(ClntDuid, ClntAddr, Iface, IAID_, T1_, T2_, reservedAddr, pref, valid, quiet);
    SrvCfgMgr().addClntAddr(this->Iface, reservedAddr);
    
    return true;
}

void TSrvOptIA_NA::releaseAllAddrs(bool quiet) {
    SPtr<TOpt> opt;
    SPtr<TIPv6Addr> addr;
    SPtr<TOptIAAddress> optAddr;
    this->firstOption();
    while ( opt = this->getOption() ) {
        if (opt->getOptType() != OPTION_IAADDR)
            continue;
        optAddr = (Ptr*) opt;
        addr = optAddr->getAddr();
        SrvAddrMgr().delClntAddr(this->ClntDuid, IAID_, addr, quiet);
        SrvCfgMgr().delClntAddr(this->Iface, addr);
    }
}

bool TSrvOptIA_NA::assignAddr(SPtr<TIPv6Addr> addr, uint32_t pref, uint32_t valid, bool quiet) {

    // Try to find a class this address belongs to.
    SPtr<TSrvCfgAddrClass> ptrClass = SrvCfgMgr().getClassByAddr(Iface, addr);
    if (ptrClass) {
        // Found! This is in-pool assignment.
        pref = ptrClass->getPref(pref);
        valid = ptrClass->getValid(valid);

        // configure this IA
        T1_ = ptrClass->getT1(T1_);
        T2_ = ptrClass->getT2(T2_);

    } else {
        // Class not found. This is out-of-pool assignment. The address does not belong
        // to any specified pool, so we need to pick the values from the interface, rather
        // than the pool.
        SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIfaceByID(Iface);
        if (!iface) {
            return false;
        }

        pref = iface->getPref(pref);
        valid = iface->getValid(valid);
        T1_ = iface->getT1(T1_);
        T2_ = iface->getT2(T2_);
    }

    SPtr<TSrvOptIAAddress> optAddr = new TSrvOptIAAddress(addr, pref, valid, this->Parent);

    /// @todo: remove get addr-params
    if (ptrClass && ptrClass->getAddrParams()) {
        Log(Debug) << "Experimental: addr-params subotion added." << LogEnd;
        optAddr->addOption((Ptr*)ptrClass->getAddrParams());
    }

    SubOptions.append((Ptr*)optAddr);

    SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS, "Assigned an address.", Parent));

    Log(Info) << "Client " << ClntDuid->getPlain() << " got " << *addr
	      << " (IAID=" << IAID_ << ", pref=" << pref << ",valid=" << valid << ")." << LogEnd;

    // register this address as used by this client
    SrvAddrMgr().addClntAddr(ClntDuid, ClntAddr, Iface, IAID_, T1_, T2_, addr, pref, valid, quiet);
    SrvCfgMgr().addClntAddr(this->Iface, addr);

    return true;
}

/// @brief tries to find address reserved for this particular client 
///
/// @return fixed address (if found)
SPtr<TIPv6Addr> TSrvOptIA_NA::getExceptionAddr()
{
    SPtr<TSrvCfgIface> ptrIface=SrvCfgMgr().getIfaceByID(Iface);
    if (!ptrIface) {
        return SPtr<TIPv6Addr>();
    }

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(ClntDuid, Parent, false
                                                           /* false = verbose */);
    if (ex)
        return ex->getAddr();

    return SPtr<TIPv6Addr>();
}

// constructor used only in RENEW, REBIND, DECLINE and RELEASE
TSrvOptIA_NA::TSrvOptIA_NA(SPtr<TSrvOptIA_NA> queryOpt,
                 SPtr<TIPv6Addr> clntAddr, SPtr<TDUID> clntDuid,
                 int iface, unsigned long &addrCount, int msgType , TMsg* parent)
    :TOptIA_NA(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    ClntDuid  = clntDuid;
    ClntAddr  = clntAddr;
    Iface     = iface;

    IAID_ = queryOpt->getIAID();

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
    case DECLINE_MSG:
        this->decline(queryOpt, addrCount);
        break;
    default: {
        Log(Warning) << "Unknown message type (" << msgType
                     << "). Cannot generate OPTION_IA_NA."<< LogEnd;
        SubOptions.append(new TOptStatusCode(STATUSCODE_UNSPECFAIL,
                                                "Unknown message type.",this->Parent));
        break;
    }
    }
}

/**
 * generates OPTION_IA_NA based on OPTION_IA_NA received in RENEW message
 *
 * @param queryOpt IA_NA option received from client in the RENEW message
 * @param complainIfMissing specifies if potential warnings should be printed or not
 *
 * @return true - if binding was renewed, false - if not found or invalid
 */
bool TSrvOptIA_NA::renew(SPtr<TSrvOptIA_NA> queryOpt, bool complainIfMissing)
{
    // find that client in addrdb
    SPtr <TAddrClient> ptrClient;
    ptrClient = SrvAddrMgr().getClient(this->ClntDuid);
    if (!ptrClient) {
      if (complainIfMissing) {
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",
                                                this->Parent));
        Log(Info) << "Unable to RENEW binding for IA(iaid=" << queryOpt->getIAID() << ", client="
                  << ClntDuid->getPlain() << ": No such client." << LogEnd;
      }
      return false;
    }

    // find that IA
    SPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(IAID_);
    if (!ptrIA) {
      if (complainIfMissing) {
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",
                                                this->Parent ));
        Log(Info) << "Unable to RENEW binding for IA(iaid=" << queryOpt->getIAID() << ", client="
                  << ClntDuid->getPlain() << ": No such IA." << LogEnd;
      }
      return false;
    }

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    T1_ = ptrIA->getT1();
    T2_ = ptrIA->getT2();

    // send addr info to client
    SPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SPtr<TOptIAAddress> optAddr;
        ptrAddr->setTimestamp();
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),ptrAddr->getValid(),
                                       this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SPtr<TOptStatusCode> ptrStatus;
    ptrStatus = new TOptStatusCode(STATUSCODE_SUCCESS,"Address(es) renewed. Greetings from planet Earth",this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );

    return true;
}

void TSrvOptIA_NA::rebind(SPtr<TSrvOptIA_NA> queryOpt,
                          unsigned long &addrCount)
{
    // find that client in addrdb
    SPtr <TAddrClient> ptrClient = SrvAddrMgr().getClient(this->ClntDuid);
    if (!ptrClient) {
        // hmmm, that's not our client
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,
                                                "Who are you? Do I know you?",this->Parent ));
        return;
    }

    // find that IA
    SPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(IAID_);
    if (!ptrIA) {
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,
                                                "I see this IAID first time.",this->Parent ));
        return;
    }

    /// @todo: 18.2.4 par. 3 (check if addrs are appropriate for this link)

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    T1_ = ptrIA->getT1();
    T2_ = ptrIA->getT2();

    // send addr info to client
    SPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SPtr<TOptIAAddress> optAddr;
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),
                                       ptrAddr->getValid(),this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SPtr<TOptStatusCode> ptrStatus;
    ptrStatus = new TOptStatusCode(STATUSCODE_SUCCESS,"Greetings from planet Earth",
                                      this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_NA::release(SPtr<TSrvOptIA_NA> queryOpt,
                           unsigned long &addrCount) {
}

void TSrvOptIA_NA::decline(SPtr<TSrvOptIA_NA> queryOpt,
                           unsigned long &addrCount)
{
}

bool TSrvOptIA_NA::doDuties()
{
    return true;
}

/// @brief Checks if address is sane and assignable.
///
/// Verifies that requested address is sane (i.e. no anyaddr, not link-local,
/// not multicast etc.) and that it is within supported pool
///
/// @param clientReq 
/// @param hint 
///
/// @return true if it is sane and assignable
SPtr<TIPv6Addr> TSrvOptIA_NA::getAddressHint(SPtr<TSrvMsg> clientReq, SPtr<TIPv6Addr> hint) {

    SPtr<TSrvCfgIface> ptrIface;
    SPtr<TIPv6Addr>    addr;
    ptrIface = SrvCfgMgr().getIfaceByID(this->Iface);
    if (!ptrIface) {
        Log(Error) << "Trying to find free address on non-existent interface (id=%d)\n"
                   << Iface << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }

    // check if this address is ok

    // is it anyaddress (::)?
    SPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
    if (*anyaddr==*hint) {
        Log(Debug) << "Client requested unspecified (" << *hint
           << ") address. Hint ignored." << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }

    // is it multicast address (ff...)?
    if ((*(hint->getAddr()))==0xff) {
        Log(Debug) << "Client requested multicast (" << *hint
                   << ") address. Hint ignored." << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }

    // is it link-local address (fe80::...)?
    char linklocal[]={0xfe, 0x80};
    if (!memcmp(hint->getAddr(),linklocal,2)) {
	Log(Debug) << "Client requested link-local (" << *hint << ") address. Hint ignored."
                   << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }

    SPtr<TSrvCfgAddrClass> ptrClass;
    ptrClass = SrvCfgMgr().getClassByAddr(this->Iface, hint);

    if (!ptrClass)
        return SPtr<TIPv6Addr>(); // NULL

    if ( !ptrClass->clntSupported(ClntDuid, ClntAddr, clientReq) )
        return SPtr<TIPv6Addr>(); // NULL

    // If the Class is valid and support the Client (based on duid, addr, clientclass)

    // best case: address belongs to supported class, and is free
    if ( SrvAddrMgr().addrIsFree(hint) ) {
        Log(Debug) << "Requested address (" << *hint << ") is free, great!" << LogEnd;
        return hint;
    }

    // medium case: addess belongs to supported class, but is used
    // however the class pool is still free
    unsigned long assignedCnt = ptrClass->getAssignedCount();
    unsigned long classMaxLease = ptrClass->getClassMaxLease();
    
    if (assignedCnt >=classMaxLease) {
        Log(Debug) << "Requested address (" << *hint
                   << ") belongs to supported class, which has reached its limit ("
                   << assignedCnt << " assigned, "
                   << classMaxLease << " max lease)." << LogEnd;
        // this class is useless
    } else {
        Log(Debug) << "Requested address (" << *hint
                   << ") belongs to supported class, but is used." << LogEnd;
        do {
            addr = ptrClass->getRandomAddr();
        } while (!SrvAddrMgr().addrIsFree(addr));
        return addr;
    }

    return SPtr<TIPv6Addr>(); // NULL
}

bool TSrvOptIA_NA::assignRandomAddr(SPtr<TSrvMsg> queryMsg, bool quiet) {
    // worst case: address does not belong to supported class
    // or specified hint is invalid (or there was no hint at all)
    SPtr<TIPv6Addr> candidate;
    SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIfaceByID(Iface);
    if (!iface) {
        Log(Error) << "Failed to find interface with ifindex=" << Iface << LogEnd;
        return false;
    }
    SPtr<TSrvCfgAddrClass> pool = iface->getRandomClass(ClntDuid, ClntAddr);

    if (!pool) {
        Log(Warning) << "Unable to find any suitable (allowed, non-full) class for this client." << LogEnd;
        return 0;
    }

    if (pool->clntSupported(ClntDuid, ClntAddr, queryMsg) &&
        pool->getAssignedCount() < pool->getClassMaxLease() ) {

        int safety = 0;

        while (safety < SERVER_MAX_IA_RANDOM_TRIES) {
            candidate = pool->getRandomAddr();

            if (SrvAddrMgr().addrIsFree(candidate) && !SrvCfgMgr().addrReserved(candidate))
                break;

            safety++;
        }
        if (safety < SERVER_MAX_IA_RANDOM_TRIES) {
            return assignAddr(candidate, pool->getPref(), pool->getValid(), quiet);
        } else {
            Log(Error) << "Unable to randomly choose address after " << SERVER_MAX_IA_RANDOM_TRIES << " tries." << LogEnd;
            return false;
        }
    }
    return false;
}


bool TSrvOptIA_NA::assignSequentialAddr(SPtr<TSrvMsg> clientMsg, bool quiet) {
    // Random pool failed. That really should not happen. We are most probably not supporting
    // this client or are completely out of leases. Last chance: iterate over all pools and
    // find suitable lease:
    SPtr<TSrvCfgIface> ptrIface=SrvCfgMgr().getIfaceByID(Iface);
    if (!ptrIface) {
	return 0;
    }

    SPtr<TSrvCfgAddrClass> pool;
    ptrIface->firstAddrClass();
    while (pool = ptrIface->getAddrClass()) {
        if (!pool->clntSupported(ClntDuid, ClntAddr, clientMsg))
            continue;
        if (pool->getAssignedCount() >= pool->getClassMaxLease())
            continue;
        break;

        SPtr<TIPv6Addr> candidate = pool->getFirstAddr();
        for (candidate = pool->getFirstAddr(); candidate != pool->getLastAddr(); ++(*candidate)) {
            if (!SrvAddrMgr().addrIsFree(candidate))
                continue;
            if (assignAddr(candidate, pool->getPref(), pool->getValid(), quiet))
                return true;
        }
    }

    // That is definite failure. I have iterated over every address in every pool
    // and all of them are not usable for one reason or ther other. I finally
    // give up.
    return false;
}
