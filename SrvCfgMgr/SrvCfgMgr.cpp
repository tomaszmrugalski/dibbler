/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.cpp,v 1.26 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

TSrvCfgMgr::TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile, string oldCfgFile)
    :TCfgMgr((Ptr*)ifaceMgr)
{
    this->IfaceMgr = ifaceMgr;

    // load config file
    if (!this->parseConfigFile(cfgFile)) {
	this->IsDone = true;
	return;
    }

    // load or create DUID
    string duidFile = this->WorkDir+"/"+(string)SRVDUID_FILE;
    if (!this->setDUID(duidFile)) {
		this->IsDone=true;
		return;
    }

    Log(Info) << "My duid is " << this->DUID->getPlain() << "." << LogEnd;

    this->dump();

    IsDone = false;
}

bool TSrvCfgMgr::parseConfigFile(string cfgFile) {
    int result;
    ifstream f;
    
    /* support for config changes between runs - currently disabled */
    //newConf=true if files differs
    //newConf=compareConfigs(cfgFile,oldCfgFile);
    //if(newConf) 
    //   this->copyFile(cfgFile,oldCfgFile);
    /* support for config changes between runs - currently disabled */

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

    if (result) {
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
	this->IsDone = true;
        return false;
    }

    // setup workdir
    this->WorkDir = parser.ParserOptStack.getLast()->getWorkDir();
    
    // analyse interfaces mentioned in config file
    if (!this->matchParsedSystemInterfaces(&parser)) {
	return false;
    }
    
    // check for invalid values, e.g. T1>T2
    if(!checkConfigConsistency()) {
        return false;
    }
    return true;
}

void TSrvCfgMgr::dump() {
    std::ofstream xmlDump;
    xmlDump.open(SRVCFGMGR_FILE);
    xmlDump << *this;
    xmlDump.close();
}

/*
 * Now parsed information should be placed in config manager
 * in accordance with information provided by interface manager
 */
bool TSrvCfgMgr::matchParsedSystemInterfaces(SrvParser *parser) {
    int cfgIfaceCnt;
    cfgIfaceCnt = parser->SrvCfgIfaceLst.count();
    Log(Debug) << cfgIfaceCnt << " interface(s) specified in " << SRVCONF_FILE << logger::endl;

    SmartPtr<TSrvCfgIface> cfgIface;
    SmartPtr<TIfaceIface>  ifaceIface;
    
    parser->SrvCfgIfaceLst.first();
    while(cfgIface=parser->SrvCfgIfaceLst.get()) {
		// for each interface from config file
		if (cfgIface->getID()==-1) {
			// ID==-1 means that user referenced to interface by name
			ifaceIface = IfaceMgr->getIfaceByName(cfgIface->getName());
		} else {
		    ifaceIface = IfaceMgr->getIfaceByID(cfgIface->getID());
		}
		if (!ifaceIface) {
		    Log(Crit) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			      << " specified in " << SRVCONF_FILE 
			      << " is not present in the system or does not support IPv6."
			      << LogEnd;
			this->IsDone = true;
			return false;
		}
		cfgIface->setName(ifaceIface->getName());
		cfgIface->setID(ifaceIface->getID());
		this->addIface(cfgIface);
		Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			  << " has been configured." << LogEnd;
    }

    if (!cfgIfaceCnt) {
		Log(Crit) << "No interfaces defined. Server startup aborted." << LogEnd;
		return false;
    }

    return true;
}

SmartPtr<TSrvCfgIface> TSrvCfgMgr::getIface() {
	return this->SrvCfgIfaceLst.get();
}

string TSrvCfgMgr::getWorkDir() {
    return WorkDir;
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
	Log(Crit) << "Interface " << iface << " does not exist in SrvCfgMgr" 
		  << LogEnd;
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

bool TSrvCfgMgr::checkConfigConsistency() {
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    firstIface();
    while(ptrIface=getIface())
    {
        TTimeZone tzone(ptrIface->getTimezone());
        if ((ptrIface->getTimezone()!="")&&(!tzone.isValid()))
        {
            clog<<logger::logCrit
                << "Not appropiate time zone option for iface(id/name) "
                <<ptrIface->getID()<<"/"<<ptrIface->getName()
                <<" is provided." << logger::endl;
            return !(this->IsDone=true);
        }
        
        ptrIface->firstAddrClass();
        while(ptrClass=ptrIface->getAddrClass())
        {
            if (ptrClass->getPref(0)>ptrClass->getValid(0x7fffffff))
            {
                clog<<logger::logCrit
                    <<"Prefered time upper bound:" <<ptrClass->getPref(0x7fffffff)
                    <<"can't be lower than valid time lower bound:"
                    <<ptrClass->getValid(0)
                    <<"for iface(id/name)"<<ptrIface->getID()<<"/"
                    <<ptrIface->getName() << logger::endl;
                return !(this->IsDone=true);
            }
            if (ptrClass->getT1(0)>ptrClass->getT2(0x7fffffff))
            {
                clog<<logger::logCrit
                    <<"T2 timeout upper bound:" <<ptrClass->getPref(0x7fffffff)
                    <<"can't be lower than T2 lower bound:"
                    <<ptrClass->getT2(0)
                    <<"for iface(id/name)"<<ptrIface->getID()<<"/"
                    <<ptrIface->getName() << logger::endl;
                return !(IsDone=true);
            }
        }
    }
    return true;

    //FIXME: fail, if configured class is multicast or link-local
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


// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream &out, TSrvCfgMgr &x) {
	out << "<SrvCfgMgr>" << std::endl;
	SmartPtr<TSrvCfgIface> ptrIface;
	x.firstIface();
	while (ptrIface = x.getIface()) {
		out << *ptrIface;
	}
	out << "</SrvCfgMgr>" << std::endl;
	return out;
}
