/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgIface.cpp,v 1.48 2008-11-11 22:41:48 thomson Exp $
 */

#include <cstdlib>
#include <sstream>
#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgPD.h"
#include "Logger.h"

#ifndef MOD_SRV_DISABLE_DNSUPDATE
#include "DNSUpdate.h"
#endif

using namespace std;

void TSrvCfgIface::addClientExceptionsLst(List(TSrvCfgOptions) exLst)
{
    Log(Debug) << exLst.count() << " per-client configurations (exceptions) added." << LogEnd;
    ExceptionsLst = exLst;
}

bool TSrvCfgIface::leaseQuerySupport()
{
    return LeaseQuery;
}


SPtr<TSrvCfgOptions> TSrvCfgIface::getClientException(SPtr<TDUID> duid, SPtr<TSrvOptRemoteID> remoteID, bool quiet)
{
    SPtr<TSrvCfgOptions> x;
    ExceptionsLst.first();
    while (x=ExceptionsLst.get()) {
	if ( duid && x->getDuid() && (*(x->getDuid()) == *duid) ) {
	    if (!quiet)
		Log(Debug) << "Found per-client configuration (exception) for client with DUID=" << x->getDuid()->getPlain() << LogEnd;
	    return x;
	}
	SPtr<TSrvOptRemoteID> id;
	id = x->getRemoteID();

	if ( remoteID && id && (remoteID->getVendor() == id->getVendor()) && (id->getVendorDataLen() == remoteID->getVendorDataLen())
	     && !memcmp(id->getVendorData(), remoteID->getVendorData(), id->getVendorDataLen()) ) {
		Log(Debug) << "Found per-client configuration (exception) for client with RemoteID: vendor=" << id->getVendor()
			   << ", data=" << id->getVendorDataPlain() << "." << LogEnd;
	    return x;
	}
    }
    return 0;
}

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


void TSrvCfgIface::firstPD() {
    this->SrvCfgPDLst.first();
}

bool TSrvCfgIface::supportPrefixDelegation() {
    return this->PrefixDelegationSupport;
}

void TSrvCfgIface::addTA(SmartPtr<TSrvCfgTA> ta) {
    this->SrvCfgTALst.append(ta);
}

void TSrvCfgIface::firstTA() {
    this->SrvCfgTALst.first();
}
SmartPtr<TSrvCfgTA> TSrvCfgIface::getTA() {
    return this->SrvCfgTALst.get();
}

void TSrvCfgIface::addPD(SmartPtr<TSrvCfgPD> pd) {
    this->PrefixDelegationSupport = true;
    this->SrvCfgPDLst.append(pd);
}

SmartPtr<TSrvCfgTA> TSrvCfgIface::getTA(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr) {
    SmartPtr<TSrvCfgTA> ta;

    // try to find preferred TA for this client
    this->SrvCfgTALst.first();
    while ( ta = this->getTA() ) {
	if (ta->clntPrefered(clntDuid, clntAddr))
	    return ta;
    }

    // prefered not found? Then find first allowed
    this->SrvCfgTALst.first();
    while ( ta = this->getTA() ) {
	if (ta->clntSupported(clntDuid, clntAddr))
	    return ta;
    }

    return 0;
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



/** Prefix delegation functions

*/

SmartPtr<TSrvCfgPD> TSrvCfgIface::getPD() {
    return SrvCfgPDLst.get();
}

SmartPtr<TSrvCfgPD> TSrvCfgIface::getPDByID(unsigned long id) {
    this->firstPD();
    SmartPtr<TSrvCfgPD> ptrPD;
    while (ptrPD = this->getPD()) {
	if (ptrPD->getID() == id)
	    return ptrPD;
    }
    return 0;
}

bool TSrvCfgIface::addClntPrefix(SmartPtr<TIPv6Addr> ptrAddr) {
    SmartPtr<TSrvCfgPD> ptrPD;
    this->firstPD();
    while (ptrPD = this->getPD() ) {
	if (ptrPD->prefixInPool(ptrAddr)) {
	    Log(Debug) << "PD: Prefix usage for class " << ptrPD->getID()
		       << " increased by 1." << LogEnd;
	    ptrPD->incrAssigned();
	    return true;
	}
    }
    Log(Warning) << "Unable to increase prefix usage: no prefix found for "
		 << *ptrAddr << LogEnd;
    return false;
}

bool TSrvCfgIface::delClntPrefix(SmartPtr<TIPv6Addr> ptrAddr) {
    SmartPtr<TSrvCfgPD> ptrPD;
    this->firstPD();
    while (ptrPD = this->getPD() ) {
	if (ptrPD->prefixInPool(ptrAddr)) {
	    ptrPD->decrAssigned();
	    Log(Debug) << "PD: Prefix usage for class " << ptrPD->getID()
		       << " decreased by 1." << LogEnd;
	    return true;
	}
    }
    Log(Warning) << "Unable to decrease address usage: no class found for "
		 << *ptrAddr << LogEnd;
    return false;
}

long TSrvCfgIface::countPD() {
    return this->SrvCfgPDLst.count();
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
    this->LeaseQuery    = opt->getLeaseQuerySupport();

    if (opt->supportFQDN()){
	AcceptUnknownFQDN = opt->acceptUnknownFQDN();
#ifndef MOD_SRV_DISABLE_DNSUPDATE
	this->setFQDNLst(opt->getFQDNLst());
	this->setFQDNMode(opt->getFQDNMode());
	this->setRevDNSZoneRootLength(opt->getRevDNSZoneRootLength());
	Log(Debug) <<"FQDN: Support is enabled on the " << this->getName()  << " interface." << LogEnd;
	Log(Debug) <<"FQDN: Mode set to " << this->getFQDNMode() << ": ";
	switch (this->getFQDNMode()) {
	case DNSUPDATE_MODE_NONE:
	    Log(Cont) << "server will not perform any updates." << LogEnd;
	    break;
	case DNSUPDATE_MODE_PTR:
	    Log(Cont) << "server will perform reverse (PTR) update only." << LogEnd;
	    break;
	case DNSUPDATE_MODE_BOTH:
	    Log(Cont) << "server will perform both (AAAA and PTR) updates." << LogEnd;
	}
#else
	Log(Error) << "DNSUpdate is disabled (please recompile)." << LogEnd;
#endif
    	Log(Debug) <<"FQDN: revDNS zoneroot lenght set to " << this->getRevDNSZoneRootLength()<< "." << LogEnd;
    }

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

    TSrvCfgOptions::setOptions(opt);
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

    this->FQDNSupport             = false;
    this->AcceptUnknownFQDN       = false;
    this->PrefixDelegationSupport = false;
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

SPtr<TSrvOptInterfaceID> TSrvCfgIface::getRelayInterfaceID() {
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

// --- option: FQDN ---
void TSrvCfgIface::setFQDNLst(List(TFQDN) *fqdn) {
    this->FQDNLst = *fqdn;
    this->FQDNSupport = true;
}

/**
 * this method tries to find a name for a client. It check client's hint, and possible reservations
 * by duid or by address
 *
 * @param duid
 * @param addr
 * @param hint
 *
 * @return
 */
SPtr<TFQDN> TSrvCfgIface::getFQDNName(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, string hint) {
    FQDNLst.first();

    SPtr<TFQDN> bestFound=0; // best FQDN found for that client

    SPtr<TFQDN> foo;

    bool duplicate = false ; // whether the hint exists in the FQDN list

    while (foo=this->FQDNLst.get()) {

	if (foo->isUsed())
	{ // client sent a hint, but it is used currently
	    if (foo->Name == hint)
	     	   duplicate = true;
            continue;
	}

	if (duid && (foo->Duid) && *(foo->Duid)== *duid) {
	    Log(Debug) << "FQDN found: " << foo->Name << " using duid " << duid->getPlain() << LogEnd;
	    return foo;
	}
	if (addr && (foo->Addr) && *(foo->Addr)==*addr) {
	    Log(Debug) << "FQDN found: " << foo->Name << " using address " << addr->getPlain() << LogEnd;
	    return foo;
	}

	if (foo->Name == hint){
	    // client asked for this name. Let's check if client is allowed to get this name.
	   duplicate = true;
	   if ( (!foo->Duid) && (!foo->Addr) ) {
		Log(Debug) << "Client's hint: " << hint << " found in fqdn list, setting fqdn to "<< foo->Name << LogEnd;
		return foo;
	    }
	}
	if (!foo->Addr && !foo->Duid) {
	    if (!bestFound)
		bestFound = foo;
	 //   return foo;
	}
    }

    if (!hint.empty() &&  !duplicate )
    {
	if (acceptUnknownFQDN()) // should the server be accepting unknown names?
	{
	    Log(Debug) << "Client's hint: " << hint <<" but not found in FQDN list, creating an entry  " <<LogEnd;
	    SPtr<TFQDN> newEntry = new TFQDN(hint,false);
	    FQDNLst.append(newEntry);
	    Log(Debug) << "Retured FQDN  " << newEntry->Name <<LogEnd;
	    return newEntry;
	} else
	{
	    Log(Info) << "FQDN: Client sent valid hint that is not mentioned in server configuration."
		      << " Currently server is configured to drop such hints. To accept them, please "
		      << "add accept-unknown-fqdn in the server.conf." << LogEnd;
	}
    }

    if (bestFound) {
	Log(Debug) << "Not reserved FQDN found: " << bestFound->Name << LogEnd;
	return bestFound;
    }

    // not found
    return 0;
}

/**
 * returns if server should accept FQDN hints that are not configured in the server.conf
 *
 *
 * @return true, if unknown names should be accepted
 */
bool TSrvCfgIface::acceptUnknownFQDN() {
    return AcceptUnknownFQDN;
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

void TSrvCfgIface::setFQDNMode(int FQDNMode) {
    this->FQDNMode = FQDNMode;
}
int TSrvCfgIface::getFQDNMode(){
    return this->FQDNMode;
}
int TSrvCfgIface::getRevDNSZoneRootLength(){
	return this->revDNSZoneRootLength;
}
void TSrvCfgIface::setRevDNSZoneRootLength(int revDNSZoneRootLength){
	this->revDNSZoneRootLength=revDNSZoneRootLength;
}

string TSrvCfgIface::getFQDNModeString() {
    switch (this->FQDNMode) {
    case 0:  return "updates disabled";
    case 1:  return "server will update PTR";
    case 2:  return "server will update PTR and AAAA";
    default: return "unknown";
    }
}


void TSrvCfgIface::addTAAddr() {
    SmartPtr<TSrvCfgTA> ta;
    this->firstTA();
    ta=this->getTA();
    if (!ta) {
	Log(Error) << "Unable to increase TA usage. TA (temporary addresses) is not found on the "
		   << this->getFullName() << " interface." << LogEnd;
	return;
    }
    ta->incrAssigned();
}

void TSrvCfgIface::delTAAddr() {
    SmartPtr<TSrvCfgTA> ta;
    this->firstTA();
    ta = this->getTA();
    if (!ta) {
	Log(Error) << "Unable to decrease TA usage. TA (temporary addresses) is not found on the "
		   << this->getFullName() << " interface." << LogEnd;
	return;
    }
    ta->decrAssigned();
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
	    << iface.RelayInterfaceID->getPlain() << "\"/>" << std::endl;
    } else {
	out << "    <!-- <relay/> -->" << std::endl;
    }

    out << "    <preference>" << (int)iface.preference << "</preference>" << std::endl;
    out << "    <ifaceMaxLease>" << iface.IfaceMaxLease << "</ifaceMaxLease>" << std::endl;
    out << "    <clntMaxLease>" << iface.ClntMaxLease << "</clntMaxLease>" << std::endl;
    out << "    <LeaseQuery>" << (iface.LeaseQuery?"1":"0") << "</LeaseQuery>" << std::endl;

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

    out << endl;
    // print IA objects
    SmartPtr<TSrvCfgAddrClass>	ia;
    iface.SrvCfgAddrClassLst.first();
    out << "    <!-- IA: non-temporary addr class count: " << iface.SrvCfgAddrClassLst.count() << "-->" << endl;
    while( ia=iface.SrvCfgAddrClassLst.get() ) {
	out << *ia;
    }

    out << endl;
    // print PD objects
    SmartPtr<TSrvCfgPD>	pd;
    iface.SrvCfgPDLst.first();
    out << "    <!-- PD: prefix delegation class count: " << iface.SrvCfgPDLst.count() << "-->" << endl;
    while( pd=iface.SrvCfgPDLst.get() ) {
	out << *pd;
    }

    out << endl;
    // print TA objects
    SmartPtr<TSrvCfgTA> ta;
    iface.firstTA();
    out << "    <!-- TA: temporary IPv6 addr class count: " << iface.SrvCfgTALst.count() << "-->" << endl;
    while( ta=iface.getTA() )
    {
	out << *ta;
    }

    out << endl << "    <!-- options -->" << endl;

    // option: DNS-SERVERS
    out << "    <!-- <dns-servers count=\"" << iface.DNSServerLst.count() << "\"> -->" << endl;
    iface.DNSServerLst.first();
    while (addr = iface.DNSServerLst.get()) {
        out << "    <dns-server>" << *addr << "</dns-server>" << endl;
    }

    // option: DOMAINS
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

    // option: VENDOR-SPEC
    if (iface.supportVendorSpec()) {
	out << "    <vendorSpecList count=\"" << iface.VendorSpec.count() << "\">" << endl;
	iface.VendorSpec.first();
        SPtr<TSrvOptVendorSpec> v;
	while (v = iface.VendorSpec.get()) {
	    out << "      <vendorSpec vendor=\"" << v->getVendor() << "\" length=\"" << v->getVendorDataLen()
	    << "\">" << v->getVendorDataPlain() << "</vendorSpec>" << endl;
	}
	out << "    </vendorSpecList>" << endl;
    } else {
	out << "    <!-- <vendorSpec/> -->" << endl;
    }

    out << "    <!-- " << iface.getExtraOptions().count() << " extra option(s) -->" << endl;

    List(TSrvOptGeneric) extraOpts;
    SPtr<TSrvOptGeneric> gen;
    extraOpts = iface.getExtraOptions();
    extraOpts.first();
    while (gen = extraOpts.get())
    {
	out << "      <extraOption length=\"" << gen->getSize()-4 << "\"/>" << endl;
    }

    // option: FQDN
    if (iface.supportFQDN()) {
      SPtr<TFQDN> f;
      List(TFQDN) * lst = iface.getFQDNLst();
      out << "    <fqdnOptions count=\"" << lst->count() << "\" prefix=\"" << iface.getRevDNSZoneRootLength() << "\">" << endl;
      lst->first();
      while (f=lst->get()) {
	    out << "       " << *f;
      }
      out << "    </fqdnOptions>" << endl;
    } else {
      out << "    <!-- <fqdnOptions/> -->" << endl;
    }


    SPtr<TSrvCfgOptions> ex;
    out << "    <!-- " << iface.ExceptionsLst.count() << " per-client parameters (exceptions) -->" << endl;
    iface.ExceptionsLst.first();
    while (ex = iface.ExceptionsLst.get()) {
	out << *ex;
    }

    out << "  </SrvCfgIface>" << endl;
    return out;
}

void TSrvCfgIface::mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst)
{

	//  Log(Info)<<"Mapping allow, deny list inside interface "<<Name<<LogEnd;
	  SmartPtr<TSrvCfgAddrClass> ptrClass;
	  this->SrvCfgAddrClassLst.first();
	  while(ptrClass=SrvCfgAddrClassLst.get()){
		  ptrClass->mapAllowDenyList(clientClassLst);
	  }


   // Map the Allow and Deny list to TA c
	  SmartPtr<TSrvCfgTA> ptrTA;
	  this->SrvCfgTALst.first();
	  while(ptrTA = SrvCfgTALst.get()){
		  ptrTA->mapAllowDenyList(clientClassLst);
	  }
	  // Map the Allow and Deny list to prefix
	  SmartPtr<TSrvCfgPD> ptrPD;
	  this->SrvCfgPDLst.first();
	  while(ptrPD = SrvCfgPDLst.get()){
		  ptrPD->mapAllowDenyList(clientClassLst);
	  }
}
