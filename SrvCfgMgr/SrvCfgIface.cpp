#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "Logger.h"

void TSrvCfgIface::firstAddrClass()
{
    this->SrvCfgAddrClassLst.first();
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getAddrClass()
{
    return SrvCfgAddrClassLst.get();
}

long TSrvCfgIface::countAddrClass()
{
    return SrvCfgAddrClassLst.count();
}

int TSrvCfgIface::getID()
{
	return ID;
}

string TSrvCfgIface::getName()
{
	return Name;
}


TSrvCfgIface::TSrvCfgIface()
{
}


TSrvCfgIface::~TSrvCfgIface()
{
}

void TSrvCfgIface::setOptions(SmartPtr<TSrvParsGlobalOpt> opt)
{
    this->isUniAddress=opt->getUnicast();
	//memcpy(UniAddress,opt.UniAddress,16);
    this->UniAddress=opt->getAddress();
    
	SmartPtr<TIPv6Addr> stat;
    
    Domain=opt->getDomain();
    opt->firstDNSSrv();
    while(stat=opt->getDNSSrv())
        this->DNSSrv.append(stat);
    
    TimeZone=opt->getTimeZone();
    opt->firstNTPSrv();
    while(stat=opt->getNTPSrv())
        this->NTPSrv.append(stat);

}

TSrvCfgIface::	TSrvCfgIface(int ifaceNr)
{
	ID=ifaceNr;
	//FIXME:here checking iface name in ConfMgr
}

TSrvCfgIface::TSrvCfgIface(string ifaceName)
{
	Name=ifaceName;
	//FIXME:here checking iface number in ConfgMgr
}
void TSrvCfgIface::setNoConfig()
{
	NoConfig=true;
}

ostream& operator<<(ostream& out,TSrvCfgIface& iface)
{
	SmartPtr<TStationID> Station;
	out<<"Parametry interfejsu:" << logger::endl;
	out<<"Nazwa:"<<iface.Name << logger::endl;
	out<<"ID:"<<iface.ID << logger::endl;
	out<<"No config.:"<<iface.NoConfig << logger::endl;
	out<<"Czy uzywac unicastu:"<<iface.isUniAddress << logger::endl;
    out<<"Adres unicastowy:"<<*(iface.UniAddress);
	out<<dec << logger::endl;

    SmartPtr<TIPv6Addr> stat;
    cout<<"Liczba serwerow NTP:"<<iface.NTPSrv.count() << logger::endl;
	iface.NTPSrv.first();
	while(stat=iface.NTPSrv.get())
		cout<<*stat << logger::endl;
	
    cout<<"TimeZone:"<<iface.TimeZone << logger::endl;

	cout<<"Liczba serwerow DNS:"<<iface.DNSSrv.count() << logger::endl;
	iface.DNSSrv.first();
	while(stat=iface.DNSSrv.get())
		cout<<*stat << logger::endl;
	
    cout<<"Domena:"<<iface.Domain << logger::endl;

	int classCnt=0;
	SmartPtr<TSrvCfgAddrClass>	groupPtr;
	iface.SrvCfgAddrClassLst.first();
	while(groupPtr=iface.SrvCfgAddrClassLst.get())
	{	
		cout<<"Nr.klasy:"<<classCnt++ << logger::endl;
		cout<<"{" << logger::endl<<*groupPtr << logger::endl<<"}" << logger::endl;
	}
	return out;
}
void TSrvCfgIface::setIfaceName(string ifaceName)
{
	this->Name=ifaceName;
}

void TSrvCfgIface::setIfaceID(int ifaceID)

{
	this->ID=ifaceID;
}
void TSrvCfgIface::addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass)
{
    this->SrvCfgAddrClassLst.append(addrClass);
}
TContainer<SmartPtr<TIPv6Addr> > TSrvCfgIface::getDNSSrvLst()
{
    return this->DNSSrv;
}
TContainer<SmartPtr<TIPv6Addr> > TSrvCfgIface::getNTPSrvLst()
{
    return this->NTPSrv;
}
string TSrvCfgIface::getDomain()
{
    return this->Domain;
}
string TSrvCfgIface::getTimeZone()
{
    return this->TimeZone;
}
