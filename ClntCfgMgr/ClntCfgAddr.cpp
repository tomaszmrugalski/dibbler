#include <iostream>
#include <iomanip>
#include <limits.h>
#include "DHCPConst.h"
#include "ClntCfgAddr.h"
#include "Logger.h"

SmartPtr<TIPv6Addr> TClntCfgAddr::get()
{
    return Addr;
}

long TClntCfgAddr::getValid()
{
    return Valid;
}


long TClntCfgAddr::getPref()
{
    return Pref;
}


TClntCfgAddr::TClntCfgAddr(SmartPtr<TIPv6Addr> addr,long valid,long pref)
{
    //memcpy(Addr,addr,16);
    Addr=addr;
    Valid = valid;
    Pref = pref;
}


TClntCfgAddr::~TClntCfgAddr()
{

}


void TClntCfgAddr::setOptions(SmartPtr<TClntParsGlobalOpt> opt)
{
	this->Valid=opt->getValid();
	this->Pref=opt->getPref();
    this->ValidSendOpt=opt->getValidSendOpt();
    this->PrefSendOpt=opt->getPrefSendOpt();
}


TClntCfgAddr::TClntCfgAddr()
{
    this->Valid=ULONG_MAX;
	this->Pref=ULONG_MAX;
	this->ValidSendOpt=Send;
	this->PrefSendOpt=Send;
	Addr=0;
	//memset(Addr,0,16);
}

TClntCfgAddr::TClntCfgAddr(SmartPtr<TIPv6Addr> addr)
{
	//memcpy(this->Addr,addr,16);
    Addr=addr;
    Valid = ULONG_MAX;
	Pref = ULONG_MAX;
}

ostream& operator<<(ostream& out,TClntCfgAddr& addr)
{
    out << "<addr prefered=\"" << addr.Pref 
		<< "\" valid=\"" << addr.Valid << "\">";
    out << *addr.Addr << "</addr>" << std::endl;	
	return out;
}
