/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Logger.h"
#include "SrvCfgOptions.h"

using namespace std;

TSrvCfgOptions::TSrvCfgOptions() {
    SetDefaults();
}

TSrvCfgOptions::TSrvCfgOptions(SPtr<TDUID> duid) {
    SetDefaults();
    Duid = duid;
}

TSrvCfgOptions::TSrvCfgOptions(SPtr<TOptVendorData> remoteID) {
    SetDefaults();
    RemoteID = remoteID;
}

TSrvCfgOptions::TSrvCfgOptions(SPtr<TIPv6Addr> clntaddr) {
    SetDefaults();
    ClntAddr = clntaddr;
}

SPtr<TDUID> TSrvCfgOptions::getDuid() const {
    return Duid;
}

SPtr<TOptVendorData> TSrvCfgOptions::getRemoteID() const {
    return RemoteID;
}

SPtr<TIPv6Addr> TSrvCfgOptions::getClntAddr() const {
    return ClntAddr;
}

void TSrvCfgOptions::SetDefaults() {
    this->VendorSpecSupport       = false;

    Duid.reset();
    RemoteID.reset();
    ClntAddr.reset();

    ExtraOpts_.clear();
    ForcedOpts_.clear();
}

// --------------------------------------------------------------------
// --- options --------------------------------------------------------
// --------------------------------------------------------------------
// --- option: DNS servers ---
// --- option: DOMAIN ---
// --- option: NTP-SERVERS ---
// --- option: TIMEZONE ---
// --- option: SIP server ---
// --- option: SIP domain ---
// --- option: LIFETIME ---

// --- option: VENDOR-SPEC INFO ---
#if 0
bool TSrvCfgOptions::supportVendorSpec() {
    if (VendorSpec.count())
        return true;
    return false;
}
#endif

List(TOptVendorSpecInfo) TSrvCfgOptions::getVendorSpecLst(unsigned int vendor) {

    SPtr<TOpt> opt;
    SPtr<TOptVendorSpecInfo> x;
    List(TOptVendorSpecInfo) returnList;
    returnList.clear();

    for (TOptList::iterator opt = ExtraOpts_.begin(); opt!=ExtraOpts_.end(); ++opt)
    {
        if ( (*opt)->getOptType() != OPTION_VENDOR_OPTS)
            continue;
        x = (Ptr*) *opt;
        if (!vendor || x->getVendor() == vendor)
        {
            // enterprise number not specified => return all
            returnList.append(x);
        }
    }

    return returnList;
}

void TSrvCfgOptions::setAddr(SPtr<TIPv6Addr> addr) {
    Addr = addr;
}

SPtr<TIPv6Addr> TSrvCfgOptions::getAddr() const {
    return Addr;
}

void TSrvCfgOptions::addExtraOption(SPtr<TOpt> custom, bool always) {
    Log(Debug) << "Setting " << (always?"mandatory ":"request-only ")
               << custom->getOptType() << " generic option (length="
               << custom->getSize() << ")." << LogEnd;

    ExtraOpts_.push_back(custom); // allways add to extra options

    if (always)
        ForcedOpts_.push_back(custom); // also add to forced, if requested so
}

const TOptList& TSrvCfgOptions::getExtraOptions() {
    return ExtraOpts_;
}

TOptPtr TSrvCfgOptions::getExtraOption(uint16_t type) {
    for (TOptList::iterator opt=ExtraOpts_.begin(); opt!=ExtraOpts_.end(); ++opt)
    {
        if ((*opt)->getOptType() == type)
            return *opt;
    }
    return TOptPtr(); // NULL
}

const TOptList& TSrvCfgOptions::getForcedOptions() {
    return ForcedOpts_;
}

bool TSrvCfgOptions::setOptions(SPtr<TSrvParsGlobalOpt> opt) {

    addExtraOptions(opt->getExtraOptions());
    addForcedOptions(opt->getForcedOptions());

    return true;
}

/// @brief Copies a list of extra options.
///
/// Extra options are options that may be requested by a client. This list also
/// contains forced options (i.e. options that are sent regardless if client
/// asks for them or not).
///
/// @param extra list of options to be copied
void TSrvCfgOptions::addExtraOptions(const TOptList& extra) {
    for (TOptList::const_iterator opt = extra.begin(); opt != extra.end(); ++opt)
        ExtraOpts_.push_back(*opt);
}

/// @brief Copies a list of forced options.
///
/// This method add a list of forced options. Forced options are the ones that
/// are sent to a client, regardless if client requested them or not.
///
/// @param forced list of forced options to be copied
void TSrvCfgOptions::addForcedOptions(const TOptList& forced) {
    for (TOptList::const_iterator opt = forced.begin(); opt != forced.end(); ++opt)
        ForcedOpts_.push_back(*opt);
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
ostream& operator<<(ostream& out,TSrvCfgOptions& iface) {

    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    out << "    <client>" << endl;
    if (iface.Duid)
        out << "      " << *iface.Duid;
    else
        out << "      <!-- <duid/> -->" << endl;
    if (iface.RemoteID)
        out << "      <RemoteID enterprise=\"" << iface.RemoteID->getVendor() << "\" length=\""
            << iface.RemoteID->getVendorDataLen() << "\">" << iface.RemoteID->getVendorDataPlain() << "</RemoteID>" << endl;
    else
        out << "      <!-- <RemoteID/> -->" << endl;

    out << "      <!-- paramteres start here -->" << endl;

    if (iface.Addr) {
        out << "      <addr>" << iface.Addr->getPlain() << "</addr>" << endl;
    } else {
        out << "      <!-- <addr/> -->" << endl;
    }

    // option: DNS-SERVERS
    // option: DOMAINS
    // NTP-SERVERS
    // option: TIMEZONE
    // option: SIP-SERVERS
    // option: SIP-DOMAINS
    // option: LIFETIME


#if 0
    // option: VENDOR-SPEC
    if (iface.VendorSpec.count()) {
        out << "      <vendorSpecList count=\"" << iface.VendorSpec.count() << "\">" << endl;
        iface.VendorSpec.first();
        SPtr<TSrvOptVendorSpec> v;
        while (v = iface.VendorSpec.get()) {
            out << v << endl;
        }
        out << "      </vendorSpecList>" << endl;
    } else {
        out << "      <!-- <vendorSpec/> -->" << endl;
    }
#endif

    out << "    </client>" << endl;

    return out;
}
