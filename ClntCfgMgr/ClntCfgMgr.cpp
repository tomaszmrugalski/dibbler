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
 * $Id: ClntCfgMgr.cpp,v 1.66 2008-09-22 17:08:52 thomson Exp $
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
#include "FlexLexer.h"


#include "ClntIfaceMgr.h"

#include "ClntParsGlobalOpt.h"
#include "TimeZone.h"

#include "FlexLexer.h"
#include "ClntParser.h"

using namespace std;

TClntCfgMgr * TClntCfgMgr::Instance = 0;

#ifdef MOD_CLNT_EMBEDDED_CFG
static bool HardcodedCfgExample(TClntCfgMgr *cfgMgr, string params);
#endif

TClntCfgMgr & TClntCfgMgr::instance()
{
  if (!Instance)
    Log(Crit) << "Application error. Tried to access CfgMgr without instanceCreate!" << LogEnd;
  // throw an exception here or something
  return *Instance;
}

void TClntCfgMgr::instanceCreate(const std::string cfgFile) {
    Instance = new TClntCfgMgr(cfgFile);
}


TClntCfgMgr::TClntCfgMgr(const string cfgFile)
  :TCfgMgr()
{
    NotifyScripts = false;
    RemoteAutoconf = false;
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

bool TClntCfgMgr::parseConfigFile(string cfgFile)
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
    Log(Debug) << "Parsing " << cfgFile << " done, result=" << result << LogEnd;
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
 * @return 
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
		Log(Error) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			   << " specified in " << CLNTCONF_FILE << " is not present or does not support IPv6."
			   << LogEnd;
		return false;
	    }
	    if (cfgIface->noConfig()) {
		Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
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
	    ifaceIface->setPrefixLength(cfgIface->getPrefixLength());

	    if (!ifaceIface->countLLAddress()) {
		if (this->inactiveMode()) {
		    Log(Notice) << "Interface " << ifaceIface->getFullName() 
				<< " is not operational yet (does not have "
				<< "link-local address), skipping it for now." << LogEnd;
		    addIface(cfgIface);
		    makeInactiveIface(cfgIface->getID(), true); // move it to InactiveLst
		    return true;
		}

		Log(Crit) << "Interface " << ifaceIface->getFullName()
			  << " is down or doesn't have any link-local address." << LogEnd;
		return false;
	    }

	    // Check if the interface is during bring-up phase 
	    // (i.e. DAD procedure for link-local addr is not complete yet)
	    char tmp[64];
	    ifaceIface->firstLLAddress();
	    inet_ntop6(ifaceIface->getLLAddress(), tmp);
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
		    makeInactiveIface(cfgIface->getID(), true); // move it to InactiveLst
		    return true;
		}

		Log(Crit) << "Interface " << ifaceIface->getFullName()
			  << " has tentative link-local address (and inactive-mode is disabled)." << LogEnd;
		return false;

	    }

	    this->addIface(cfgIface);
	    Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
				  << " configuation has been loaded." << LogEnd;
	}
    } else {
	// user didn't specified any interfaces in config file, so
	// we'll try to configure each interface we could find
	Log(Warning) << "Config file does not contain any interface definitions. Trying to autodetect."
		     << LogEnd;

	List(TIPv6Addr) dnsList;
	dnsList.clear();
	parser->ParserOptStack.getLast()->setDNSServerLst(&dnsList);
	
	int cnt = 0;
	ClntIfaceMgr().firstIface();
	while ( ifaceIface = ClntIfaceMgr().getIface() ) {
	    // for each interface present in the system...
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
	    if ( !(ifaceIface->getMacLen() > 5) ) {
		Log(Notice) << "Interface " << ifaceIface->getFullName() 
                            << " has MAC address length " << ifaceIface->getMacLen() 
                            << " (6 or more required), ignoring." << LogEnd;
		continue;
	    }
	    ifaceIface->firstLLAddress();
	    if (!ifaceIface->getLLAddress()) {
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
	    this->addIface(cfgIface);

	    Log(Info) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
                      << " has been added." << LogEnd;
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

void TClntCfgMgr::makeInactiveIface(int ifindex, bool inactive)
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
    return 0;
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
    return 0;
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
    return true;
}

bool TClntCfgMgr::validateIface(SPtr<TClntCfgIface> ptrIface) {

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
    
    SPtr<TClntCfgIA> ptrIA;
    ptrIface->firstIA();
    while(ptrIA=ptrIface->getIA())
    {
	if (!this->validateIA(ptrIface, ptrIA)) 
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
    Digest         = opt->getDigest();
    LogLevel       = logger::getLogLevel();
    LogName        = logger::getLogName();
    ScriptsDir     = opt->getScriptsDir();
    NotifyScripts  = opt->getNotifyScripts();
    AnonInfRequest = opt->getAnonInfRequest();
    InsistMode     = opt->getInsistMode();   // should the client insist on receiving all options
                                                   // i.e. sending INF-REQUEST if REQUEST did not grant required opts
    InactiveMode   = opt->getInactiveMode(); // should the client accept not ready interfaces?
    FQDNFlagS      = opt->getFQDNFlagS();
    MappingPrefix  = opt->getMappingPrefix(); // experimental feature
    UseConfirm     = opt->getConfirm(); // should client try to send CONFIRM?
    
    // user has specified DUID type, just in case if new DUID will be generated
    if (parser->DUIDType != DUID_TYPE_NOT_DEFINED) {
      DUIDType = parser->DUIDType;
      //Log(Debug) << "DUID type set to " << parser->DUIDType << "." << LogEnd;
      DUIDEnterpriseNumber = parser->DUIDEnterpriseNumber;
      DUIDEnterpriseID     = parser->DUIDEnterpriseID;
    }

#ifndef MOD_DISABLE_AUTH
    AuthEnabled = opt->getAuthEnabled();
    if (AuthEnabled)
        AuthAcceptMethods = opt->getAuthAcceptMethods();
    else
        AuthAcceptMethods.clear();
    AAASPI = getAAASPIfromFile();
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
        Digest = DIGEST_NONE;
    else
        Digest = type;
}

DigestTypes TClntCfgMgr::getDigest()
{
    return Digest;
}

bool TClntCfgMgr::isDone() {
    return IsDone;
}

string TClntCfgMgr::getScriptsDir()
{
    return ScriptsDir;
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
	    return 0;

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
    
    	    makeInactiveIface(x->getID(), false); // move it to InactiveLst
    	    return x;
    	}
    }

    return 0;
}

#ifndef MOD_DISABLE_AUTH
uint32_t TClntCfgMgr::getAAASPI() {
    return AAASPI;
}

List(DigestTypes) TClntCfgMgr::getAuthAcceptMethods()
{
    return AuthAcceptMethods;
}

bool TClntCfgMgr::getAuthEnabled()
{
    return AuthEnabled;
}
#endif

bool TClntCfgMgr::getFQDNFlagS()
{
    return FQDNFlagS;
}

bool TClntCfgMgr::getMappingPrefix()
{
    return MappingPrefix;
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
    strum << "  <scriptsDir>" << x.getScriptsDir() << "</scriptsDir>" << endl;
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

bool TClntCfgMgr::getNotifyScripts()
{
    return NotifyScripts;
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
