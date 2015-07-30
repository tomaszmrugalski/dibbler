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
    :Preference_(SERVER_DEFAULT_PREFERENCE), RapidCommit_(SERVER_DEFAULT_RAPIDCOMMIT),
     IfaceMaxLease_(SERVER_DEFAULT_IFACEMAXLEASE), ClntMaxLease_(SERVER_DEFAULT_CLNTMAXLEASE),
     Unicast_(), LeaseQuery_(SERVER_DEFAULT_LEASEQUERY), Relay_(false),
     RelayName_("[unknown]"), RelayID_(-1), RelayInterfaceID_(),
     FQDNSupport_(false), FQDNMode_(0/*DNS_UPDATE_MODE_NONE*/),
     UnknownFQDN_(SERVER_DEFAULT_UNKNOWN_FQDN), FQDNDomain_("")
{
    RevDNSZoneRootLength_ = SERVER_DEFAULT_DNSUPDATE_REVDNS_ZONE_LEN;

    // don't support leasequery unless explicitly configured to do so
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

/// @brief adds option to the list (and merges vendor-options if possible)
///
/// Adds new option to the list. However, if the option being added in vendor-option,
/// it tries to find if there's existing vendor option with the same vendor-id. If
/// there is, it just copies sub-options from the option being added to the option
/// that is already defined.
///
/// @param list list of options (new option will be added or merged here)
/// @param custom new option to be added
void TSrvParsIfaceOpt::addOption(TOptList& list, TOptPtr custom) {

    if (custom->getOptType() == OPTION_VENDOR_OPTS) {

        SPtr<TOptVendorSpecInfo> newone = (Ptr*) (custom);
        for (TOptList::iterator opt=ExtraOpts.begin(); opt!=ExtraOpts.end(); ++opt)
        {
            if ((*opt)->getOptType() != OPTION_VENDOR_OPTS)
                continue;
            SPtr<TOptVendorSpecInfo> existing = (Ptr*) (*opt);

            if (existing->getVendor() == newone->getVendor()) {
                newone->firstOption();
                while (TOptPtr subopt = newone->getOption()) {
                    existing->addOption(subopt);
                }
                return;
            }
        }
    }

    // This wasn't vendor-option or vendor-id didn't match, add it the usual way
    list.push_back(custom);
}

void TSrvParsIfaceOpt::addExtraOption(SPtr<TOpt> custom, bool always) {

    addOption(ExtraOpts, custom);

    if (always)
        addOption(ForcedOpts, custom); // also add to forced, if requested so
}

const TOptList& TSrvParsIfaceOpt::getExtraOptions() {
    return ExtraOpts;
}

TOptPtr TSrvParsIfaceOpt::getExtraOption(uint16_t type) {
    for (TOptList::iterator opt=ExtraOpts.begin(); opt!=ExtraOpts.end(); ++opt)
    {
        if ((*opt)->getOptType() == type)
            return *opt;
    }
    return TOptPtr(); // NULL
}

const TOptList& TSrvParsIfaceOpt::getForcedOptions() {
    return ForcedOpts;
}
