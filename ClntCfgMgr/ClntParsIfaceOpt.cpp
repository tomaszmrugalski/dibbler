/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsIfaceOpt.cpp,v 1.2 2004-05-23 22:37:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#include "ClntParsIfaceOpt.h"
#include <iostream>
#include <iomanip>

using namespace std;


TClntParsIfaceOpt::TClntParsIfaceOpt() : TClntParsIAOpt()
{
	DNSSrvSendOpt=Send;
	DNSSrvReqOpt=Request;
	//NISServer="";
	//NISSrvSendOpt=Send;	
	//NISSrvReqOpt=Request;
	NTPSrvSendOpt=Send;
	NTPSrvReqOpt=Request;
	NewGroup=false;
	Domain="";
	DomainReqOpt=Request;
	DomainSendOpt=Send;
	TZone="";
	TimeZoneReqOpt=Request;
	TimeZoneSendOpt=Send;
    
    ReqDNSSrv=false;
    ReqNTPSrv=false;
    ReqDomainName=false;
    ReqTimeZone=false;
    
    NoIAs = false;
}

//-- DNS connected methods --
void TClntParsIfaceOpt::addDNSSrv(SmartPtr<TIPv6Addr> addr) 
{ 
    this->DNSSrv.append(addr);
}

void TClntParsIfaceOpt::firstDNSSrv()
{ 
    this->DNSSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getDNSSrv() 
{ 
    return this->DNSSrv.get();
}

void TClntParsIfaceOpt::setDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->DNSSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->DNSSrv.append(addr);
}

void TClntParsIfaceOpt::addAppDNSSrv(SmartPtr<TIPv6Addr> addr) 
{ 
    this->AppDNSSrv.append(addr);
}

void TClntParsIfaceOpt::firstAppDNSSrv() 
{ 
    this->AppDNSSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getAppDNSSrv() 
{ 
    return this->AppDNSSrv.get();
}

void TClntParsIfaceOpt::setAppDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->AppDNSSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->AppDNSSrv.append(addr);
}

void TClntParsIfaceOpt::addPrepDNSSrv(SmartPtr<TIPv6Addr> addr) 
{ 
    this->PrepDNSSrv.append(addr);
}

void TClntParsIfaceOpt::firstPrepDNSSrv() 
{ 
    this->PrepDNSSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getPrepDNSSrv() 
{ 
    return this->PrepDNSSrv.get();
}

void TClntParsIfaceOpt::setPrepDNSSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->PrepDNSSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->PrepDNSSrv.append(addr);
}

EReqOpt TClntParsIfaceOpt::getDNSSrvReqOpt() 
{ 
    return this->DNSSrvReqOpt;
}

void TClntParsIfaceOpt::setDNSSrvReqOpt(EReqOpt opt) 
{ 
    this->DNSSrvReqOpt=opt;
}

ESendOpt TClntParsIfaceOpt::getDNSSrvSendOpt() 
{ 
    return this->DNSSrvSendOpt;
}

void TClntParsIfaceOpt::setDNSSrvSendOpt(ESendOpt opt) 
{ 
    this->DNSSrvSendOpt=opt;
}

string TClntParsIfaceOpt::getDomain() 
{ 
    return this->Domain;
}

void TClntParsIfaceOpt::setDomain(string domain) 
{ 
    this->Domain=domain;
}

EReqOpt TClntParsIfaceOpt::getDomainReqOpt() 
{ 
    return this->DomainReqOpt;
}

void TClntParsIfaceOpt::setDomainReqOpt(EReqOpt opt) 
{ 
    this->DomainReqOpt=opt;
}

ESendOpt TClntParsIfaceOpt::getDomainSendOpt()
{
    return this->DomainSendOpt;
}
void TClntParsIfaceOpt::setDomainSendOpt(ESendOpt opt) 
{
    this->DomainSendOpt=opt;
}

//-- NTP connected methods --
void TClntParsIfaceOpt::addNTPSrv(SmartPtr<TIPv6Addr> addr) 
{ 
    this->NTPSrv.append(addr);
}

void TClntParsIfaceOpt::firstNTPSrv() 
{ 
    this->NTPSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getNTPSrv() 
{
    return this->NTPSrv.get();
}

void TClntParsIfaceOpt::setNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->NTPSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->NTPSrv.append(addr);
}

void TClntParsIfaceOpt::addAppNTPSrv(SmartPtr<TIPv6Addr> addr) 
{
    this->AppNTPSrv.append(addr);
}

void TClntParsIfaceOpt::firstAppNTPSrv() 
{ 
    this->AppNTPSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getAppNTPSrv() 
{
    return this->AppNTPSrv.get();
}

void TClntParsIfaceOpt::setAppNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->AppNTPSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->AppNTPSrv.append(addr);
}

void TClntParsIfaceOpt::addPrepNTPSrv(SmartPtr<TIPv6Addr> addr) 
{
    this->PrepNTPSrv.append(addr);
}

void TClntParsIfaceOpt::firstPrepNTPSrv() 
{ 
    this->PrepNTPSrv.first();
}

SmartPtr<TIPv6Addr> TClntParsIfaceOpt::getPrepNTPSrv() 
{
    return this->PrepNTPSrv.get();
}

void TClntParsIfaceOpt::setPrepNTPSrvLst(TContainer<SmartPtr<TIPv6Addr> > *lst)
{
    SmartPtr<TIPv6Addr> addr;
    this->PrepNTPSrv.clear();
    lst->first();
    while(addr=lst->get())
        this->PrepNTPSrv.append(addr);
}

EReqOpt TClntParsIfaceOpt::getNTPSrvReqOpt() 
{ 
    return this->NTPSrvReqOpt;
}

void TClntParsIfaceOpt::setNTPSrvReqOpt(EReqOpt opt) 
{ 
    this->NTPSrvReqOpt=opt;
}

ESendOpt TClntParsIfaceOpt::getNTPSrvSendOpt() 
{ 
    return this->NTPSrvSendOpt;
}

void TClntParsIfaceOpt::setNTPSrvSendOpt(ESendOpt opt) 
{ 
    this->NTPSrvSendOpt=opt;
}

string TClntParsIfaceOpt::getTimeZone() 
{ 
    return this->TZone;
}

void TClntParsIfaceOpt::setTimeZone(string TimeZone) 
{ 
    this->TZone=TimeZone;
}

ESendOpt TClntParsIfaceOpt::getTimeZoneSendOpt() 
{ 
    return this->TimeZoneSendOpt;
}
void TClntParsIfaceOpt::setTimeZoneSendOpt(ESendOpt opt) 
{ 
    this->TimeZoneSendOpt=opt;
}

EReqOpt TClntParsIfaceOpt::getTimeZoneReqOpt() 
{ 
    return this->TimeZoneReqOpt;
}
void TClntParsIfaceOpt::setTimeZoneReqOpt(EReqOpt opt) 
{ 
    this->TimeZoneReqOpt=opt;
}

bool TClntParsIfaceOpt::isNewGroup() 
{ 
    return this->NewGroup;
}

void TClntParsIfaceOpt::setNewGroup(bool newGr) 
{ 
    this->NewGroup=newGr;
}

bool TClntParsIfaceOpt::getReqDNSSrv()
{
    return this->ReqDNSSrv;
}

bool TClntParsIfaceOpt::getReqNTPSrv()
{
    return this->ReqNTPSrv;
}

bool TClntParsIfaceOpt::getReqDomainName()
{
    return this->ReqDomainName;
}

bool TClntParsIfaceOpt::getReqTimeZone()
{
    return this->ReqTimeZone;
}

void TClntParsIfaceOpt::setReqDNSSrv(bool req)
{
    this->ReqDNSSrv=req;
}

void TClntParsIfaceOpt::setReqNTPSrv(bool req)
{
    this->ReqNTPSrv=req;
}

void TClntParsIfaceOpt::setReqDomainName(bool req)
{
    this->ReqDomainName=req;
}

void TClntParsIfaceOpt::setReqTimeZone(bool req)
{
    this->ReqTimeZone=req;
}

bool TClntParsIfaceOpt::getIsIAs()
{
    return !this->NoIAs;
}

void TClntParsIfaceOpt::setIsIAs(bool state)
{
    this->NoIAs=!state;
}
