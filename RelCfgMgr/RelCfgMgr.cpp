/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelCfgMgr.cpp,v 1.3 2005-01-24 00:42:57 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include "RelCommon.h"

using namespace std;

#include "IfaceMgr.h"
#include "RelIfaceMgr.h"
#include "RelCfgIface.h"

#include "FlexLexer.h"
#include "RelParser.h"
#include "RelCfgMgr.h"

int TRelCfgMgr::NextRelayID = RELAY_MIN_IFINDEX;

TRelCfgMgr::TRelCfgMgr(TCtx ctx, string cfgFile, string xmlFile)
    :TCfgMgr(ctx.IfaceMgr)
{
    this->Ctx = ctx;

    // load config file
    if (!this->parseConfigFile(cfgFile)) {
	this->IsDone = true;
	return;
    }

    this->XmlFile = xmlFile;
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
	return false;
    }
    yyFlexLexer lexer(&f,&clog);
    RelParser parser(&lexer);
    Log(Debug) << "Parsing " << cfgFile << " config file..." << LogEnd;
    result = parser.yyparse();
    Log(Debug) << "Parsing config done." << LogEnd;
    f.close();

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
    
    return true;
}

void TRelCfgMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

bool TRelCfgMgr::setupGlobalOpts(SmartPtr<TRelParsGlobalOpt> opt) {
    this->Workdir   = opt->getWorkDir();
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

    SmartPtr<TRelCfgIface> cfgIface;
    SmartPtr<TIfaceIface>  ifaceIface;
    
    lst->first();
    while(cfgIface=lst->get()) {

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

SmartPtr<TRelCfgIface> TRelCfgMgr::getIface() {
	return this->IfaceLst.get();
}

string TRelCfgMgr::getWorkdir() {
    return Workdir;
}

void TRelCfgMgr::addIface(SmartPtr<TRelCfgIface> ptr) {
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

bool TRelCfgMgr::validateConfig() {
    SmartPtr<TRelCfgIface> ptrIface;

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

bool TRelCfgMgr::validateIface(SmartPtr<TRelCfgIface> ptrIface)
{
    if ( (ptrIface->getInterfaceID() == (signed int)DHCPV6_INFINITY) &&
	(ptrIface->getClientUnicast() || ptrIface->getClientMulticast()) ) {
	Log(Crit) << "Client addresse defined without Interface-ID on the " << ptrIface->getName() << "/" << ptrIface->getID()
		  << " interface." << LogEnd;
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

SmartPtr<TRelCfgIface> TRelCfgMgr::getIfaceByID(int iface) {
    SmartPtr<TRelCfgIface> ptrIface;
    this->firstIface();
    while ( ptrIface = this->getIface() ) {
	if ( ptrIface->getID()==iface )
	    return ptrIface;
    }
    Log(Error) << "There is no interface with ifindex=" << iface << " in the CfgMgr." << LogEnd;
    return 0; // NULL
}


SmartPtr<TRelCfgIface> TRelCfgMgr::getIfaceByInterfaceID(int iface) {
    SmartPtr<TRelCfgIface> ptrIface;
    this->firstIface();
    while ( ptrIface = this->getIface() ) {
	if ( ptrIface->getInterfaceID()==iface )
	    return ptrIface;
    }
    Log(Error) << "There is not interface with interfaceID=" << iface 
	       << " in the CfgMgr." << LogEnd;
    return 0; // NULL
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream &out, TRelCfgMgr &x) {
    out << "<RelCfgMgr>" << std::endl;
    out << "  <workdir>" << x.getWorkdir() << "</workdir>" << std::endl;
    
    SmartPtr<TRelCfgIface> ptrIface;
    x.firstIface();
    while (ptrIface = x.getIface()) {
	out << *ptrIface;
    }
    out << "</RelCfgMgr>" << std::endl;
    return out;
}
