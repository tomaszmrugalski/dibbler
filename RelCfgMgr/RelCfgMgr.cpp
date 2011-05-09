/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelCfgMgr.cpp,v 1.13 2008-08-29 00:07:31 thomson Exp $
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include "IfaceMgr.h"
#include "RelIfaceMgr.h"
#include "RelCfgIface.h"
#include "FlexLexer.h"
#include "RelParser.h"
#include "RelCfgMgr.h"

using namespace std;

int TRelCfgMgr::NextRelayID = RELAY_MIN_IFINDEX;

TRelCfgMgr * TRelCfgMgr::Instance = 0;

TRelCfgMgr::TRelCfgMgr(const std::string cfgFile, const std::string xmlFile)
    :TCfgMgr(), XmlFile(xmlFile)
{
    // load config file
    if (!this->parseConfigFile(cfgFile)) {
        this->IsDone = true;
        return;
    }

    this->dump();
    this->IsDone = false;
}

bool TRelCfgMgr::parseConfigFile(string cfgFile) {
    int result;
    ifstream f;

    // parse config file
    f.open( cfgFile.c_str() );
    if ( ! f.is_open() ) {
	Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd;
	this->IsDone  = true;
	return false; 
    } else {
	Log(Notice) << "Parsing " << cfgFile << " config file..." << LogEnd;
    }
    yyFlexLexer lexer(&f,&clog);
    RelParser parser(&lexer);
    result = parser.yyparse();
    Log(Debug) << "Parsing config done." << LogEnd;
    f.close();

    this->LogLevel = logger::getLogLevel();
    this->LogName  = logger::getLogName();

    if (result) {
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
        return false;
    }

    // setup global options
    this->setupGlobalOpts(parser.ParserOptStack.getLast());
    
    // analyse interfaces mentioned in config file
    if (!this->matchParsedSystemInterfaces(&parser.RelCfgIfaceLst)) {
	return false;
    }
    
    // check if config is sane
    if(!this->validateConfig()) {
        return false;
    }

    if (guessMode())
	Log(Debug) << "Guess-mode enabled. RELAY-REPL from the server without interface-id option will be accepted." << LogEnd;
    else
	Log(Debug) << "Guess-mode disabled. RELAY-REPL from the server without interface-id option will be dropped." << LogEnd;
    
    return true;
}

void TRelCfgMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

bool TRelCfgMgr::setupGlobalOpts(SPtr<TRelParsGlobalOpt> opt) {
    this->Workdir          = opt->getWorkDir();
    this->GuessMode        = opt->getGuessMode();
    this->InterfaceIDOrder = opt->getInterfaceIDOrder();

    this->RemoteID         = opt->getRemoteID();
    this->Echo             = opt->getEcho();
    return true;
}

/*
 * Now parsed information should be placed in config manager
 * in accordance with information provided by interface manager
 */
bool TRelCfgMgr::matchParsedSystemInterfaces(List(TRelCfgIface) * lst) {
    int cfgIfaceCnt;
    cfgIfaceCnt = lst->count();
    Log(Debug) << cfgIfaceCnt << " interface(s) specified in " << RELCONF_FILE << LogEnd;

    SPtr<TRelCfgIface> cfgIface;
    SPtr<TIfaceIface>  ifaceIface;
    
    lst->first();
    while(cfgIface=lst->get()) {

	// physical interface
	if (cfgIface->getID()==-1) {
	    // ID==-1 means that user referenced to interface by name
	    ifaceIface = RelIfaceMgr().getIfaceByName(cfgIface->getName());
	} else {
	    ifaceIface = RelIfaceMgr().getIfaceByID(cfgIface->getID());
	}
	if (!ifaceIface) {
	    Log(Crit) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
		      << " is not present in the system or does not support IPv6."
		      << LogEnd;
	    return false;
	}

        if (!ifaceIface->countLLAddress()) {
	    Log(Crit) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID()
		      << " is down or doesn't have any link-layer address. " << LogEnd;
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

SPtr<TRelCfgIface> TRelCfgMgr::getIface() {
	return this->IfaceLst.get();
}

void TRelCfgMgr::addIface(SPtr<TRelCfgIface> ptr) {
    IfaceLst.append(ptr);
}

void TRelCfgMgr::firstIface() {
    IfaceLst.first();
}

long TRelCfgMgr::countIface() {
    return IfaceLst.count();
}

TRelCfgMgr::~TRelCfgMgr() {
    Log(Debug) << "RelCfgMgr cleanup." << LogEnd;
}

bool TRelCfgMgr::isDone() {
    return IsDone;
}

bool TRelCfgMgr::guessMode() {
    return GuessMode;
}

bool TRelCfgMgr::validateConfig() {
    SPtr<TRelCfgIface> ptrIface;

    if (!this->countIface()) {
	Log(Crit) << "Config problem: No interface defined." << LogEnd;
	return false;
    }

    if ( this->countIface()<2 ) {
	Log(Warning) << "There is only one interface defined. At least two is a sanite configuration." << LogEnd;
    }

    firstIface();
    while(ptrIface=getIface()) {
	if (!this->validateIface(ptrIface))
	    return false;
    }
    return true;
}

bool TRelCfgMgr::validateIface(SPtr<TRelCfgIface> ptrIface)
{
    if ( (ptrIface->getInterfaceID() == (signed int)DHCPV6_INFINITY) &&
	(ptrIface->getClientUnicast() || ptrIface->getClientMulticast()) ) {
	Log(Crit) << "Client address defined without Interface-ID on the " << ptrIface->getFullName() <<  " interface." << LogEnd;
	return false;
    }
    if ( !ptrIface->getServerUnicast() && !ptrIface->getServerMulticast() &&
	 !ptrIface->getClientUnicast() && !ptrIface->getClientMulticast() ) {
	Log(Warning) << "Both server and client support is defined on the "  << ptrIface->getName() 
		     << "/" << ptrIface->getID() << " interface." << LogEnd;
    }

    if ( (ptrIface->getServerUnicast() || ptrIface->getServerMulticast()) &&
	 (ptrIface->getClientUnicast() || ptrIface->getClientMulticast()) ) {
	Log(Warning) << "Both server and client support is defined on the "  << ptrIface->getName() 
		     << "/" << ptrIface->getID() << " interface." << LogEnd;
    }
    return true;
}

SPtr<TRelCfgIface> TRelCfgMgr::getIfaceByID(int iface) 
{
    SPtr<TRelCfgIface> ptrIface;
    this->firstIface();
    while ( ptrIface = this->getIface() ) {
	if ( ptrIface->getID()==iface )
	    return ptrIface;
    }
    Log(Error) << "There is no interface with ifindex=" << iface << " in the CfgMgr." << LogEnd;
    return 0;
}


SPtr<TRelCfgIface> TRelCfgMgr::getIfaceByInterfaceID(int iface) 
{
    SPtr<TRelCfgIface> ptrIface;
    this->firstIface();
    while ( ptrIface = this->getIface() ) {
	if ( ptrIface->getInterfaceID()==iface )
	    return ptrIface;
    }
    Log(Error) << "There is no interface with interfaceID=" << iface 
	       << " in the CfgMgr." << LogEnd;
    return 0;
}

ERelIfaceIdOrder TRelCfgMgr::getInterfaceIDOrder()
{
    return InterfaceIDOrder;
}

SPtr<TOptVendorData> TRelCfgMgr::getRemoteID()
{
    return RemoteID;
}

SPtr<TRelOptEcho> TRelCfgMgr::getEcho()
{
    return Echo;
}

void TRelCfgMgr::instanceCreate( const std::string cfgFile, const std::string xmlFile )
{
    if (Instance)
        Log(Crit) << "RelCfgMgr instance already created. Application error!" << LogEnd;
    Instance = new TRelCfgMgr(cfgFile, xmlFile);
}

TRelCfgMgr& TRelCfgMgr::instance()
{
    if (!Instance)
        Log(Crit) << "RelCfgMgr instance not created yet. Application error. Crashing in 3... 2... 1..." << LogEnd;
    return *Instance;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream &out, TRelCfgMgr &x) {
    out << "<RelCfgMgr>" << std::endl;
    out << "  <workdir>" << x.getWorkDir()  << "</workdir>" << endl;
    out << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    out << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;
    out << "  <InterfaceIDOrder>";
    
    switch (x.InterfaceIDOrder) {
    case REL_IFACE_ID_ORDER_BEFORE:
	out << "before";
	break;
    case REL_IFACE_ID_ORDER_AFTER:
	out << "after";
	break;
    case REL_IFACE_ID_ORDER_NONE:
	out << "none";
	break;
    default:
	out << "unknown(" << x.InterfaceIDOrder << ")";
	break;
    }
    out << "</InterfaceIDOrder>" << endl;

    if (SPtr<TOptVendorData> r = x.getRemoteID()) {
	out << "  <RemoteID enterprise=\"" << r->getVendor() << "\" length=\"" << r->getVendorDataLen() 
	    << "\">" << r->getVendorDataPlain() << "</RemoteID>" << endl;
    } else {
	out << "  <!-- <RemoteID/> -->" << endl;
    }

    if (x.getEcho()) {
	SPtr<TRelOptEcho> e = x.getEcho();
	out << "  <EchoRequest count=\"" << e->count() << "\">";
	for (int i=0;i<e->count();i++) {
	    out << e->getReqOpt(i) << " ";
	}
	out << "</EchoRequest>" << endl;
    } else {
	out << "  <!-- <EchoRequest/> -->" << endl;
    }
    
    SPtr<TRelCfgIface> ptrIface;
    x.firstIface();
    while (ptrIface = x.getIface()) {
	out << *ptrIface;
    }
    out << "</RelCfgMgr>" << std::endl;
    return out;
}
