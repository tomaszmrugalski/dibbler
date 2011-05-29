/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: CfgMgr.cpp,v 1.20 2009-07-22 20:19:08 thomson Exp $
 */

#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "CfgMgr.h"
#include "Logger.h"
#include "Portable.h"
#include "Logger.h"

TCfgMgr::TCfgMgr()
 :IsDone(false), 
  DUIDType(DUID_TYPE_LLT), /* default DUID type: LLT */ 
  DdnsProto(DNSUPDATE_TCP),
  _DDNSTimeout(DNSUPDATE_DEFAULT_TIMEOUT) // default is 1000 ms
{
	
}

TCfgMgr::~TCfgMgr() {
}

void TCfgMgr::setDDNSProtocol(DNSUpdateProtocol proto) {
    DdnsProto = proto;
}

// method compares both files and if differs
// returns true if files differs and false in the other case
bool TCfgMgr::compareConfigs(const string cfgFile, const string oldCfgFile)
{
	std::ifstream oldF,newF;
    bool newConf=false;
    int  len;
    // It's common for client and server so why not to create CfgMgr
    //open new and saved config files
    newF.open(cfgFile.c_str(),   ios::in|ios::binary);
    oldF.open(oldCfgFile.c_str(),ios::in|ios::binary);
    //is it possible to open both ?
    if(!newF.fail())
    {
        if (!oldF.fail())
        {
            //yes - comparision of old file and new file
            //first the length of both files
            newF.seekg(0,ios::end);
            oldF.seekg(0,ios::end);			
            if((len=newF.tellg())!=oldF.tellg())
                newConf=true; //there is a diffrence at least in length
            else
            {
                //next contents
                newF.seekg(0,ios::beg);
                oldF.seekg(0,ios::beg);
                while((!newF.eof())&&(!oldF.eof())&&(!newConf)) 
                    newConf=(newF.get()!=oldF.get());
            }
            newF.close();
            oldF.close();
        }
        else
        {
            //there is no old config file, so there is new config
            //and it should be make copy of new one
            newF.close();
            newConf=true;
        }
    }
    else
    {
	Log(Error) << "Can't open file " << cfgFile 
            << ". Config comparision aborted." << LogEnd;
        
    }
    return newConf;
}

// replaces copy cfgFile to oldCfgFile
void TCfgMgr::copyFile(const string cfgFile, const string oldCfgFile)
{
    ifstream newF;
    ofstream oOldF;
    //try to open input file
    newF.open(cfgFile.c_str(), ios::in|ios::binary);
    if (newF.fail()) 
	Log(Error) << "Can't open file "<<cfgFile 
		  <<", when trying to save new config file." << LogEnd;
    else
    {
        newF.seekg(0,ios::end);
        int newLen=newF.tellg();
        newF.seekg(0,ios::beg);
        //try to open output file
        oOldF.open(oldCfgFile.c_str(),ios::trunc|ios::out|ios::binary);
        if (oOldF.fail()) 
        {
	    Log(Error) << "Can't open file "<<oldCfgFile
		       <<", when trying to save new config file"<< LogEnd;
            newF.close();
        }
        else
            //if both opened try to copy
            while(newLen-->0)
                oOldF.put(newF.get());
    }
    newF.close();
    oOldF.close();
}



bool TCfgMgr::loadDUID(const string duidFile)
{
    ifstream f;
    f.open(duidFile.c_str());
    if ( !(f.is_open())  ) {
        // unable to open DUID file
        Log(Notice) << "Unable to open DUID file (" << duidFile << "), generating new DUID." << LogEnd;
	return false;
    }

    string s;
    getline(f,s);
    f.close();

    this->DUID = new TDUID(s.c_str());

    return true;
}

bool TCfgMgr::setDUID(const string filename, TIfaceMgr & ifaceMgr) {
    
    // --- load DUID ---
    if (this->loadDUID(filename)) {
        Log(Info) << "My DUID is " << this->DUID->getPlain() << "." << LogEnd;
        return true;
    }

    SPtr<TIfaceIface> realIface;

    bool found=false;
    
	ifaceMgr.firstIface();
    if (this->DUIDType == DUID_TYPE_EN) {
        realIface = ifaceMgr.getIface(); // use the first interface. It will be ignored anyway
        found = true; 

        if (!realIface) {
            Log(Error) << "Unable to find any interfaces. Can't generate DUID" << LogEnd;
            return false;
        }
    }
    while( (!found) && (realIface=ifaceMgr.getIface()) )
    {
        realIface->firstLLAddress();
        char buf[64];
        memset(buf,0,64);

        if (!realIface->getMac()) {
          Log(Debug) << "DUID creation: Interface " << realIface->getFullName() 
                     << " skipped: no link addresses." << LogEnd;
          continue;
        }
        if ( realIface->getMacLen()<6 ) {
          Log(Debug) << "DUID creation: Interface " << realIface->getFullName() 
                     << " skipped: MAC length is " << realIface->getMacLen() 
                     << ", but at least 6 is required." << LogEnd;
          continue;
        }
        if ( realIface->flagLoopback() ) {
            Log(Debug) << "DUID creation: Interface " << realIface->getFullName() 
                     << " skipped: Interface is loopback." << LogEnd;
            continue;
        }
        if ( !memcmp(realIface->getMac(),buf,realIface->getMacLen()) ) {
          Log(Debug) << "DUID creation: Interface " << realIface->getName() << "/" << realIface->getID() 
                     << " skipped: MAC is all zero. " << LogEnd;
          continue;
        }
        if ( !realIface->flagUp() ) {
          Log(Debug) << "DUID creation: Interface " << realIface->getName() << "/" << realIface->getID() 
                     << " skipped: Interface is down." << LogEnd;
          continue;
        }

        found=true;
    }

    if(found) {
	if ( this->generateDUID(filename, realIface->getMac(),
				realIface->getMacLen(), realIface->getHardwareType())) {
        if (this->DUIDType!=DUID_TYPE_EN)
	      Log(Notice) << "DUID creation: generated using " << realIface->getFullName() << " interface." << LogEnd;
        Log(Info) << "My DUID is " << this->DUID->getPlain() << "." << LogEnd;
        return true;
	} else {
	    Log(Crit) << "DUID creation: generation attempt based on " << realIface->getFullName() << " interface failed." << LogEnd;
	    return false;
	}
    } 
    
    Log(Crit) << "Cannot generate DUID, because there is no up and running interface with "
		 << "MAC address at least 6 bytes long." << LogEnd;
    this->DUID=new TDUID();
    return false;
}

bool TCfgMgr::generateDUID(const string duidFile,char * mac,int macLen, int macType)
{
    ofstream f;
    f.open( duidFile.c_str() );
    if (!f.is_open()) {
      Log(Crit) << "Unable to create/write " << duidFile << " file." << LogEnd;
      return false;
    }
    string duidType;
    int DUIDlen = 0;
    char *DUID  = 0;
    long cur_time = 0;
    
    switch (this->DUIDType) {
      case DUID_TYPE_LLT:
        duidType = "link-local+time (duid-llt)";
        DUIDlen=macLen+8;
        DUID = new char[DUIDlen];
        *((u_short*)DUID)=htons(this->DUIDType);
        *((u_short*)(DUID+2))=htons((short)macType);
        cur_time=now();
        
        *(((u_long*)(DUID+4)))=htonl((cur_time-946684800) & 0xFFFFFFFF);
        /* 946684800=Number of seconds between midnight (UTC), January
           2000 and midnight (UTC), January 1970. It is 30 years.
           7 leap years of 366 days. 23 years of 365 days.
           Modified by Eric Gamess (UCV)
        */
        
        for (int i=0;i<macLen; i++)
          DUID[i+8]=mac[i];
        break;
      case DUID_TYPE_LL:
        duidType= "link-local (duid-ll)";
        DUIDlen = macLen+4;
        DUID = new char[DUIDlen];
        *((u_short*)DUID)=htons(this->DUIDType);
        *((u_short*)(DUID+2))=htons((short)macType);
        for (int i=0;i<macLen; i++)
          DUID[i+4]=mac[i];
        break;
      case DUID_TYPE_EN:
        Log(Debug) << "DUID creation: EN: EnterpriseNumber=" << DUIDEnterpriseNumber << ", Enterprise ID=" <<
          DUIDEnterpriseID->getPlain() << LogEnd;
        duidType="Enterprise Number (duid-en)";
        DUIDlen = 6 + DUIDEnterpriseID->getLen();
        DUID = new char[DUIDlen];
        *((u_short*)DUID)=htons(this->DUIDType);
        *(((u_long*)(DUID+2)))=htonl(this->DUIDEnterpriseNumber);
        this->DUIDEnterpriseID->storeSelf(DUID+6);
        break;
      default:
        Log(Error) << "Invalid DUID type." << LogEnd;
        return false;
    }
    
    Log(Notice) << "DUID creation: Generating " << DUIDlen << "-bytes long " << duidType << " DUID." << LogEnd;
    this->DUID=new TDUID(DUID,DUIDlen);
    delete [] DUID;
    f << this->DUID->getPlain();
    f.close();
    return true;
}

void TCfgMgr::setWorkdir(std::string workdir) {
    this->Workdir = workdir;
}

string TCfgMgr::getWorkDir() {
    return Workdir;
}

string TCfgMgr::getLogName()
{
    return LogName;
}

int TCfgMgr::getLogLevel()
{
    return LogLevel;
}

SPtr<TDUID> TCfgMgr::getDUID()
{
    return DUID;
}
