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
    this->NoConfig = false;
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

TSrvCfgIface::TSrvCfgIface(int ifaceNr)
{
    this->Name="[unknown]";
    this->ID=ifaceNr;
    this->NoConfig=false;
}

TSrvCfgIface::TSrvCfgIface(string ifaceName)
{
    this->Name=ifaceName;
    this->ID = -1;
    this->NoConfig=false;
}
void TSrvCfgIface::setNoConfig()
{
	NoConfig=true;
}

ostream& operator<<(ostream& out,TSrvCfgIface& iface)
{
    SmartPtr<TStationID> Station;
    out << "  <SrvCfgIface ";
    out << "name=\""<<iface.Name << "\" ";
    out << "id=\""<<iface.ID << "\" ";
    if (iface.NoConfig) {
	out << "no-config=\"true\" ";
    }
    if (iface.isUniAddress) {
	out << "uniaddress=\"true\" ";
    }
    out << ">" << std::endl;
    if (iface.isUniAddress) {
	out << "  <unicast>" << *(iface.UniAddress) << "</unicast>" << std::endl;
    }
    
    SmartPtr<TIPv6Addr> stat;
    out << "  <!-- NTP servers count: " << iface.NTPSrv.count() << "-->" << logger::endl;
    iface.NTPSrv.first();
    while(stat=iface.NTPSrv.get())
	out << "    <ntp>" << *stat << "</ntp>" << logger::endl;
    
    out << "    <timezone>" << iface.TimeZone << "</timezone>" << logger::endl;
    
    out << "    <!-- DNS Resolvers count: " << iface.DNSSrv.count() << "-->" << logger::endl;
    iface.DNSSrv.first();
    while(stat=iface.DNSSrv.get())
	out << "  <dns>" << *stat << "</dns>" << logger::endl;
    
    out << "    <domain>" << iface.Domain << "</domain>" << logger::endl;
    
    int classCnt=0;
    SmartPtr<TSrvCfgAddrClass>	groupPtr;
    iface.SrvCfgAddrClassLst.first();
    out << "    <!-- IPv6 addr class count: " << iface.SrvCfgAddrClassLst.count() 
	<< "-->" << logger::endl;
    while(groupPtr=iface.SrvCfgAddrClassLst.get())
    {	
	out << *groupPtr;
    }
    out << "  </SrvCfgIface>" << std::endl;
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
