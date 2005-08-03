/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgMgr.cpp,v 1.34 2005-08-03 23:17:11 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.33  2005/07/21 23:29:32  thomson
 * Logging cleanup.
 *
 * Revision 1.32  2005/07/17 21:09:52  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.31  2005/03/07 22:45:14  thomson
 * Fixed problem with non-existent interface name (bug #105)
 *
 * Revision 1.30  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.29  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.28  2004/12/27 20:48:22  thomson
 * Problem with absent link local addresses fixed (bugs #90, #91)
 *
 * Revision 1.27  2004/12/07 22:55:14  thomson
 * Problem with null pointer solved.
 *
 * Revision 1.26  2004/12/07 20:51:35  thomson
 * Link local safety checks added (bug #39)
 *
 * Revision 1.25  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.24  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.23  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.22  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.21  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.20  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.19  2004/07/11 14:08:39  thomson
 * Missing/invalid interface specifed in cfg, results in client shutdown
 *
 * Revision 1.18  2004/07/05 00:53:03  thomson
 * Various changes.
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

#include "IfaceMgr.h"
#include "ClntIfaceMgr.h"

#include "ClntParsGlobalOpt.h"
#include "TimeZone.h"

#include "FlexLexer.h"
#include "ClntParser.h"

TClntCfgMgr::TClntCfgMgr(SmartPtr<TClntIfaceMgr> ClntIfaceMgr, 
                         const string cfgFile,const string oldCfgFile)
    :TCfgMgr((Ptr*)ClntIfaceMgr)
{
    int result;
    ifstream f;
    this->IfaceMgr = ClntIfaceMgr;
    this->IsDone=false;

    /* support for config changes between runs - currently disabled */
    // bool newConf=false; //newConf=true if files differs
    // newConf=compareConfigs(cfgFile,oldCfgFile);
    // if(newConf) 
    //   this->copyFile(cfgFile,oldCfgFile);
    /* support for config changes between runs - currently disabled */

    // parse config file
    f.open(cfgFile.c_str());
    if ( ! f.is_open()  ) {
	Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd; 
	this->IsDone = true;
	return;
    } else {
	Log(Notice) << "Parsing " << cfgFile << " config file..." << LogEnd;
    }
    yyFlexLexer lexer(&f,&clog);
    clntParser parser(&lexer);
    result = parser.yyparse();
    Log(Debug) << "Parsing " << cfgFile << " done." << LogEnd;
    f.close();

    this->LogLevel = logger::getLogLevel();
    this->LogName  = logger::getLogName();

    if (result) {
        //Result!=0 means config errors. Finish whole DHCPClient 
        Log(Crit) << "Fatal error during config parsing." << LogEnd;
        this->IsDone = true; 
        this->DUID=new TDUID();
        return;
    }

    // match parsed interfaces with interfaces detected in system
    matchParsedSystemInterfaces(&parser);
  
    // check config consistency
    if(!validateConfig()) {
        this->IsDone=true;
        return;
    }

    // load or create DUID
    string duidFile = (string)CLNTDUID_FILE;
    if (!setDUID(duidFile)) {
	this->IsDone=true;
	return;
    }
    this->dump();
    
    IsDone = false;
}

void TClntCfgMgr::dump() {
    // store ClntCfgMgr in file
    std::ofstream xmlDump;
    xmlDump.open(CLNTCFGMGR_FILE);
    xmlDump << *this;
    xmlDump.close();
}

/*
  match parsed interfaces with interfaces detected in system. 
  CfgIface objects are created placed in CfgMgr. 
*/
bool TClntCfgMgr::matchParsedSystemInterfaces(clntParser *parser) {
    int cfgIfaceCnt;
    cfgIfaceCnt = parser->ClntCfgIfaceLst.count();
    Log(Debug) << cfgIfaceCnt << " interface(s) specified in " << CLNTCONF_FILE << LogEnd;

    SmartPtr<TClntCfgIface> cfgIface;
    SmartPtr<TIfaceIface> ifaceIface;

    if (cfgIfaceCnt) {
	// user specified some interfaces in config file
	parser->ClntCfgIfaceLst.first();
	while(cfgIface = parser->ClntCfgIfaceLst.get()) {
	    // for each interface (from config file)
	    if (cfgIface->getID()==-1) {
		ifaceIface = IfaceMgr->getIfaceByName(cfgIface->getName());
	    } else {
		ifaceIface = IfaceMgr->getIfaceByID(cfgIface->getID());
	    }

	    if (!ifaceIface) {
		Log(Error) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			      << " specified in " << CLNTCONF_FILE << " is not present or does not support IPv6."
			      << LogEnd;
		this->IsDone = true;
		continue;
	    }
	    if (cfgIface->noConfig()) {
		Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			       << " has flag no-config set, so it is ignored." << LogEnd;
		continue;
	    }

	    cfgIface->setIfaceName(ifaceIface->getName());
	    cfgIface->setIfaceID(ifaceIface->getID());

	    if (!ifaceIface->countLLAddress()) {
		Log(Crit) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID() 
			  << " is down or doesn't have any link-layer address." << LogEnd;
		this->IsDone = true;
		continue;
	    }

	    this->addIface(cfgIface);
	    Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			 << " configuation has been loaded." << LogEnd;
	}
    } else {
	// user didn't specified any interfaces in config file, so
	// we'll try to configure each interface we could find
	Log(Warning) << "Config file does not contain any interface definitions."
			<< LogEnd;
	
	IfaceMgr->firstIface();
	while ( ifaceIface = IfaceMgr->getIface() ) {
	    // for each interface present in the system...
	    if (!ifaceIface->flagUp()) {
		Log(Notice) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID() 
			       << " is down, ignoring." << LogEnd;
		continue;
	    }
	    if (!ifaceIface->flagRunning()) {
		Log(Notice) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID() 
			       << " has flag RUNNING not set, ignoring." << LogEnd;
		continue;
	    }
	    if (!ifaceIface->flagMulticast()) {
		Log(Notice) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID() 
			       << " is not multicast capable, ignoring." << LogEnd;
		continue;
	    }
	    if ( !(ifaceIface->getMacLen() > 5) ) {
		Log(Notice) << "Interface " << ifaceIface->getName() << "/" << ifaceIface->getID() 
			       << " has MAC address length " << ifaceIface->getMacLen() 
			       << " (6 or more required), ignoring." << LogEnd;
		continue;
	    }

	    // One address...
	    SmartPtr<TClntCfgAddr> addr(new TClntCfgAddr());
	    addr->setOptions(parser->ParserOptStack.getLast());

	    // ... is stored in one IA...
	    SmartPtr<TClntCfgIA> ia(new TClntCfgIA(parser->ParserOptStack.getLast()->getIncedIAIDCnt()));
	    ia->setOptions(parser->ParserOptStack.getLast());
	    ia->addAddr(addr);

	    
	    // ... which is stored in one group...
	    SmartPtr<TClntCfgGroup> group(new TClntCfgGroup());
	    group->setOptions(parser->ParserOptStack.getLast());
	    group->addIA(ia);
	    
	    // ... on this newly created interface...
	    cfgIface = SmartPtr<TClntCfgIface>(new TClntCfgIface(ifaceIface->getID()));
	    cfgIface->setIfaceName(ifaceIface->getName());
	    cfgIface->setIfaceID(ifaceIface->getID());
	    cfgIface->addGroup(group);
	    cfgIface->setOptions(parser->ParserOptStack.getLast());

	    // ... which is added to ClntCfgMgr
	    this->addIface(cfgIface);

	    Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			 << " has been added." << LogEnd;
	}
    }
    return true;
}

SmartPtr<TClntCfgIface> TClntCfgMgr::getIface()
{
    return ClntCfgIfaceLst.get();
}

void TClntCfgMgr::addIface(SmartPtr<TClntCfgIface> ptr)
{
    ClntCfgIfaceLst.append(ptr);
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
    //FIXME: Implement this in some distant future
    return false;
}

int TClntCfgMgr::countAddrForIA(long IAID)
{
    SmartPtr<TClntCfgIface> ptrIface;
    firstIface();
    while (ptrIface = getIface() ) 
    {
        SmartPtr<TClntCfgGroup> ptrGroup;
        ptrIface->firstGroup();
        while (ptrGroup = ptrIface->getGroup() ) 
        {
            SmartPtr<TClntCfgIA> ptrIA;
            ptrGroup->firstIA();
            while (ptrIA = ptrGroup->getIA())
                if (ptrIA->getIAID()==IAID)
                    return ptrIA->countAddr();
        }
    }    
    return 0;
}

SmartPtr<TClntCfgIA> TClntCfgMgr::getIA(long IAID)
{
    SmartPtr<TClntCfgIface> ptrIface;
    firstIface();
    while (ptrIface = getIface() ) 
    {
        SmartPtr<TClntCfgGroup> ptrGroup;
        ptrIface->firstGroup();
        while (ptrGroup = ptrIface->getGroup() ) 
        {
            SmartPtr<TClntCfgIA> ptrIA;
            ptrGroup->firstIA();
            while (ptrIA = ptrGroup->getIA())
                if (ptrIA->getIAID()==IAID)
                    return ptrIA;
        }
    }        
    return SmartPtr<TClntCfgIA>();
}

bool TClntCfgMgr::setIAState(int iface, int iaid, enum EState state)
{
    firstIface();
    SmartPtr<TClntCfgIface> ptrIface;
    while (ptrIface = getIface() ) {
        if ( ptrIface->getID() == iface ) break;
    }
    if (! ptrIface) {
	Log(Error) <<"ClntCfgMgr: Unable to set IA state (id=" << iaid 
            << "):Interface " << iface << " not found." << LogEnd;
        return false;
    }

    SmartPtr<TClntCfgGroup> ptrGroup;
    ptrIface->firstGroup();

    while (ptrGroup = ptrIface->getGroup() ) {
        SmartPtr<TClntCfgIA> ptrIA;
        ptrGroup->firstIA();
        while (ptrIA = ptrGroup->getIA()) 
        {
            if ( ptrIA->getIAID() == iaid ) {
                ptrIA->setState(state);
                return true;
            }
        }
    }
    Log(Error) << "ClntCfgMgr: Unable to set IA state (id=" << iaid << ")" << LogEnd;
    return false;
}	    

//check whether T1<T2 and Pref<Valid and at least T1<=Valid
bool TClntCfgMgr::validateConfig()
{
    //Is everything so far is ok
    if (IsDone) return false;
    SmartPtr<TClntCfgIface> ptrIface;
    this->ClntCfgIfaceLst.first();
    while(ptrIface=ClntCfgIfaceLst.get())
    {
	if (!this->validateIface(ptrIface)) {
	    return false;
	}
    }
    return true;
}

bool TClntCfgMgr::validateIface(SmartPtr<TClntCfgIface> ptrIface) {
    SmartPtr<TClntCfgGroup> ptrGroup;
    ptrIface->firstGroup();
    if(ptrIface->isReqTimezone()&&(ptrIface->getProposedTimezone()!=""))
    {   
	TTimeZone tmp(ptrIface->getProposedTimezone());
	if(!tmp.isValid())
	{
	    Log(Crit) << "Wrong time zone option for the " << ptrIface->getName() 
		      << "/" <<ptrIface->getID() << " interface." << LogEnd;
	    return false;
	}
    }
    
    while(ptrGroup=ptrIface->getGroup())
    {
	SmartPtr<TClntCfgIA> ptrIA;
	ptrGroup->firstIA();
	while(ptrIA=ptrGroup->getIA())
	{
	    if (!this->validateIA(ptrIface, ptrIA)) 
		return false;
	}
	
    }
    return true;
}

bool TClntCfgMgr::validateIA(SmartPtr<TClntCfgIface> ptrIface, SmartPtr<TClntCfgIA> ptrIA) {

    if ( ptrIA->getT2()<ptrIA->getT1() ) 
    {
	Log(Crit) << "T1 can't be lower than T2 for IA " << *ptrIA << "on the " << ptrIface->getName() 
		  << "/" << ptrIface->getID() << " interface." << LogEnd;
	return false;
    }
    SmartPtr<TClntCfgAddr> ptrAddr;
    ptrIA->firstAddr();
    while(ptrAddr=ptrIA->getAddr())
    {
	if (!this->validateAddr(ptrIface, ptrIA, ptrAddr))
	    return false;
    }
    return true;
}

bool TClntCfgMgr::validateAddr(SmartPtr<TClntCfgIface> ptrIface, 
			       SmartPtr<TClntCfgIA> ptrIA,
			       SmartPtr<TClntCfgAddr> ptrAddr) {
    SmartPtr<TIPv6Addr> addr = ptrAddr->get();
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


SmartPtr<TClntCfgGroup> TClntCfgMgr::getGroupForIA(long IAID)
{
    SmartPtr<TClntCfgIface> iface;
    firstIface();
    while(iface=getIface())
    {
        SmartPtr<TClntCfgGroup> group;
        iface->firstGroup();
        while(group=iface->getGroup())
        {
            SmartPtr<TClntCfgIA> ia;
            group->firstIA();
            while(ia=group->getIA())
                if (ia->getIAID()==IAID)
                    return group;
        }
    }
    return SmartPtr<TClntCfgGroup>();
}

SmartPtr<TClntCfgIface> TClntCfgMgr::getIface(int id)
{
    firstIface();
    SmartPtr<TClntCfgIface> iface;
    while(iface=getIface())
        if (iface->getID()==id) return iface;
    return SmartPtr<TClntCfgIface> ();
}

SmartPtr<TClntCfgIface> TClntCfgMgr::getIfaceByIAID(int iaid)
{
    SmartPtr<TClntCfgIface> iface;
    firstIface();
    while(iface=getIface())
    {
        SmartPtr<TClntCfgGroup> group;
        iface->firstGroup();
        while(group=iface->getGroup())
        {
            SmartPtr<TClntCfgIA> ia;
            group->firstIA();
            while(ia=group->getIA())
                if (ia->getIAID()==iaid)
                    return iface;
        }
    }
    return SmartPtr<TClntCfgIface>();
}

bool TClntCfgMgr::isDone() {
    return this->IsDone;
}

TClntCfgMgr::~TClntCfgMgr() {
    Log(Debug) << "ClntCfgMgr cleanup." << LogEnd;
}


ostream & operator<<(ostream &strum, TClntCfgMgr &x)
{
    strum << "<ClntCfgMgr>" << endl;
    strum << "  <workdir>" << x.getWorkDir()  << "</workdir>" << endl;
    strum << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    strum << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;
    if (x.DUID)
        strum << "  " << *x.DUID;
    else
        strum << "  <!-- duid not set -->";

    SmartPtr<TClntCfgIface> ptr;
    x.firstIface();

    while ( ptr = x.getIface() ) {
        strum << *ptr;
    }

    strum << "</ClntCfgMgr>" << endl;
    return strum;
}


