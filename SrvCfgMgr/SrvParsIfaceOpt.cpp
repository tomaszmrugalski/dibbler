/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.cpp,v 1.4 2004-09-03 23:20:23 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.2  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 */

#include "SrvParsIfaceOpt.h"
#include "DHCPConst.h"

TSrvParsIfaceOpt::TSrvParsIfaceOpt(void)
{
    this->UniAddress=false;
    this->Address=new TIPv6Addr();
    this->Unicast=false;
    this->Domain        = SERVER_DEFAULT_DOMAIN;
    this->TimeZone      = SERVER_DEFAULT_TIMEZONE;
    this->ClntMaxLease  = SERVER_DEFAULT_CLNTMAXLEASE;
    this->IfaceMaxLease = SERVER_DEFAULT_IFACEMAXLEASE;
    this->Preference    = SERVER_DEFAULT_PREFERENCE;
    this->RapidCommit   = SERVER_DEFAULT_RAPIDCOMMIT;
}

TSrvParsIfaceOpt::~TSrvParsIfaceOpt(void)
{
}

bool TSrvParsIfaceOpt::uniAddress()
{
    return this->UniAddress;
}
void TSrvParsIfaceOpt::setUniAddress(bool isUni)
{
    this->UniAddress=isUni;
}

void TSrvParsIfaceOpt::setAddress(SmartPtr<TIPv6Addr> addr)
{
    this->Address=addr;
}

SmartPtr<TIPv6Addr> TSrvParsIfaceOpt::getAddress()
{
    return this->Address;
}

//DNS - oriented routines    
void TSrvParsIfaceOpt::addDNSSrv(SmartPtr<TIPv6Addr> addr)
{
    this->DNSSrv.append(addr);
}

void TSrvParsIfaceOpt::firstDNSSrv()
{
    this->DNSSrv.first();
}
SmartPtr<TIPv6Addr> TSrvParsIfaceOpt::getDNSSrv()
{
    return this->DNSSrv.get();
}
void TSrvParsIfaceOpt::setDNSSrv(TContainer<SmartPtr<TIPv6Addr> > *dnsSrv)
{
    this->DNSSrv.clear();
    dnsSrv->first();
    SmartPtr<TIPv6Addr> addr;
    while(addr=dnsSrv->get())
        this->DNSSrv.append(addr);
}

void TSrvParsIfaceOpt::setDomain(string domain)
{
    this->Domain=domain;
}
string TSrvParsIfaceOpt::getDomain()
{
    return this->Domain;
}

//NTP - oriented routines
void TSrvParsIfaceOpt::addNTPSrv(SmartPtr<TIPv6Addr> addr)
{
    this->NTPSrv.append(addr);
}

void TSrvParsIfaceOpt::firstNTPSrv()
{
    this->NTPSrv.first();
}
SmartPtr<TIPv6Addr> TSrvParsIfaceOpt::getNTPSrv()
{
    return this->NTPSrv.get();
}

void TSrvParsIfaceOpt::setNTPSrv(TContainer<SmartPtr<TIPv6Addr> > *ntpSrv)
{
    this->NTPSrv.clear();
    ntpSrv->first();
    SmartPtr<TIPv6Addr> addr;
    while(addr=ntpSrv->get())
        this->NTPSrv.append(addr);
}

void TSrvParsIfaceOpt::setTimeZone(string timeZone)
{
    this->TimeZone=timeZone;
}
string TSrvParsIfaceOpt::getTimeZone()
{
    return this->TimeZone;
}

void TSrvParsIfaceOpt::setIfaceMaxLease(long maxLease) {
    this->IfaceMaxLease=maxLease;
}

long TSrvParsIfaceOpt::getIfaceMaxLease() {
    return this->IfaceMaxLease;
}

void TSrvParsIfaceOpt::setClntMaxLease(long clntMaxLease) {
    this->ClntMaxLease = clntMaxLease;
}

long TSrvParsIfaceOpt::getClntMaxLease()
{
    return this->ClntMaxLease;
}

void TSrvParsIfaceOpt::setPreference(char pref)
{
    this->Preference=pref;
}
char TSrvParsIfaceOpt::getPreference()
{
    return this->Preference;
}
void TSrvParsIfaceOpt::setUnicast(bool unicast)
{
    this->Unicast=unicast;
}

bool TSrvParsIfaceOpt::getUnicast()
{
    return this->Unicast;
}

void TSrvParsIfaceOpt::setRapidCommit(bool rapidComm)
{
    this->RapidCommit=rapidComm;
}

bool TSrvParsIfaceOpt::getRapidCommit()
{
    return this->RapidCommit;
}
