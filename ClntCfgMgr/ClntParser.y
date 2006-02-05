%name clntParser

%header{
#include <iostream>
#include <string>
#include <malloc.h>
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "ClntParser.h"
#include "ClntParsGlobalOpt.h"
#include "ClntCfgIface.h"
#include "ClntCfgAddr.h"
#include "ClntCfgIA.h"
#include "ClntCfgTA.h"
#include "ClntCfgGroup.h"
#include "Logger.h"

    using namespace std;
    
#define YY_USE_CLASS
%}

%{
#include "FlexLexer.h"
%}
// --- CLASS MEMBERS ---

%define MEMBERS yyFlexLexer * lex;                                          \
/*List of options in scope stack,the most fresh is last in the list*/       \
TContainer<SmartPtr<TClntParsGlobalOpt> > ParserOptStack;                   \
/*List of parsed interfaces/IAs/Addresses, last */                          \
/*interface/IA/address is just being parsing or have been just parsed*/     \
TContainer<SmartPtr<TClntCfgIface> > ClntCfgIfaceLst;                       \
TContainer<SmartPtr<TClntCfgGroup> > ClntCfgGroupLst;                       \
TContainer<SmartPtr<TClntCfgIA> >    ClntCfgIALst;                          \
TContainer<SmartPtr<TClntCfgTA> >    ClntCfgTALst;                          \
TContainer<SmartPtr<TClntCfgAddr> >  ClntCfgAddrLst;                        \
/*Pointer to list which should contain either rejected servers or */        \
/*preffered servers*/                                                       \
TContainer<SmartPtr<TStationID> > PresentStationLst;                        \
TContainer<SmartPtr<TIPv6Addr> > PresentAddrLst;                            \
TContainer<SmartPtr<string> > PresentStringLst;                             \
/*method check whether interface with id=ifaceNr has been */                \
/*already declared */                                                       \
bool CheckIsIface(int ifaceNr);                                             \
/* method check if interface with id=ifaceName has been already declared */ \
bool CheckIsIface(string ifaceName);                                        \
void StartIfaceDeclaration();                                               \
bool EndIfaceDeclaration();                                                 \
void EmptyIface();                                                          \
void StartIADeclaration(bool aggregation);                                  \
void EndIADeclaration();                                                    \
void EmptyIA();                                                             \
void EmptyAddr();                                                           \
bool iaidSet;                                                               \
unsigned int iaid;                                                          \
virtual ~clntParser();

%define CONSTRUCTOR_PARAM yyFlexLexer * lex
%define CONSTRUCTOR_CODE                                                    \
    this->lex = lex;                                                        \
    ParserOptStack.append(new TClntParsGlobalOpt());                        \
    ParserOptStack.getFirst()->setIAIDCnt(1);                               \
    ParserOptStack.getLast();

%union    
{
    int ival;    
    char *strval;  
    struct SDuid  {      
        int length;    
        char* duid;  
    } duidval;  
    char addrval[16];  
}

%{
namespace std
{
    extern yy_clntParser_stype yylval;
}
%}

%token T1_,T2_,PREF_TIME_,DNS_SERVER_,VALID_TIME_, UNICAST_
%token NTP_SERVER_, DOMAIN_, TIME_ZONE_, SIP_SERVER_, SIP_DOMAIN_
%token NIS_SERVER_, NISP_SERVER_, NIS_DOMAIN_, NISP_DOMAIN_, FQDN_
%token LIFETIME_
%token IFACE_,NO_CONFIG_,REJECT_SERVERS_,PREFERRED_SERVERS_
%token IA_,TA_,IAID_,ADDRES_,IPV6ADDR_,WORKDIR_, RAPID_COMMIT_,STATELESS_
%token OPTION_
%token LOGNAME_, LOGLEVEL_, LOGMODE_
%token <strval>     STRING_
%token <ival>       HEXNUMBER_
%token <ival>       INTNUMBER_
%token <addrval>    IPV6ADDR_
%token <duidval>    DUID_
    
%type  <ival> Number

%%

/////////////////////////////////////////////////////////////////////////////
// rules section
/////////////////////////////////////////////////////////////////////////////

Grammar
: GlobalDeclarationList
|
;

GlobalDeclarationList
    : GlobalOptionDeclaration
    | InterfaceDeclaration
    | GlobalDeclarationList GlobalOptionDeclaration
    | GlobalDeclarationList InterfaceDeclaration
    ;

InterfaceDeclaration
/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
:IFACE_ STRING_ '{' 
{
    CheckIsIface(string($2)); //If no - everything is ok
    StartIfaceDeclaration();
}
InterfaceDeclarationsList '}'
{
    //Information about new interface has been read
    //Add it to list of read interfaces
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    //FIXME:used of char * should be always realeased
    delete [] $2;
    EndIfaceDeclaration();
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number '{' 
{
    CheckIsIface($2);   //If no - everything is ok
    StartIfaceDeclaration();
}
InterfaceDeclarationsList '}'
{
    ClntCfgIfaceLst.append(new TClntCfgIface($2) );
    if (!EndIfaceDeclaration())
	YYABORT;
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' { }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ STRING_ '{' '}' 
{
    CheckIsIface(string($2));
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    delete [] $2;
    EmptyIface();
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 { }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number '{' '}'
{
    CheckIsIface($2);
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    EmptyIface();
}
    
/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' no-config
/////////////////////////////////////////////////////////////////////////////
|IFACE_ STRING_ NO_CONFIG_
{
    CheckIsIface(string($2));
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->setNoConfig();
    delete $2;
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 no-config
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number NO_CONFIG_
{
    CheckIsIface($2);
    ClntCfgIfaceLst.append(SmartPtr<TClntCfgIface> (new TClntCfgIface($2)) );
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->setNoConfig();
}
;

////////////////////////////////////////////////////////////////////////////
//Interface specific declarations //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
InterfaceDeclarationsList
: InterfaceOptionDeclaration
| InterfaceDeclarationsList InterfaceOptionDeclaration
| IADeclaration
| InterfaceDeclarationsList IADeclaration 
| TADeclaration
| InterfaceDeclarationsList TADeclaration
;

TADeclaration
/////////////////////////////////////////////////////////////////////////////
// TA options
/////////////////////////////////////////////////////////////////////////////
:TA_
{ 
    //Log(Crit) << "TA without params." << LogEnd;
    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
}
|TA_ '{' 
{
    //Log(Crit) << "TA with params started." << LogEnd;
    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
    this->iaidSet = false;
}
TADeclarationList
{
    if (this->iaidSet)
	this->ClntCfgTALst.getLast()->setIAID(this->iaid);
    //Log(Crit) << "TA with params ended." << LogEnd;
}
'}'
|TA_ '{' '}'
{
    //Log(Crit) << "TA without params." << LogEnd;
    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
}
;

TADeclarationList
:TADeclarationList IAID
|IAID
;

IAID
:IAID_ Number
{
    this->iaidSet = true;
    this->iaid = $2;
    Log(Crit) << "IAID=" << this->iaid << " parsed." << LogEnd;
}
;

IADeclaration
/////////////////////////////////////////////////////////////////////////////
// ia { options-inside }
/////////////////////////////////////////////////////////////////////////////
:IA_ '{'   
{ 
    StartIADeclaration(false);
}
IADeclarationList '}'
{
    EndIADeclaration();
}

/////////////////////////////////////////////////////////////////////////////
// ia { }
/////////////////////////////////////////////////////////////////////////////
|IA_ '{' '}'
{
    StartIADeclaration(true);
    EndIADeclaration();
}

/////////////////////////////////////////////////////////////////////////////
// ia (1 address is added by default)
/////////////////////////////////////////////////////////////////////////////
|IA_
{
    StartIADeclaration(true);
    EndIADeclaration();
}
;

IADeclarationList
:IAOptionDeclaration
|IADeclarationList IAOptionDeclaration
|ADDRESDeclaration 
|IADeclarationList ADDRESDeclaration
;

ADDRESDeclaration
: ADDRES_ '{' 
{
    SmartPtr<TClntParsGlobalOpt> globalOpt = ParserOptStack.getLast();
    SmartPtr<TClntParsGlobalOpt> newOpt = new TClntParsGlobalOpt(*globalOpt);
    ParserOptStack.append(newOpt);
}
ADDRESDeclarationList '}'
{
    //ClntCfgAddrLst.append(SmartPtr<TClntCfgAddr> (new TClntCfgAddr()));
    //set proper options specific for this Address
    //ClntCfgAddrLst.getLast()->setOptions(&(*ParserOptStack.getLast()));
    if (ParserOptStack.count())
	ParserOptStack.delLast();
}
//In this agregated declaration no address hints are allowed
|ADDRES_ Number '{' 
{
    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ParserOptStack.getLast()->setAddrHint(false);
}
ADDRESDeclarationList '}'
{
    for (int i=0;i<$2; i++) EmptyAddr();
    ParserOptStack.delLast();
}

|ADDRES_ Number '{' '}'
{
    for (int i=0;i<$2; i++) EmptyAddr();
}

|ADDRES_ '{' '}'
{
    EmptyAddr();
}

|ADDRES_ Number
{
    for (int i=0;i<$2; i++) EmptyAddr();
}

|ADDRES_ 
{
    EmptyAddr();
}
;

ADDRESDeclarationList
:  ADDRESOptionDeclaration 
|  ADDRESDeclarationList ADDRESOptionDeclaration
|  IPV6ADDR_  
{
    if (ParserOptStack.getLast()->getAddrHint())
    {
        ClntCfgAddrLst.append(new TClntCfgAddr(new TIPv6Addr($1)));
        ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
    }
    else
	//here is agregated version of IA
	YYABORT; 
}
|  ADDRESDeclarationList IPV6ADDR_
{
    if (ParserOptStack.getLast()->getAddrHint())
    {
	ClntCfgAddrLst.append(new TClntCfgAddr(new TIPv6Addr($2)));
	ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
    }
    else
	//here is agregated version of IA
	YYABORT;
}
;

GlobalOptionDeclaration
: InterfaceOptionDeclaration
| LogModeOption
| LogNameOption
| LogLevelOption
| WorkDirOption
;

InterfaceOptionDeclaration
: IAOptionDeclaration 
| NoIAsOptions
| UnicastOption
| DNSServerOption
| DomainOption
| NTPServerOption
| TimeZoneOption
| SIPServerOption
| SIPDomainOption
| FQDNOption
| NISServerOption
| NISPServerOption
| NISDomainOption
| NISPDomainOption
| LifetimeOption
;

IAOptionDeclaration
: T1Option
| T2Option
| RejectServersOption
| PreferServersOption
| RapidCommitOption
| ADDRESOptionDeclaration
;

ADDRESOptionDeclaration
: PreferredTimeOption
| ValidTimeOption
;

LogLevelOption
: LOGLEVEL_ Number 
{
    logger::setLogLevel($2);
}
;

LogModeOption
: LOGMODE_ STRING_ {
    logger::setLogMode($2);
}

LogNameOption
: LOGNAME_ STRING_ {
    logger::setLogName($2);
}

NoIAsOptions
:   STATELESS_
{
    ParserOptStack.getLast()->setIsIAs(false);
}   
;

WorkDirOption
:   WORKDIR_ STRING_
{
    ParserOptStack.getLast()->setWorkDir($2);
}
;

RejectServersOption
:REJECT_SERVERS_ 
{
    //ParserOptStack.getLast()->clearRejedSrv();
    PresentStationLst.clear();
    if (!(ParserOptStack.getLast()->isNewGroup())) 
	ParserOptStack.getLast()->setNewGroup(true);
} ADDRESDUIDList
{
    ParserOptStack.getLast()->setRejedSrvLst(&PresentStationLst);
}
;  

PreferServersOption
:PREFERRED_SERVERS_ 
{
    PresentStationLst.clear();
    if (!(ParserOptStack.getLast()->isNewGroup())) 
	ParserOptStack.getLast()->setNewGroup(true);
} ADDRESDUIDList{
    ParserOptStack.getLast()->setPrefSrvLst(&PresentStationLst);
}
;


PreferredTimeOption
:PREF_TIME_ Number 
{
    ParserOptStack.getLast()->setPref($2);
}
;

RapidCommitOption
:   RAPID_COMMIT_ Number 
{ 
    if ((!(ParserOptStack.getLast()->isNewGroup()))&&
	(ParserOptStack.getLast()->getRapidCommit()!=(bool)$2))
	ParserOptStack.getLast()->setNewGroup(true);
    ParserOptStack.getLast()->setRapidCommit($2);
}
;

ValidTimeOption
:VALID_TIME_ Number
{
    ParserOptStack.getLast()->setValid($2);
}
;

T1Option
:T1_ Number
{
    ParserOptStack.getLast()->setT1($2);
}
;

T2Option
:T2_ Number
{
    ParserOptStack.getLast()->setT2($2);
}
;

UnicastOption
:UNICAST_ Number
{
    switch($2) {
    case 0:
	ParserOptStack.getLast()->setUnicast(false);
	break;
    case 1:
	ParserOptStack.getLast()->setUnicast(true);
	break;
    default:
	Log(Error) << "Invalid parameter (" << $2 << ") passed to unicast in line " 
		   << lex->YYText() << "." << LogEnd;
	return 1;
    }
}
;

ADDRESDUIDList
: IPV6ADDR_   
{
    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TIPv6Addr($1))));
}
| DUID_
{
    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TDUID($1.duid,$1.length))));
}
| ADDRESDUIDList ',' IPV6ADDR_
{
    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TIPv6Addr($3))));
}
| ADDRESDUIDList ',' DUID_
{
    PresentStationLst.append(SmartPtr<TStationID> (new TStationID( new TDUID($3.duid,$3.length))));
}
;

ADDRESSList
: IPV6ADDR_   {PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr($1)));}
| ADDRESSList ',' IPV6ADDR_   {PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr($3)));}
;

StringList
: STRING_ { PresentStringLst.append(SmartPtr<string> (new string($1))); }
| StringList ',' STRING_ { PresentStringLst.append(SmartPtr<string> (new string($3))); }

Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;

//////////////////////////////////////////////////////////////////////
//DNS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DNSServerOption
:OPTION_ DNS_SERVER_ 
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setDNSServerLst(&PresentAddrLst);
}
|OPTION_ DNS_SERVER_ 
{
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setDNSServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//DOMAIN option///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DomainOption
:OPTION_ DOMAIN_
{
    PresentStringLst.clear();
    ParserOptStack.getLast()->setDomainLst(&PresentStringLst);
}
|OPTION_ DOMAIN_ { 
    PresentStringLst.clear();
} StringList
{
    ParserOptStack.getLast()->setDomainLst(&PresentStringLst);
}
;

//////////////////////////////////////////////////////////////////////
//NTP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NTPServerOption
:OPTION_ NTP_SERVER_ 
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNTPServerLst(&PresentAddrLst);
}
|OPTION_ NTP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNTPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//TIMEZONE option/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TimeZoneOption
: OPTION_ TIME_ZONE_ 
{
    ParserOptStack.getLast()->setTimezone(string(""));
  }
|OPTION_ TIME_ZONE_ STRING_
{
    ParserOptStack.getLast()->setTimezone($3);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPServerOption
:OPTION_ SIP_SERVER_ 
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setSIPServerLst(&PresentAddrLst);
}
|OPTION_ SIP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setSIPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPDomainOption
:OPTION_ SIP_DOMAIN_
{
    PresentStringLst.clear();
    ParserOptStack.getLast()->setSIPDomainLst(&PresentStringLst);
}
|OPTION_ SIP_DOMAIN_ {
    PresentStringLst.clear();
} StringList
{
    ParserOptStack.getLast()->setSIPDomainLst(&PresentStringLst);
}
;

//////////////////////////////////////////////////////////////////////
//FQDN option/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
FQDNOption
:OPTION_ FQDN_
{
    ParserOptStack.getLast()->setFQDN(string(""));
}
|OPTION_ FQDN_ STRING_
{
    ParserOptStack.getLast()->setFQDN($3);
}
;

//////////////////////////////////////////////////////////////////////
//NIS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISServerOption
:OPTION_ NIS_SERVER_ 
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNISServerLst(&PresentAddrLst);
}
|OPTION_ NIS_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNISServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-SERVER option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPServerOption
: OPTION_ NISP_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNISPServerLst(&PresentAddrLst);
}
| OPTION_ NISP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNISPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NIS-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISDomainOption
:OPTION_ NIS_DOMAIN_
{
    ParserOptStack.getLast()->setNISDomain("");
}
|OPTION_ NIS_DOMAIN_ STRING_
{
    ParserOptStack.getLast()->setNISDomain($3);
}
;


//////////////////////////////////////////////////////////////////////
//NISP-DOMAIN option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPDomainOption
:OPTION_ NISP_DOMAIN_
{
    ParserOptStack.getLast()->setNISPDomain("");
}
|OPTION_ NISP_DOMAIN_ STRING_ 
{
    ParserOptStack.getLast()->setNISPDomain($3);
}
;

//////////////////////////////////////////////////////////////////////
//LIFETIME option/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
LifetimeOption
:OPTION_ LIFETIME_
{
    ParserOptStack.getLast()->setLifetime();
}
;

%%

/////////////////////////////////////////////////////////////////////////////
// programs section
/////////////////////////////////////////////////////////////////////////////

//
/** 
 * method check whether interface with id=ifaceNr has been 
 * already declared. YYABORT macro is called to abort parsing.
 * 
 * @param ifaceNr 
 * 
 * @return true if not declared.
 */bool clntParser::CheckIsIface(int ifaceNr)
{
  SmartPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) YYABORT;
  return true;
}
    
//method check whether interface with id=ifaceName has been
//already declared 
bool clntParser::CheckIsIface(string ifaceName)
{
  SmartPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) YYABORT;
  };
  return true;
}

//method creates new scope appropriately for interface options and declarations
//clears all lists except the list of interfaces and adds new group
void clntParser::StartIfaceDeclaration()
{
  //Interface scope, so parameters associated with global scope are pushed on stack
  ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
  ParserOptStack.getLast()->setNewGroup(false);
  ClntCfgGroupLst.clear();
  ClntCfgIALst.clear();
  ClntCfgAddrLst.clear();
  //creation of default, new group
  ClntCfgGroupLst.append(new TClntCfgGroup());
  ClntCfgGroupLst.getLast()->setOptions(ParserOptStack.getLast());

}

bool clntParser::EndIfaceDeclaration()
{
    SmartPtr<TClntCfgIface> iface = ClntCfgIfaceLst.getLast();
    if (!iface) {
	Log(Crit) << "Internal error: Interface not found. Something is wrong. Very wrong." << LogEnd;
	YYABORT;
    }

    //set interface options on the basis of just read information
    iface->setOptions(ParserOptStack.getLast());

    if ( (iface->stateless()) && (ClntCfgIALst.count()) ) {
	Log(Crit) << "Interface " << iface->getID() << " is configured stateless, "
	    " but has " << ClntCfgIALst.count() << " IA(s) defined." << LogEnd;
	    YYABORT;
    }

#if 0    
    if ( (!iface->stateless()) && (!ClntCfgIALst.count()) ) {
	Log(Debug) << "IAs not defined on this interface. Assuming 1 IA." << LogEnd;
	EmptyIface();
	return true;
    }
#endif

#if 0
    // add all IAs to the group
    SmartPtr<TClntCfgIA> ia;
    ClntCfgIALst.first();
    Log(Debug) << "### " << ClntCfgIALst.count() << " IA(s) to copy" << LogEnd;
    while (ia=ClntCfgIALst.get()) {
	ClntCfgGroupLst.getLast()->addIA(ia);
	Log(Debug) << "### Adding IA with iaid=" << ia->getIAID() << LogEnd;
    }
#endif

    // add all non-empty groups
    Log(Debug) << "### " << ClntCfgGroupLst.count() << " group(s) to copy" << LogEnd;
    SmartPtr<TClntCfgGroup> group;
    ClntCfgGroupLst.first();
    while (group=ClntCfgGroupLst.get()) {
	Log(Debug) << "### group->countIA()=" << group->countIA() << LogEnd;
	if (group->countIA()) 
	    ClntCfgIfaceLst.getLast()->addGroup(group);
    }
    
    //add all TAs to the interface
    SmartPtr<TClntCfgTA> ptrTA;
    ClntCfgTALst.first();
    while ( ptrTA = ClntCfgTALst.get() ) {
	iface->addTA(ptrTA);
    }

    //restore global options
    ParserOptStack.delLast();
    ClntCfgIALst.clear();
    ClntCfgGroupLst.clear();
    return true;
}   

void clntParser::EmptyIface()
{
    //set iface options on the basis of recent information
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    //add one IA with one address to this iface
    ClntCfgIfaceLst.getLast()->addGroup(new TClntCfgGroup());
    EmptyIA();
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->getLastGroup()->
    addIA(ClntCfgIALst.getLast());
}

/** 
 * method creates new scope appropriately for interface options and declarations
 * clears list of addresses
 * 
 * @param aggregation - does this IA contains suboptions ( ia { ... } )
 */void clntParser::StartIADeclaration(bool aggregation)
{
  ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
  ParserOptStack.getLast()->setNewGroup(false);
  ParserOptStack.getLast()->setAddrHint(!aggregation);
  ClntCfgAddrLst.clear();
}

/** 
 * Inbelivable piece of crap code. If you read this, rewrite this code immediately.
 * 
 */void clntParser::EndIADeclaration()
{
    if(!ClntCfgAddrLst.count()) {
	EmptyIA();
    } else {
	SmartPtr<TClntCfgIA> ia = new TClntCfgIA();
	ClntCfgIALst.append(ia);
	SmartPtr<TClntCfgAddr> ptr;
	ClntCfgAddrLst.first();
	while(ptr=ClntCfgAddrLst.get())
	    ia->addAddr(ptr);
    }

    //set proper options specific for this IA
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());

    Log(Debug) << "### IA with iaid=" << ClntCfgIALst.getLast()->getIAID() << " parsed." << LogEnd;

    // FIXME: remove groups completly
    ClntCfgGroupLst.append(new TClntCfgGroup());
    ClntCfgGroupLst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgGroupLst.getLast()->addIA(ClntCfgIALst.getLast());
    ClntCfgAddrLst.clear();
    ParserOptStack.delLast();

    //this IA matches with previous ones and can be grouped with them
    //so it's should be left on the list and be appended with them to present list
}

/** 
 * method adds 1 IA object (containing 1 address) to the ClntCfgIA list.
 * Both objects' properties are set to last parsed values
 * 
 */void clntParser::EmptyIA()
{
    EmptyAddr();
    ClntCfgIALst.append(new TClntCfgIA());
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIALst.getLast()->addAddr(ClntCfgAddrLst.getLast());
}   

/** 
 * method adds empty address to the ClntCfgAddrList list and sets
 * its properties to last parsed values
 * 
 */
void clntParser::EmptyAddr()
{
    ClntCfgAddrLst.append(new TClntCfgAddr());
    ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
}   


int clntParser::yylex()
{
    memset(&std::yylval,0, sizeof(std::yylval));
    memset(&this->yylval,0, sizeof(this->yylval));
    int x = this->lex->yylex();
    this->yylval=std::yylval;
    return x;
}

/** 
 * This method is called when parsing error is detected.
 * 
 * @param m - first invalid character
 */
void clntParser::yyerror(char *m)
{
    Log(Crit) << "Config parse error: line " << lex->lineno() 
	      << ", unexpected [" << lex->YYText() << "] token." << LogEnd;
}

/** 
 * Desctructor. Just cleans things up
 * 
 */clntParser::~clntParser() {
    this->ClntCfgIfaceLst.clear();
    this->ClntCfgGroupLst.clear();
    this->ClntCfgIALst.clear();
    this->ClntCfgTALst.clear();
    this->ClntCfgAddrLst.clear();
}
