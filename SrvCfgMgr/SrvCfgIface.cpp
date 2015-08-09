/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <cstdlib>
#include <sstream>
#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgPD.h"
#include "Logger.h"
#include "Opt.h"
#include "SrvMsg.h"
#include "DNSUpdate.h"

using namespace std;

void TSrvCfgIface::addClientExceptionsLst(List(TSrvCfgOptions) exLst)
{
    Log(Debug) << exLst.count() << " per-client configurations (exceptions) added." << LogEnd;
    ExceptionsLst_ = exLst;
}

bool TSrvCfgIface::leaseQuerySupport() const
{
    return LeaseQuery_;
}

SPtr<TSrvCfgOptions> TSrvCfgIface::getClientException(SPtr<TDUID> duid,
                                                      TMsg * parent, bool quiet) {

    SPtr<TOptVendorData> remoteID;
    TSrvMsg* par = (TSrvMsg*)(parent);
    SPtr<TIPv6Addr> peer;
    if (par) {
        remoteID = par->getRemoteID();
        peer = par->getClientPeer();
        Log(Debug) << "Checking exceptions for link-local=" << peer->getPlain() << LogEnd;
    }

    SPtr<TSrvCfgOptions> x;
    ExceptionsLst_.first();
    while (x = ExceptionsLst_.get()) {
        if ( duid && x->getDuid() && (*(x->getDuid()) == *duid) ) {
            if (!quiet)
                Log(Debug) << "Found per-client configuration (exception) for client with DUID="
                           << x->getDuid()->getPlain() << LogEnd;
            return x;
        }
        SPtr<TOptVendorData> remoteid;
        remoteid = x->getRemoteID();

        if ( remoteID && remoteid && (remoteID->getVendor() == remoteid->getVendor())
             && (remoteid->getVendorDataLen() == remoteID->getVendorDataLen())
             && !memcmp(remoteid->getVendorData(), remoteID->getVendorData(), remoteid->getVendorDataLen()) ) {
            Log(Debug) << "Found per-client configuration (exception) for client with RemoteID: vendor="
                       << remoteid->getVendor() << ", data="
                       << remoteid->getVendorDataPlain() << "." << LogEnd;
            return x;
        }
        if ( peer && x && x->getClntAddr() && *(peer) == *(x->getClntAddr()) ) {
            Log(Debug) << "Found per-client configuration (exception) for client with link-local="
                       << peer->getPlain() << LogEnd;
            return x;
        }
    }
    return SPtr<TSrvCfgOptions>();
}

/// @brief Checks if address is reserved.
///
/// Iterates over exceptions list and checks if specified address is reserved.
///
/// @param addr Address in question.
///
/// @return True if reserved (false otherwise).
bool TSrvCfgIface::addrReserved(SPtr<TIPv6Addr> addr)
{
    SPtr<TSrvCfgOptions> x;
    ExceptionsLst_.first();
    while (x=ExceptionsLst_.get()) {
        if ( (x->getAddr()) && (*x->getAddr() == *addr) )
            return true;
    }
    return false;
}

/// @brief removes reserved addresses/prefixes from cache
///
/// @return number of removed entries
unsigned int TSrvCfgIface::removeReservedFromCache() {
    unsigned int cnt = 0;
    SPtr<TSrvCfgOptions> x;
    ExceptionsLst_.first();
    while (x=ExceptionsLst_.get()) {
        if (x->getAddr())
            cnt += SrvAddrMgr().delCachedEntry(x->getAddr(), IATYPE_IA);
        if (x->getPrefix())
            cnt += SrvAddrMgr().delCachedEntry(x->getPrefix(), IATYPE_PD);
    }
    return cnt;
}

/// @brief Checks if prefix is reserved.
///
/// Iterates over exceptions list and checks if specified prefix is reserved.
///
/// @param prefix prefix in question.
///
/// @return True if reserved (false otherwise).
bool TSrvCfgIface::prefixReserved(SPtr<TIPv6Addr> prefix)
{
    SPtr<TSrvCfgOptions> x;
    ExceptionsLst_.first();
    while (x=ExceptionsLst_.get()) {
        if (x->getPrefix() && (*x->getPrefix() == *prefix) )
            return true;
    }
    return false;
}

/// @brief Checks if a prefix is reserved for another client.
///
/// @param pfx checked prefix (mandatory)
/// @param duid Client's duid (mandatory)
/// @param myRemoteID (can be NULL)
/// @param linkLocal (can be NULL)
///
/// @return true if reserved for some else, false = not reserved
bool TSrvCfgIface::checkReservedPrefix(SPtr<TIPv6Addr> pfx, SPtr<TDUID> duid,
                                       SPtr<TOptVendorData> myRemoteID,
                                       SPtr<TIPv6Addr> linkLocal) {
    // sanity check
    if (!pfx || !duid) {
        // should not happen
        Log(Error) << "Reservation check failed. Required parameters not specified." << LogEnd;
        return true;
    }

    SPtr<TSrvCfgOptions> x;
    ExceptionsLst_.first();
    Log(Debug) << "Checking prefix " << pfx->getPlain() << " against reservations ... " << LogEnd;
    while (x=ExceptionsLst_.get()) {

        if (!x->getPrefix()) // that is not prefix reservation
            continue;

        if ( *(x->getPrefix()) != (*pfx) )
            continue; // that is not the prefix we are looking for

        // we found the prefix we are looking for. Let's check if we can use it

        // DUID based reservation?
        if (x->getDuid()) {
            if (*duid == *x->getDuid()) {
                return false; // reserved for us!
            } else {
                Log(Debug) << "Prefix " << x->getPrefix()->getPlain() << " is reserved for DUID="
                           << x->getDuid()->getPlain() << LogEnd;
                return true;
            }
        }

        // remote-id based reservation?
        SPtr<TOptVendorData> remoteid = x->getRemoteID();
        if (remoteid) {
            if ( (myRemoteID->getVendor() == remoteid->getVendor()) &&
                 (myRemoteID->getVendorDataLen() == remoteid->getVendorDataLen()) &&
                 (!memcmp(myRemoteID->getVendorData(), remoteid->getVendorData(), remoteid->getVendorDataLen())) ) {
                return false; // reserved for us!
            } else {
                Log(Debug) << "Prefix " << x->getPrefix()->getPlain() << "is reserved for remote-id="
                           << remoteid->getPlain() << LogEnd;
                return true; // no, sorry. It's somebody else's prefix
            }
        }


        // link-local based reservation
        SPtr<TIPv6Addr> addr = x->getClntAddr();
        if (addr) {
            if (*linkLocal == *addr) {
                return false; // reserved for us!
            } else {
                Log(Debug) << "Prefix " << x->getPrefix()->getPlain()
                           << " is reserved for link-local address "
                           << addr->getPlain() << LogEnd;
                return true;
            }
        }

        Log(Error) << "Found reservation for prefix " << x->getPrefix()->getPlain()
                   << ", but it is misconfigured (no DUID, remote-id nor link-local specified)"
                   << LogEnd;

        // this reservation is malformed let's not use it
        return true;
    }
  return false;
}

void TSrvCfgIface::firstAddrClass() {
    SrvCfgAddrClassLst_.first();
}

/// @brief Returns ID of the preferred pool for specified client
///
/// tries to find if there is a class, where client is on white-list
///
/// @param duid client's DUID
/// @param clntAddr client's address
///
/// @return ID of prefered pool (or -1 if there is none)
int TSrvCfgIface::getPreferedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr) {
    SPtr<TSrvCfgAddrClass> ptrClass;
    SrvCfgAddrClassLst_.first();
    while(ptrClass=SrvCfgAddrClassLst_.get()) {
        if (ptrClass->clntPrefered(duid, clntAddr)) {
            return ptrClass->getID();
        }
    }
    return -1;
}

/// tries to find a class, which client is allowed to use
///
/// @param duid client's DUID
/// @param clntAddr client's linkaddress
///
/// @return classid (or -1 if no suitable class is found)
int TSrvCfgIface::getAllowedAddrClassID(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr) {
    unsigned int clsid[100];
    unsigned int share[100];
    unsigned int cnt = 0;
    unsigned int sum = 0;
    unsigned int rnd;

    /// @todo Buffer overflow for more than 100 classes

    SPtr<TSrvCfgAddrClass> ptrClass;
    SrvCfgAddrClassLst_.first();
    while( (ptrClass=SrvCfgAddrClassLst_.get()) && (cnt<100) ) {
        if (ptrClass->clntSupported(duid, clntAddr) &&
            ptrClass->getClassMaxLease() > ptrClass->getAssignedCount()) {
            clsid[cnt]   = ptrClass->getID();
            share[cnt]   = ptrClass->getShare();
            sum         += ptrClass->getShare();
            cnt++;
        }
    }

    if (!cnt)
        return -1; // this client is not supported by any class

    rnd = rand() % sum;

    unsigned int j = 0;

    for (unsigned int i = 0; i < cnt; i++) {
        j += share[i];
        if (j >= rnd) {
            return clsid[i];
        }
    }

    return clsid[cnt-1];
}


void TSrvCfgIface::firstPD() {
    SrvCfgPDLst_.first();
}

bool TSrvCfgIface::supportPrefixDelegation() const {
    return SrvCfgPDLst_.count();
}

void TSrvCfgIface::addTA(SPtr<TSrvCfgTA> ta) {
    SrvCfgTALst_.append(ta);
}

void TSrvCfgIface::firstTA() {
    SrvCfgTALst_.first();
}
SPtr<TSrvCfgTA> TSrvCfgIface::getTA() {
    return SrvCfgTALst_.get();
}

void TSrvCfgIface::addPD(SPtr<TSrvCfgPD> pd) {
    SrvCfgPDLst_.append(pd);
}

SPtr<TSrvCfgTA> TSrvCfgIface::getTA(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr) {
    SPtr<TSrvCfgTA> ta;

    // try to find preferred TA for this client
    SrvCfgTALst_.first();
    while ( ta = getTA() ) {
        if (ta->clntPrefered(clntDuid, clntAddr))
            return ta;
    }

    // prefered not found? Then find first allowed
    SrvCfgTALst_.first();
    while ( ta = getTA() ) {
        if (ta->clntSupported(clntDuid, clntAddr))
            return ta;
    }

    return SPtr<TSrvCfgTA>(); // NULL
}

SPtr<TSrvCfgAddrClass> TSrvCfgIface::getAddrClass() {
    return SrvCfgAddrClassLst_.get();
}

SPtr<TSrvCfgAddrClass> TSrvCfgIface::getClassByID(unsigned long id) {
    firstAddrClass();
    SPtr<TSrvCfgAddrClass> ptrClass;
    while (ptrClass = getAddrClass()) {
        if (ptrClass->getID() == id)
            return ptrClass;
    }
    return SPtr<TSrvCfgAddrClass>(); // NULL
}

void TSrvCfgIface::addClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet /* =false*/) {
    SPtr<TSrvCfgAddrClass> ptrClass;
    firstAddrClass();
    while (ptrClass = getAddrClass() ) {
        if (ptrClass->addrInPool(ptrAddr)) {
            unsigned int count = ptrClass->incrAssigned();
            if (quiet)
                return;
            Log(Debug) << "Address usage for class " << ptrClass->getID()
                       << " increased to " << count << "." << LogEnd;
            return;
        }
    }
    Log(Warning) << "Unable to increase address usage: no class found for "
                 << *ptrAddr << LogEnd;
}

void TSrvCfgIface::delClntAddr(SPtr<TIPv6Addr> ptrAddr, bool quiet /* =false*/) {
    SPtr<TSrvCfgAddrClass> ptrClass;
    firstAddrClass();
    while (ptrClass = getAddrClass() ) {
        if (ptrClass->addrInPool(ptrAddr)) {
            unsigned long count = ptrClass->decrAssigned();
            if (quiet)
                return;
            Log(Debug) << "Address usage for class " << ptrClass->getID()
                       << " decreased to " << count << "." << LogEnd;
            return;
        }
    }
    Log(Warning) << "Unable to decrease address usage: no class found for "
                 << *ptrAddr << LogEnd;
}

SPtr<TSrvCfgAddrClass> TSrvCfgIface::getRandomClass(SPtr<TDUID> clntDuid,
                                                    SPtr<TIPv6Addr> clntAddr) {
    long classid;

    // step 1: Is there a class reserved for this client?

    // if there is class where client is on whitelist, it should be used rather than any other class
    // that would be also suitable
    classid = getPreferedAddrClassID(clntDuid, clntAddr);
    if(classid > -1) {
        Log(Debug) << "Found prefered class " << classid << " for client (duid = " << *clntDuid << ", addr = "
                   << *clntAddr << ")" << LogEnd;
        return getClassByID(classid);
    }

    // Get one of the normal classes
    classid = getAllowedAddrClassID(clntDuid, clntAddr);
    if(classid > -1) {
        Log(Debug) << "Prefered class for client not found, using classid=" << classid << "." << LogEnd;
        return getClassByID(classid);
    }

    // This is some kind of problem...
    // we are out of addresses, or we really don't like this client
    Log(Warning) << "No class is available for client (duid=" << clntDuid->getPlain() << ", addr="
                 << clntAddr->getPlain() << ")." << LogEnd;
    return SPtr<TSrvCfgAddrClass>(); // NULL
}

long TSrvCfgIface::countAddrClass() const {
    return SrvCfgAddrClassLst_.count();
}

SPtr<TSrvCfgPD> TSrvCfgIface::getPD() {
    return SrvCfgPDLst_.get();
}

SPtr<TSrvCfgPD> TSrvCfgIface::getPDByID(unsigned long id) {
    firstPD();
    SPtr<TSrvCfgPD> ptrPD;
    while (ptrPD = getPD()) {
        if (ptrPD->getID() == id)
            return ptrPD;
    }
    return SPtr<TSrvCfgPD>(); // NULL
}

bool TSrvCfgIface::addClntPrefix(SPtr<TIPv6Addr> ptrAddr, bool quiet /* =false */) {
    SPtr<TSrvCfgPD> ptrPD;
    firstPD();
    while (ptrPD = getPD() ) {
        if (ptrPD->prefixInPool(ptrAddr)) {
            unsigned long count = ptrPD->incrAssigned();
            if (quiet)
                return true;
            Log(Debug) << "PD: Prefix usage for class " << ptrPD->getID()
                       << " increased to " << count << "." << LogEnd;
            return true;
        }
    }
    Log(Warning) << "Unable to increase prefix usage: no prefix found for "
                 << *ptrAddr << LogEnd;
    return false;
}

bool TSrvCfgIface::delClntPrefix(SPtr<TIPv6Addr> ptrAddr, bool quiet /* =false */) {
    SPtr<TSrvCfgPD> ptrPD;
    firstPD();
    while (ptrPD = getPD() ) {
        if (ptrPD->prefixInPool(ptrAddr)) {
            unsigned long count = ptrPD->decrAssigned();
            if (quiet)
                return true;
            Log(Debug) << "PD: Prefix usage for class " << ptrPD->getID()
                       << " decreased to " << count << "." << LogEnd;
            return true;
        }
    }
    Log(Warning) << "Unable to decrease address usage: no class found for "
                 << *ptrAddr << LogEnd;
    return false;
}

long TSrvCfgIface::countPD() const {
    return SrvCfgPDLst_.count();
}


int TSrvCfgIface::getID() const {
    return ID_;
}

string TSrvCfgIface::getName() const {
    return Name_;
}

string TSrvCfgIface::getFullName() const {
    ostringstream oss;
    oss << ID_;
    return string(Name_) + "/" + oss.str();
}

SPtr<TIPv6Addr> TSrvCfgIface::getUnicast() {
    return Unicast_;
}


TSrvCfgIface::~TSrvCfgIface() {
}

void TSrvCfgIface::setOptions(SPtr<TSrvParsGlobalOpt> opt) {
    // default options
    Preference_ = opt->getPreference();
    IfaceMaxLease_ = opt->getIfaceMaxLease();
    ClntMaxLease_  = opt->getClntMaxLease();
    RapidCommit_   = opt->getRapidCommit();
    Unicast_       = opt->getUnicast();
    LeaseQuery_    = opt->getLeaseQuerySupport();

    T1Min_    = opt->getT1Beg();
    T1Max_    = opt->getT1End();
    T2Min_    = opt->getT2Beg();
    T2Max_    = opt->getT2End();
    PrefMin_  = opt->getPrefBeg();
    PrefMax_  = opt->getPrefEnd();
    ValidMin_ = opt->getValidBeg();
    ValidMax_ = opt->getValidEnd();

    if (opt->supportFQDN()){
        UnknownFQDN_ = opt->getUnknownFQDN();
        FQDNDomain_  = opt->getFQDNDomain();

        setFQDNLst(opt->getFQDNLst());
        FQDNMode_ = opt->getFQDNMode();

        setRevDNSZoneRootLength(opt->getRevDNSZoneRootLength());
        Log(Debug) <<"FQDN: Support is enabled on the " << getFullName()  << " interface." << LogEnd;
        Log(Debug) <<"FQDN: Mode set to " << getFQDNMode() << ": ";
        switch (getFQDNMode()) {
        case DNSUPDATE_MODE_NONE:
            Log(Cont) << "server will not perform any updates." << LogEnd;
            break;
        case DNSUPDATE_MODE_PTR:
            Log(Cont) << "server will perform reverse (PTR) update only." << LogEnd;
            break;
        case DNSUPDATE_MODE_BOTH:
            Log(Cont) << "server will perform both (AAAA and PTR) updates." << LogEnd;
        }
        Log(Debug) <<"FQDN: RevDNS zoneroot lenght set to " << getRevDNSZoneRootLength()<< "." << LogEnd;
    }

    if (opt->isRelay()) {
        Relay_ = true;
        RelayName_        = opt->getRelayName();
        RelayID_          = opt->getRelayID();
        RelayInterfaceID_ = opt->getRelayInterfaceID();
    } else {
        Relay_ = false;
        RelayName_ = "";
        RelayID_ = -1;
        RelayInterfaceID_.reset();
    }

    TSrvCfgOptions::setOptions(opt);
}

TSrvCfgIface::TSrvCfgIface(int ifindex) {
    setDefaults();
    ID_ = ifindex;
}

TSrvCfgIface::TSrvCfgIface(const std::string& ifaceName) {
    setDefaults();
    Name_ = ifaceName;
}

void TSrvCfgIface::setDefaults() {
    NoConfig_ = false;
    Name_ = "[unknown]";
    ID_ = -1;
    Relay_ = false;
    RevDNSZoneRootLength_ = SERVER_DEFAULT_DNSUPDATE_REVDNS_ZONE_LEN;
    RelayID_ = -1;
    Preference_ = 0;

    IfaceMaxLease_ = SERVER_DEFAULT_IFACEMAXLEASE;
    ClntMaxLease_ = SERVER_DEFAULT_CLNTMAXLEASE;
    RapidCommit_ = SERVER_DEFAULT_RAPIDCOMMIT;
    LeaseQuery_ = SERVER_DEFAULT_LEASEQUERY;

    FQDNMode_ = DNSUPDATE_MODE_NONE;
    UnknownFQDN_ = SERVER_DEFAULT_UNKNOWN_FQDN;

    T1Min_    = SERVER_DEFAULT_MIN_T1;
    T1Max_    = SERVER_DEFAULT_MAX_T1;
    T2Min_    = SERVER_DEFAULT_MIN_T2;
    T2Max_    = SERVER_DEFAULT_MAX_T2;
    PrefMin_  = SERVER_DEFAULT_MIN_PREF;
    PrefMax_  = SERVER_DEFAULT_MAX_PREF;
    ValidMin_ = SERVER_DEFAULT_MIN_VALID;
    ValidMax_ = SERVER_DEFAULT_MAX_VALID;
}

void TSrvCfgIface::setNoConfig() {
    NoConfig_ = true;
}

unsigned char TSrvCfgIface::getPreference() const {
    return Preference_;
}

void TSrvCfgIface::setName(const std::string& ifaceName) {
    Name_ = ifaceName;
}

void TSrvCfgIface::setID(int ifaceID) {
    ID_ = ifaceID;
}

bool TSrvCfgIface::getRapidCommit() const {
    return RapidCommit_;
}


void TSrvCfgIface::addAddrClass(SPtr<TSrvCfgAddrClass> addrClass) {
    SrvCfgAddrClassLst_.append(addrClass);
}

long TSrvCfgIface::getIfaceMaxLease() const {
    return IfaceMaxLease_;
}

unsigned long TSrvCfgIface::getClntMaxLease() const {
    return ClntMaxLease_;
}

string TSrvCfgIface::getRelayName() const {
    return RelayName_;
}

int TSrvCfgIface::getRelayID() const {
    return RelayID_;
}

SPtr<TSrvOptInterfaceID> TSrvCfgIface::getRelayInterfaceID() const {
    return RelayInterfaceID_;
}

bool TSrvCfgIface::isRelay() const {
    return Relay_;
}

void TSrvCfgIface::setRelayName(const std::string& name) {
    RelayName_ = name;
}

void TSrvCfgIface::setRelayID(int id) {
    RelayID_ = id;
}

// --- option: FQDN ---
void TSrvCfgIface::setFQDNLst(List(TFQDN) *fqdn) {
    FQDNLst_ = *fqdn;
}

/**
 * this method tries to find a name for a client. It check client's hint, and possible reservations
 * by duid or by address
 *
 * @param duid
 * @param addr
 * @param hint
 *
 * @return
 */
SPtr<TFQDN> TSrvCfgIface::getFQDNName(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr,
                                      const std::string& hint) {

    SPtr<TFQDN> alternative; // best FQDN found for that client
    SPtr<TFQDN> foo;

    FQDNLst_.first();
    while ( foo = FQDNLst_.get()) {

        if (foo->isUsed()) {
            // client sent a hint, but it is used currently
            if ( (foo->getDuid()) && (*foo->getDuid() == *duid) &&
                 (foo->getAddr()) && (*foo->getAddr() == *addr)) {
                Log(Debug) << "FQDN: This client (DUID=" << duid->getPlain()
                           << ") has already assigned name " << foo->getName()
                           <<" to its address " << foo->getAddr()->getPlain() << "." << LogEnd;
                return foo;
            }

            if ( (foo->getName() == hint) && (*foo->getDuid() == *duid) ) {
                Log(Debug) << "FQDN: Client requested " << hint << ", it is already assinged to this client. Reusing." << LogEnd;
                return foo;
            }
            continue;
        }

        if (duid && (foo->getDuid()) && *(foo->getDuid())== *duid) {
            Log(Debug) << "FQDN found: " << foo->getName() << " using duid " << duid->getPlain() << LogEnd;
            return foo;
        }
        if (addr && (foo->getAddr()) && *(foo->getAddr())==*addr) {
            Log(Debug) << "FQDN found: " << foo->getName() << " using address " << addr->getPlain() << LogEnd;
            return foo;
        }

        if (foo->getName() == hint){
            // client asked for this name. Let's check if client is allowed to get this name.
            if ( (!foo->getDuid()) && (!foo->getAddr()) ) {
                Log(Debug) << "Client's hint: " << hint << " found in fqdn list, setting fqdn to "
                           << foo->getName() << LogEnd;
                return foo;
            }
        }
        if (!foo->getAddr() && !foo->getDuid()) {
            if (!alternative)
                alternative = foo;
        }
    }

    switch (UnknownFQDN_)
    {
    default:
    {
        Log(Error) << "FQDN: Invalid unknown-fqdn mode specified (" << UnknownFQDN_ << ")." << LogEnd;
        return SPtr<TFQDN>(); // NULL
    }
    case UNKNOWN_FQDN_REJECT:
    {
        Log(Info) << "FQDN: Client sent valid hint (" << hint << ") that is not "
                  << "mentioned in server configuration. Server is configured to "
                  << "drop such hints. To accept them, please "
                  << "add 'accept-unknown-fqdn X' in the server.conf (with X>0)." << LogEnd;
        return SPtr<TFQDN>(); // NULL
    }
    case UNKKOWN_FQDN_ACCEPT_POOL:
    {
        if (alternative)
            Log(Info) << "FQDN: Client requested " << hint << ", but assigning other name ("
                      << alternative->getName() << ") from available pool instead." << LogEnd;
        return alternative;
    }
    case UNKNOWN_FQDN_ACCEPT:
    {
        Log(Info) << "FQDN: Accepting unknown (" << hint <<") FQDN requested by client." <<LogEnd;
        SPtr<TFQDN> newEntry = new TFQDN(duid, hint, false);
        FQDNLst_.append(newEntry);
        // Log(Debug) << "Retured FQDN  " << newEntry->getName() <<LogEnd;
        return newEntry;
    }
    case UNKNOWN_FQDN_APPEND:
    {
        string assignedDomain = hint;
        std::string::size_type j = assignedDomain.find(".");
        assignedDomain = assignedDomain.substr(0, j); // chop off anything after first dot

        // sanity check
        for (int k=assignedDomain.length(); k>=0; k--){
            char x = assignedDomain[k];
            if (!isalpha(x) && !isdigit(x) && (x!='-') ) {
                assignedDomain.replace(k,1,""); // remove all inappropriate chars
            }
        }

        assignedDomain += "." + FQDNDomain_;
        SPtr<TFQDN> newEntry = new TFQDN(duid, assignedDomain, false);
        FQDNLst_.append(newEntry);
        Log(Info) << "FQDN: Client requested " << hint <<", assigning " << assignedDomain
                  << "." <<LogEnd;
        return newEntry;
    }
    case UNKNOWN_FQDN_PROCEDURAL:
    {
        string tmp = addr->getPlain();
        std::string::size_type j = 0;
        while ( (j = tmp.find("::")) != std::string::npos)
            tmp.replace(j, 1, "-");
        while ( (j = tmp.find(':')) != std::string::npos)
            tmp.replace(j, 1, "-");
        tmp = tmp + "." + FQDNDomain_;
        SPtr<TFQDN> newEntry = new TFQDN(duid, tmp, false);
        FQDNLst_.append(newEntry);
        Log(Info) << "FQDN: Client requested " << hint <<", assigning " << tmp << "." <<LogEnd;
        return newEntry;
    }

    }

    // not found
    return SPtr<TFQDN>(); // NULL
}

SPtr<TDUID> TSrvCfgIface::getFQDNDuid(const std::string& name) {
    /// @todo: Implement this!
    SPtr<TDUID> res = new TDUID();
    return res;
}

List(TFQDN) *TSrvCfgIface::getFQDNLst() {
    return &FQDNLst_;
}

bool TSrvCfgIface::supportFQDN() const {
    return FQDNLst_.count() || (UnknownFQDN_ >= UNKNOWN_FQDN_ACCEPT);
}

int TSrvCfgIface::getFQDNMode() const{
    return FQDNMode_;
}

int TSrvCfgIface::getRevDNSZoneRootLength() const{
    return RevDNSZoneRootLength_;
}

void TSrvCfgIface::setRevDNSZoneRootLength(int revDNSZoneRootLength){
    RevDNSZoneRootLength_ = revDNSZoneRootLength;
}

string TSrvCfgIface::getFQDNModeString() const {
    switch (FQDNMode_) {
    case 0:  return "updates disabled";
    case 1:  return "server will update PTR";
    case 2:  return "server will update PTR and AAAA";
    default: return "unknown";
    }
}


void TSrvCfgIface::addTAAddr() {
    SPtr<TSrvCfgTA> ta;
    firstTA();
    ta = getTA();
    if (!ta) {
        Log(Error) << "Unable to increase TA usage. TA (temporary addresses) is not found on the "
                   << getFullName() << " interface." << LogEnd;
        return;
    }
    ta->incrAssigned();
}

void TSrvCfgIface::delTAAddr() {
    SPtr<TSrvCfgTA> ta;
    firstTA();
    ta = getTA();
    if (!ta) {
        Log(Error) << "Unable to decrease TA usage. TA (temporary addresses) is not found on the "
                   << getFullName() << " interface." << LogEnd;
        return;
    }
    ta->decrAssigned();
}

void TSrvCfgIface::addSubnet(SPtr<TIPv6Addr> min, SPtr<TIPv6Addr> max) {
    Subnets_.push_back(THostRange(min, max));
}

bool TSrvCfgIface::addrInSubnet(SPtr<TIPv6Addr> addr) {
    for (std::vector<THostRange>::const_iterator range = Subnets_.begin();
         range != Subnets_.end(); ++range) {
        if (range->in(addr)) {
            return true;
        }
    }
    return false;
}

bool TSrvCfgIface::subnetDefined() {
    return !Subnets_.empty();
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TSrvCfgIface& iface) {
    SPtr<THostID> Station;
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    out << dec;
    out << "  <SrvCfgIface name=\""<<iface.Name_ << "\" ifindex=\"" << iface.ID_ << "\">" << endl;

    if (iface.Relay_) {
        out << "    <relay name=\"" << iface.RelayName_ << "\" ifindex=\"" << iface.RelayID_;
        if (iface.RelayInterfaceID_) {
          out << "\" interfaceid=\"" << iface.RelayInterfaceID_->getPlain() << "\"";
        } else {
          out << "\" interfaceid=null";
        }
        out << "/>" << std::endl;
    } else {
        out << "    <!-- <relay/> -->" << std::endl;
    }

    out << "    <!--" << iface.Subnets_.size() << " subnet(s) -->" << std::endl;
    for (std::vector<THostRange>::const_iterator range = iface.Subnets_.begin();
         range != iface.Subnets_.end(); ++range) {
        out << "    <subnet>" << range->getAddrL()->getPlain() << "-"
            << range->getAddrR()->getPlain() << "</subnet>" << std::endl;
    }

    out << "    <preference>" << (int)iface.Preference_ << "</preference>" << std::endl;
    out << "    <ifaceMaxLease>" << iface.IfaceMaxLease_ << "</ifaceMaxLease>" << std::endl;
    out << "    <clntMaxLease>" << iface.ClntMaxLease_ << "</clntMaxLease>" << std::endl;
    out << "    <LeaseQuery>" << (iface.LeaseQuery_?"1":"0") << "</LeaseQuery>" << std::endl;

    out << "    <T1 min=\"" << iface.T1Min_ << "\" max=\"" << iface.T1Max_  << "\" />" << endl;
    out << "    <T2 min=\"" << iface.T2Min_ << "\" max=\"" << iface.T2Max_  << "\" />" << endl;
    out << "    <pref min=\"" << iface.PrefMin_ << "\" max=\""<< iface.PrefMax_  << "\" />" <<endl;
    out << "    <valid min=\"" << iface.ValidMin_ << "\" max=\""<< iface.ValidMax_ << "\" />" << endl;

    if (iface.Unicast_) {
        out << "    <unicast>" << *(iface.Unicast_) << "</unicast>" << endl;
    } else {
        out << "    <!-- <unicast/> -->" << endl;
    }

    if (iface.RapidCommit_) {
        out << "    <rapid-commit/>" << std::endl;
    } else {
        out << "    <!-- <rapid-commit/> -->" << std::endl;
    }

    out << endl;
    // print IA objects
    SPtr<TSrvCfgAddrClass>	ia;
    iface.SrvCfgAddrClassLst_.first();
    out << "    <!-- IA: non-temporary addr class count: "
        << iface.SrvCfgAddrClassLst_.count() << "-->" << endl;
    while( ia=iface.SrvCfgAddrClassLst_.get() ) {
        out << *ia;
    }

    out << endl;
    // print PD objects
    SPtr<TSrvCfgPD>	pd;
    iface.SrvCfgPDLst_.first();
    out << "    <!-- PD: prefix delegation class count: " << iface.SrvCfgPDLst_.count() << "-->" << endl;
    while( pd=iface.SrvCfgPDLst_.get() ) {
        out << *pd;
    }

    out << endl;
    // print TA objects
    SPtr<TSrvCfgTA> ta;
    iface.firstTA();
    out << "    <!-- TA: temporary IPv6 addr class count: " << iface.SrvCfgTALst_.count() << "-->" << endl;
    while( ta=iface.getTA() )
    {
        out << *ta;
    }

    out << endl << "    <!-- options -->" << endl;

    // option: DNS-SERVERS
    // option: DOMAINS
    // NTP-SERVERS
    // option: TIMEZONE
    // option: SIP-SERVERS
    // option: SIP-DOMAINS
    // option: LIFETIME

    // option: VENDOR-SPEC
/*    if (iface.supportVendorSpec()) {
        out << "    <vendorSpecList count=\"" << iface.VendorSpec.count() << "\">" << endl;
        iface.VendorSpec.first();
        SPtr<TSrvOptVendorSpec> v;
        while (v = iface.VendorSpec.get()) {
            out << "      <vendorSpec vendor=\"" << v->getVendor() << "\">" << endl;
            SPtr<TOpt> sub;
            v->firstOption();
            while (sub = v->getOption()) {
                out << "        <option code=\"" << sub->getOptType()
                    << "\" length=\"" << sub->getSize() << "\">"
                    << sub->getPlain() << "</option>" << endl;
            }
            out << "      </vendorSpec>" << endl;
        }
        out << "    </vendorSpecList>" << endl;
    } else {
        out << "    <!-- <vendorSpec/> -->" << endl;
        }*/

    out << "    <!-- " << iface.getExtraOptions().size() << " extra option(s) -->" << endl;

    TOptList extraLst =  iface.getExtraOptions();
    for (TOptList::iterator extra = extraLst.begin(); extra!=extraLst.end(); ++extra)
    {
        out << "      <extraOption type=\"" << (*extra)->getOptType()
            << "\" length=\"" << (*extra)->getSize() << "\">" << ((*extra)->getPlain()) << "</extraOption>" << endl;
    }

    // option: FQDN
    if (iface.supportFQDN()) {
      SPtr<TFQDN> f;
      List(TFQDN) * lst = iface.getFQDNLst();
      out << "    <fqdnOptions count=\"" << lst->count() << "\" prefix=\""
          << iface.getRevDNSZoneRootLength() << "\""
          << " domain=\"" << iface.FQDNDomain_ << "\""
          << " unknownFqdnMode=\"" << iface.UnknownFQDN_ << "\""
          << ">" << endl;
      lst->first();
      while ((f = lst->get())) {
            out << "       " << *f;
      }
      out << "    </fqdnOptions>" << endl;
    } else {
      out << "    <!-- <fqdnOptions/> -->" << endl;
    }


    SPtr<TSrvCfgOptions> ex;
    out << "    <!-- " << iface.ExceptionsLst_.count() << " per-client parameters (exceptions) -->" << endl;
    iface.ExceptionsLst_.first();
    while (ex = iface.ExceptionsLst_.get()) {
        out << *ex;
    }

    out << "  </SrvCfgIface>" << endl;
    return out;
}

void TSrvCfgIface::mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst)
{
    //  Log(Info)<<"Mapping allow, deny list inside interface "<<Name<<LogEnd;
    SPtr<TSrvCfgAddrClass> ptrClass;
    SrvCfgAddrClassLst_.first();
    while(ptrClass = SrvCfgAddrClassLst_.get()){
        ptrClass->mapAllowDenyList(clientClassLst);
    }

    // Map the Allow and Deny list to TA c
    SPtr<TSrvCfgTA> ptrTA;
    SrvCfgTALst_.first();
    while(ptrTA = SrvCfgTALst_.get()){
        ptrTA->mapAllowDenyList(clientClassLst);
    }
    // Map the Allow and Deny list to prefix
    SPtr<TSrvCfgPD> ptrPD;
    SrvCfgPDLst_.first();
    while(ptrPD = SrvCfgPDLst_.get()){
        ptrPD->mapAllowDenyList(clientClassLst);
    }
}


uint32_t TSrvCfgIface::chooseTime(uint32_t min, uint32_t max, uint32_t proposal)
{
    if (proposal < min)
        return min;
    if (proposal > max)
        return max;
    return proposal;
}

uint32_t TSrvCfgIface::getT1(uint32_t proposal) {
    return chooseTime(T1Min_, T1Max_, proposal);
}

uint32_t TSrvCfgIface::getT2(uint32_t proposal) {
    return chooseTime(T2Min_, T2Max_, proposal);
}

uint32_t TSrvCfgIface::getPref(uint32_t proposal) {
    return chooseTime(PrefMin_, PrefMax_, proposal);
}

uint32_t TSrvCfgIface::getValid(uint32_t proposal) {
    return chooseTime(ValidMin_, ValidMax_, proposal);
}

/// checks if given address/prefix is valid on this interface
///
/// @param type IA, TA or PD
/// @param addr address or prefix to be confirmed
///
/// @return YES, NO or UNKNOWN (if no subnet is defined and addr is outside of pool)
EAddrStatus TSrvCfgIface::confirmAddress(TIAType type, SPtr<TIPv6Addr> addr) {

    string what = "address";
    if (type == IATYPE_PD)
        what = "prefix";

    Log(Debug) << "Confirm that " << what << " " << addr->getPlain()
               << " is on-link:";

    if (subnetDefined()) {
        // this is easy to check - client defined subnet
        // we just check if the address is in subnet and we're done

        if (addrInSubnet(addr)) {
            Log(Cont) << "yes (belongs to defined subnet)." << LogEnd;
            return ADDRSTATUS_YES;
        } else {
            Log(Cont) << "no (outside of defined subnet)." << LogEnd;
            return ADDRSTATUS_NO;
        }

    } else {

        // Ok, admin was lazy enough and did not specify subnet
        // parameter for us. We have to check it the hard way
        bool inPool;
        switch (type) {
        case IATYPE_IA:
            inPool = addrInPool(addr);
            break;
        case IATYPE_TA:
            inPool = addrInTaPool(addr);
            break;
        case IATYPE_PD:
            inPool = prefixInPdPool(addr);
            break;
        default:
            // should never happen
            inPool = false;
        }

        if (inPool) {
            Log(Cont) << "yes (belongs to defined class)." << LogEnd;
            return ADDRSTATUS_YES;
        } else {
            Log(Cont) << "unknown (outside of defined class)." << LogEnd;
            return ADDRSTATUS_UNKNOWN;
        }
    }
}

/// checks if address is in NA pool
///
/// @param addr address to be checked
///
/// @return true if in pool, false otherwise
bool TSrvCfgIface::addrInPool(SPtr<TIPv6Addr> addr) {
    firstAddrClass();
    SPtr<TSrvCfgAddrClass> ptrClass;
    bool inPool = false;
    while (ptrClass = getAddrClass()) {
        inPool = ptrClass->addrInPool(addr);
        if (!inPool)
            return false;
    }
    return inPool;
}

/// checks if address is in TA pool
///
/// @param addr address to be checked
///
/// @return true if in pool, false otherwise
bool TSrvCfgIface::addrInTaPool(SPtr<TIPv6Addr> addr) {
    firstTA();
    SPtr<TSrvCfgTA> ptrClass;
    bool inPool = false;
    while (ptrClass = getTA()) {
        inPool = ptrClass->addrInPool(addr);
        if (!inPool)
            return false;
    }
    return inPool;
}

/// checks if prefix is in PD pool
///
/// @param prefix prefix to be checked
///
/// @return true if in pool, false otherwise
bool TSrvCfgIface::prefixInPdPool(SPtr<TIPv6Addr> prefix) {
    firstPD();
    SPtr<TSrvCfgPD> ptrClass;
    bool inPool = false;
    while (ptrClass = getPD()) {
        inPool = ptrClass->prefixInPool(prefix);
        if (!inPool)
            return false;
    }
    return inPool;
}

