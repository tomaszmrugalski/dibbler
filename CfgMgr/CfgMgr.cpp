/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: CfgMgr.cpp,v 1.12 2005-02-01 00:57:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.10  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.9  2004/07/01 18:12:12  thomson
 * DUID creation failure results in client/server shutdown (bugs #44, #45)
 *                                                                           
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include <iostream>
#include <fstream>
#include <string>
//#include <iomanip>
#include "CfgMgr.h"
#include "Logger.h"
#include "Portable.h"
#include "Logger.h"
#include "Iface.h"

TCfgMgr::TCfgMgr(SmartPtr<TIfaceMgr> IfaceMgr) {
    this->IfaceMgr = IfaceMgr;
}

TCfgMgr::~TCfgMgr() {
    this->IfaceMgr = 0;
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

    f.seekg(0,ios::end);
    long DUIDlen = f.tellg();
    f.seekg(0,ios::beg);
    char *DUID = new char[DUIDlen>>1];
    char digit;
    for (int i=0;i<DUIDlen; i++)	
    {
	f.read(&digit,1);
	if (isalpha(digit))
	    digit=toupper(digit)-'A'+10;
	else
	    digit-='0';
	DUID[i>>1]<<=4;
	DUID[i>>1]|=digit;
    }
    DUIDlen>>=1;
    this->DUID=new TDUID(DUID,DUIDlen);
    
    delete [] DUID;
    f.close();
    return true;
}

bool TCfgMgr::setDUID(const string filename) {
    
    // --- load DUID ---
    if (this->loadDUID(filename))
	return true;

    SmartPtr<TIfaceIface> realIface;

    bool found=false;
    IfaceMgr->firstIface();
    while( (!found) && (realIface=IfaceMgr->getIface()) )
    {
        realIface->firstLLAddress();
        char buf[64];
        memset(buf,0,64);

        if (!realIface->getMac()) {
          Log(Debug) << "DUID creation: Interface " << realIface->getName() << "/" << realIface->getID() 
                     << " skipped: no link addresses." << LogEnd;
          continue;
        }
        if ( realIface->getMacLen()<6 ) {
          Log(Debug) << "DUID creation: Interface " << realIface->getName() << "/" << realIface->getID() 
                     << " skipped: MAC length is " << realIface->getMacLen() << ", but at least 6 is required." << LogEnd;
          continue;
        }
        if ( realIface->flagLoopback() ) {
          Log(Debug) << "DUID creation: Interface " << realIface->getName() << "/" << realIface->getID() 
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
      return this->generateDUID(filename, realIface->getMac(),
				realIface->getMacLen(), realIface->getHardwareType());
    } 
    
    Log(Crit) << "Cannot generate DUID, because there is no up and running interface with "
		 << "MAC address at least 6 bytes long." << LogEnd;
    this->DUID=new TDUID();
    return false;
};

bool TCfgMgr::generateDUID(const string duidFile,char * mac,int macLen, int macType)
{
    ofstream f;
    f.open( duidFile.c_str() );
    if (!f.is_open()) {
      Log(Crit) << "Unable to write " << duidFile << " file." << LogEnd;
      return false;
    }
    
    int DUIDlen=macLen+8;
    char *DUID = new char[DUIDlen];
    
    f << "0001"<<setfill('0')<<setw(4)<<hex<<macType;
    *((u_short*)DUID)=htons(0x0001);
    *((u_short*)(DUID+2))=htons((short)macType);

    long cur_time=now();
    *(((u_long*)(DUID+4)))=htonl(cur_time);
    f <<cur_time;

    for (int i=0;i<macLen; i++)
    {
        f<<setw(2)<<(int)mac[i];
        DUID[i+8]=mac[i];
    }

    this->DUID=new TDUID(DUID,DUIDlen);
    delete DUID;

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

SmartPtr<TDUID> TCfgMgr::getDUID()
{
    return DUID;
}
