#include <fstream>
#include <string>

#ifdef LINUX
#include <FlexLexer.h>
#include "SrvParser.h"
#endif
#ifdef WIN32
#include "SrvLexer.h"
#include "SrvParser.h"
#endif

#include "SrvCfgMgr.h"
#include "IfaceMgr.h"
#include "SrvCfgIface.h"
#include "SrvIfaceMgr.h"
#include "SmartPtr.h"
#include "Logger.h"
#include "AddrClient.h"
#include "TimeZone.h"
using namespace std;

TSrvCfgMgr::TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile, string oldCfgFile)
{
    int result;
    bool newConf=false;
    ifstream f,oldF,newF;
    this->IfaceMgr = ifaceMgr;
    
    //newConf=true if files differs
    newConf=compareConfigs(cfgFile,oldCfgFile);
    
    //FIXME:newConf should be returned to Srv/ClntAddrMgr - whether release
    //addresses 
    if(newConf) 
	this->copyFile(cfgFile,oldCfgFile);
    //if files differs - make copy of new config

#ifdef WIN32
    SrvLexer lexer;
    SrvParser parser;
    
    if (parser.yycreate(&lexer)) {
	lexer.yyin = fopen(cfgFile.c_str(),"r");
	if (lexer.yycreate(&parser))
	    result = parser.yyparse();
    }
#endif

#ifdef LINUX
    f.open( cfgFile.c_str() );
    if ( ! f.is_open() ) {
	std::clog << logger::logCrit << "Unable to open " << cfgFile << " file." << logger::endl;
	this->IsDone = true;
	return;
    }
    yyFlexLexer lexer(&f,&clog);
    SrvParser parser(&lexer);
    result = parser.yyparse();

    f.close();
#endif

    if (result) {
	clog << "config error" << logger::endl;
	//Finish whole DHCPClient 
	//FIXME:I don't like isDone, it should be either parameter passed 
	//		by reference or exception should be thrown
	IsDone = true; 
	return;
    }
    //Now parsed information should be place in config manager
    //in accordance with information provided by interface manager
    SmartPtr<TIfaceIface> iface;
    IfaceMgr->firstIface();
    //so for each iface in the system
    while(iface=IfaceMgr->getIface())
	//which is to be configured
	if ( (iface) &&
	     (iface->flagUp()) &&
	     (iface->flagRunning()) &&
	     (iface->flagMulticast()) )
	{
	    //try to find it in parsed config file
	    SmartPtr<TSrvCfgIface> cfgIface,foundIface;
	    parser.SrvCfgIfaceLst.first();
	    while(cfgIface=parser.SrvCfgIfaceLst.get())
		if((iface->getID()==cfgIface->getID())||
		   (string(iface->getName())==string(cfgIface->getName())))
		    //yes - it's provided by user
		{
		    if(foundIface) //Oops! He did it again.
			std::clog << logger::logWarning 
				  << "Redefinition of interface - last one is valid." << logger::endl;
		    //So it is memorized and removed from information descirbing config file
		    foundIface=cfgIface;
		    parser.SrvCfgIfaceLst.del();
		};
	    //Have we found any interface ?
	    if (foundIface) 
	    {
		//Here should be match iface name and iface id from IfaceMgr
		//there is declaration of this iface in config file
		if(iface->getID()==foundIface->getID()) {
		    foundIface->setIfaceName(iface->getName());
		}
		else {
		    foundIface->setIfaceID(iface->getID());
		}
		this->addIface(foundIface);
	    };
	};
    //So we configure approprieatly all interfaces in the system
    //Rest of interfaces described in config file can't be configured
    //because they don't exist, are not up/running etc.
    //so appropriate warning should be logged
    SmartPtr<TSrvCfgIface> cfgIface;
    parser.SrvCfgIfaceLst.first();
    while(cfgIface=parser.SrvCfgIfaceLst.get()) {
	std::clog << logger::logError << "Unable to configure " << cfgIface->getName()<<"/"
	    << cfgIface->getID() << " interface. Reason: "
	    << "is loopback, not present in system (IfaceMgr), down, " 
	    << "not running or is not multicast-capable." << logger::endl;
    }

    this->WorkDir = parser.ParserOptStack.getLast()->getWorkDir();
    this->LogName = parser.ParserOptStack.getLast()->getLogName();
    this->LogLevel= parser.ParserOptStack.getLast()->getLogLevel();

    if(!checkConfigConsistency())
    {
        this->IsDone=true;
        return;
    }

    SmartPtr<TIfaceIface> realIface;
    ////FIXME:get first iface - and pray it won't be loopback or other shit
    bool found=false;
    ifaceMgr->firstIface();

    while((!found)&&(realIface=ifaceMgr->getIface()))
        if (realIface->getMacLen()) found=true; ;
    if(found)    
        this->setDUID(this->WorkDir+"/"+(string)SRVDUID_FILE,
		      (char*)realIface->getMac(),
		      (int)realIface->getMacLen(),
		      (int)realIface->getHardwareType());
    else
    {
        IsDone=true;
        std::clog<<logger::logCrit
		 <<"Cannot generate DUID, because there is no appropriate interface" << logger::endl;
    }

    IsDone = false;
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
