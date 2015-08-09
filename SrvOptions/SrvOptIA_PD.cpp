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

#include <sstream>
#include "SrvOptIA_PD.h"
#include "SrvOptIAPrefix.h"
#include "OptStatusCode.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPDefaults.h"
#include "Msg.h"
#include "SrvCfgMgr.h"

using namespace std;

TSrvOptIA_PD::TSrvOptIA_PD(uint32_t iaid, uint32_t t1, uint32_t t2, int Code,
                           const std::string& Text, TMsg* parent)
    :TOptIA_PD(iaid, t1, t2, parent), PDLength(0) {
    Iface = parent->getIface();
    SubOptions.append(new TOptStatusCode(Code, Text, parent));
}

TSrvOptIA_PD::TSrvOptIA_PD(uint32_t iaid, uint32_t t1, uint32_t t2, TMsg* parent)
    :TOptIA_PD(iaid, t1, t2, parent), PDLength(0) {
    if (parent) {
        Iface = parent->getIface();
    } else {
        Iface = -1;
    }
}

/*
 * Create IA_PD option based on receive buffer
 */
TSrvOptIA_PD::TSrvOptIA_PD(char * buf, int bufsize, TMsg* parent)
    :TOptIA_PD(buf, bufsize, parent), PDLength(0) {
    int pos=0;

    Iface = parent->getIface();

    /// @todo: implement unpack
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
        SrvAddrMgr().delPrefix(ClntDuid, IAID_, prefix, quiet);
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
    SPtr<TAddrIA> pd = client->getPD(IAID_);
    if (!pd)
        return false;
    if (!pd->countPrefix())
        return false;

    SPtr<TAddrPrefix> prefix;
    pd->firstPrefix();
    while (prefix = pd->getPrefix()) {
        Log(Debug) << "Assinging existing lease: prefix=" << prefix->get()->getPlain() << "/"
                   << prefix->getLength() << ", pref=" << prefix->getPref() << ", valid="
                   << prefix->getValid() << LogEnd;
        SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(prefix->get(), prefix->getLength(),
                                                   prefix->getPref(), prefix->getValid(),
                                                   this->Parent);
        SubOptions.append((Ptr*)optPrefix);
    }

    T1_ = pd->getT1();
    T2_ = pd->getT2();

    stringstream status;
    status << "Here's your existing lease: " << countPrefixes() << " prefix(es).";
    SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS, status.str(), this->Parent) );
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
        cached = SrvAddrMgr().getCachedEntry(ClntDuid, IATYPE_PD);
        if (cached)
            hint = cached;
    }

    // Get the list of prefixes
    prefixLst.clear();
    prefixLst = getFreePrefixes(clientMsg, hint);
    ostringstream buf;
    prefixLst.first();
    while (prefix = prefixLst.get()) {
        buf << prefix->getPlain() << "/" << this->PDLength << " ";
        optPrefix = new TSrvOptIAPrefix(prefix, (char)this->PDLength, this->Prefered, this->Valid,
                                        this->Parent);
        SubOptions.append((Ptr*)optPrefix);

        // We do actual reservation here, even if it is SOLICIT. For SOLICIT, we will release
        // the prefix before sending ADVERTISE. We need to do this. Otherwise we could start
        // sending duplicate prefixes if client requested multiple IA_PDs.
        SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(Iface);
        if (!cfgIface) {
            Log(Error) << "Missing configuration interface with ifindex=" << Iface << LogEnd;
            return false;
        }

        // every prefix has to be remembered in AddrMgr, e.g. when there are 2 pools defined,
        // prefixLst contains entries from each pool, so 2 prefixes has to be remembered
        SrvAddrMgr().addPrefix(this->ClntDuid, this->ClntAddr, cfgIface->getName(),
                               Iface, IAID_, T1_, T2_, prefix, Prefered, Valid,
                               this->PDLength, false);

        // Increase prefix pool usage counter
        SrvCfgMgr().incrPrefixCount(Iface, prefix);
    }
    Log(Info) << "PD:" << (fake?"(would be)":"") << " assigned prefix(es):" << buf.str() << LogEnd;

    if (prefixLst.count()) {
        stringstream tmp;
        tmp << "Assigned " << prefixLst.count() << " prefix(es).";
        SubOptions.append( new TOptStatusCode(STATUSCODE_SUCCESS, tmp.str(), Parent) );
        return true;
    }

    SubOptions.append( new TOptStatusCode(STATUSCODE_NOPREFIXAVAIL,
                                             "Unable to provide any prefixes. Sorry.", this->Parent) );
    return false;
}

/// @brief constructor used in replies to SOLICIT, REQUEST, RENEW, REBIND, DECLINE and RELEASE
///
/// @param clientMsg client message that server responds to
/// @param queryOpt IA_PD option from client message
/// @param parent message that we are currently building
TSrvOptIA_PD::TSrvOptIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt, TMsg* parent)
    :TOptIA_PD(queryOpt->getIAID(), queryOpt->getT1(), queryOpt->getT2(), parent)
{
    int msgType = clientMsg->getType();
    ClntDuid  = clientMsg->getClientDUID();
    ClntAddr  = clientMsg->getRemoteAddr();
    Iface     = clientMsg->getIface();

    SPtr<TSrvCfgIface> ptrIface = SrvCfgMgr().getIfaceByID(Iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex="
                   << Iface << ". Something is wrong, VERY wrong." << LogEnd;
        return;
    }

    // is the prefix delegation supported?
    if ( !ptrIface->supportPrefixDelegation() ) {
        SPtr<TOptStatusCode> ptrStatus;
        ptrStatus = new TOptStatusCode(STATUSCODE_NOPREFIXAVAIL,
                    "Server support for prefix delegation is not enabled. Sorry buddy.",
                                       Parent);
        this->SubOptions.append((Ptr*)ptrStatus);
        return;
    }

    bool fake  = false; // is this assignment for real?
    if (msgType == SOLICIT_MSG)
        fake = true;

    if (parent->getType() == REPLY_MSG)
        fake = false;

    switch (msgType) {
    case SOLICIT_MSG:
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
        SubOptions.append(new TOptStatusCode(STATUSCODE_UNSPECFAIL,
                                             "Unknown message type.",this->Parent));
        break;
    }
    }
}

/**
 * this method is used to prepare response to IA_PD received in SOLICIT and REQUEST messages
 * (i.e. it tries to assign prefix as requested by client)
 *
 * @param clientMsg original message received from a client
 * @param queryOpt specific option in clientMsg that we are answering now
 * @param ptrIface pointer to SrvCfgIface
 * @param fake is this fake request? (fake = solicit, so nothing is actually assigned)
 */
void TSrvOptIA_PD::solicitRequest(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt,
                                  SPtr<TSrvCfgIface> ptrIface, bool fake) {

    // --- Is this PD without IAPREFIX options? ---
    SPtr<TIPv6Addr> hint;
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
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOPREFIXAVAIL,tmp.str(), Parent));
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
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",
                                                this->Parent));
        return;
    }

    // find that IA
    SPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getPD(IAID_);
    if (!ptrIA) {
        SubOptions.append(new TOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",
                                                this->Parent ));
        return;
    }

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    T1_ = ptrIA->getT1();
    T2_ = ptrIA->getT2();

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
    SPtr<TOptStatusCode> ptrStatus;
    ptrStatus = new TOptStatusCode(STATUSCODE_SUCCESS,"Prefix(es) renewed.", this->Parent);
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
    SubOptions.append(new TOptStatusCode(STATUSCODE_NOPREFIXAVAIL, "You tried to decline a prefix. Are you crazy?", Parent));
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

    SPtr<TSrvCfgOptions> ex = ptrIface->getClientException(ClntDuid, Parent, false/* = verbose */);
    if (!ex)
        return false;

    SPtr<TIPv6Addr> reservedPrefix = ex->getPrefix();
    if (!reservedPrefix) {
        // there's no reserved prefix for this lad
        return false;
    }

    // we've got fixed prefix, yay! Let's try to calculate its preferred and valid lifetimes
    SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIfaceByID(Iface);
    if (!iface) {
        // this should never happen
        Log(Error) << "Unable to find interface with ifindex=" << Iface << " in SrvCfgMgr."
                   << LogEnd;
        return false;
    }

    // if the lease is not within normal range, treat it as fixed, infinite one
    uint32_t pref = DHCPV6_INFINITY;
    uint32_t valid = DHCPV6_INFINITY;
    SPtr<TSrvOptIAPrefix> hint = (Ptr*) req->getOption(OPTION_IAPREFIX);
    if (hint) {
        pref = hint->getPref();
        valid = hint->getValid();
    }

    SPtr<TSrvCfgPD> pool;
    iface->firstPD();
    while (pool = iface->getPD()) {
        // This is not the pool you are looking for.
        if (!pool->prefixInPool(reservedPrefix))
            continue;

        // we found matching pool! Yay!
        T1_ = pool->getT1(req->getT1());
        T2_ = pool->getT2(req->getT2());

        pref = pool->getPrefered(pref);
        valid = pool->getValid(valid);

        Log(Info) << "Reserved in-pool prefix " << reservedPrefix->getPlain() << "/"
                  << static_cast<unsigned int>(ex->getPrefixLen())
                  << " for this client found, assigning." << LogEnd;
        SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(reservedPrefix, ex->getPrefixLen(),
                                                   pref, valid, Parent);
        SubOptions.append(optPrefix);

        SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed in-pool prefix.",
                                             Parent));

        SrvAddrMgr().addPrefix(ClntDuid, this->ClntAddr, iface->getName(), Iface, IAID_,
                               T1_, T2_, reservedPrefix, pref, valid, ex->getPrefixLen(), false);

        // but CfgMgr has to increase usage only once. Don't ask my why :)
        SrvCfgMgr().incrPrefixCount(Iface, reservedPrefix);

        return true;
    }

    // This address does not belong to any pool. Assign it anyway
    T1_ = iface->getT1(req->getT1());
    T2_ = iface->getT2(req->getT2());
    pref = iface->getPref(pref);
    valid = iface->getValid(valid);

    Log(Info) << "Reserved out-of-pool address " << reservedPrefix->getPlain()
              << static_cast<unsigned int>(ex->getPrefixLen())
              << " for this client found, assigning." << LogEnd;
    SPtr<TOpt> optPrefix = new TSrvOptIAPrefix(reservedPrefix, ex->getPrefixLen(), pref, valid,
                                               Parent);
    SubOptions.append(optPrefix);

    SubOptions.append(new TOptStatusCode(STATUSCODE_SUCCESS,"Assigned fixed out-of-pool address.",
                                         Parent));

    SrvAddrMgr().addPrefix(ClntDuid, this->ClntAddr, iface->getName(), Iface, IAID_,
                           T1_, T2_, reservedPrefix, pref, valid, ex->getPrefixLen(), false);

    return true;
}

/**
 * @brief returns list of free prefixes for this client
 *
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
 * @param clientMsg message received from a client
 * @param cli_hint hint provided by client (or ::)
 *
 * @return - list of prefixes
 */
List(TIPv6Addr) TSrvOptIA_PD::getFreePrefixes(SPtr<TSrvMsg> clientMsg, SPtr<TIPv6Addr> cli_hint) {

    SPtr<TSrvCfgIface> ptrIface;
    SPtr<TIPv6Addr>    prefix;
    SPtr<TSrvCfgPD>    ptrPD;
    bool validHint = true;
    List(TIPv6Addr) lst;

    lst.clear();
    ptrIface = SrvCfgMgr().getIfaceByID(this->Iface);
    if (!ptrIface) {
      Log(Error) << "PD: Trying to find free prefix on non-existent interface (ifindex="
                 << this->Iface << ")." << LogEnd;
      return lst; // empty list
    }

    if (!ptrIface->supportPrefixDelegation()) {
      // this method should not be called anyway
      Log(Error) << "PD: Prefix delegation is not supported on the " << ptrIface->getFullName()
                 << "." << LogEnd;
      return lst; // empty list
    }

    ptrIface->firstPD();
    ptrPD = ptrIface->getPD();
    // should be ==, asigned>total should never happen
    if (ptrPD->getAssignedCount() >= ptrPD->getTotalCount()) {
      Log(Error) << "PD: Unable to grant any prefixes: Already asigned " << ptrPD->getAssignedCount()
                 << " out of " << ptrPD->getTotalCount() << "." << LogEnd;
      return lst; // empty list
    }

    // check if this prefix is ok

    // is it anyaddress (::)?
    SPtr<TIPv6Addr> anyaddr = new TIPv6Addr();
    if (*anyaddr==*cli_hint) {
        Log(Debug) << "PD: Client requested unspecified (" << *cli_hint
                   << ") prefix. Hint ignored." << LogEnd;
        validHint = false;
    }

    // is it multicast address (ff...)?
    if ((*(cli_hint->getAddr()))==0xff) {
        Log(Debug) << "PD: Client requested multicast (" << *cli_hint
                   << ") prefix. Hint ignored." << LogEnd;
        validHint = false;
    }

    // is it link-local address (fe80::...)?
    char linklocal[]={0xfe, 0x80};
    if (!memcmp(cli_hint->getAddr(),linklocal,2)) {
        Log(Debug) << "PD: Client requested link-local (" << *cli_hint
                   << ") prefix. Hint ignored." << LogEnd;
        validHint = false;
    }

    SPtr<TOptVendorData> remoteID;
    TSrvMsg * par = (TSrvMsg*)(Parent);
    if (par) {
        remoteID = par->getRemoteID();
    }

    if ( validHint ) {
        // hint is valid, try to use it
        ptrPD = SrvCfgMgr().getClassByPrefix(this->Iface, cli_hint);

        // if the PD allow the hint, based on DUID, Addr, and Msg from client
        if (ptrPD && ptrPD->clntSupported(ClntDuid, ClntAddr, clientMsg)) {

            // Let's make a copy of the hint (we may need to tweak the hint in a second)
            SPtr<TIPv6Addr> hint(new TIPv6Addr(cli_hint->getAddr()));

            // Now zero the remaining part
            hint->truncate(0, ptrPD->getPD_Length());
            
            // Is this hint reserved for someone else?
            if (!ptrIface->checkReservedPrefix(hint, ClntDuid, remoteID, ClntAddr))
            {
                // Nope, not reserved.

                // case 2: address belongs to supported class, and is free
                if ( SrvAddrMgr().prefixIsFree(hint) ) {
                    Log(Debug) << "PD: Requested prefix (" << *hint << ") is free, great!" << LogEnd;
                    this->PDLength = ptrPD->getPD_Length();
                    this->Prefered = ptrPD->getPrefered(this->Prefered);
                    this->Valid    = ptrPD->getValid(this->Valid);
                    T1_       = ptrPD->getT1(T1_);
                    T2_       = ptrPD->getT2(T2_);
                    lst.append(hint);
                    return lst;
                } else {

                    // case 3: hint is used, but we can assign another prefix from the same pool
                    do {
                        prefix=ptrPD->getRandomPrefix();
                    } while (!SrvAddrMgr().prefixIsFree(prefix));
                    lst.append(prefix);

                    this->PDLength = ptrPD->getPD_Length();
                    this->Prefered = ptrPD->getPrefered(this->Prefered);
                    this->Valid    = ptrPD->getValid(this->Valid);
                    T1_       = ptrPD->getT1(T1_);
                    T2_       = ptrPD->getT2(T2_);
                    return lst;
                } // if hint is used
            } // if this hint is reserved for someone?
        } // if client is supported at all
    } // if this is a valid hint

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

    int attempts = SERVER_MAX_PD_RANDOM_TRIES;
    while (attempts--) {
        List(TIPv6Addr) lst;
        lst = ptrPD->getRandomList();
        lst.first();
        bool allFree = true;
        while (prefix = lst.get()) {
            if (!SrvAddrMgr().prefixIsFree(prefix) || SrvCfgMgr().prefixReserved(prefix)) {
                allFree = false;
            }
        }
        if (allFree) {
            this->PDLength = ptrPD->getPD_Length();
            this->Prefered = ptrPD->getPrefered(this->Prefered);
            this->Valid    = ptrPD->getValid(this->Valid);
            T1_       = ptrPD->getT1(T1_);
            T2_       = ptrPD->getT2(T2_);
            return lst;
        }
    };

    // failed to find available prefixes after 100 attempts. Return empty list
    return List(TIPv6Addr)();
}
