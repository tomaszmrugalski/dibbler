/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgIface.cpp,v 1.7 2004-06-17 23:53:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/06/06 22:12:29  thomson
 * Preference option has changed scope from class to interface
 *
 *                                                                           
 */

#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "Logger.h"

void TSrvCfgIface::firstAddrClass() {
    this->SrvCfgAddrClassLst.first();
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getAddrClass() {
    return SrvCfgAddrClassLst.get();
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getRandomClass(SmartPtr<TDUID> clntDuid, 
							SmartPtr<TIPv6Addr> clntAddr) {
    // FIXME: randomize it
    this->firstAddrClass();
    return this->getAddrClass();
}

long TSrvCfgIface::countAddrClass() {
    return SrvCfgAddrClassLst.count();
}

int TSrvCfgIface::getID() {
	return ID;
}

string TSrvCfgIface::getName() {
	return Name;
}

TSrvCfgIface::~TSrvCfgIface() {
}

void TSrvCfgIface::setOptions(SmartPtr<TSrvParsGlobalOpt> opt) {
    this->isUniAddress = opt->getUnicast();
    this->UniAddress   = opt->getAddress();
    this->preference   = opt->getPreference();
    this->Domain       = opt->getDomain();
    this->TimeZone     = opt->getTimeZone();
    
    SmartPtr<TIPv6Addr> stat;

    // copy DNS servers
    opt->firstDNSSrv();
    while(stat=opt->getDNSSrv())
        this->DNSSrv.append(stat);

    // copy NTP servers
    opt->firstNTPSrv();
    while(stat=opt->getNTPSrv())
        this->NTPSrv.append(stat);
}



/*
 * default contructor
 */
TSrvCfgIface::TSrvCfgIface() {
    this->Name = "[unknown]";
    this->ID = -1;
    this->NoConfig = false;
    this->preference = 0;
}

TSrvCfgIface::TSrvCfgIface(int ifaceNr) {
    this->Name="[unknown]";
    this->ID=ifaceNr;
    this->NoConfig=false;
    this->preference = 0;
}

TSrvCfgIface::TSrvCfgIface(string ifaceName) {
    this->Name=ifaceName;
    this->ID = -1;
    this->NoConfig=false;
}
void TSrvCfgIface::setNoConfig() {
    NoConfig=true;
}

unsigned char TSrvCfgIface::getPreference() {
    return this->preference;
}

void TSrvCfgIface::setIfaceName(string ifaceName) {
	this->Name=ifaceName;
}

void TSrvCfgIface::setIfaceID(int ifaceID) {
	this->ID=ifaceID;
}

void TSrvCfgIface::addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass) {
    this->SrvCfgAddrClassLst.append(addrClass);
}

TContainer<SmartPtr<TIPv6Addr> > TSrvCfgIface::getDNSSrvLst() {
    return this->DNSSrv;
}

TContainer<SmartPtr<TIPv6Addr> > TSrvCfgIface::getNTPSrvLst() {
    return this->NTPSrv;
}

string TSrvCfgIface::getDomain() {
    return this->Domain;
}

string TSrvCfgIface::getTimeZone() {
    return this->TimeZone;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TSrvCfgIface& iface) {
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
    out << "  <preference>" << (int)iface.preference << "</preference>" << std::endl;
    
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
