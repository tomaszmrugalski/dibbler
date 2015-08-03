/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Petr Pisar <petr.pisar(at)atlas(dot)cz>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *          Nguyen Vinh Nghiem
 *          Grzegorz Pluto <g.pluto(at)u-r-b-a-n(dot)pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "SmartPtr.h"
#include "Portable.h"
#include "FlexLexer.h"
#include "SrvParser.h"
#include "SrvCfgMgr.h"
#include "SrvCfgIface.h"
#include "Logger.h"
#include "IfaceMgr.h"
#include "SrvIfaceMgr.h"
#include "AddrMgr.h"
#include "SrvParser.h"
#include "OptDUID.h"

using namespace std;


TSrvCfgMgr * TSrvCfgMgr::Instance = 0;
int TSrvCfgMgr::NextRelayID = RELAY_MIN_IFINDEX;

TSrvCfgMgr::TSrvCfgMgr(const std::string& cfgFile, const std::string& xmlFile)
    :TCfgMgr(), XmlFile(xmlFile), Reconfigure_(false), PerformanceMode_(false),
     DropUnicast_(false)
{
    setDefaults();

    // load config file
    if (!this->parseConfigFile(cfgFile)) {
            IsDone = true;
              return;
    }

    // load or create DUID
    string duidFile = (string)SRVDUID_FILE;
    if (!setDUID(duidFile, SrvIfaceMgr())) {
                this->IsDone=true;
                return;
    }

    this->dump();

#ifndef MOD_DISABLE_AUTH
    AuthKeys = new KeyList();
#endif

    IsDone = false;
}

void TSrvCfgMgr::setDefaults()
{
    BulkLQAccept = BULKLQ_ACCEPT;
    BulkLQTcpPort = BULKLQ_TCP_PORT;
    BulkLQMaxConns = BULKLQ_MAX_CONNS;
    BulkLQTimeout = BULKLQ_TIMEOUT;
}

bool TSrvCfgMgr::parseConfigFile(const std::string& cfgFile) {
    int result;
    ifstream f;

    // parse config file
    f.open( cfgFile.c_str() );
    if ( ! f.is_open() ) {
            Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd;
            return false;
    } else {
        Log(Notice) << "Parsing " << cfgFile << " config file..." << LogEnd;
    }
    yyFlexLexer lexer(&f,&clog);
    SrvParser parser(&lexer);
    parser.CfgMgr = this; // just a workaround (parser is called, while SrvCfgMgr is still
                          // in constructor, so instance() singleton method can't be called
    result = parser.yyparse();
    Log(Debug) << "Parsing " << cfgFile << " done." << LogEnd;
    f.close();

    this->LogLevel = logger::getLogLevel();
    this->LogName  = logger::getLogName();

    if (result) {
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
        this->IsDone = true;
        return false;
    }

    // setup global options
    this->setGlobalOptions(parser.ParserOptStack.getLast());

    // setup ClientClass  List
    ClientClassLst = parser.SrvCfgClientClassLst;

    Log(Info) << ClientClassLst.count() << " client class(es) defined." << LogEnd;

    // analyse interfaces mentioned in config file
    if (!this->matchParsedSystemInterfaces(&parser)) {
        this->IsDone = true;
        return false;
    }

    // check for invalid values, e.g. T1>T2
    if(!this->validateConfig()) {
        this->IsDone = true;
        return false;
    }

    if (this->stateless()) {
        Log(Notice) << "Running in stateless mode." << LogEnd;
    } else {
        Log(Notice) << "Running in stateful mode." << LogEnd;
    }

    // @todo: remove this info later
    Log(Debug) << "Bulk-leasequery: enabled=" << (BulkLQAccept?"yes":"no")
               << ", TCP port=" << BulkLQTcpPort
               << ", max conns=" << BulkLQMaxConns
               << ", timeout=" << BulkLQTimeout << LogEnd;

    return true;
}

void TSrvCfgMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

bool TSrvCfgMgr::setGlobalOptions(SPtr<TSrvParsGlobalOpt> opt) {
    this->Workdir          = opt->getWorkDir();
    this->Stateless        = opt->getStateless();
    this->CacheSize        = opt->getCacheSize();
    this->InterfaceIDOrder = opt->getInterfaceIDOrder();
    this->InactiveMode     = opt->getInactiveMode(); // should the client accept not ready interfaces?
    this->GuessMode        = opt->getGuessMode();

    return true;
}

/*
 * Now parsed information should be placed in config manager
 * in accordance with information provided by interface manager
 */
bool TSrvCfgMgr::matchParsedSystemInterfaces(SrvParser *parser) {
    int cfgIfaceCnt;
    cfgIfaceCnt = parser->SrvCfgIfaceLst.count();
    Log(Debug) << cfgIfaceCnt << " interface(s) specified in " << SRVCONF_FILE << LogEnd;

    SPtr<TSrvCfgIface> cfgIface;
    SPtr<TIfaceIface>  ifaceIface;

    parser->SrvCfgIfaceLst.first();
    while(cfgIface=parser->SrvCfgIfaceLst.get()) {
        // for each interface from config file

        // map deny and allow list
        cfgIface->mapAllowDenyList(parser->SrvCfgClientClassLst);

        // relay interface
        if (cfgIface->isRelay()) {

            cfgIface->setID(this->NextRelayID++);

            // let's find physical interface underneath
            SPtr<TSrvCfgIface> under_relay = cfgIface;
            ifaceIface = SPtr<TIfaceIface>();
            while (!ifaceIface && under_relay && under_relay->getRelayName() != "") {
                ifaceIface = SrvIfaceMgr().getIfaceByName(under_relay->getRelayName());
                if (!ifaceIface) {
                    under_relay = getIfaceByName(under_relay->getRelayName());
                }
            }

            if (!ifaceIface) {
                Log(Crit) << "Interface " << cfgIface->getFullName()
                          << " defined physical interface as " << cfgIface->getRelayName()
                          << ", but there is no such interface present." << LogEnd;
                return false;
            }
            cfgIface->setRelayID(ifaceIface->getID());

            addIface(cfgIface);

            continue; // skip physical interface checking part
        }

        // physical interface
        if (cfgIface->getID()==-1) {
            // ID==-1 means that user referenced to interface by name
            ifaceIface = SrvIfaceMgr().getIfaceByName(cfgIface->getName());
        } else {
            ifaceIface = SrvIfaceMgr().getIfaceByID(cfgIface->getID());
        }
        if (!ifaceIface) {
            Log(Crit) << "Interface " << cfgIface->getFullName()
                      << " is not present in the system or does not support IPv6."
                      << LogEnd;
            return false;
        }

        // Complete name and ID (one of them usually misses depending on
        // identifier used in config file.
        /// @todo: Client's class uses setIface{Name,ID}(). We should unite the
        // method names.
        cfgIface->setName(ifaceIface->getName());
        cfgIface->setID(ifaceIface->getID());

        // Check for link scope address presence
        if (!ifaceIface->countLLAddress()) {
            if (this->inactiveMode()) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " is not operational yet (does not have link scope address), skipping it for now." << LogEnd;
                this->addIface(cfgIface);
                this->makeInactiveIface(cfgIface->getID(), true); // move it to InactiveLst
                continue;
            }

            Log(Crit) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID()
                      << " is down or doesn't have any link scope address." << LogEnd;
            return false;
        }

        // Check if the interface is during bring-up phase (i.e. DAD procedure for link-local addr is not complete yet)
        char tmp[64];
        ifaceIface->firstLLAddress();
        inet_ntop6(ifaceIface->getLLAddress(), tmp);
        if (is_addr_tentative(ifaceIface->getName(), ifaceIface->getID(), tmp) == LOWLEVEL_TENTATIVE_YES) {
            Log(Notice) << "Interface " << ifaceIface->getFullName() << " has link-scope address " << tmp
                        << ", but it is currently tentative." << LogEnd;

            if (this->inactiveMode()) {
                Log(Notice) << "Interface " << ifaceIface->getFullName()
                            << " is not operational yet (link-scope address is not ready), skipping it for now." << LogEnd;
                addIface(cfgIface);
                makeInactiveIface(cfgIface->getID(), true); // move it to InactiveLst
                continue;
            }

            Log(Crit) << "Interface " << ifaceIface->getFullName()
                      << " has tentative link-scope address (and inactive-mode is disabled)." << LogEnd;
            return false;

        }

        this->addIface(cfgIface);
        Log(Info) << "Interface " << cfgIface->getFullName()
                  << " configuration has been loaded." << LogEnd;
    }
    return true;
}

SPtr<TSrvCfgIface> TSrvCfgMgr::getIface() {
        return this->SrvCfgIfaceLst.get();
}

void TSrvCfgMgr::addIface(SPtr<TSrvCfgIface> ptr) {
    SrvCfgIfaceLst.append(ptr);
}

/**
 * Moves interface identified by ifindex between SrvCfgIfaceLst and InactiveLst.
 *
 * @param ifindex Index of the moving interface
 * @param inactive true for makeing inactive, false for makeing active
 */
void TSrvCfgMgr::makeInactiveIface(int ifindex, bool inactive) {
    SPtr<TSrvCfgIface> x;

    if (inactive)
    {
        SrvCfgIfaceLst.first();
        while (x= SrvCfgIfaceLst.get()) {
            if (x->getID() == ifindex) {
                Log(Info) << "Switching " << x->getFullName() << " to inactive-mode." << LogEnd;
                SrvCfgIfaceLst.del();
                InactiveLst.append(x);
                return;
            }
        }
        Log(Error) << "Unable to switch interface ifindex=" << ifindex << " to inactive-mode: interface not found." << LogEnd;
        // something is wrong, VERY wrong
    }

    else
    {
        InactiveLst.first();
        while (x= InactiveLst.get()) {
            if (x->getID() == ifindex) {
                Log(Info) << "Switching " << x->getFullName() << " to normal mode." << LogEnd;
                InactiveLst.del();
                InactiveLst.first();
                addIface(x);
                return;
            }
        }

        Log(Error) << "Unable to switch interface ifindex=" << ifindex << " from inactive-mode to normal operation: interface not found." << LogEnd;
    }
}

/**
 * Returns number of inactive interfaces
 */
int TSrvCfgMgr::inactiveIfacesCnt() {
    return InactiveLst.count();
}

void TSrvCfgMgr::firstIface() {
    SrvCfgIfaceLst.first();
}

long TSrvCfgMgr::countIface() {
    return SrvCfgIfaceLst.count();
}

/**
 *
 * Tryies to find interface in list of inactive interfaces which becames
 * ready. In positive case such an interface is moved back into standard list.
 * Only first matching interface is processed.
 *
 * @returns Interface that becomes ready and has been moved out of inactive list.
 * If no such interface exists, 0 will be returned.
 */
SPtr<TSrvCfgIface> TSrvCfgMgr::checkInactiveIfaces() {
    if (!InactiveLst.count())
        return SPtr<TSrvCfgIface>(); // NULL

    SrvIfaceMgr().redetectIfaces();
    SPtr<TSrvCfgIface> x;
    SPtr<TIfaceIface> iface;
    InactiveLst.first();
    while (x = InactiveLst.get()) {
        iface = SrvIfaceMgr().getIfaceByID(x->getID());
        if (!iface) {
            Log(Error) << "TSrvCfgMgr::checkInactiveIfaces(): " <<
                "Unable to find interface with ifindex=" << x->getID() << LogEnd;
            continue;
        }
        iface->firstLLAddress();
        if (iface->flagUp() && iface->flagRunning() && iface->getLLAddress()) {
            // check if its link-local address is not tentative
            char tmp[64];
            iface->firstLLAddress();
            inet_ntop6(iface->getLLAddress(), tmp);
            if (is_addr_tentative(iface->getName(), iface->getID(), tmp)==LOWLEVEL_TENTATIVE_YES) {
                Log(Debug) << "Interface " << iface->getFullName() << " is up and running, but link-scope address " << tmp
                           << " is currently tentative." << LogEnd;
                continue;
            }


            makeInactiveIface(x->getID(), false); // move it to InactiveLst
            return x;
        }
    }

    return SPtr<TSrvCfgIface>(); // NULL
}


TSrvCfgMgr::~TSrvCfgMgr() {
    Log(Debug) << "SrvCfgMgr cleanup." << LogEnd;
}

/**
 * returns how many addresses can be assigned to this client?
 * factors used:
 * - iface-max-lease
 * - clntSupported()
 * - class-max-lease in each class
 * - assignedCount in each class
 *
 * @param clntDuid
 * @param clntAddr
 * @param iface
 *
 * @return return value <= clnt-max-lease <= iface-max-lease
 */
long TSrvCfgMgr::countAvailAddrs(SPtr<TDUID> clntDuid,
                                 SPtr<TIPv6Addr> clntAddr,  int iface)
{
    /// @todo: long long long int (128bit) could come in handy
    double avail         = 0; // how many are available?
    double ifaceAssigned = 0; // how many are assigned on this iface?
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Interface " << iface << " does not exist in SrvCfgMgr." << LogEnd;
        return 0;
    }

    unsigned long ifaceMaxLease = ptrIface->getIfaceMaxLease();

    SPtr<TSrvCfgAddrClass> ptrClass;
    ptrIface->firstAddrClass();
    while (ptrClass = ptrIface->getAddrClass()) {
        if (!ptrClass->clntSupported(clntDuid,clntAddr))
            continue;
        unsigned long classMaxLease;
        unsigned long classAssigned;
        double tmp;
        classMaxLease = ptrClass->getClassMaxLease();
        classAssigned = ptrClass->getAssignedCount();
        tmp = classMaxLease - classAssigned;
        ifaceAssigned += classAssigned;
        if (tmp>0)
            avail += tmp;
    }

    if (avail > (ifaceMaxLease-ifaceAssigned))
        avail = ifaceMaxLease-ifaceAssigned;
    return (long)avail;
}

/**
 * return class, which address belongs to
 *
 * @param iface
 * @param addr
 *
 * @return pointer to the class (or 0, if no suitable class found)
 */
SPtr<TSrvCfgAddrClass> TSrvCfgMgr::getClassByAddr(int iface, SPtr<TIPv6Addr> addr)
{
    this->firstIface();
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);

    if (!ptrIface) {
        Log(Error) << "Trying to find class on unknown (" << iface <<") interface." << LogEnd;
        return SPtr<TSrvCfgAddrClass>(); // NULL
    }

    SPtr<TSrvCfgAddrClass> ptrClass;
    ptrIface->firstAddrClass();
    while (ptrClass = ptrIface->getAddrClass()) {
        if (ptrClass->addrInPool(addr))
            return ptrClass;
    }

    return SPtr<TSrvCfgAddrClass>(); // NULL
}

/**
 * get a class, which prefix belongs to
 *
 * @param iface
 * @param addr
 *
 * @return class (or 0 if no class is found)
 */
SPtr<TSrvCfgPD> TSrvCfgMgr::getClassByPrefix(int iface, SPtr<TIPv6Addr> addr)
{
    this->firstIface();
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);

    if (!ptrIface) {
        Log(Error) << "Trying to find class on unknown (" << iface <<") interface." << LogEnd;
        return SPtr<TSrvCfgPD>();
    }

    SPtr<TSrvCfgPD> ptrClass;
    ptrIface->firstPD();
    while (ptrClass = ptrIface->getPD()) {
        if (ptrClass->prefixInPool(addr))
            return ptrClass;
    }

    return SPtr<TSrvCfgPD>();
}


//on basis of duid/address/iface assign addresss to client
SPtr<TIPv6Addr> TSrvCfgMgr::getRandomAddr(SPtr<TDUID> clntDuid,
                                              SPtr<TIPv6Addr> clntAddr,
                                              int iface) {
    SPtr<TSrvCfgIface> ptrIface;
    SPtr<TSrvCfgAddrClass> ptrClass;
    SPtr<TIPv6Addr>    any;

    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        return SPtr<TIPv6Addr>();
    }

    any = new TIPv6Addr();
    ptrIface->firstAddrClass();
    ptrClass = ptrIface->getAddrClass();
    return ptrClass->getRandomAddr();

    /// @todo: get addrs from first address only
}

/// Checks if a given client is supported
///
/// @param msg message sent by client
///
/// @return true if supported, false otherwise
bool TSrvCfgMgr::isClntSupported(SPtr<TSrvMsg> msg) {
    int iface=msg->getIface();
    SPtr<TIPv6Addr> clntAddr = msg->getRemoteAddr();

    SPtr<TOpt> opt = msg->getOption(OPTION_CLIENTID);
    SPtr<TDUID> duid;
    if (!opt) {
        // malformed message or anonymous inf-request
        duid = new TDUID("", 0); // zero-length DUID
    } else {
        SPtr<TOptDUID> clientId = (Ptr*) opt;
        duid = clientId->getDUID();
    }

    SPtr<TSrvCfgIface> ptrIface;
    firstIface();
    while((ptrIface=getIface())&&(ptrIface->getID()!=iface)) ;

    /** @todo: reject-client and accept-only does not work in stateless mode */
    if (this->stateless())
        return true;

    int classCnt = 0;
    if (ptrIface)
    {
        SPtr<TSrvCfgAddrClass> ptrClass;
        ptrIface->firstAddrClass();
        while(ptrClass=ptrIface->getAddrClass()) {
            if (ptrClass->clntSupported(duid,clntAddr))
                return true;
            classCnt++;
        }

        SPtr<TSrvCfgPD> pd;
        ptrIface->firstPD();
        while ( pd=ptrIface->getPD() ) {
            if (pd->clntSupported(duid, clntAddr, msg))
                return true;
            classCnt++;
        }
    }
    if (!classCnt && ptrIface) {
        Log(Warning) << "There are no address class defined on the " << ptrIface->getFullName()
                     << ". Maybe you are trying to configure clients on cascade relay interface? "
                     << "If that is so, please define separate class on this interface, too." << LogEnd;
    } else if (!classCnt) {
        Log(Warning) << "Interface not found."
                     << "Maybe you are trying to configure clients on cascade relay interface? "
                     << "If that is so, please define separate class on this interface, too." << LogEnd;
    }
    return false;
}

#if 0
/*
* Method checks whether client is supported and assigned addresses from any class
*/
bool TSrvCfgMgr::isClntSupported(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, int iface, SPtr<TSrvMsg> msg)
{
   SPtr<TSrvCfgIface> ptrIface;
   firstIface();
   while((ptrIface=getIface())&&(ptrIface->getID()!=iface)) ;

   /** @todo: reject-client and accept-only does not work in stateless mode */
   if (this->stateless())
        return true;

   int classCnt = 0;
   if (ptrIface)
   {
       SPtr<TSrvCfgAddrClass> ptrClass;
       ptrIface->firstAddrClass();
       while(ptrClass=ptrIface->getAddrClass()) {
           if (ptrClass->clntSupported(duid,clntAddr,msg))
               return true;
            classCnt++;
        }
   }
   if (!classCnt) {
        Log(Warning) << "There are no address class defined on the " << ptrIface->getFullName()
                     << ". Maybe you are trying to configure clients on cascade relay interface? "
                     << "If that is so, please define separate class on this interface, too." << LogEnd;
   }
   return false;
}
#endif

/// checks if an address is reserved (checks all interfaces)
///
/// @param addr
///
/// @return true if reserved, false otherwise
bool TSrvCfgMgr::addrReserved(SPtr<TIPv6Addr> addr) {
    SPtr<TSrvCfgIface> iface;
    SrvCfgIfaceLst.first();
    while (iface = SrvCfgIfaceLst.get()) {
        if (iface->addrReserved(addr))
            return true;
    }
    return false;
}

/// checks if a prefix is reserved (checks all interfaces)
///
/// @param prefix prefix to be checked
///
/// @return true if reserved, false otherwise
bool TSrvCfgMgr::prefixReserved(SPtr<TIPv6Addr> prefix) {
    SPtr<TSrvCfgIface> iface;
    SrvCfgIfaceLst.first();
    while (iface = SrvCfgIfaceLst.get()) {
        if (iface->prefixReserved(prefix))
            return true;
    }
    return false;
}

bool TSrvCfgMgr::isDone() {
    return IsDone;
}

bool TSrvCfgMgr::validateConfig() {
    SPtr<TSrvCfgIface> ptrIface;

    if (0 == (this->countIface() + this->inactiveIfacesCnt())) {
        Log(Crit) << "Config problem: No interface defined." << LogEnd;
        return false;
    }

    firstIface();
    while(ptrIface=getIface()) {
        if (!this->validateIface(ptrIface))
            return false;
    }
    return true;
}

bool TSrvCfgMgr::validateIface(SPtr<TSrvCfgIface> ptrIface)
{
    SPtr<TIfaceIface> iface = SrvIfaceMgr().getIfaceByID(ptrIface->getID());

    if (ptrIface->countAddrClass() && stateless()) {
        Log(Crit) << "Config problem: Interface " << ptrIface->getFullName()
                  << ": Class definitions present, but stateless mode set." << LogEnd;
        return false;
    }
    if (!ptrIface->countAddrClass() && !ptrIface->countPD()
        && !ptrIface->getTA() && !stateless()) {
        if (!ptrIface->isRelay()) {
            Log(Crit) << "Config problem: Interface " << ptrIface->getName() << "/" << ptrIface->getID()
                      << ": No class definitions (IA,TA or PD) present, but stateless mode not set." << LogEnd;
            return false;
        } else {
            Log(Notice) << "Interface " << ptrIface->getFullName() << " has no address or prefix pools defined."
                        << LogEnd;
        }
    }

    if (ptrIface->supportFQDN() && !ptrIface->getExtraOption(OPTION_DNS_SERVERS)) {
        Log(Crit) << "FQDN defined on the " << ptrIface->getFullName() << ", but no DNS servers defined."
                  << " Please disable FQDN support or add DNS servers." << LogEnd;
        return false;
    }

    SPtr<TSrvCfgAddrClass> ptrClass;
    ptrIface->firstAddrClass();
    while(ptrClass=ptrIface->getAddrClass()) {
        if (!this->validateClass(ptrIface, ptrClass)) {
            Log(Crit) << "Config problem: Interface " << ptrIface->getName() << "/" << ptrIface->getID()
                      << ": Invalid class defined." << LogEnd;
            return false;
        }
    }
    return true;
}

bool TSrvCfgMgr::validateClass(SPtr<TSrvCfgIface> ptrIface, SPtr<TSrvCfgAddrClass> ptrClass)
{
    if (ptrClass->isLinkLocal()) {
        Log(Crit) << "One of the classes defined on the " << ptrIface->getName() << "/" << ptrIface->getID()
                  << " overlaps with link local addresses." << LogEnd;
        return false;
    }

    if ( ptrClass->getPref(0) > ptrClass->getValid(0x7fffffff) )
    {
        Log(Crit) << "Prefered time max value (" <<ptrClass->getPref(0x7fffffff)
                  << ") can't be lower than valid time min. value (" << ptrClass->getValid(0)
                  << ") on the " << ptrIface->getName() << "/"
                  << ptrIface->getID() << " interface." << LogEnd;
        return false;
    }
    if ( ptrClass->getT1(0)>ptrClass->getT2(0x7fffffff) )
    {
        Log(Crit) << "T1 timeout upper bound (" <<ptrClass->getPref(0x7fffffff)
                  << " can't be lower than T2 lower bound (" << ptrClass->getT2(0)
                  << ") on the "<<ptrIface->getName()<<"/"
                  << ptrIface->getID() << " interface." << LogEnd;
        return false;
    }
    return true;
}

SPtr<TSrvCfgIface> TSrvCfgMgr::getIfaceByID(int iface) {
    SPtr<TSrvCfgIface> ptrIface;
    firstIface();
    while ( ptrIface = getIface() ) {
        if ( ptrIface->getID()==iface )
            return ptrIface;
    }
    Log(Error) << "Invalid interface (ifindex=" << iface
               << ") specifed: no such interface." << LogEnd;
    return SPtr<TSrvCfgIface>(); // NULL
}

SPtr<TSrvCfgIface> TSrvCfgMgr::getIfaceByName(const std::string& name) {
    SPtr<TSrvCfgIface> ptrIface;
    firstIface();
    while ( ptrIface = getIface() ) {
        if ( ptrIface->getName()==name ) {
            return ptrIface;
        }
    }
    Log(Error) << "Invalid interface (name=" << name
               << ") specifed: no such interface." << LogEnd;
    return SPtr<TSrvCfgIface>(); // NULL
}


void TSrvCfgMgr::delClntAddr(int iface, SPtr<TIPv6Addr> addr) {
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Warning) << "Unable to decrease address usage: unknown interface (id="
                     << iface << ")" << LogEnd;
        return;
    }
    ptrIface->delClntAddr(addr);
}

void TSrvCfgMgr::addClntAddr(int iface, SPtr<TIPv6Addr> addr) {
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Warning) << "Unable to increase address usage: unknown interface (id="
                     << iface << ")" << LogEnd;
        return;
    }
    ptrIface->addClntAddr(addr);
}

void TSrvCfgMgr::addTAAddr(int iface) {
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to increase address usage: interface (id="
                   << iface << ") not found." << LogEnd;
        return;
    }
    ptrIface->addTAAddr();
}

void TSrvCfgMgr::delTAAddr(int iface) {
    SPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Error) << "Unable to decrease address usage: interface (id="
                   << iface << ") not found." << LogEnd;
        return;
    }
    ptrIface->delTAAddr();
}

bool TSrvCfgMgr::stateless() {
    return this->Stateless;
}

bool TSrvCfgMgr::inactiveMode()
{
    return InactiveMode;
}

bool TSrvCfgMgr::guessMode()
{
    return GuessMode;
}

bool TSrvCfgMgr::setupRelay(SPtr<TSrvCfgIface> cfgIface) {
    SPtr<TIfaceIface> iface;
    string name = cfgIface->getRelayName();

    iface = SrvIfaceMgr().getIfaceByName(name);
    if (!iface) {
        Log(Crit) << "Underlaying interface for " << cfgIface->getFullName()
                  << " with name " << name << " is missing." << LogEnd;
        return false;
    }
    cfgIface->setRelayID(iface->getID());

    return true;
}

/**
 * returns size (in bytes of the configured cache size
 *
 * @return
 */
int TSrvCfgMgr::getCacheSize() {
    return this->CacheSize;
}

ESrvIfaceIdOrder TSrvCfgMgr::getInterfaceIDOrder()
{
    return InterfaceIDOrder;
}

/// @ decreases prefix usage count (i.e. decreases prefix-pool usage by one)
/// Actual prefix is also deleted in AddrMgr.
///
/// @param ifindex interface index
/// @param prefix  prefix to be deleted
///
/// @return true is deletion was successful
bool TSrvCfgMgr::decrPrefixCount(int ifindex, SPtr<TIPv6Addr> prefix)
{
    SPtr<TSrvCfgIface> iface;
    iface = getIfaceByID(ifindex);
    if (!iface) {
              Log(Error) << "Unable to find interface with ifindex=" << ifindex << ", prefix deletion aborted." << LogEnd;
              return false;
    }
    return iface->delClntPrefix(prefix);
}

bool TSrvCfgMgr::incrPrefixCount(int ifindex, SPtr<TIPv6Addr> prefix)
{
    SPtr<TSrvCfgIface> iface;
    iface = getIfaceByID(ifindex);
    if (!iface) {
        Log(Error) << "Unable to find interface with ifindex=" << ifindex << ", prefix increase count aborted." << LogEnd;
        return false;
    }
    return iface->addClntPrefix(prefix);
}

#ifndef MOD_DISABLE_AUTH

/// @todo move this to CfgMgr and unify with TClntCfgMgr::setAuthAcceptMethods
void TSrvCfgMgr::setAuthDigests(const DigestTypesLst& types) {
  DigestTypesLst_ = types;
}

DigestTypesLst TSrvCfgMgr::getAuthDigests() {
    return DigestTypesLst_;
}

enum DigestTypes TSrvCfgMgr::getDigest() {
    if (DigestTypesLst_.empty())
        return DIGEST_NONE;

    return DigestTypesLst_[0];
}

#endif

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream &out, TSrvCfgMgr &x) {
    out << "<SrvCfgMgr>" << std::endl;
    out << "  <workDir>" << x.getWorkDir()  << "</workDir>" << endl;
    out << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    out << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;
    out << "  <InactiveMode>" << (x.InactiveMode?1:0) << "</InactiveMode>" << endl;
    out << "  <GuessMode>" << (x.GuessMode?1:0) << "</GuessMode>" << endl;
    if (x.DUID)
        out << "  " << *x.DUID;
    else
        out << "  <!-- DUID not set -->" << std::endl;

    out << "  <InterfaceIDOrder>";
    switch (x.InterfaceIDOrder) {
    case SRV_IFACE_ID_ORDER_BEFORE:
        out << "before";
        break;
    case SRV_IFACE_ID_ORDER_AFTER:
        out << "after";
        break;
    case SRV_IFACE_ID_ORDER_NONE:
        out << "none";
        break;
    default:
        out << "unknown(" << x.InterfaceIDOrder << ")";
        break;
    }
    out << "</InterfaceIDOrder>" << endl;

#ifndef MOD_DISABLE_AUTH
    out << "  <auth count=\"" << x.DigestTypesLst_.size() << "\">";
    for (DigestTypesLst::const_iterator dig = x.DigestTypesLst_.begin();
	 dig != x.DigestTypesLst_.end(); ++dig) {
      out << getDigestName(*dig) << " ";
    }
    out << "</auth>" << endl;
#endif

    if (x.stateless())
        out << "  <stateless/>" << std::endl;
    else
        out << "  <!-- <stateless/> -->" << std::endl;

    SPtr<TSrvCfgIface> ptrIface;
    x.firstIface();
    while (ptrIface = x.getIface()) {
        out << *ptrIface;
    }
    out << "</SrvCfgMgr>" << std::endl;
    return out;
}

/// @todo: Fix this! We check if message is from accepted client, but don't do anything with that info
void TSrvCfgMgr::InClientClass(SPtr<TSrvMsg> msg)
{
        // For each client class, check whether the message belong to ClientClass
        ClientClassLst.first();
        SPtr<TSrvCfgClientClass> clntClass;

        while( clntClass= ClientClassLst.get() )
        {
            Log(Debug) << "Checking if client belongs to " << clntClass->getClassName() << " client class";
                if (clntClass->isStatisfy(msg))
                        Log(Cont)<<" ... yes." << LogEnd;
                else
                        Log(Cont)<<" ... no." << LogEnd;
        }
}

void TSrvCfgMgr::setReconfigureSupport(bool reconf) {
    Reconfigure_ = reconf;
}

bool TSrvCfgMgr::getReconfigureSupport()
{
    return Reconfigure_;
}

/// @brief removes reserved entries from the cache
void TSrvCfgMgr::removeReservedFromCache() {
    SPtr<TSrvCfgIface> iface;
    SrvCfgIfaceLst.first();
    unsigned int cnt = 0;
    while (iface = SrvCfgIfaceLst.get()) {
        cnt += iface->removeReservedFromCache();
    }
    if (cnt) {
        Log(Info) << "Removed " << cnt << " leases from cache that are reserved." << LogEnd;
    }
}

/**
 * sets pool usage counters (used during bringup, after AddrDB is loaded from file)
 *
 */
void TSrvCfgMgr::setCounters()
{
    int iaCnt = 0, pdCnt = 0;
    SrvAddrMgr().firstClient();
    SPtr<TAddrClient> client;
    SPtr<TSrvCfgIface> iface;
    while (client = SrvAddrMgr().getClient()) {

        // addresses
        SPtr<TAddrIA> ia;
        client->firstIA();
        while ( ia=client->getIA() ) {
            iface = getIfaceByID(ia->getIfindex());
            if (!iface)
                continue;

            SPtr<TAddrAddr> addr;
            ia->firstAddr();
            while ( addr=ia->getAddr() ) {
                iface->addClntAddr(addr->get(), true/*quiet*/);
                iaCnt++;
            }
        }

        // prefixes
        client->firstPD();
        while (ia = client->getPD() ) {
            iface = getIfaceByID(ia->getIfindex());
            if (!iface)
                continue;
            SPtr<TAddrPrefix> prefix;
            ia->firstPrefix();
            while ( prefix=ia->getPrefix() ) {
                iface->addClntPrefix(prefix->get(), true);
                pdCnt++;
            }
        }
    }
    Log(Debug) << "Increased pools usage: currently " << iaCnt << " address(es) and " << pdCnt << " prefix(es) are leased." << LogEnd;
}


void TSrvCfgMgr::instanceCreate(const std::string& cfgFile, const std::string& xmlDumpFile )
{
    if (Instance) {
        Log(Crit) << "SrvCfgMgr already created. Application error!" << LogEnd;
        return;
    }
    Instance = new TSrvCfgMgr(cfgFile, xmlDumpFile);

}

TSrvCfgMgr & TSrvCfgMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "SrvCfgMgr not initalized yet. Application error. Emergency shutdown." << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}

    // Bulk-LeaseQuery
void TSrvCfgMgr::bulkLQAccept(bool enabled)
{
    BulkLQAccept = enabled;
}

void TSrvCfgMgr::bulkLQTcpPort(unsigned short portNumber)
{
    BulkLQTcpPort = portNumber;
}

void TSrvCfgMgr::bulkLQMaxConns(unsigned int maxConnections)
{
    BulkLQMaxConns = maxConnections;
}

void TSrvCfgMgr::bulkLQTimeout(unsigned int timeout)
{
    BulkLQTimeout = timeout;
}

/// Sets DNS server address suitable for DNS Update
///
/// @param ddnsAddress DNS server address
void TSrvCfgMgr::setDDNSAddress(SPtr<TIPv6Addr> ddnsAddress)
{
    FqdnDdnsAddress = ddnsAddress;
}

/// Returns DNS server address suitable for DNS Update
///
/// @param iface interface index
///
/// @return DNS address (or NULL)
SPtr<TIPv6Addr> TSrvCfgMgr::getDDNSAddress(int iface)
{
    if (FqdnDdnsAddress)
        return FqdnDdnsAddress;

    SPtr<TSrvCfgIface> ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
        Log(Warning) << "No global DNS Update address specified and can't find dns-address on "
                     << "interface " << iface << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }

    SPtr<TIPv6Addr> DNSAddr;

    SPtr<TOptAddrLst> opt = (Ptr*) ptrIface->getExtraOption(OPTION_DNS_SERVERS);
    if (!opt) {
        Log(Error) << "DDNS: DNS Update aborted. DNS server address is not specified." << LogEnd;
        return SPtr<TIPv6Addr>(); // NULL
    }
    List(TIPv6Addr) DNSSrvLst = opt->getAddrLst();
    DNSSrvLst.first();
    if (DNSSrvLst.count())
        DNSAddr = DNSSrvLst.get();

    if (!DNSAddr) {
    }
    return DNSAddr;
}

/// @brief returns ifindex of an interface with specified interface-id
///
/// @param interfaceID pointer to TSrvOptInterfaceID
///
/// @return interface index (or -1 if not found)
int TSrvCfgMgr::getRelayByInterfaceID(SPtr<TSrvOptInterfaceID> interfaceID) {
    if (!interfaceID) {
        return -1;
    }

    firstIface();
    while (SPtr<TSrvCfgIface> cfgIface = getIface()) {
        SPtr<TSrvOptInterfaceID> cfgIfaceID = cfgIface->getRelayInterfaceID();
        if (cfgIfaceID && (*cfgIfaceID == *interfaceID)) {
            return cfgIface->getID();
        }
    }

    return -1;
}


/// @brief returns ifindex of an interface with matched address
///
/// @param addr address to be matched
///
/// @return interface index (or -1 if not found)
int TSrvCfgMgr::getRelayByLinkAddr(SPtr<TIPv6Addr> addr) {
    SPtr<TSrvCfgIface> cfgIface;

    firstIface();
    while (cfgIface = getIface()) {
        if (cfgIface->addrInSubnet(addr)) {
            Log(Debug) << "Address " << addr->getPlain() << " matched on interface "
                       << cfgIface->getFullName() << LogEnd;
            return cfgIface->getID();
        }
    }

    Log(Warning) << "Finding RELAYs using link address failed." << LogEnd;
    return -1;
}

/// @brief return any relay (used with guess-mode on)
///
/// @return interface index of the first relay (or -1 if there are no relays)
int TSrvCfgMgr::getAnyRelay() {
    SPtr<TSrvCfgIface> cfgIface;

    firstIface();
    while (cfgIface = getIface()) {
        if (cfgIface->isRelay()) {
            Log(Debug) << "Guess-mode: Picked " << cfgIface->getFullName() << " as relay." << LogEnd;
            return cfgIface->getID();
        }
    }

    return -1;
}

#ifndef MOD_DISABLE_AUTH
/// returns key-id that should be used for a given client-id
///
/// @param clientid client identifier
///
/// @return Key ID to be used (or 0)
uint32_t TSrvCfgMgr::getDelayedAuthKeyID(const char* mapping_file, SPtr<TDUID> clientid) {

    ifstream f(mapping_file, ios::in);

    if (!f.is_open()) {
        Log(Error) << "Can't open keys mapping file: " << mapping_file << LogEnd;
        // map not found or is inaccessible
        return 0;
    }

    string lookingfor = clientid->getPlain();

    for( std::string line; getline( f, line ); )
    {
        if (line.empty())
            continue;
        if (!line.empty() && (line[0] == '#') )
            continue;

        std::istringstream iss(line);
        string duid;
        uint32_t keyid;

        // parse the line. We don't really care if it is malformed.
        // If it is, server will not use the right key
        iss >> duid >> hex >> keyid;

        duid = duid.substr(0, duid.find(","));

        if (duid == lookingfor) {
            return keyid;
        }
    }

    // no key found
    return 0;
}
#endif

void TSrvCfgMgr::setPerformanceMode(bool mode) {
    PerformanceMode_ = mode;
}

bool TSrvCfgMgr::getPerformanceMode() {
    return PerformanceMode_;
}

void TSrvCfgMgr::dropUnicast(bool drop) {
    DropUnicast_ = drop;
}

bool TSrvCfgMgr::dropUnicast() {
    return DropUnicast_;
}
