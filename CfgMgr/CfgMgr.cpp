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
        
        delete DUID;
        f.close();
    }
}

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

/*int TCfgMgr::getDUIDlen()
{
    return DUIDlen;
}
*/
