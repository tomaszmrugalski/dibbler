/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.cpp,v 1.14 2004-05-23 23:46:02 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2004/05/23 21:35:31  thomson
 * *** empty log message ***
 *
 * Revision 1.12  2004/05/23 21:24:50  thomson
 * *** empty log message ***
 *
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
    int result;
    ifstream f;
    this->IfaceMgr = ifaceMgr;
    
    /* support for config changes between runs - currently disabled */
    //newConf=true if files differs
    //newConf=compareConfigs(cfgFile,oldCfgFile);
    //if(newConf) 
    //   this->copyFile(cfgFile,oldCfgFile);
    /* support for config changes between runs - currently disabled */

    // parse config file
    f.open( cfgFile.c_str() );
    if ( ! f.is_open() ) {
	std::clog << logger::logCrit << "Unable to open " << cfgFile << " file." << logger::endl;
	this->IsDone = true;
	return;
    }
    yyFlexLexer lexer(&f,&clog);
    SrvParser parser(&lexer);
    std::clog << logger::logDebug << "Parsing config file..." << logger::endl;
    result = parser.yyparse();
    std::clog << logger::logDebug << "Parsing config done." << logger::endl;
    f.close();

    if (result) {
        //Result!=0 means config errors. Finish whole DHCPClient 
        Log(logCrit) << "Config error." << logger::endl;
        IsDone = true; 
        return;
    }

    this->matchParsedSystemInterfaces(&parser);

    this->WorkDir = parser.ParserOptStack.getLast()->getWorkDir();

    if(!checkConfigConsistency())
    {
        this->IsDone=true;
        return;
    }

    // load or create DUID
    if ( !loadDUID(this->WorkDir+"/"+(string)SRVDUID_FILE) ) {
	this->IsDone=true;
	return;
    }

    std::ofstream xmlDump;
    xmlDump.open(SRVCFGMGR_FILE);
    xmlDump << *this;
    xmlDump.close();

    IsDone = false;
}

/*
 * Now parsed information should be place in config manager
 * in accordance with information provided by interface manager
 */
bool TSrvCfgMgr::matchParsedSystemInterfaces(SrvParser *parser) {
    int cfgIfaceCnt;
    cfgIfaceCnt = parser->SrvCfgIfaceLst.count();
    Log(logDebug) << cfgIfaceCnt << " interface(s) specified in " << SRVCONF_FILE << logger::endl;

    SmartPtr<TSrvCfgIface> cfgIface;
    SmartPtr<TIfaceIface>  ifaceIface;

    if (!cfgIfaceCnt) {
	Log(logCrit) << "No interfaces defined. Server startup aborted." << logger::endl;
	return false;
    }
    
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
	    Log(logCrit) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
			 << " specified in " << CLNTCFGMGR_FILE << " is not present in the system."
			 << logger::endl;
	    return false;
	}
	cfgIface->setIfaceName(ifaceIface->getName());
	cfgIface->setIfaceID(ifaceIface->getID());
	this->addIface(cfgIface);
	Log(logInfo) << "Interface " << cfgIface->getName() << "/" << cfgIface->getID() 
		     << " has been added." << logger::endl;
    }
    return true;
}

SmartPtr<TSrvCfgIface> TSrvCfgMgr::getIface()
{
	return this->SrvCfgIfaceLst.get();
}

SmartPtr<TSrvCfgIface> TSrvCfgMgr::getIface(int ifaceid)
{
    firstIface();
    SmartPtr<TSrvCfgIface> ptrIface;
    while(ptrIface=SrvCfgIfaceLst.get())
        if (ptrIface->getID()==ifaceid)
            return ptrIface;
	return SmartPtr<TSrvCfgIface>();
}


string TSrvCfgMgr::getWorkDir()
{
	return WorkDir;
}

void TSrvCfgMgr::addIface(SmartPtr<TSrvCfgIface> ptr)
{
	SrvCfgIfaceLst.append(ptr);
}

void TSrvCfgMgr::firstIface()
{
	SrvCfgIfaceLst.first();
}

int TSrvCfgMgr::countIfaces()
{
	return SrvCfgIfaceLst.count();
}

TSrvCfgMgr::~TSrvCfgMgr()
{
}


long TSrvCfgMgr::getMaxAddrsPerClient( SmartPtr<TDUID> duid, 
						 SmartPtr<TIPv6Addr> clntAddr,  int iface)
{
	//FIXME: 
    //For all classes by which client is supported get MaxAddrPerClient
    //and return sum of this value (it must be smaller then MAX_LONG)
	return 10;
}

long TSrvCfgMgr::getFreeAddrs( SmartPtr<TDUID> duid, 
					 SmartPtr<TIPv6Addr> clntAddr,  int iface)
{
    //FIXME: I don't know how SrvCfgMgr can count this and what for
	return 8;
}
//Can client(DUID,clntAddr) have address addr ?
bool TSrvCfgMgr::addrIsSupported( SmartPtr<TDUID> duid,
				   SmartPtr<TIPv6Addr> clntAddr ,  SmartPtr<TIPv6Addr> addr)
{
	return true;
}


//it get's IP address and iface and should return class which describes it
SmartPtr<TSrvCfgAddrClass> TSrvCfgMgr::getClassByAddr(int iface, SmartPtr<TIPv6Addr> addr)
{
    // FIXME: implement this for real
    SrvCfgIfaceLst.first();
    SmartPtr<TSrvCfgIface> ptrIface;

    while ( ptrIface = SrvCfgIfaceLst.get() ) {
	// we got interface, now check classes
	SmartPtr<TSrvCfgAddrClass> ptrClass;
    ptrIface->firstAddrClass();
    return ptrIface->getAddrClass();
    }
    return SmartPtr<TSrvCfgAddrClass>(); // NULL
}

//on basis of duid/address/iface assign addresss to client
SmartPtr<TIPv6Addr> TSrvCfgMgr::getRandomAddr( SmartPtr<TDUID> duid, 
				  SmartPtr<TIPv6Addr> clntAddr, long T1,long T2,
				  long Pref,long Valid)
{
    //FIXME:
    static char addr[] = {0x3f,0xfe,0x83,0x20,0x00,0x3,0x2,0x10, 0,0,0,0,0 ,0x1,0,0x2};
    for (int i=0; i<16; i++)
	addr[i] = (char) (rand()%255);
    return new TIPv6Addr(addr);
}

SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > TSrvCfgMgr::getClassesForClient(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        int iface, bool rapid)
{
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > 
            ret(new TContainer<SmartPtr<TSrvCfgAddrClass> >);
    SmartPtr<TSrvCfgIface> ptrIface;
    firstIface();
    while((ptrIface=getIface())&&(ptrIface->getID()!=iface)) ;
    if (ptrIface)
    {
        SmartPtr<TSrvCfgAddrClass> ptrClass;
        ptrIface->firstAddrClass();
        while(ptrClass=ptrIface->getAddrClass())
            if (ptrClass->clntSupported(duid,clntAddr)&&
                ((ptrClass->getRapidCommit()==rapid)||!rapid))
                    ret->append(ptrClass);
    }
    return ret;
}

//Method checks whether client is supported and assigned addresses from any class
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

bool TSrvCfgMgr::isDone()
{
    return IsDone;
}

bool TSrvCfgMgr::checkConfigConsistency()
{
    SmartPtr<TSrvCfgIface> ptrIface;
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    firstIface();
    while(ptrIface=getIface())
    {
        TTimeZone tzone(ptrIface->getTimeZone());
        if ((ptrIface->getTimeZone()!="")&&(!tzone.isValid()))
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
}

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
