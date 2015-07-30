/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include "SmartPtr.h"
#include "Portable.h"
#include "ClntCfgMgr.h"
#include "ClntCfgIface.h"
#include "Logger.h"

using namespace std;

#include "FlexLexer.h"
#include "ClntIfaceMgr.h"
#include "ClntParsGlobalOpt.h"
#include "ClntParser.h"
#include "hex.h"

TClntCfgMgr * TClntCfgMgr::Instance = 0;

#ifdef MOD_CLNT_EMBEDDED_CFG
static bool HardcodedCfgExample(TClntCfgMgr *cfgMgr, string params);
#endif

extern std::string CLNTCONF_FILE;

TClntCfgMgr & TClntCfgMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "Application error. Tried to access CfgMgr without instanceCreate!" << LogEnd;
        instanceCreate(CLNTCONF_FILE);
    }
  // throw an exception here or something
  return *Instance;
}

void TClntCfgMgr::instanceCreate(const std::string& cfgFile) {
    Instance = new TClntCfgMgr(cfgFile);
}


TClntCfgMgr::TClntCfgMgr(const std::string& cfgFile)
    :TCfgMgr(), ScriptName(DEFAULT_SCRIPT), ObeyRaBits_(false)
{

#ifdef MOD_REMOTE_AUTOCONF
    RemoteAutoconf = false;
#endif

    Reconfigure = false;

    // parse configuration file
    if (!parseConfigFile(cfgFile)) {
        IsDone = true;
        return;
    }

    // load or create DUID
    string duidFile = (string)CLNTDUID_FILE;
    if (!setDUID(duidFile, ClntIfaceMgr())) {
        IsDone = true;
        return;
    }
    this->dump();

#ifndef MOD_DISABLE_AUTH
    AuthKeys = new KeyList(); // create dummy empty keylist
#endif
}

void TClntCfgMgr::dump() {
    // store ClntCfgMgr in file
    std::ofstream xmlDump;
    xmlDump.open(CLNTCFGMGR_FILE);
    xmlDump << *this;
    xmlDump.close();
}

bool TClntCfgMgr::parseConfigFile(const std::string& cfgFile)
{
    #ifndef MOD_CLNT_EMBEDDED_CFG
    // --- normal operation: read config. file ---

    // parse config file
    ifstream f;
    f.open(cfgFile.c_str());
    if ( ! f.is_open()  ) {
        Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd;
            return false;
    } else {
        Log(Notice) << "Parsing " << cfgFile << " config file..." << LogEnd;
    }
    yyFlexLexer lexer(&f,&clog);
    ClntParser parser(&lexer);
    parser.CfgMgr = this; // just a workaround to access CfgMgr while still being in constructor
    int result = parser.yyparse();
    Log(Debug) << "Parsing " << cfgFile << " done, result=" << result
               << (result?"(failure)":"(success)") << LogEnd;
    f.close();

    if (result) {
        //Result!=0 means config errors. Finish whole DHCPClient
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
        this->DUID=new TDUID();
        return false;
    }

    if (!setGlobalOptions(&parser)) {
        return false;
    }

    // match parsed interfaces with interfaces detected in system
    if (!matchParsedSystemInterfaces(&parser)) {
        return false;
    }
#else
    // --- use hardcoded config ---
    // use your favourite configuration generator function here
    HardcodedCfgFunc *cfgMaker = &HardcodedCfgExample;

    // call your function here
    cfgMaker(this, cfgFile);

#endif

    // check config consistency
    if(!validateConfig()) {
        return false;
    }

    return true;
}

/**
 * match parsed interfaces with interfaces detected in system.
 * ClntCfgIface objects copied to CfgMgr.
 *
 * @param parser
 *
 * @return true if ok, false if interface definitions are incorrect
 */
bool TClntCfgMgr::matchParsedSystemInterfaces(ClntParser *parser) {
    int cfgIfaceCnt;

    cfgIfaceCnt = parser->ClntCfgIfaceLst.count();
    Log(Debug) << cfgIfaceCnt << " interface(s) specified in " << CLNTCONF_FILE << LogEnd;

    SPtr<TClntCfgIface> cfgIface;
    SPtr<TIfaceIface> ifaceIface;

    if (cfgIfaceCnt) {
        // user specified some interfaces in config file
        parser->ClntCfgIfaceLst.first();
        while(cfgIface = parser->ClntCfgIfaceLst.get()) {
            // for each interface (from config file)
            if (cfgIface->getID()==-1) {
                ifaceIface = ClntIfaceMgr().getIfaceByName(cfgIface->getName());
            } else {
                ifaceIface = ClntIfaceMgr().getIfaceByID(cfgIface->getID());
            }

            if (!ifaceIface) {
                if (inactiveMode()) {
                    Log(Info) << "Interface " << cfgIface->getFullName()
                              << " is not currently available (that's ok, inactive-mode enabled)." << LogEnd;
                    continue;
                }
                Log(Error) << "Interface " << cfgIface->getFullName()
                           << " specified in " << CLNTCONF_FILE << " is not present or does not support IPv6."
                           << LogEnd;
                return false;
            }
            if (cfgIface->noConfig()) {
                Log(Info) << "Interface " << cfgIface->getFullName()
                          << " has flag no-config set, so it is ignored." << LogEnd;
                continue;
            }

#ifdef MOD_REMOTE_AUTOCONF
            if (RemoteAutoconf) {
                List(TIPv6Addr) emptyLst;
                SPtr<TOpt> optNeighbors = new TOptAddrLst(OPTION_NEIGHBORS, emptyLst, 0);
                Log(Debug) << "Enabled Neighbors option on " << cfgIface->getFullName() << LogEnd;
                cfgIface->addExtraOption(optNeighbors, false);
            }
#endif

            cfgIface->setIfaceName(ifaceIface->getName());
            cfgIface->setIfaceID(ifaceIface->getID());

            // setup default prefix length (used when IPv6 address is added to the interface)
            ifaceIface->setPrefixLength(cfgIface->getOnLinkPrefixLength());

            if (!ifaceIface->flagUp() || !ifaceIface->countLLAddress()) {
                if (inactiveMode()) {
                    Log(Notice) << "Interface " << ifaceIface->getFullName()
                                << " is not operational yet (does not have "
                                << "link-local address or is down), skipping it for now." << LogEnd;
                    addIface(cfgIface);
                    makeInactiveIface(cfgIface->getID(), true, true, true); // move it to InactiveLst
                    continue;
                }

                Log(Crit) << "Interface " << ifaceIface->getFullName()
                          << " is down or doesn't have any link-local address." << LogEnd;
                return false;
            }

            // Check if the interface is during bring-up phase
            // (i.e. DAD procedure for link-local addr is not complete yet)
            char tmp[64];
            if (cfgIface->getBindToAddr()) {
                inet_ntop6(cfgIface->getBindToAddr()->getPlain(), tmp);
            } else {
                ifaceIface->firstLLAddress();
                inet_ntop6(ifaceIface->getLLAddress(), tmp);
            }

            if (is_addr_tentative(ifaceIface->getName(), ifaceIface->getID(), tmp)
                == LOWLEVEL_TENTATIVE_YES) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " has link-local address " << tmp
                            << ", but it is currently tentative." << LogEnd;

                if (this->inactiveMode()) {
                    Log(Notice) << "Interface " << ifaceIface->getFullName()
                                << " is not operational yet (link-local address "
                                << "is not ready), skipping it for now." << LogEnd;
                    addIface(cfgIface);
                    makeInactiveIface(cfgIface->getID(), true, true, true); // move it to InactiveLst
                    continue;
                }

                Log(Crit) << "Interface " << ifaceIface->getFullName()
                          << " has tentative link-local address (and inactive-mode is disabled)." << LogEnd;
                return false;
            }

            if (obeyRaBits()) {
                if (!ifaceIface->getMBit() && !ifaceIface->getOBit()) {
                    Log(Info) << "Interface " << cfgIface->getFullName()
                              << " configuration loaded, but did not receive a Router "
                              << "Advertisement with M or O bits set, adding to inactive."
                              << LogEnd;
                    addIface(cfgIface);
                    makeInactiveIface(cfgIface->getID(), true, true, true); // move it to inactive list
                    continue;
                }
                cfgIface->setMbit(ifaceIface->getMBit());
                cfgIface->setObit(ifaceIface->getOBit());
            }

            addIface(cfgIface);
            Log(Info) << "Interface " << cfgIface->getFullName()
                      << " configuration has been loaded." << LogEnd;
        }
        return countIfaces() || (inactiveMode() && inactiveIfacesCnt());
    } else {
        // user didn't specified any interfaces in config file, so
        // we'll try to configure each interface we could find
        Log(Warning) << "Config file does not contain any interface definitions. Trying to autodetect."
                     << LogEnd;

        List(TIPv6Addr) dnsList;
        dnsList.clear();
        parser->ParserOptStack.getLast()->setDNSServerLst(&dnsList);

        // Try to add a hostname
        char hostname[255];
        if (get_hostname(hostname, 255) == LOWLEVEL_NO_ERROR) {
            parser->ParserOptStack.getLast()->setFQDN(string(hostname));
	    } else {
            parser->ParserOptStack.getLast()->setFQDN(string(""));
        }

        int cnt = 0;
        ClntIfaceMgr().firstIface();
        while ( ifaceIface = ClntIfaceMgr().getIface() ) {
            // for each interface present in the system...
            if (!inactiveMode()) {
                if (!ifaceIface->flagUp()) {
                    Log(Notice) << "Interface " << ifaceIface->getFullName() << " is down, ignoring." << LogEnd;
                    continue;
                }
                if (!ifaceIface->flagRunning()) {
                    Log(Notice) << "Interface " << ifaceIface->getFullName()
                                << " has flag RUNNING not set, ignoring." << LogEnd;
                    continue;
                }
                if (!ifaceIface->flagMulticast()) {
                    Log(Notice) << "Interface " << ifaceIface->getFullName()
                                << " is not multicast capable, ignoring." << LogEnd;
                    continue;
                }
            }
            if ( !(ifaceIface->getMacLen() > 5) ) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " has MAC address length " << ifaceIface->getMacLen()
                            << " (6 or more required), ignoring." << LogEnd;
                continue;
            }

            // ignore disabled Teredo pseudo-interface on Win (and other similar useless junk)
            const static unsigned char zeros[] = {0,0,0,0,0,0,0,0};
            const static unsigned char ones[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
            if ( (ifaceIface->getMacLen()<=8) &&
                 (!memcmp(zeros, ifaceIface->getMac(), min(8,ifaceIface->getMacLen())) ||
                  !memcmp(ones, ifaceIface->getMac(), min(8,ifaceIface->getMacLen()))) ) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " has invalid MAC address "
                            << hexToText((uint8_t*)ifaceIface->getMac(), ifaceIface->getMacLen(), true)
                            << ", ignoring." << LogEnd;
                continue;
            }

            if (!ifaceIface->countLLAddress()) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " has no link-local address, ignoring. "
                            << "(Disconnected? Not associated? No-link?)" << LogEnd;
                continue;
            }

            // One address...
            SPtr<TClntCfgAddr> addr(new TClntCfgAddr());
            addr->setOptions(parser->ParserOptStack.getLast());

            // ... is stored in one IA...
            SPtr<TClntCfgIA> ia = new TClntCfgIA();
            ia->setOptions(parser->ParserOptStack.getLast());
            ia->addAddr(addr);

            // ... on this newly created interface...
            cfgIface = SPtr<TClntCfgIface>(new TClntCfgIface(ifaceIface->getID()));
            cfgIface->setIfaceName(ifaceIface->getName());
            cfgIface->setIfaceID(ifaceIface->getID());
            cfgIface->addIA(ia);
            cfgIface->setOptions(parser->ParserOptStack.getLast());

            // ... which is added to ClntCfgMgr
            addIface(cfgIface);

            Log(Info) << "Interface " << cfgIface->getFullName()
                      << " has been added." << LogEnd;

            if (inactiveMode() && !ifaceIface->flagRunning() ) {
                makeInactiveIface(cfgIface->getID(), true, true, true); // move it to InactiveLst
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " is not operational yet"
                            << " (not running), made inactive." << LogEnd;
            }
            cnt ++;
        }
        if (!cnt) {
            Log(Crit) << "Unable to detect any suitable interfaces. If there are any interfaces that you"
                      << " want to have configured, please specify them in client.conf file." << LogEnd;
            return false;
        }
    }
    return true;
}

SPtr<TClntCfgIface> TClntCfgMgr::getIface()
{
    return ClntCfgIfaceLst.get();
}

void TClntCfgMgr::addIface(SPtr<TClntCfgIface> ptr)
{
    ClntCfgIfaceLst.append(ptr);
}

void TClntCfgMgr::makeInactiveIface(int ifindex, bool inactive, bool managed,
                                    bool otherConf)
{
    SPtr<TClntCfgIface> x;

    if (inactive)
    {
        ClntCfgIfaceLst.first();
        while (x= ClntCfgIfaceLst.get()) {
            if (x->getID() == ifindex) {
                Log(Info) << "Switching " << x->getFullName() << " to inactive-mode." << LogEnd;
                ClntCfgIfaceLst.del();
                InactiveLst.append(x);
                return;
            }
        }
        Log(Error) << "Unable to switch interface ifindex=" << ifindex
                   << " to inactive-mode: interface not found." << LogEnd;
        return;
    }

    InactiveLst.first();
    while (x= InactiveLst.get()) {
        if (x->getID() == ifindex) {
            Log(Info) << "Switching " << x->getFullName() << " to normal mode." << LogEnd;
            InactiveLst.del();
            InactiveLst.first();
            x->setMbit(managed);
            x->setObit(otherConf);
            addIface(x);
            return;
        }
    }

    Log(Error) << "Unable to switch interface ifindex=" << ifindex
               << " from inactive-mode to normal operation: interface not found." << LogEnd;
}

void TClntCfgMgr::firstIface()
{
    ClntCfgIfaceLst.first();
}

int TClntCfgMgr::countIfaces()
{
    return ClntCfgIfaceLst.count();
}

bool TClntCfgMgr::getReconfigure()
{
    return Reconfigure;
}

void TClntCfgMgr::setReconfigure(bool enable)
{
    Reconfigure = enable;
}


int TClntCfgMgr::countAddrForIA(long IAID)
{
    SPtr<TClntCfgIface> iface;
    firstIface();
    while (iface = getIface() )
    {
        SPtr<TClntCfgIA> ia;
        iface->firstIA();
        while (ia = iface->getIA())
            if (ia->getIAID()==IAID)
                return ia->countAddr();
    }
    return 0;
}

SPtr<TClntCfgIA> TClntCfgMgr::getIA(long IAID)
{
    SPtr<TClntCfgIface> iface;
    firstIface();
    while (iface = getIface() )
    {
        SPtr<TClntCfgIA> ia;
        iface->firstIA();
        while (ia = iface->getIA())
            if (ia->getIAID()==IAID)
                return ia;
    }
    return SPtr<TClntCfgIA>(); // NULL
}

SPtr<TClntCfgPD> TClntCfgMgr::getPD(long IAID)
{
    SPtr<TClntCfgIface> iface;
    firstIface();
    while (iface = getIface() )
    {
        SPtr<TClntCfgPD> pd;
        iface->firstPD();
        while (pd = iface->getPD())
            if (pd->getIAID()==IAID)
                return pd;
    }
    return SPtr<TClntCfgPD>(); // NULL
}

bool TClntCfgMgr::setIAState(int iface, int iaid, enum EState state)
{
    firstIface();
    SPtr<TClntCfgIface> ptrIface;
    while (ptrIface = getIface() ) {
        if ( ptrIface->getID() == iface ) break;
    }
    if (! ptrIface) {
        Log(Error) <<"ClntCfgMgr: Unable to set IA state (id=" << iaid
            << "):Interface " << iface << " not found." << LogEnd;
        return false;
    }

    SPtr<TClntCfgIA> ia;
    ptrIface->firstIA();

    while (ia = ptrIface->getIA())
    {
        if ( ia->getIAID() == iaid ) {
            ia->setState(state);
            return true;
        }
    }

    Log(Error) << "ClntCfgMgr: Unable to set IA state (id=" << iaid << ")" << LogEnd;
    return false;
}

//check whether T1<T2 and Pref<Valid and at least T1<=Valid
bool TClntCfgMgr::validateConfig()
{
    if (IsDone)
                return false; //Is everything so far ok?
    SPtr<TClntCfgIface> ptrIface;
    this->ClntCfgIfaceLst.first();
    while(ptrIface=ClntCfgIfaceLst.get())
    {
        if (!this->validateIface(ptrIface)) {
            return false;
        }
    }

#ifndef MOD_DISABLE_AUTH
    // Validate authentication settings
    switch (getAuthProtocol()) {
    case AUTH_PROTO_NONE:
        break;
    case AUTH_PROTO_DELAYED: {
        const std::vector<DigestTypes> digests = getAuthAcceptMethods();
        if (digests.empty()) {
            Log(Error) << "AUTH: No digests specified. For delayed-auth HMAC-MD5 must be used."
                       << LogEnd;
            return false;
        }
        if (digests.size() > 1) {
            Log(Warning) << "AUTH: More than one digest type allowed for delayed-auth,"
                         << " expected only HMAC-MD5" << LogEnd;
        }
        if (digests[0] != DIGEST_HMAC_MD5) {
            Log(Error) << "AUTH: First digest type for delayed-auth is "
                       << getDigestName(digests[0]) << ", but only HMAC-MD5 is allowed for delayed-auth."
                       << LogEnd;
            return false;
        }
        break;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {
        const std::vector<DigestTypes> digests = getAuthAcceptMethods();
        if (digests.size() > 1) {
            Log(Warning) << "AUTH: More than one digest type allowed for reconfigure-key,"
                         << " expected only HMAC-MD5." << LogEnd;
        }
        if (digests[0] != DIGEST_HMAC_MD5) {
            Log(Error) << "AUTH: First digest type for reconfigure-key is "
                       << getDigestName(digests[0]) << ", but only HMAC-MD5 is allowed for delayed-auth."
                       << LogEnd;
            return false;
        }
        break;
    }
    case AUTH_PROTO_DIBBLER:
        break;
    }
#endif

    return true;
}

bool TClntCfgMgr::validateIface(SPtr<TClntCfgIface> ptrIface) {

    SPtr<TClntCfgIA> ptrIA;
    ptrIface->firstIA();
    while(ptrIA=ptrIface->getIA())
    {
        if (!this->validateIA(ptrIface, ptrIA))
            return false;
    }

    SPtr<TClntCfgPD> ptrPD;
    ptrIface->firstPD();
    while(ptrPD=ptrIface->getPD())
    {
        if (!this->validatePD(ptrIface, ptrPD))
            return false;
    }

    return true;
}

bool TClntCfgMgr::validateIA(SPtr<TClntCfgIface> ptrIface, SPtr<TClntCfgIA> ptrIA) {

    if ( ptrIA->getT2()<ptrIA->getT1() )
    {
        Log(Crit) << "T1 can't be lower than T2 for IA " << *ptrIA << "on the " << ptrIface->getName()
                  << "/" << ptrIface->getID() << " interface." << LogEnd;
        return false;
    }
    SPtr<TClntCfgAddr> ptrAddr;
    ptrIA->firstAddr();
    while(ptrAddr=ptrIA->getAddr())
    {
        if (!this->validateAddr(ptrIface, ptrIA, ptrAddr))
            return false;
    }
    return true;
}

bool TClntCfgMgr::validateAddr(SPtr<TClntCfgIface> ptrIface,
                               SPtr<TClntCfgIA> ptrIA,
                               SPtr<TClntCfgAddr> ptrAddr) {
    SPtr<TIPv6Addr> addr = ptrAddr->get();
    if ( addr && addr->linkLocal()) {
        Log(Crit) << "Address " << ptrAddr->get()->getPlain() << " specified in IA "
                  << ptrIA->getIAID() << " on the " << ptrIface->getName() << "/" << ptrIface->getID()
                  << " interface is link local." << LogEnd;
        return false;
    }
    if( ptrAddr->getPref()>ptrAddr->getValid() ) {
        Log(Crit) << "Prefered time " << ptrAddr->getPref() << " can't be lower than valid lifetime "
                  << ptrAddr->getValid() << " for IA " << ptrIA->getIAID() << " on the "
                  << ptrIface->getName() << "/" << ptrIface->getID() << " interface." << LogEnd;
        return false;
    }
    if ((unsigned long)ptrIA->getT1()>(unsigned long)ptrAddr->getValid()) {
        Log(Crit) << "Valid lifetime " << ptrAddr->getValid() << " can't be lower than T1 " <<ptrIA->getT1()
                  << "(address can't be renewed) in IA " << ptrIA->getIAID() << " on the "
                  << ptrIface->getName() << "/" << ptrIface->getName() << " interface." << LogEnd;
        return false;
    }

    return true;
}

bool TClntCfgMgr::validatePD(SPtr<TClntCfgIface> ptrIface, SPtr<TClntCfgPD> ptrPD) {

    if ( ptrPD->getT1() && ptrPD->getT2() && (ptrPD->getT2() < ptrPD->getT1()) )
    {
        Log(Crit) << "Non-zero T1 can't be lower than non-zero T2 for PD " << *ptrPD
                  << " on the " << ptrIface->getFullName() << " interface." << LogEnd;
        return false;
    }
    SPtr<TClntCfgPrefix> ptrPrefix;
    ptrPD->firstPrefix();
    while(ptrPrefix = ptrPD->getPrefix())
    {
        if (!this->validatePrefix(ptrIface, ptrPD, ptrPrefix))
            return false;
    }
    return true;
}

bool TClntCfgMgr::validatePrefix(SPtr<TClntCfgIface> ptrIface,
                                 SPtr<TClntCfgPD> ptrPD,
                                 SPtr<TClntCfgPrefix> ptrPrefix) {
    if( ptrPrefix->getPref() > ptrPrefix->getValid() ) {
        Log(Crit) << "Preferred time " << ptrPrefix->getPref() << " can't be lower than valid lifetime "
                  << ptrPrefix->getValid() << " for PD " << ptrPD->getIAID() << " on the "
                  << ptrIface->getFullName() << " interface." << LogEnd;
        return false;
    }

    // Note that valid-lifetime can be larger than the T1. This is uncommon, but
    // valid scenario. It's a way to tell the client to never renew.
    // This may be useful if ISP wants his clients to periodically change their
    // delegated prefixes.
    //
    // Quote from RFC3633, section 10:
    // A requesting router discards any prefixes for which the preferred
    // lifetime is greater than the valid lifetime.  A delegating router
    // ignores the lifetimes set by the requesting router if the preferred
    // lifetime is greater than the valid lifetime and ignores the values
    // for T1 and T2 set by the requesting router if those values are
    // greater than the preferred lifetime.

    return true;
}

SPtr<TClntCfgIface> TClntCfgMgr::getIface(int id)
{
    firstIface();
    SPtr<TClntCfgIface> iface;
    while(iface=getIface())
        if (iface->getID()==id) return iface;
    return SPtr<TClntCfgIface> ();
}

SPtr<TClntCfgIface> TClntCfgMgr::getIfaceByIAID(int iaid)
{
    SPtr<TClntCfgIface> iface;
    firstIface();
    while(iface=getIface())
    {
        SPtr<TClntCfgIA> ia;
        iface->firstIA();
        while(ia=iface->getIA())
            if (ia->getIAID()==iaid)
                return iface;
    }
    return SPtr<TClntCfgIface>();
}

bool TClntCfgMgr::setGlobalOptions(ClntParser * parser)
{
    SPtr<TClntParsGlobalOpt> opt = parser->ParserOptStack.getLast();
    LogLevel       = logger::getLogLevel();
    LogName        = logger::getLogName();
    AnonInfRequest = opt->getAnonInfRequest();
    InsistMode     = opt->getInsistMode();// should the client insist on receiving
                                          // all options i.e. sending INF-REQUEST
                                          // if REQUEST did not grant required opts
    if (opt->getInactiveMode()) // should the client accept not ready interfaces?
        InactiveMode   = true;  // This may have been set up already by obeyRaBits()

    FQDNFlagS      = opt->getFQDNFlagS();
    UseConfirm     = opt->getConfirm(); // should client try to send CONFIRM?

    // user has specified DUID type, just in case if new DUID will be generated
    if (parser->DUIDType != DUID_TYPE_NOT_DEFINED) {
        DUIDType = parser->DUIDType;
        DUIDEnterpriseNumber = parser->DUIDEnterpriseNumber;
        DUIDEnterpriseID     = parser->DUIDEnterpriseID;
    }

#ifndef MOD_DISABLE_AUTH
    if (getAuthProtocol() == AUTH_PROTO_DIBBLER) {
        SPI_ = getAAASPIfromFile();
        if (SPI_) {
            Log(Debug) << "Auth: Read SPI=" << hex << SPI_ << dec
                       << " from a AAA-SPI file:" << CLNT_AAASPI_FILE << LogEnd;
        } else {
            Log(Crit) << "Auth: Dibbler protocol configured, but unable to read AAA-SPI file:"
                      << CLNT_AAASPI_FILE << LogEnd;
            return false;
        }

        // now let's try to load specified key
        unsigned len = 0;
        char * ptr = getAAAKey(SPI_, &len);
        if (len == 0) {
            Log(Crit) << "Auth: Failed to load AAA key from file "
                      << getAAAKeyFilename(SPI_) << LogEnd;
            return false;
        }
        free(ptr); // we don't really need the key yet.
    }
#endif

    return true;
}

#ifdef MOD_REMOTE_AUTOCONF
void TClntCfgMgr::setRemoteAutoconf(bool enable) {
    Log(Info) << "Remote autoconf " << (enable?"enabled":"disabled") << LogEnd;
    RemoteAutoconf = enable;
}

bool TClntCfgMgr::getRemoteAutoconf() {
    return RemoteAutoconf;
}
#endif

void TClntCfgMgr::setDigest(DigestTypes type)
{
    if (type >= DIGEST_INVALID)
        Digest_ = DIGEST_NONE;
    else
        Digest_ = type;
}

DigestTypes TClntCfgMgr::getDigest()
{
    return Digest_;
}

bool TClntCfgMgr::isDone() {
    return IsDone;
}

bool TClntCfgMgr::anonInfRequest()
{
    return AnonInfRequest;
}

bool TClntCfgMgr::inactiveMode()
{
    return InactiveMode;
}

bool TClntCfgMgr::addInfRefreshTime()
{
    /* this can be considered a workaround, but for now ask for inf-refresh-time
       only when running in stateless mode */

    // are there any stateful interfaces?
    SPtr<TClntCfgIface> ptr;
    firstIface();
    while ( ptr = getIface() ) {
        if (!ptr->stateless())
            return false;
    }

    return true;
}

bool TClntCfgMgr::insistMode()
{
    return InsistMode;
}

int TClntCfgMgr::inactiveIfacesCnt()
{
    return InactiveLst.count();
}

SPtr<TClntCfgIface> TClntCfgMgr::checkInactiveIfaces()
{
    if (!InactiveLst.count())
        return SPtr<TClntCfgIface>(); // NULL

    ClntIfaceMgr().redetectIfaces();
    SPtr<TClntCfgIface> x;
    SPtr<TIfaceIface> iface;
    InactiveLst.first();
    while (x = InactiveLst.get()) {
        iface = ClntIfaceMgr().getIfaceByID(x->getID());
        if (!iface) {
            Log(Error) << "Unable to find interface with ifindex=" << x->getID() << LogEnd;
            continue;
        }
        iface->firstLLAddress();
        if (iface->flagUp() && iface->flagRunning() && iface->getLLAddress()) {
            // check if its link-local address is not tentative
            char tmp[64];
            iface->firstLLAddress();
            inet_ntop6(iface->getLLAddress(), tmp);
            if (is_addr_tentative(iface->getName(), iface->getID(), tmp)==LOWLEVEL_TENTATIVE_YES) {
                Log(Debug) << "Interface " << iface->getFullName()
                           << " is up and running, but link-local address " << tmp
                           << " is currently tentative." << LogEnd;
                continue;
            }

            if (obeyRaBits() && !iface->getMBit() && !iface->getOBit()) {
                Log(Debug) << "Interface " << iface->getFullName()
                           << " is up and running, but did not receive Router Advertisement "
                           << "with either M or O bits set." << LogEnd;
                continue;
            }

            bool managed = !obeyRaBits() || iface->getMBit();
            bool otherConf = !obeyRaBits() || iface->getOBit();

            makeInactiveIface(x->getID(), false, managed, otherConf); // move it to active mode
            return x;
        }
    }

    return SPtr<TClntCfgIface>(); // NULL
}

#ifndef MOD_DISABLE_AUTH
uint32_t TClntCfgMgr::getSPI() {
    return SPI_;
}

/// @todo move this to CfgMgr and unify with TSrvCfgMgr::setAuthDigests
void TClntCfgMgr::setAuthAcceptMethods(const std::vector<DigestTypes>& methods) {
    AuthAcceptMethods_ = methods;
    if (!methods.empty()) {
        Log(Debug) << "AUTH: " << methods.size() << " method(s) accepted:";

        for (unsigned i = 0; i < methods.size(); ++i) {
            Log(Cont) << " " << getDigestName(methods[i]);
            if (i==0) {
                Log(Cont) << "(default)";
            }
        }
        Log(Cont) << LogEnd;

        // Use the first method as the default one
        Digest_ = methods[0];
    }
}

const std::vector<DigestTypes>& TClntCfgMgr::getAuthAcceptMethods() {
    return AuthAcceptMethods_;
}
#endif

bool TClntCfgMgr::getFQDNFlagS()
{
    return FQDNFlagS;
}

bool TClntCfgMgr::useConfirm()
{
    return UseConfirm;
}

TClntCfgMgr::~TClntCfgMgr() {
    Log(Debug) << "ClntCfgMgr cleanup." << LogEnd;
}

ostream & operator<<(ostream &strum, TClntCfgMgr &x)
{
    strum << "<ClntCfgMgr>" << endl;
    strum << "  <workdir>" << x.getWorkDir()  << "</workdir>" << endl;
    strum << "  <scriptName>" << x.getScript() << "</scriptName>" << endl;
    strum << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    strum << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;
    strum << "  <AnonInfRequest>" << (x.anonInfRequest()?1:0) << "</AnonInfRequest>" << endl;
    strum << "  <InsistMode>" << (x.InsistMode?1:0) << "</InsistMode>" << endl;
    strum << "  <InactiveMode>" << (x.InactiveMode?1:0) << "</InactiveMode>" << endl;
    strum << "  <FQDNFlagS>" << (x.FQDNFlagS?1:0) << "</FQDNFlagS>" << endl;
    strum << "  <useConfirm>" << (x.UseConfirm?1:0) << "</useConfirm>" << endl;
    strum << "  <digest>";
    switch (x.getDigest()) {
    case DIGEST_NONE:
        strum << "digest-none";
            break;
    case DIGEST_HMAC_SHA1:
            strum << "digest-hmac-sha1";
            break;
    default:
        strum << x.getDigest();
            break;
    };
    strum << "</digest>" << endl;
    if (x.DUID)
        strum << "  " << *x.DUID;
    else
        strum << "  <!-- duid not set -->";

    SPtr<TClntCfgIface> ptr;
    x.firstIface();

    while ( ptr = x.getIface() ) {
        strum << *ptr;
    }

    strum << "</ClntCfgMgr>" << endl;
    return strum;
}

void TClntCfgMgr::setDownlinkPrefixIfaces(List(std::string)& ifaces) {
    ifaces.first();
    SPtr<string> iface;
    Log(Debug) << "PD: Following interfaces marked as downlink:";
    while (iface = ifaces.get()) {
        Log(Cont) << " " << *iface;
        DownlinkPrefixIfaces_.push_back(*iface);
    }
    Log(Cont) << LogEnd;
}

void TClntCfgMgr::obeyRaBits(bool obey) {
    ObeyRaBits_ = obey;
    if (obey) {
        InactiveMode = true;
    }
}

bool TClntCfgMgr::obeyRaBits() {
    return ObeyRaBits_;
}

#ifdef MOD_CLNT_EMBEDDED_CFG
/**
 * this is example hardcoded configuration file
 *
 * @param cfgMgr
 * @param params
 *
 * @return
 */
bool HardcodedCfgExample(TClntCfgMgr *cfgMgr, string params)
{
    Log(Info) << "Using hardcoded config. file." << LogEnd;

    // there's no way to set some parameters directly, to fake ClntParsGlobalOpt
    // must be created.
    SPtr<TClntParsGlobalOpt> opt = new TClntParsGlobalOpt();

    // Pretend to have parsed empty DNS list (i.e. request DNS server configuration, but don't
    // provide any hints)
    List(TIPv6Addr) dnsList;
    dnsList.clear();
    opt->setDNSServerLst(&dnsList);
    // Pretend to have parsed rapid-commit request
    opt->setRapidCommit(true);

    // Create interface, which will be configured
    SPtr<TClntCfgIface> iface = new TClntCfgIface("eth0");
    iface->setIfaceID(3);

    // set all "parsed" options on this interface
    iface->setOptions(opt);

    // Create one IA
    SPtr<TClntCfgIA> ia = new TClntCfgIA();
    ia->setIAID(123);

    // set parameters in the "parsed" objects
    opt->setT1(900);
    opt->setT2(1200);
    ia->setOptions(opt);

    // optional: add a requested address to IA
    // request for a 2000::123:456 address with preferred lifetime set to 1800
    // and valid lifetime set to 3600
    SPtr<TIPv6Addr> addr = new TIPv6Addr("2000::123:456", true);
    SPtr<TClntCfgAddr> cfgAddr = new TClntCfgAddr(addr, 3600, 1800);
    ia->addAddr(cfgAddr);

    // add IA to a interface
    iface->addIA(ia);

    // add interface to a CfgMgr
    cfgMgr->addIface(iface);

    return true;
}
#endif
