/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.cpp,v 1.8 2006-07-03 18:19:12 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2006-03-03 21:09:34  thomson
 * FQDN support added.
 *
 * Revision 1.6  2005/01/03 21:57:08  thomson
 * Relay support added.
 *
 * Revision 1.5  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.4  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.3  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.2  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 */

#include "SrvParsIfaceOpt.h"
#include "DHCPConst.h"
#include "Logger.h"

TSrvParsIfaceOpt::TSrvParsIfaceOpt(void)
{
    this->Unicast       = 0;
    this->Preference    = SERVER_DEFAULT_PREFERENCE;
    this->RapidCommit   = SERVER_DEFAULT_RAPIDCOMMIT;
    this->ClntMaxLease  = SERVER_DEFAULT_CLNTMAXLEASE;
    this->IfaceMaxLease = SERVER_DEFAULT_IFACEMAXLEASE;
    
    // options
    this->DNSServerSupport  = false;
    this->DomainSupport     = false;
    this->NTPServerSupport  = false;
    this->TimezoneSupport   = false;
    this->SIPServerSupport  = false;
    this->SIPDomainSupport  = false;
    this->FQDNSupport       = false;
    this->NISServerSupport  = false;
    this->NISDomainSupport  = false;
    this->NISPServerSupport = false;
    this->NISPDomainSupport = false;
    this->LifetimeSupport   = false;

    this->Relay = false;
    this->RelayName = "[unknown]";
    this->RelayID = -1;
    this->RelayInterfaceID = -1;
}

TSrvParsIfaceOpt::~TSrvParsIfaceOpt(void) {
}

// --- unicast ---
void TSrvParsIfaceOpt::setUnicast(SmartPtr<TIPv6Addr> addr) {
    this->Unicast = addr;
}

SmartPtr<TIPv6Addr> TSrvParsIfaceOpt::getUnicast() {
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

long TSrvParsIfaceOpt::getClntMaxLease()
{
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

void TSrvParsIfaceOpt::setRelayInterfaceID(int id) {
    this->Relay = true;
    this->RelayInterfaceID= id;
}

string TSrvParsIfaceOpt::getRelayName() {
    return this->RelayName;
}

int TSrvParsIfaceOpt::getRelayID() {
    return this->RelayID;
}

int TSrvParsIfaceOpt::getRelayInterfaceID() {
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
void TSrvParsIfaceOpt::setSIPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
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

string TSrvParsIfaceOpt::getFQDNName(SmartPtr<TDUID> duid) {
    int cpt = 1;
    string res = "";
    SmartPtr<TFQDN> foo = FQDNLst.getFirst();
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

string TSrvParsIfaceOpt::getFQDNName(SmartPtr<TIPv6Addr> addr) {
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

string TSrvParsIfaceOpt::getFQDNName() {
    int cpt = 1;
    string res = "";
    SmartPtr<TFQDN> foo = FQDNLst.getFirst();
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

SmartPtr<TDUID> TSrvParsIfaceOpt::getFQDNDuid(string name) {
    SmartPtr<TDUID> res = new TDUID();
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
bool TSrvParsIfaceOpt::supportFQDN() {
    return this->FQDNSupport;
}


// --- option: NIS server ---
void TSrvParsIfaceOpt::setNISServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
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
void TSrvParsIfaceOpt::setNISPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
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
