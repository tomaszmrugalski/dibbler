/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: CfgMgr.cpp,v 1.6 2004-05-23 19:12:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
        std::clog << logger::logError << "Can't open file " << cfgFile 
            << ". Config comparision aborted." << logger::endl;
        //Exception or return false
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
        std::clog << logger::logError << "Can't open file "<<cfgFile 
		  <<", when trying to save new config file." << logger::endl;
    else
    {
        newF.seekg(0,ios::end);
        int newLen=newF.tellg();
        newF.seekg(0,ios::beg);
        //try to open output file
        oOldF.open(oldCfgFile.c_str(),ios::trunc|ios::out|ios::binary);
        if (oOldF.fail()) 
        {
            std::clog << logger::logError << "Can't open file "<<oldCfgFile
		      <<", when trying to save new config file"<< logger::endl;
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

void TCfgMgr::setDUID(const string duidFile,char * mac,int macLen, int macType)
{
    ifstream f;
    // DUID - It's common for client and server so why not to create CfgMgr
    f.open(duidFile.c_str());
    if ( !(f.is_open())  ) {
        // unable to open DUID file
        std:: clog << logger::logNotice << "Unable to open DUID file (" << duidFile 
            << "), generating new DUID." << logger::endl;

        this->generateDUID(duidFile,mac,macLen,macType);
        // FIXME: what if open fails? - at least it would be a little strange
        //		  but could we keep opened file (of coz in mode r/w)
        //        I don't know what for should be file open - DUID is set in CfgMgr
        f.close();
    }
    else
    {
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
    }
}

bool TCfgMgr::loadDUID() {
    
    // --- load DUID ---
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
	return true;
    } 
	
    Log(logCrit) << "Cannot generate DUID, because there is no interface with "
		 << "MAC address." << logger::endl;
    this->DUID=new TDUID();
    return false;
};

void TCfgMgr::generateDUID(const string duidFile,char * mac,int macLen, int macType)
{
    //FIXME:DUID should be generated on basis of one of the ifaces' mac address
    //FIXME:what if file create fails?
    //		I think service stops. Hmmm. Exceptions are neccesity (Marek)
    ofstream f;
    f.open( duidFile.c_str() );
    if (!f.is_open()) {
	std::clog << logger::logCrit << "Unable to write " << duidFile << " file." << logger::endl;
//	this->isDone = true;
	return;
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
}

SmartPtr<TDUID> TCfgMgr::getDUID()
{
    return DUID;
}
