/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.cpp,v 1.34 2005-02-01 00:57:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.33  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.32  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.31  2005/01/03 21:57:08  thomson
 * Relay support added.
 *
 * Revision 1.30  2004/12/27 20:48:22  thomson
 * Problem with absent link local addresses fixed (bugs #90, #91)
 *
 * Revision 1.29  2004/12/07 22:55:50  thomson
 * Typos corrected.
 *
 * Revision 1.28  2004/12/07 00:43:03  thomson
 * Server no longer support link local addresses (bug #38),
 * Server now supports stateless mode (bug #71)
 *
 * Revision 1.27  2004/12/02 00:51:05  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.26  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.25  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.24  2004/07/11 14:04:54  thomson
 * Downed/invalid interface specified in cfg now results in server shutdown (bug #23)
 *
 * Revision 1.23  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.22  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.21  2004/07/01 18:12:46  thomson
 * Minor clean up.
 *
 * Revision 1.20  2004/06/29 22:03:36  thomson
 * *MaxLease option partialy implemented/fixed.
 *
 * Revision 1.19  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 * Revision 1.18  2004/06/20 21:00:45  thomson
 * Various fixes.
 *
 * Revision 1.17  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 * Revision 1.16  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.15  2004/05/24 21:16:37  thomson
 * Various fixes.
 *
 * Revision 1.14  2004/05/23 23:46:02  thomson
 * *** empty log message ***
 *                                                                           
 */

#include <iostream>
#include <fstream>
#include <string>
#include "SmartPtr.h"
#include "Portable.h"
#include "SrvCfgMgr.h"
#include "SrvCfgIface.h"
#include "Logger.h"

using namespace std;

#include "IfaceMgr.h"
#include "SrvIfaceMgr.h"

#include "AddrClient.h"
#include "TimeZone.h"

#include "FlexLexer.h"
#include "SrvParser.h"

int TSrvCfgMgr::NextRelayID = RELAY_MIN_IFINDEX;

TSrvCfgMgr::TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile, string xmlFile)
    :TCfgMgr((Ptr*)ifaceMgr)
{
    this->IfaceMgr = ifaceMgr;

    // load config file
    if (!this->parseConfigFile(cfgFile)) {
	this->IsDone = true;
	return;
    }

    // load or create DUID
    string duidFile = (string)SRVDUID_FILE;
    if (!this->setDUID(duidFile)) {
		this->IsDone=true;
		return;
    }

    Log(Info) << "My duid is " << this->DUID->getPlain() << "." << LogEnd;

    this->XmlFile = xmlFile;
    this->dump();

    IsDone = false;
}

bool TSrvCfgMgr::parseConfigFile(string cfgFile) {
    int result;
    ifstream f;

    // parse config file
    f.open( cfgFile.c_str() );
    if ( ! f.is_open() ) {
	Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd;
	return false;
    }
    yyFlexLexer lexer(&f,&clog);
    SrvParser parser(&lexer);
    Log(Debug) << "Parsing config file..." << LogEnd;
    result = parser.yyparse();
    Log(Debug) << "Parsing config done." << LogEnd;
    f.close();

    this->LogLevel = logger::getLogLevel();
    this->LogName  = logger::getLogName();

    if (result) {
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
	this->IsDone = true;
        return false;
    }

    // setup global options
    this->setupGlobalOpts(parser.ParserOptStack.getLast());
    
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

    return true;
}

void TSrvCfgMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

bool TSrvCfgMgr::setupGlobalOpts(SmartPtr<TSrvParsGlobalOpt> opt) {
    this->Workdir   = opt->getWorkDir();
    this->Stateless = opt->getStateless();
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

    SmartPtr<TSrvCfgIface> cfgIface;
    SmartPtr<TIfaceIface>  ifaceIface;
    
    parser->SrvCfgIfaceLst.first();
    while(cfgIface=parser->SrvCfgIfaceLst.get()) {
	// for each interface from config file
	
	// relay interface
	if (cfgIface->isRelay()) {
	    cfgIface->setID(this->NextRelayID++);
	    if (!this->setupRelay(cfgIface)) {
		return false;
	    }
	    this->addIface(cfgIface);

	    continue; // skip physical interface checking part
	}
	
	// physical interface
	if (cfgIface->getID()==-1) {
	    // ID==-1 means that user referenced to interface by name
	    ifaceIface = IfaceMgr->getIfaceByName(cfgIface->getName());
	} else {
	    ifaceIface = IfaceMgr->getIfaceByID(cfgIface->getID());
	}
	if (!ifaceIface) {
	    Log(Crit) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
		      << " is not present in the system or does not support IPv6."
		      << LogEnd;
	    return false;
	}

        if (!ifaceIface->countLLAddress()) {
	    Log(Crit) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID()
		      << " is down or doesn't have any link-layer address." << LogEnd;
	    return false;
        }

	cfgIface->setName(ifaceIface->getName());
	cfgIface->setID(ifaceIface->getID());
	this->addIface(cfgIface);
	Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
		  << " configuration has been loaded." << LogEnd;
    }
    return true;
}

SmartPtr<TSrvCfgIface> TSrvCfgMgr::getIface() {
	return this->SrvCfgIfaceLst.get();
}

void TSrvCfgMgr::addIface(SmartPtr<TSrvCfgIface> ptr) {
    SrvCfgIfaceLst.append(ptr);
}

void TSrvCfgMgr::firstIface() {
    SrvCfgIfaceLst.first();
}

long TSrvCfgMgr::countIface() {
    return SrvCfgIfaceLst.count();
}

TSrvCfgMgr::~TSrvCfgMgr() {
    Log(Debug) << "SrvCfgMgr cleanup." << LogEnd;
}

/*
 * returns how many addresses can be assigned to this client?
 * factors used: 
 * - iface-max-lease
 * - clntSupported()
 * - class-max-lease in each class
 * - assignedCount in each class
 * return value <= clnt-max-lease <= iface-max-lease
 */
long TSrvCfgMgr::countAvailAddrs(SmartPtr<TDUID> clntDuid, 
				 SmartPtr<TIPv6Addr> clntAddr,  int iface)
{
    // FIXME: long long long int (128bit) could come in handy
    double avail         = 0; // how many are available?
    double ifaceAssigned = 0; // how many are assigned on this iface?
    SmartPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	Log(Error) << "Interface " << iface << " does not exist in SrvCfgMgr." << LogEnd;
	return 0;
    }

    unsigned long ifaceMaxLease = ptrIface->getIfaceMaxLease();

    SmartPtr<TSrvCfgAddrClass> ptrClass;
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

/*
 * get a class, which address belongs to
 */ 
SmartPtr<TSrvCfgAddrClass> TSrvCfgMgr::getClassByAddr(int iface, SmartPtr<TIPv6Addr> addr)
{
    this->firstIface();
    SmartPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);

    if (!ptrIface) {
	Log(Error) << "Trying to find class on unknown (" << iface <<") interface." << LogEnd;
	return 0; // NULL
    }

    SmartPtr<TSrvCfgAddrClass> ptrClass;
    ptrIface->firstAddrClass();
    while (ptrClass = ptrIface->getAddrClass()) {
	if (ptrClass->addrInPool(addr))
	    return ptrClass;
    }

    return 0; // NULL
}

//on basis of duid/address/iface assign addresss to client
SmartPtr<TIPv6Addr> TSrvCfgMgr::getRandomAddr(SmartPtr<TDUID> clntDuid, 
					      SmartPtr<TIPv6Addr> clntAddr,
					      int iface) {
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    SmartPtr<TIPv6Addr>    any;

    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	return 0;
    }

    any = new TIPv6Addr();
    ptrIface->firstAddrClass();
    ptrClass = ptrIface->getAddrClass();
    return ptrClass->getRandomAddr();

    //FIXME: get addrs from first address only
}


/* 
 * Method checks whether client is supported and assigned addresses from any class
 */
bool TSrvCfgMgr::isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface)
{
    SmartPtr<TSrvCfgIface> ptrIface;
    firstIface();
    while((ptrIface=getIface())&&(ptrIface->getID()!=iface)) ;

    if (ptrIface)
    {
        SmartPtr<TSrvCfgAddrClass> ptrClass;
        ptrIface->firstAddrClass();
        while(ptrClass=ptrIface->getAddrClass())
            if (ptrClass->clntSupported(duid,clntAddr))
                return true;
    }
    return false;
}

bool TSrvCfgMgr::isDone() {
    return IsDone;
}

bool TSrvCfgMgr::validateConfig() {
    SmartPtr<TSrvCfgIface> ptrIface;

    if (!this->countIface()) {
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

bool TSrvCfgMgr::validateIface(SmartPtr<TSrvCfgIface> ptrIface)
{
    if (ptrIface->countAddrClass() && this->stateless()) {
	Log(Crit) << "Config problem: Interface " << ptrIface->getName() << "/" << ptrIface->getID() 
		  << ": Class definitions present, but stateless mode set." << LogEnd;
	return false;
    }
    if (!ptrIface->countAddrClass() && !this->stateless()) {
	Log(Crit) << "Config problem: Interface " << ptrIface->getName() << "/" << ptrIface->getID() 
		  << ": No class definitions present, but stateless mode not set." << LogEnd;
	return false;
    }

    SmartPtr<TSrvCfgAddrClass> ptrClass;
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

bool TSrvCfgMgr::validateClass(SmartPtr<TSrvCfgIface> ptrIface, SmartPtr<TSrvCfgAddrClass> ptrClass)
{
    if (ptrClass->isLinkLocal()) {
	Log(Crit) << "One of the classes defined on the " << ptrIface->getName() << "/" << ptrIface->getID()
		  << " overlaps with link local addresses." << LogEnd;
	return false;
    }
    
    if ( ptrClass->getPref(0) > ptrClass->getValid(0x7fffffff) )
    {
	Log(Crit) << "Prefered time upper bound " <<ptrClass->getPref(0x7fffffff)
		  << " can't be lower than valid time lower bound " << ptrClass->getValid(0)
		  << "on the "<<ptrIface->getName()<<"/"
		  << ptrIface->getID() << " interface." << LogEnd;
	return false;
    }
    if ( ptrClass->getT1(0)>ptrClass->getT2(0x7fffffff) )
    {
	Log(Crit) << "T2 timeout upper bound:" <<ptrClass->getPref(0x7fffffff)
		  << " can't be lower than T2 lower bound:" << ptrClass->getT2(0)
		  << "on the "<<ptrIface->getName()<<"/"
		  << ptrIface->getID() << " interface." << LogEnd;
	return false;
    }
    return true;
}

SmartPtr<TSrvCfgIface> TSrvCfgMgr::getIfaceByID(int iface) {
    SmartPtr<TSrvCfgIface> ptrIface;
    this->firstIface();
    while ( ptrIface = this->getIface() ) {
	if ( ptrIface->getID()==iface )
	    return ptrIface;
    }
    Log(Error) << "Invalid interface (id=" << iface 
	       << ") specifed, cannot get address." << LogEnd;
    return 0; // NULL
}


void TSrvCfgMgr::delClntAddr(int iface, SmartPtr<TIPv6Addr> addr) {
    SmartPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	Log(Warning) << "Unable to decrease address usage: unknown interface (id=" 
		     << iface << ")" << LogEnd;
	return;
    }
    ptrIface->delClntAddr(addr);
}

void TSrvCfgMgr::addClntAddr(int iface, SmartPtr<TIPv6Addr> addr) {
    SmartPtr<TSrvCfgIface> ptrIface;
    ptrIface = this->getIfaceByID(iface);
    if (!ptrIface) {
	Log(Warning) << "Unable to increase address usage: unknown interface (id=" 
		     << iface << ")" << LogEnd;
	return;
    }
    ptrIface->addClntAddr(addr);
}

bool TSrvCfgMgr::stateless() {
    return this->Stateless;
}

bool TSrvCfgMgr::setupRelay(SmartPtr<TSrvCfgIface> cfgIface) {
    SmartPtr<TIfaceIface> iface;
    string name = cfgIface->getRelayName();

    iface = IfaceMgr->getIfaceByName(name);
    if (!iface) {
	Log(Crit) << "Underlaying interface for " << cfgIface->getName() << "/" << cfgIface->getID()
		  << " with name " << name << " is missing." << LogEnd;
	return false;
    }
    cfgIface->setRelayID(iface->getID());

    if (!IfaceMgr->setupRelay(cfgIface->getName(), cfgIface->getID(), iface->getID(), cfgIface->getRelayInterfaceID())) {
	Log(Crit) << "Relay setup for " << cfgIface->getName() << "/" << cfgIface->getID() << " interface failed." << LogEnd;
	return false;
    }

    return true;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream &out, TSrvCfgMgr &x) {
    out << "<SrvCfgMgr>" << std::endl;
    out << "  <workDir>" << x.getWorkDir()  << "</workDir>" << endl;
    out << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    out << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;
    out << "  " << *x.DUID;

    if (x.stateless())
	out << "  <stateless/>" << std::endl;
    else
	out << "  <!-- <stateless/> -->" << std::endl;
    
    SmartPtr<TSrvCfgIface> ptrIface;
    x.firstIface();
    while (ptrIface = x.getIface()) {
	out << *ptrIface;
    }
    out << "</SrvCfgMgr>" << std::endl;
    return out;
}
