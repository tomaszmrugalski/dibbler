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
{
    int result;
    bool newConf=false;
    ifstream f,oldF,newF;
    this->IfaceMgr = ClntIfaceMgr;
    this->IsDone=false;
    //newConf=true if files differs
    newConf=compareConfigs(cfgFile,oldCfgFile);

    //FIXME:newConf should be returned to Srv/ClntAddrMgr - whether release
    //addresses 
    if(newConf) 
        this->copyFile(cfgFile,oldCfgFile);
    //if files differs - make copy of new config
	
	// now this code is common to Linux and win32. Cool.
    f.open(cfgFile.c_str());
    if ( ! f.is_open()  ) {
        std::clog << logger::logCrit << "Unable to open " << cfgFile << " file." << logger::endl; 
	this->IsDone = true;
        return;
    }
    yyFlexLexer lexer(&f,&clog);
    clntParser parser(&lexer);
    std::clog << logger::logDebug << "Parsing " << cfgFile << "...";
    result = parser.yyparse();
    std::clog << "done." << logger::endl;
    f.close();
	// now this code is common to Linux and win32. Cool.


    if (result) {
        std::clog << logger::logCrit << "Config error." << logger::endl;
        //Finish whole DHCPClient 
        IsDone = true; 
        this->DUID=new TDUID();
        return;
    }

    //Now parsed information should be place in config manager
    //in accordance with information provided by interface manager
    SmartPtr<TIfaceIface> iface;
    IfaceMgr->firstIface();
    //so for each iface in the system
    while(iface=IfaceMgr->getIface())
        //which is supposed to be configured
        if ( (iface) &&
            (iface->flagUp()) &&
            (iface->flagRunning()) &&
            (iface->flagMulticast()) &&
			(iface->getMacLen()>5) )
        {
            //try to find it in parsed config file
            SmartPtr<TClntCfgIface> cfgIface,foundIface;
            parser.ClntCfgIfaceLst.first();
            while(cfgIface=parser.ClntCfgIfaceLst.get())
                if((iface->getID()==cfgIface->getID())||
                    (string(iface->getName())==string(cfgIface->getName())))
                    //yes - it's provided by user
                {
                    if(foundIface) //Oops! He did it again.
                        clog<<"Defiinition of interface repeated - last one is valid." << logger::endl;
                    //So it is memorized and removed from information descirbing config file
                    foundIface=cfgIface;
                    parser.ClntCfgIfaceLst.del();
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
            }
            else
            {
                //no declaration, so one IA with one address should be created
                //with global options
                foundIface=SmartPtr<TClntCfgIface>(new TClntCfgIface(iface->getID()));
                SmartPtr<TClntCfgGroup> group(new TClntCfgGroup());
                group->setOptions(parser.ParserOptStack.getLast());
                SmartPtr<TClntCfgIA> ia(new TClntCfgIA(parser.ParserOptStack.getLast()->getIncedIAIDCnt()));
                ia->setOptions(parser.ParserOptStack.getLast());
                SmartPtr<TClntCfgAddr> addr(new TClntCfgAddr());
                addr->setOptions(parser.ParserOptStack.getLast());
                //append created iface, group and address
                foundIface->addGroup(group);
                group->addIA(ia);
                ia->addAddr(addr);
                this->addIface(foundIface);
            };
        };
    //So we configure approprieatly all interfaces in the system
    //Rest of interfaces described in config file can't be configured
    //because they don't exist, are not up/running etc.
    //so appropriate warning should be logged
    SmartPtr<TClntCfgIface> cfgIface;
    parser.ClntCfgIfaceLst.first();
    while(cfgIface=parser.ClntCfgIfaceLst.get()) {
        std::clog << logger::logError << "Unable to configure " << cfgIface->getName()<<"/"
            <<cfgIface->getID() << " interface. Reason: "
            << "is loopback, not present in system (IfaceMgr), down, " 
            << "not running or is not multicast-capable." << logger::endl;
    }
    this->WorkDir = parser.ParserOptStack.getLast()->getWorkDir();
    this->LogName = parser.ParserOptStack.getLast()->getLogname();
    this->LogLevel= parser.ParserOptStack.getLast()->getLogLevel();

    logger::setLogname(this->LogName);
    logger::setLogLevel(this->LogLevel);
    
    if(!checkConfigConsistency())
    {
        this->IsDone=true;
        return;
    }
    //FIXME:I think that all IA's should be set in some state, because new attribute
    //		state appeared in class TClntCfgIA, but in what state ? (Marek)

    SmartPtr<TIfaceIface> realIface;
    ////FIXME:get first iface - and pray it won't be loopback or other shit
    bool found=false;
    IfaceMgr->firstIface();
    while( (!found) && (realIface=IfaceMgr->getIface()) )
    {
        realIface->firstLLAddress();
	char buf[64];
	memset(buf,0,64);
        if ( realIface->getLLAddress() && 
	     (realIface->getMacLen() > 5) &&
	     memcmp(realIface->getMac(),buf,realIface->getMacLen()) &&
	     realIface->flagUp() &&
	     !realIface->flagLoopback() )
            found=true;
    }

	if(found) {
        this->setDUID(this->WorkDir+"/"+(string)CLNTDUID_FILE,
        (char*)realIface->getMac(),
        (int)realIface->getMacLen(),
        (int)realIface->getHardwareType());
	} else {
        IsDone=true;
        std::clog<<logger::logCrit
            <<"Cannot generate DUID, because there is no appropriate interface." << logger::endl;
        this->DUID=new TDUID();
        //this->DUIDlen=0;
        return;
    };
	
	std::ofstream xmlDump;
	xmlDump.open(CLNTCFGMGR_FILE);
	xmlDump << *this;
	xmlDump.close();

    IsDone = false;
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

string TClntCfgMgr::getWorkDir()
{
    return WorkDir;
}

bool TClntCfgMgr::getReconfigure()
{
    //FIXME
    return false;
}

string TClntCfgMgr::getLogName()
{
    return LogName;
}

int TClntCfgMgr::getLogLevel()
{
    return LogLevel;
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
        std::clog << logger::logError <<"ClntCfgMgr: Unable to set IA state (id=" << iaid 
            << "):Interface " << iface << " not found." << logger::endl;
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
    std::clog << logger::logError << "ClntCfgMgr: Unable to set IA state (id=" 
        << iaid << ")" << logger::endl;
    return false;
}	    

//check whether T1<T2 and Pref<Valid and at least T1<=Valid
bool TClntCfgMgr::checkConfigConsistency()
{
    //Is everything so far is ok
    if (IsDone) return false;
    SmartPtr<TClntCfgIface> ptrIface;
    this->ClntCfgIfaceLst.first();
    while(ptrIface=ClntCfgIfaceLst.get())
    {
        SmartPtr<TClntCfgGroup> ptrGroup;
        ptrIface->firstGroup();
        if(ptrIface->isReqTimeZone()&&(ptrIface->getProposedTimeZone()!=""))
        {   
            TTimeZone tmp(ptrIface->getProposedTimeZone());
            if(!tmp.isValid())
            {
                this->IsDone=true;
                clog<<logger::logCrit
                    <<"Wrong time zone option for:"
                    <<"iface(id/name)"<<ptrIface->getID()<<"/"
                    <<ptrIface->getName() << logger::endl;
                return !(IsDone=true);
            }
        }

        while(ptrGroup=ptrIface->getGroup())
        {
            SmartPtr<TClntCfgIA> ptrIA;
            ptrGroup->firstIA();
            while(ptrIA=ptrGroup->getIA())
            {
                if ((unsigned long)ptrIA->getT2()<(unsigned long)ptrIA->getT1()) 
                {
                    clog<<logger::logCrit
                        <<"T1 can't be lower than T2 for IA:"<<*ptrIA << logger::endl
                        <<"in iface(id/name)"<<ptrIface->getID()<<"/"
                        <<ptrIface->getName() << logger::endl;
                    return !(IsDone=true);
                }
                SmartPtr<TClntCfgAddr> ptrAddr;
                ptrIA->firstAddr();
                while(ptrAddr=ptrIA->getAddr())
                {
                    if((unsigned long)ptrAddr->getPref()>(unsigned long)ptrAddr->getValid())
                    {
                        clog<<logger::logCrit
                        <<"Prefered time:" <<ptrAddr->getPref()
                        <<"can't be lower than Valid lifetime:"<<ptrAddr->getValid()
                        <<"for IA:"<<*ptrIA << logger::endl
                        <<"in iface(id/name)"<<ptrIface->getID()<<"/"
                        <<ptrIface->getName() << logger::endl;
                        return !(IsDone=true);
                    }
                    if ((unsigned long)ptrIA->getT1()>(unsigned long)ptrAddr->getValid())
                    {
                        clog<<logger::logCrit
                        <<"Valid lifetime:"<<ptrAddr->getValid()
                        <<"can't be lower than T1 "<<ptrIA->getT1()
                        <<"(address can't be renewed) in IA:"<<*ptrIA << logger::endl
                        <<"in iface(id/name)"<<ptrIface->getID()<<"/"
                        <<ptrIface->getName() << logger::endl;
                        return !(IsDone=true);
                    }
                }
            }
            
        }
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

bool TClntCfgMgr::isDone()
{
    return this->IsDone;
}

ostream & operator<<(ostream &strum, TClntCfgMgr &x)
{
    strum << "<ClntCfgMgr>" << endl;
    strum << "  " << *x.DUID;
    strum << "  <WorkDir>" << x.getWorkDir()  << "</WorkDir>" << endl;
    strum << "  <LogName>" << x.getLogName()  << "</LogName>" << endl;
    strum << "  <LogLevel>" << x.getLogLevel() << "</LogLevel>" << endl;

    SmartPtr<TClntCfgIface> ptr;
    x.firstIface();

    while ( ptr = x.getIface() ) {
        strum << *ptr;
    }

    strum << "<ClntCfgMgr>" << endl;
    return strum;
}


