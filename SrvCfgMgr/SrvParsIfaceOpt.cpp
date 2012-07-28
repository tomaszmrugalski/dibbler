/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *    changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <climits>
#include "SrvParsIfaceOpt.h"
#include "OptAddr.h"
#include "OptString.h"
#include "DHCPDefaults.h"
#include "Logger.h"

using namespace std;

TSrvParsIfaceOpt::TSrvParsIfaceOpt(void)
{
    Unicast_       = 0;
    Preference_    = SERVER_DEFAULT_PREFERENCE;
    RapidCommit_   = SERVER_DEFAULT_RAPIDCOMMIT;
    ClntMaxLease_  = SERVER_DEFAULT_CLNTMAXLEASE;
    IfaceMaxLease_ = SERVER_DEFAULT_IFACEMAXLEASE;
    RevDNSZoneRootLength_ = SERVER_DEFAULT_DNSUPDATE_REVDNS_ZONE_LEN;

    // don't support leasequery unless explicitly configured to do so
    LeaseQuery_    = SERVER_DEFAULT_LEASEQUERY;

    // options
    FQDNSupport_      = false;
    FQDNMode_         = 0; // (DNS_UPDATE_MODE_NONE)
    VendorSpecSupport = false;

    UnknownFQDN_ = SERVER_DEFAULT_UNKNOWN_FQDN;
    FQDNDomain_ = "";

    Relay_ = false;
    RelayName_ = "[unknown]";
    RelayID_ = -1;
    RelayInterfaceID_ = -1;
}

TSrvParsIfaceOpt::~TSrvParsIfaceOpt(void) {
}

void TSrvParsIfaceOpt::setUnknownFQDN(EUnknownFQDNMode mode, const std::string& domain) {
    UnknownFQDN_ = mode;
    FQDNDomain_ = domain;
}

/**
 * returns enum that specifies, how to handle unknown FQDNs
 *
 *
 * @return
 */
EUnknownFQDNMode TSrvParsIfaceOpt::getUnknownFQDN() {
    return UnknownFQDN_;
}

std::string TSrvParsIfaceOpt::getFQDNDomain() {
    return FQDNDomain_;
}

void TSrvParsIfaceOpt::setLeaseQuerySupport(bool support) {
    LeaseQuery_ = support;
}

bool TSrvParsIfaceOpt::getLeaseQuerySupport() {
    return LeaseQuery_;
}

// --- unicast ---
void TSrvParsIfaceOpt::setUnicast(SPtr<TIPv6Addr> addr) {
    Unicast_ = addr;
}

SPtr<TIPv6Addr> TSrvParsIfaceOpt::getUnicast() {
    return Unicast_;
}

// --- iface-max-lease ---
void TSrvParsIfaceOpt::setIfaceMaxLease(long maxLease) {
    IfaceMaxLease_ = maxLease;
}

long TSrvParsIfaceOpt::getIfaceMaxLease() {
    return IfaceMaxLease_;
}

// --- clnt max lease ---
void TSrvParsIfaceOpt::setClntMaxLease(long clntMaxLease) {
    ClntMaxLease_ = clntMaxLease;
}

long TSrvParsIfaceOpt::getClntMaxLease() {
    return ClntMaxLease_;
}

// --- preference ---
void TSrvParsIfaceOpt::setPreference(char pref) {
    Preference_ = pref;
}
char TSrvParsIfaceOpt::getPreference() {
    return Preference_;
}

// --- rapid commit ---
void TSrvParsIfaceOpt::setRapidCommit(bool rapidComm) {
    RapidCommit_ = rapidComm;
}

bool TSrvParsIfaceOpt::getRapidCommit() {
    return RapidCommit_;
}

// --- relay related ---
void TSrvParsIfaceOpt::setRelayName(std::string name) {
    Relay_ = true;
    RelayName_ = name;
    RelayID_ = -1;
}

void TSrvParsIfaceOpt::setRelayID(int id) {
    Relay_ = true;
    RelayName_ = "[unknown]";
    RelayID_ = id;
}

void TSrvParsIfaceOpt::setRelayInterfaceID(SPtr<TSrvOptInterfaceID> id) {
    Relay_ = true;
    RelayInterfaceID_ = id;
}

string TSrvParsIfaceOpt::getRelayName() {
    return RelayName_;
}

int TSrvParsIfaceOpt::getRelayID() {
    return RelayID_;
}

SPtr<TSrvOptInterfaceID> TSrvParsIfaceOpt::getRelayInterfaceID() {
    return RelayInterfaceID_;
}

bool TSrvParsIfaceOpt::isRelay() {
    return Relay_;
}

// --- option: DNS servers ---
// --- option: DOMAIN ---
// --- option: NTP-SERVERS ---
// --- option: TIMEZONE ---
// --- option: SIP server ---
// --- option: SIP domain ---
// --- option: LIFETIME ---

#if 0
// --- option: VENDOR-SPEC INFO ---
void TSrvParsIfaceOpt::setVendorSpec(List(TOptVendorSpecInfo) vendor)
{
    VendorSpec = vendor;
    VendorSpecSupport = true;
}
bool TSrvParsIfaceOpt::supportVendorSpec()
{
    return VendorSpecSupport;
}

List(TOptVendorSpecInfo) TSrvParsIfaceOpt::getVendorSpec()
{
    return VendorSpec;
}
#endif

// --- option: FQDN ---
void TSrvParsIfaceOpt::setFQDNLst(List(TFQDN) *fqdn) {
    FQDNLst_ = *fqdn;
    FQDNSupport_ = true;
}

List(TFQDN) *TSrvParsIfaceOpt::getFQDNLst() {
    return &this->FQDNLst_;
}

int TSrvParsIfaceOpt::getFQDNMode(){
    return FQDNMode_;
}
void TSrvParsIfaceOpt::setFQDNMode(int FQDNMode){
    FQDNMode_ = FQDNMode;
}
int TSrvParsIfaceOpt::getRevDNSZoneRootLength(){
    return RevDNSZoneRootLength_;
}
void TSrvParsIfaceOpt::setRevDNSZoneRootLength(int revDNSZoneRootLength){
    RevDNSZoneRootLength_ = revDNSZoneRootLength;
}

bool TSrvParsIfaceOpt::supportFQDN() {
    return FQDNSupport_;
}


void TSrvParsIfaceOpt::addExtraOption(SPtr<TOpt> custom, bool always) {
    //Log(Debug) << "Setting " << (always?"mandatory ":"request-only ")
    //           << custom->getOptType() << " generic option (length="
    //           << custom->getSize() << ")." << LogEnd;

    ExtraOpts.push_back(custom); // allways add to extra options

    if (always)
        ForcedOpts.push_back(custom); // also add to forced, if requested so
}

const TOptList& TSrvParsIfaceOpt::getExtraOptions() {
    return ExtraOpts;
}

SPtr<TOpt> TSrvParsIfaceOpt::getExtraOption(uint16_t type) {
    for (TOptList::iterator opt=ExtraOpts.begin(); opt!=ExtraOpts.end(); ++opt)
    {
        if ((*opt)->getOptType() == type)
            return *opt;
    }
    return 0;
}

const TOptList& TSrvParsIfaceOpt::getForcedOptions() {
    return ForcedOpts;
}
