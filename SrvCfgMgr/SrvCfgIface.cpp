/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgIface.cpp,v 1.21 2006-03-03 21:09:34 thomson Exp $
 */

#include <sstream>
#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "Logger.h"

using namespace std;

void TSrvCfgIface::firstAddrClass() {
    this->SrvCfgAddrClassLst.first();
}

/*
 * tries to find if there is a class, where client is on white-list
 */
bool TSrvCfgIface::getPreferedAddrClassID(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, unsigned long &classid) {
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    this->SrvCfgAddrClassLst.first();
    while(ptrClass=SrvCfgAddrClassLst.get()) {
        if (ptrClass->clntPrefered(duid, clntAddr)) { 
            classid=ptrClass->getID();
            return true;
        }
    }
    return false;
}

/*
 * tries to find a class, which client is allowed to use
 */
bool TSrvCfgIface::getAllowedAddrClassID(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, unsigned long &classid) {
    unsigned int clsid[100];
    unsigned int share[100];
    unsigned int cnt = 0;
    unsigned int sum = 0;
    unsigned int rnd;

    SmartPtr<TSrvCfgAddrClass> ptrClass;
    this->SrvCfgAddrClassLst.first();
    while( (ptrClass=SrvCfgAddrClassLst.get()) && (cnt<100) ) {
        if (ptrClass->clntSupported(duid, clntAddr)) {
            clsid[cnt]   = ptrClass->getID();
	    share[cnt]   = ptrClass->getShare();
	    sum         += ptrClass->getShare();
	    cnt++;
        }
    }

    if (!cnt) 
	return false; // this client is not supported by any class

    rnd = rand() % sum;

    unsigned int j=0;
    
    for (unsigned int i=0; i<100;i++) {
	j += share[i];
	if (j>=rnd) {
	    classid = clsid[i];
	    break;
	}
    }

    return true;
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getAddrClass() {
    return SrvCfgAddrClassLst.get();
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getClassByID(unsigned long id) {
    this->firstAddrClass();
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    while (ptrClass = this->getAddrClass()) {
	if (ptrClass->getID() == id)
	    return ptrClass;
    }
    return 0;
}

void TSrvCfgIface::addClntAddr(SmartPtr<TIPv6Addr> ptrAddr) {
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    this->firstAddrClass();
    while (ptrClass = this->getAddrClass() ) {
	if (ptrClass->addrInPool(ptrAddr)) {
	    Log(Debug) << "Address usage for class " << ptrClass->getID()
		       << " increased by 1." << LogEnd;
	    ptrClass->incrAssigned();
	    return;
	}
    }
    Log(Warning) << "Unable to increase address usage: no class found for " 
		 << *ptrAddr << LogEnd;
}

void TSrvCfgIface::delClntAddr(SmartPtr<TIPv6Addr> ptrAddr) {
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    this->firstAddrClass();
    while (ptrClass = this->getAddrClass() ) {
	if (ptrClass->addrInPool(ptrAddr)) {
	    ptrClass->decrAssigned();
	    Log(Debug) << "Address usage for class " << ptrClass->getID()
		       << " decreased by 1." << LogEnd;
	    return;
	}
    }
    Log(Warning) << "Unable to decrease address usage: no class found for " 
		 << *ptrAddr << LogEnd;
}

SmartPtr<TSrvCfgAddrClass> TSrvCfgIface::getRandomClass(SmartPtr<TDUID> clntDuid, 
							SmartPtr<TIPv6Addr> clntAddr) {

    unsigned long classid;

    // step 1: Is there a class reserved for this client?

    // if there is class where client is on whitelist, it should be used rather than any other class 
    // that would be also suitable
    if(this->getPreferedAddrClassID(clntDuid, clntAddr, classid)) {
      Log(Debug) << "Found prefered class for client (duid = " << *clntDuid << ", addr = "
  	        << *clntAddr << ")" << LogEnd;
      return this->getClassByID(classid);
    } 

    // Get one of the normal classes
    if(this->getAllowedAddrClassID(clntDuid, clntAddr, classid)) {
	Log(Debug) << "Prefered class for client not found, using classid=" << classid << "." << LogEnd; 
	return this->getClassByID(classid);
    } 

    // This is some kind of problem...
    Log(Error) << "No class is available for client (duid=" << clntDuid->getPlain() << ", addr="
	       << clntAddr->getPlain() << ")." << LogEnd;
    return 0;
}

long TSrvCfgIface::countAddrClass() {
    return this->SrvCfgAddrClassLst.count();
}

int TSrvCfgIface::getID() {
    return this->ID;
}

string TSrvCfgIface::getName() {
    return this->Name;
}

string TSrvCfgIface::getFullName() {
    ostringstream oss;
    oss << this->ID;
    return string(this->Name)
	+"/"
	+oss.str();
}

SmartPtr<TIPv6Addr> TSrvCfgIface::getUnicast() {
	return this->Unicast;
}


TSrvCfgIface::~TSrvCfgIface() {
}

void TSrvCfgIface::setOptions(SmartPtr<TSrvParsGlobalOpt> opt) {
    // default options
    this->preference    = opt->getPreference();
    this->IfaceMaxLease = opt->getIfaceMaxLease();
    this->ClntMaxLease  = opt->getClntMaxLease();
    this->RapidCommit   = opt->getRapidCommit();
    this->Unicast       = opt->getUnicast();
    
    // option: DNS-SERVERS
    if (opt->supportDNSServer())  this->setDNSServerLst(opt->getDNSServerLst());
    if (opt->supportDomain())     this->setDomainLst(opt->getDomainLst());
    if (opt->supportNTPServer())  this->setNTPServerLst(opt->getNTPServerLst());
    if (opt->supportTimezone())   this->setTimezone(opt->getTimezone());
    if (opt->supportFQDN()){
	    this->setFQDNLst(opt->getFQDNLst());
	    Log(Debug) <<"FQDN Support is enabled on the " << this->getFullName()  << " interface." << LogEnd;
    }
    if (opt->supportSIPServer())  this->setSIPServerLst(opt->getSIPServerLst());
    if (opt->supportSIPDomain())  this->setSIPDomainLst(opt->getSIPDomainLst());
    if (opt->supportNISServer())  this->setNISServerLst(opt->getNISServerLst());
    if (opt->supportNISDomain())  this->setNISDomain(opt->getNISDomain());
    if (opt->supportNISPServer()) this->setNISPServerLst(opt->getNISPServerLst());
    if (opt->supportNISPDomain()) this->setNISPDomain(opt->getNISPDomain());
    if (opt->supportLifetime())   this->setLifetime(opt->getLifetime());

    if (opt->isRelay()) {
	this->Relay = true;
	this->RelayName        = opt->getRelayName();
	this->RelayID          = opt->getRelayID();
	this->RelayInterfaceID = opt->getRelayInterfaceID();
    } else {
	this->Relay = false;
	this->RelayName = "";
	this->RelayID = 0;
	this->RelayInterfaceID = 0;
    }
}

/*
 * default contructor
 */
TSrvCfgIface::TSrvCfgIface() {
    this->setDefaults();
}

TSrvCfgIface::TSrvCfgIface(int ifaceNr) {
    this->setDefaults();
    this->ID=ifaceNr;
}

TSrvCfgIface::TSrvCfgIface(string ifaceName) {
    this->setDefaults();
    this->Name=ifaceName;
}

void TSrvCfgIface::setDefaults() {
    this->ID = -1;
    this->NoConfig=false;
    this->Name = "[unknown]";
    this->ID = -1;
    this->NoConfig = false;
    this->preference = 0;
    
    this->DNSServerSupport  = false;
    this->DomainSupport     = false;
    this->NTPServerSupport  = false;
    this->TimezoneSupport   = false;
    this->FQDNSupport       = false;
    this->SIPServerSupport  = false;
    this->SIPDomainSupport  = false;
    this->NISServerSupport  = false;
    this->NISDomainSupport  = false;
    this->NISPServerSupport = false;
    this->NISPDomainSupport = false;
    this->LifetimeSupport   = false;
}

void TSrvCfgIface::setNoConfig() {
    this->NoConfig=true;
}

unsigned char TSrvCfgIface::getPreference() {
    return this->preference;
}

void TSrvCfgIface::setName(string ifaceName) {
    this->Name=ifaceName;
}

void TSrvCfgIface::setID(int ifaceID) {
    this->ID=ifaceID;
}

bool TSrvCfgIface::getRapidCommit() {
    return this->RapidCommit;
}


void TSrvCfgIface::addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass) {
    this->SrvCfgAddrClassLst.append(addrClass);
}

long TSrvCfgIface::getIfaceMaxLease() {
    return this->IfaceMaxLease;
}

unsigned long TSrvCfgIface::getClntMaxLease() {
    return this->ClntMaxLease;
}

string TSrvCfgIface::getRelayName() {
    return this->RelayName;
}

int TSrvCfgIface::getRelayID() {
    return this->RelayID;
}

int TSrvCfgIface::getRelayInterfaceID() {
    return this->RelayInterfaceID;
}

bool TSrvCfgIface::isRelay() {
    return this->Relay;
}

void TSrvCfgIface::setRelayName(string name) {
    this->RelayName = name;
}

void TSrvCfgIface::setRelayID(int id) {
    this->RelayID = id;
}


// --------------------------------------------------------------------
// --- options --------------------------------------------------------
// --------------------------------------------------------------------
// --- option: DNS servers ---
void TSrvCfgIface::setDNSServerLst(List(TIPv6Addr) *lst) {
    this->DNSServerLst = *lst;
    this->DNSServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgIface::getDNSServerLst() {
    return &this->DNSServerLst;
}
bool TSrvCfgIface::supportDNSServer(){
    return this->DNSServerSupport;
}

// --- option: DOMAIN ---
void TSrvCfgIface::setDomainLst(List(string) * lst) {
    this->DomainLst = *lst;
    this->DomainSupport = true;
}
List(string) * TSrvCfgIface::getDomainLst() {
    return &this->DomainLst;
}
bool TSrvCfgIface::supportDomain(){
    return this->DomainSupport;
}

// --- option: NTP-SERVERS ---
void TSrvCfgIface::setNTPServerLst(List(TIPv6Addr) * lst) {
    this->NTPServerLst = *lst;
    this->NTPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgIface::getNTPServerLst() {
    return &this->NTPServerLst;
}
bool TSrvCfgIface::supportNTPServer(){
    return this->NTPServerSupport;
}

// --- option: TIMEZONE ---
void TSrvCfgIface::setTimezone(string timezone) {
    this->Timezone=timezone;
    this->TimezoneSupport = true;
}
string TSrvCfgIface::getTimezone() {
    return this->Timezone;
}
bool TSrvCfgIface::supportTimezone(){
    return this->NTPServerSupport;
}

// --- option: SIP server ---
void TSrvCfgIface::setSIPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->SIPServerLst = *lst;
    this->SIPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgIface::getSIPServerLst() {
    return &this->SIPServerLst;
}
bool TSrvCfgIface::supportSIPServer(){
    return this->SIPServerSupport;
}

// --- option: SIP domain ---
List(string) * TSrvCfgIface::getSIPDomainLst() { 
    return &this->SIPDomainLst;
}
void TSrvCfgIface::setSIPDomainLst(List(string) * domain) { 
    this->SIPDomainLst = *domain;
    this->SIPDomainSupport = true;
}
bool TSrvCfgIface::supportSIPDomain() {
    return this->SIPDomainSupport;
}

// --- option: FQDN ---

void TSrvCfgIface::setFQDNLst(List(TFQDN) *fqdn) {
    this->FQDNLst = *fqdn;
    this->FQDNSupport = true;
}

string TSrvCfgIface::getFQDNName(SmartPtr<TDUID> duid) {
    int cpt = 1;
    string res = "";
    SmartPtr<TFQDN> foo = FQDNLst.getFirst();
    
    Log(Debug) << "FQDN : Looking for DUID : "<< *duid << LogEnd;
    FQDNLst.first();

    foo = FQDNLst.get();
    while(!(*(*foo).Duid == *duid) && cpt<FQDNLst.count()){
        foo = FQDNLst.get();	
 	cpt++;
    }
    
    if (cpt<FQDNLst.count()) {
        res = (*foo).Name;
    } else {
        if (*(*foo).Duid == *duid) {
	    res = (*foo).Name;
	}
    }
    Log(Debug) << "FQDN found : " << res << LogEnd;
    return res;
}

string TSrvCfgIface::getFQDNName(SmartPtr<TIPv6Addr> addr) {
    int cpt = 1;
    string res = "";
    SmartPtr<TFQDN> foo = FQDNLst.getFirst();
    FQDNLst.first();
    foo = FQDNLst.get();
    while(!(*(*foo).Addr == *addr) && cpt<FQDNLst.count()){
        foo = FQDNLst.get();
 	cpt++;
    }
    if (cpt<FQDNLst.count()) {
        res = (*foo).Name;
    } else {
        if (*(*foo).Addr == *addr) {
	    res = (*foo).Name;
	}
    }
    return res;
}

string TSrvCfgIface::getFQDNName() {
    int cpt = 1;
    string res = "";
    SmartPtr<TFQDN> foo = FQDNLst.getFirst();
    FQDNLst.first();
    foo = FQDNLst.get();
    while((!(*(*foo).Addr == *(new TIPv6Addr())) || 
           !(*(*foo).Duid == *(new TDUID())) ||
           (*foo).used == true ) &&
          cpt<FQDNLst.count()){
        foo = FQDNLst.get();
        cpt++;
    }
    if (cpt<FQDNLst.count()) {
        FQDNLst.getPrev();
        (*(FQDNLst.get())).used=true;
        res = (*foo).Name;
    } else {
        if (*(*foo).Addr == *(new TIPv6Addr()) && (*(*foo).Duid == *(new TDUID())) &&
            (*foo).used==false) {
            (*foo).used=true;
	    res = (*foo).Name;
	}
    }
    return res;
}


SmartPtr<TDUID> TSrvCfgIface::getFQDNDuid(string name) {
    SmartPtr<TDUID> res = new TDUID();
    return res;
}

List(TFQDN) *TSrvCfgIface::getFQDNLst() {
    return &this->FQDNLst;
}

bool TSrvCfgIface::supportFQDN() {
    return this->FQDNSupport;
}


// --- option: NIS server ---
void TSrvCfgIface::setNISServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISServerLst     = *lst;
    this->NISServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgIface::getNISServerLst() {
    return &this->NISServerLst;
}
bool TSrvCfgIface::supportNISServer(){
    return this->NISServerSupport;
}

// --- option: NIS domain ---
void TSrvCfgIface::setNISDomain(string domain) { 
    this->NISDomain=domain;
    this->NISDomainSupport=true;
}
string TSrvCfgIface::getNISDomain() { 
    return this->NISDomain;
}
bool TSrvCfgIface::supportNISDomain() {
    return this->NISDomainSupport;
}

// --- option: NIS+ server ---
void TSrvCfgIface::setNISPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISPServerLst = *lst;
    this->NISPServerSupport = true;
}
List(TIPv6Addr) * TSrvCfgIface::getNISPServerLst() {
    return &this->NISPServerLst;
}
bool TSrvCfgIface::supportNISPServer(){
    return this->NISPServerSupport;
}

// --- option: NIS+ domain ---
void TSrvCfgIface::setNISPDomain(string domain) { 
    this->NISPDomain=domain;
    this->NISPDomainSupport=true;
}
string TSrvCfgIface::getNISPDomain() { 
    return this->NISPDomain;
}
bool TSrvCfgIface::supportNISPDomain() {
    return this->NISPDomainSupport;
}

// --- option: LIFETIME ---
void TSrvCfgIface::setLifetime(unsigned int x) {
    this->Lifetime = x;
    this->LifetimeSupport = true;
}
unsigned int TSrvCfgIface::getLifetime() {
    return this->Lifetime;
}

bool TSrvCfgIface::supportLifetime() {
    return this->LifetimeSupport;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TSrvCfgIface& iface) {
    SmartPtr<TStationID> Station;
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

    out << dec;
    out << "  <SrvCfgIface name=\""<<iface.Name << "\" ifindex=\""<<iface.ID << "\">" << endl;

    if (iface.Relay) {
	out << "    <relay name=\"" << iface.RelayName << "\" ifindex=\"" << iface.RelayID << "\" interfaceid=\""
	    << iface.RelayInterfaceID << "\"/>" << std::endl;
    } else {
	out << "    <!-- <relay/> -->" << std::endl;
    }

    out << "    <preference>" << (int)iface.preference << "</preference>" << std::endl;
    out << "    <ifaceMaxLease>" << iface.IfaceMaxLease << "</ifaceMaxLease>" << std::endl;
    out << "    <clntMaxLease>" << iface.ClntMaxLease << "</clntMaxLease>" << std::endl;
       
    if (iface.Unicast) {
        out << "    <unicast>" << *(iface.Unicast) << "</unicast>" << endl;
    } else {
        out << "    <!-- <unicast/> -->" << endl;
    }

    if (iface.RapidCommit) {
        out << "    <rapid-commit/>" << std::endl;
    } else {
        out << "    <!-- <rapid-commit/> -->" << std::endl;
    }

    SmartPtr<TSrvCfgAddrClass>	groupPtr;
    iface.SrvCfgAddrClassLst.first();
    out << "    <!-- IPv6 addr class count: " << iface.SrvCfgAddrClassLst.count() << "-->" << endl;
    while(groupPtr=iface.SrvCfgAddrClassLst.get())
    {	
	out << *groupPtr;
    }
    
    out << "    <!-- options -->" << endl;

    // DNS-SERVERS
    out << "    <!-- <dns-servers count=\"" << iface.DNSServerLst.count() << "\"> -->" << endl;
    iface.DNSServerLst.first();
    while (addr = iface.DNSServerLst.get()) {
        out << "    <dns-server>" << *addr << "</dns-server>" << endl;
    }

    // DOMAINS
    out << "    <!-- <domains count=\"" << iface.DomainLst.count() << "\"> -->" << endl;
    iface.DomainLst.first();
    while (str = iface.DomainLst.get()) {
        out << "    <domain>" << *str << "</domain>" << endl;
    }

    // NTP-SERVERS
    out << "    <!-- <ntp-servers count=\"" << iface.NTPServerLst.count() << "\"> -->" << endl;
    iface.NTPServerLst.first();
    while (addr = iface.NTPServerLst.get()) {
        out << "    <ntp-server>" << *addr << "</ntp-server>" << endl;
    }

    // option: TIMEZONE
    if (iface.supportTimezone()) {
        out << "    <timezone>" << iface.Timezone << "</timezone>" << endl;
    } else {
        out << "    <!-- <timezone/> -->" << endl;
    }

    // option: SIP-SERVERS
    out << "    <!-- <sip-servers count=\"" << iface.SIPServerLst.count() << "\"> -->" << endl;
    iface.SIPServerLst.first();
    while (addr = iface.SIPServerLst.get()) {
        out << "    <sip-server>" << *addr << "</sip-server>" << endl;
    }

    // option: SIP-DOMAINS
    out << "    <!-- <sip-domains count=\"" << iface.SIPDomainLst.count() << "\"> -->" << endl;
    iface.SIPDomainLst.first();
    while (str = iface.SIPDomainLst.get()) {
        out << "    <sip-domain>" << *str << "</sip-domain>" << endl;
    }

    // option: FQDN
    if (iface.supportFQDN()) {
        out <<"     <fqdn>" << iface.FQDNLst.count() << "</fqdn>" << endl;
    } else {
        out << "    <!-- <fqdn/> -->" << endl;
    }

    // option: NIS-SERVERS
    out << "    <!-- <nis-servers count=\"" << iface.NISServerLst.count() << "\"> -->" << endl;
    iface.NISServerLst.first();
    while (addr = iface.NISServerLst.get()) {
        out << "    <nis-server>" << *addr << "</nis-server>" << endl;
    }

    // option: NIS-DOMAIN
    if (iface.supportNISDomain()) {
        out << "    <nis-domain>" << iface.NISDomain << "</nis-domain>" << endl;
    } else {
        out << "    <!-- <nis-domain/> -->" << endl;
    }

    // option: NIS+-SERVERS
    out << "    <!-- <nisplus-servers count=\"" << iface.NISPServerLst.count() << "\"> -->" << endl;
    iface.NISPServerLst.first();
    while (addr = iface.NISPServerLst.get()) {
        out << "    <nisplus-server>" << *addr << "</nisplus-server>" << endl;
    }

    // option: NIS+-DOMAIN
    if (iface.supportNISPDomain()) {
        out << "    <nisplus-domain>" << iface.NISPDomain << "</nisplus-domain>" << endl;
    } else {
        out << "    <!-- <nisplus-domain/> -->" << endl;
    }

    // option: LIFETIME
    if (iface.supportLifetime()) {
        out << "    <lifetime>" << iface.Lifetime << "</lifetime>" << endl;
    } else {
        out << "    <!-- <lifetime/> -->" << endl;
    }
    
    out << "  </SrvCfgIface>" << endl;
    return out;
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.20  2005/08/03 22:47:34  thomson
 * Support for 'share' parameter added,
 * Support for randomization between classes added.
 *
 * Revision 1.19  2005/08/02 23:43:27  thomson
 * *** empty log message ***
 *
 * Revision 1.18  2005/08/02 00:33:58  thomson
 * White-list bug fixed (bug #120),
 * Minor compilation warnings in gcc 4.0 removed.
 *
 * Revision 1.17  2005/05/02 21:48:42  thomson
 * getFullName() method implemented.
 *
 * Revision 1.16  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.15  2005/01/03 21:57:08  thomson
 * Relay support added.
 *
 * Revision 1.14  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.13  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.12  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.8  2004/06/28 21:34:18  thomson
 * DUID is now parsed properly and SrvCfgMgr dumps valid xml file.
 *
 * Revision 1.7  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 * Revision 1.6  2004/06/06 22:12:29  thomson
 * Preference option has changed scope from class to interface
 *                                                                           
 */
