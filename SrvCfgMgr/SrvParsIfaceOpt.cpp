/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *    changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.cpp,v 1.18 2008-11-11 22:41:49 thomson Exp $
 *
 */

#include <climits>
#include "SrvParsIfaceOpt.h"
#include "OptAddr.h"
#include "OptString.h"
#include "DHCPDefaults.h"
#include "Logger.h"

TSrvParsIfaceOpt::TSrvParsIfaceOpt(void)
{
    Unicast       = 0;
    Preference    = SERVER_DEFAULT_PREFERENCE;
    RapidCommit   = SERVER_DEFAULT_RAPIDCOMMIT;
    ClntMaxLease  = SERVER_DEFAULT_CLNTMAXLEASE;
    IfaceMaxLease = SERVER_DEFAULT_IFACEMAXLEASE;
    LeaseQuery    = false; // don't support leasequery unless explicitly configured to do so

    // options
    DNSServerSupport  = false;
    DomainSupport     = false;
    NTPServerSupport  = false;
    TimezoneSupport   = false;
    SIPServerSupport  = false;
    SIPDomainSupport  = false;
    FQDNSupport       = false;
    NISServerSupport  = false;
    NISDomainSupport  = false;
    NISPServerSupport = false;
    NISPDomainSupport = false;
    LifetimeSupport   = false;
    VendorSpecSupport = false;

    UnknownFQDN = SERVER_DEFAULT_UNKNOWN_FQDN;
    FQDNDomain = "";

    Relay = false;
    RelayName = "[unknown]";
    RelayID = -1;
    RelayInterfaceID = -1;
}

TSrvParsIfaceOpt::~TSrvParsIfaceOpt(void) {
}

void TSrvParsIfaceOpt::setUnknownFQDN(EUnknownFQDNMode mode, const std::string domain) {
    UnknownFQDN = mode;
    FQDNDomain = domain;
}

/** 
 * returns enum that specifies, how to handle unknown FQDNs
 * 
 * 
 * @return 
 */
EUnknownFQDNMode TSrvParsIfaceOpt::getUnknownFQDN() {
    return UnknownFQDN;
}

std::string TSrvParsIfaceOpt::getFQDNDomain() {
    return FQDNDomain;
}

void TSrvParsIfaceOpt::setLeaseQuerySupport(bool support) {
    this->LeaseQuery = support;
}

bool TSrvParsIfaceOpt::getLeaseQuerySupport() {
    return LeaseQuery;
}

// --- unicast ---
void TSrvParsIfaceOpt::setUnicast(SPtr<TIPv6Addr> addr) {
    this->Unicast = addr;
}

SPtr<TIPv6Addr> TSrvParsIfaceOpt::getUnicast() {
    return this->Unicast;
}

// --- iface-max-lease ---
void TSrvParsIfaceOpt::setIfaceMaxLease(long maxLease) {
    this->IfaceMaxLease=maxLease;
}

long TSrvParsIfaceOpt::getIfaceMaxLease() {
    return this->IfaceMaxLease;
}

// --- clnt max lease ---
void TSrvParsIfaceOpt::setClntMaxLease(long clntMaxLease) {
    this->ClntMaxLease = clntMaxLease;
}

long TSrvParsIfaceOpt::getClntMaxLease() {
    return this->ClntMaxLease;
}

// --- preference ---
void TSrvParsIfaceOpt::setPreference(char pref) {
    this->Preference=pref;
}
char TSrvParsIfaceOpt::getPreference() {
    return this->Preference;
}

// --- rapid commit ---
void TSrvParsIfaceOpt::setRapidCommit(bool rapidComm) {
    this->RapidCommit=rapidComm;
}

bool TSrvParsIfaceOpt::getRapidCommit() {
    return this->RapidCommit;
}

// --- relay related ---
void TSrvParsIfaceOpt::setRelayName(string name) {
    this->Relay      = true;
    this->RelayName  = name;
    this->RelayID    = -1;
}

void TSrvParsIfaceOpt::setRelayID(int id) {
    this->Relay      = true;
    this->RelayName  = "[unknown]";
    this->RelayID    = id;
}

void TSrvParsIfaceOpt::setRelayInterfaceID(SPtr<TSrvOptInterfaceID> id) {
    this->Relay = true;
    this->RelayInterfaceID= id;
}

string TSrvParsIfaceOpt::getRelayName() {
    return this->RelayName;
}

int TSrvParsIfaceOpt::getRelayID() {
    return this->RelayID;
}

SPtr<TSrvOptInterfaceID> TSrvParsIfaceOpt::getRelayInterfaceID() {
    return this->RelayInterfaceID;
}

bool TSrvParsIfaceOpt::isRelay() {
    return this->Relay;
}

// --- option: DNS servers ---
void TSrvParsIfaceOpt::setDNSServerLst(List(TIPv6Addr) *lst) {
    this->DNSServerLst = *lst;
    this->DNSServerSupport = true;
}

List(TIPv6Addr) * TSrvParsIfaceOpt::getDNSServerLst() {
    return &this->DNSServerLst;
}
bool TSrvParsIfaceOpt::supportDNSServer(){
    return this->DNSServerSupport;
}

// --- option: DOMAIN ---
void TSrvParsIfaceOpt::setDomainLst(List(string) * lst) {
    this->DomainLst = *lst;
    this->DomainSupport = true;
}

List(string) * TSrvParsIfaceOpt::getDomainLst() {
    return &this->DomainLst;
}
bool TSrvParsIfaceOpt::supportDomain(){
    return this->DomainSupport;
}

#if 0
// --- option: VENDOR-SPEC INFO ---
void TSrvParsIfaceOpt::setVendorSpec(List(TOptVendorSpecInfo) vendor)
{
    VendorSpec = vendor;
    VendorSpecSupport = true;
}
bool TSrvParsIfaceOpt::supportVendorSpec()
{
    return VendorSpecSupport;
}

List(TOptVendorSpecInfo) TSrvParsIfaceOpt::getVendorSpec()
{
    return VendorSpec;
}
#endif

// --- option: NTP-SERVERS ---
void TSrvParsIfaceOpt::setNTPServerLst(List(TIPv6Addr) * lst) {
    this->NTPServerLst = *lst;
    this->NTPServerSupport = true;
}
List(TIPv6Addr) * TSrvParsIfaceOpt::getNTPServerLst() {
    return &this->NTPServerLst;
}
bool TSrvParsIfaceOpt::supportNTPServer(){
    return this->NTPServerSupport;
}

// --- option: TIMEZONE ---
void TSrvParsIfaceOpt::setTimezone(string timezone) {
    this->Timezone=timezone;
    this->TimezoneSupport = true;
}
string TSrvParsIfaceOpt::getTimezone() {
    return this->Timezone;
}
bool TSrvParsIfaceOpt::supportTimezone(){
    return this->NTPServerSupport;
}

// --- option: SIP server ---
void TSrvParsIfaceOpt::setSIPServerLst(TContainer<SPtr<TIPv6Addr> > *lst) {
    this->SIPServerLst = *lst;
    this->SIPServerSupport = true;
}
List(TIPv6Addr) * TSrvParsIfaceOpt::getSIPServerLst() {
    return &this->SIPServerLst;
}
bool TSrvParsIfaceOpt::supportSIPServer(){
    return this->SIPServerSupport;
}

// --- option: SIP domain ---
List(string) * TSrvParsIfaceOpt::getSIPDomainLst() { 
    return &this->SIPDomainLst;
}
void TSrvParsIfaceOpt::setSIPDomainLst(List(string) * domain) { 
    this->SIPDomainLst = *domain;
    this->SIPDomainSupport = true;
}
bool TSrvParsIfaceOpt::supportSIPDomain() {
    return this->SIPDomainSupport;
}


// --- option: FQDN ---

void TSrvParsIfaceOpt::setFQDNLst(List(TFQDN) *fqdn) {
    this->FQDNLst = *fqdn;
    this->FQDNSupport = true;
}

string TSrvParsIfaceOpt::getFQDNName(SPtr<TDUID> duid) {
    int cpt = 1;
    string res = "";
    SPtr<TFQDN> foo = FQDNLst.getFirst();
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
    return res;
}

string TSrvParsIfaceOpt::getFQDNName(SPtr<TIPv6Addr> addr) {
    int cpt = 1;
    string res = "";
    SPtr<TFQDN> foo = FQDNLst.getFirst();
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

string TSrvParsIfaceOpt::getFQDNName() {
    int cpt = 1;
    string res = "";
    SPtr<TFQDN> foo = FQDNLst.getFirst();
    FQDNLst.first();
    foo = FQDNLst.get();
    Log(Debug)<<"FQDN used ? : "<<(*foo).used<<LogEnd;
    Log(Debug)<<"FQDN Addr ? : "<<*(*foo).Addr<<LogEnd;
    Log(Debug)<<"FQDN Duid ? : "<<*(*foo).Duid<<LogEnd;
    while((!(*(*foo).Addr == *(new TIPv6Addr())) || 
           !(*(*foo).Duid == *(new TDUID())) ||
           (*foo).used == true ) &&
          cpt<FQDNLst.count()){
        foo = FQDNLst.get();
        Log(Debug)<<"FQDN <while>used ? : "<<(*foo).used<<LogEnd;
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

SPtr<TDUID> TSrvParsIfaceOpt::getFQDNDuid(string name) {
    SPtr<TDUID> res = new TDUID();
    return res;
}

List(TFQDN) *TSrvParsIfaceOpt::getFQDNLst() {
    return &this->FQDNLst;
}

int TSrvParsIfaceOpt::getFQDNMode(){
	return this->FQDNMode;
}
void TSrvParsIfaceOpt::setFQDNMode(int FQDNMode){
	this->FQDNMode=FQDNMode;
}
int TSrvParsIfaceOpt::getRevDNSZoneRootLength(){
	return this->revDNSZoneRootLength;
}
void TSrvParsIfaceOpt::setRevDNSZoneRootLength(int revDNSZoneRootLength){
	this->revDNSZoneRootLength = revDNSZoneRootLength;
}

bool TSrvParsIfaceOpt::supportFQDN() {
    return this->FQDNSupport;
}

// --- option: NIS server ---
void TSrvParsIfaceOpt::setNISServerLst(TContainer<SPtr<TIPv6Addr> > *lst) {
    this->NISServerLst     = *lst;
    this->NISServerSupport = true;
}
List(TIPv6Addr) * TSrvParsIfaceOpt::getNISServerLst() {
    return &this->NISServerLst;
}
bool TSrvParsIfaceOpt::supportNISServer(){
    return this->NISServerSupport;
}

// --- option: NIS domain ---
void TSrvParsIfaceOpt::setNISDomain(string domain) { 
    this->NISDomain=domain;
    this->NISDomainSupport=true;
}
string TSrvParsIfaceOpt::getNISDomain() { 
    return this->NISDomain;
}
bool TSrvParsIfaceOpt::supportNISDomain() {
    return this->NISDomainSupport;
}

// --- option: NIS+ server ---
void TSrvParsIfaceOpt::setNISPServerLst(TContainer<SPtr<TIPv6Addr> > *lst) {
    this->NISPServerLst = *lst;
    this->NISPServerSupport = true;
}
List(TIPv6Addr) * TSrvParsIfaceOpt::getNISPServerLst() {
    return &this->NISPServerLst;
}
bool TSrvParsIfaceOpt::supportNISPServer(){
    return this->NISPServerSupport;
}

// --- option: NIS+ domain ---
void TSrvParsIfaceOpt::setNISPDomain(string domain) { 
    this->NISPDomain=domain;
    this->NISPDomainSupport=true;
}
string TSrvParsIfaceOpt::getNISPDomain() { 
    return this->NISPDomain;
}
bool TSrvParsIfaceOpt::supportNISPDomain() {
    return this->NISPDomainSupport;
}

// --- option: LIFETIME ---
void TSrvParsIfaceOpt::setLifetime(unsigned int x) {
    this->Lifetime = x;
    this->LifetimeSupport = true;
}
unsigned int TSrvParsIfaceOpt::getLifetime() {
    return this->Lifetime;
}

bool TSrvParsIfaceOpt::supportLifetime() {
    return this->LifetimeSupport;
}

