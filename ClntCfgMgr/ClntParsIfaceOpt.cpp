/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsIfaceOpt.cpp,v 1.6 2004-11-29 21:21:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/11/01 23:31:24  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.4  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.3  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.2  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#include "ClntParsIfaceOpt.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"

using namespace std;

bool TClntParsIfaceOpt::isNewGroup() 
{ 
    return this->NewGroup;
}

void TClntParsIfaceOpt::setNewGroup(bool newGr) 
{ 
    this->NewGroup=newGr;
}

bool TClntParsIfaceOpt::getIsIAs()
{
    return !this->NoIAs;
}

void TClntParsIfaceOpt::setIsIAs(bool state)
{
    this->NoIAs=!state;
}

void TClntParsIfaceOpt::setUnicast(bool unicast)
{
    this->Unicast = unicast;
}

bool TClntParsIfaceOpt::getUnicast()
{
    return this->Unicast;
}

TClntParsIfaceOpt::TClntParsIfaceOpt() : TClntParsIAOpt()
{
    DNSServerLst.clear();
    DomainLst.clear();
    NTPServerLst.clear();
    Timezone="";
    SIPServerLst.clear();
    SIPDomainLst.clear();
    FQDN    ="";
    NISServerLst.clear();
    NISDomain = "";
    NISPServerLst.clear();
    NISPDomain = "";
    Lifetime = false;
    
    NewGroup=false;
    NoIAs   = false;
    this->Unicast = CLIENT_DEFAULT_UNICAST;
    
    ReqDNSServer  = false;
    ReqDomain     = false;
    ReqNTPServer  = false;
    ReqTimezone   = false;
    ReqSIPServer  = false;
    ReqSIPDomain  = false;
    ReqFQDN       = false;
    ReqNISServer  = false;
    ReqNISPServer = false;
    ReqNISDomain  = false;
    ReqNISPDomain = false;
    ReqLifetime   = false;
}

TClntParsIfaceOpt::~TClntParsIfaceOpt() {
}

// --- option: DNS server ---

List(TIPv6Addr) * TClntParsIfaceOpt::getDNSServerLst() { 
    return &this->DNSServerLst;
}
void TClntParsIfaceOpt::setDNSServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->DNSServerLst = *lst;
    this->ReqDNSServer = true;
}
bool TClntParsIfaceOpt::getReqDNSServer() {
    return this->ReqDNSServer;
}

// --- option: domain ---
List(string) * TClntParsIfaceOpt::getDomainLst() { 
    return &this->DomainLst;
}
void TClntParsIfaceOpt::setDomainLst(List(string) * domain) { 
    this->DomainLst=*domain;
    this->ReqDomain=true;
}
bool TClntParsIfaceOpt::getReqDomain() {
    return this->ReqDomain;
}

// --- option: NTP-SERVERS ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNTPServerLst() {
    return &this->NTPServerLst;
}
void TClntParsIfaceOpt::setNTPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NTPServerLst = *lst;
    this->ReqNTPServer = true;
}
bool TClntParsIfaceOpt::getReqNTPServer(){
    return this->ReqNTPServer;
}

// --- option: Timezone ---
void TClntParsIfaceOpt::setTimezone(string Timezone) { 
    this->Timezone=Timezone;
    this->ReqTimezone=true;
}
bool TClntParsIfaceOpt::getReqTimezone() {
    return this->ReqTimezone;
}
string TClntParsIfaceOpt::getTimezone() { 
    return this->Timezone;
}


// --- option: SIP server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getSIPServerLst() {
    return &this->SIPServerLst;
}
void TClntParsIfaceOpt::setSIPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->SIPServerLst = *lst;
    this->ReqSIPServer = true;
}
bool TClntParsIfaceOpt::getReqSIPServer(){
    return this->ReqSIPServer;
}

// --- option: SIP domain ---
List(string) * TClntParsIfaceOpt::getSIPDomainLst() { 
    return &this->SIPDomainLst;
}
void TClntParsIfaceOpt::setSIPDomainLst(List(string) * domain) { 
    this->SIPDomainLst=*domain;
    this->ReqSIPDomain=true;
}
bool TClntParsIfaceOpt::getReqSIPDomain() {
    return this->ReqSIPDomain;
}

// --- option: FQDN ---
void TClntParsIfaceOpt::setFQDN(string fqdn) { 
    this->FQDN=fqdn;
    this->ReqFQDN=true;
}
bool TClntParsIfaceOpt::getReqFQDN() {
    return this->ReqFQDN;
}
string TClntParsIfaceOpt::getFQDN() { 
    return this->FQDN;
}

// --- option: NIS server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNISServerLst() {
    return &this->NISServerLst;
}
void TClntParsIfaceOpt::setNISServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISServerLst = *lst;
    this->ReqNISServer = true;
}
bool TClntParsIfaceOpt::getReqNISServer(){
    return this->ReqNISServer;
}
// --- option: NIS domain ---
string TClntParsIfaceOpt::getNISDomain() { 
    return this->NISDomain;
}
void TClntParsIfaceOpt::setNISDomain(string domain) { 
    this->NISDomain=domain;
    this->ReqNISDomain=true;
}
bool TClntParsIfaceOpt::getReqNISDomain() {
    return this->ReqNISDomain;
}

// --- option: NIS+ server ---
List(TIPv6Addr) * TClntParsIfaceOpt::getNISPServerLst() {
    return &this->NISPServerLst;
}
void TClntParsIfaceOpt::setNISPServerLst(TContainer<SmartPtr<TIPv6Addr> > *lst) {
    this->NISPServerLst = *lst;
    this->ReqNISPServer = true;
}
bool TClntParsIfaceOpt::getReqNISPServer(){
    return this->ReqNISPServer;
}
// --- option: NIS+ domain ---
string TClntParsIfaceOpt::getNISPDomain() { 
    return this->NISPDomain;
}
void TClntParsIfaceOpt::setNISPDomain(string domain) { 
    this->NISPDomain=domain;
    this->ReqNISPDomain=true;
}
bool TClntParsIfaceOpt::getReqNISPDomain() {
    return this->ReqNISPDomain;
}

// --- option: Lifetime ---

bool TClntParsIfaceOpt::getLifetime() { 
    return this->Lifetime;
}
void TClntParsIfaceOpt::setLifetime() { 
    this->Lifetime = true;
    this->ReqLifetime = true;
}
bool TClntParsIfaceOpt::getReqLifetime() {
    return this->ReqLifetime;
}
