%name SrvParser

%header{
#include <iostream>
#include <string>
#include <malloc.h>
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "SrvParser.h"
#include "SrvParsGlobalOpt.h"
#include "SrvParsClassOpt.h"
#include "SrvParsIfaceOpt.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "DUID.h"
#include "Logger.h"

#define YY_USE_CLASS
%}
%{
#include <FlexLexer.h>


%}

// class definition
%define MEMBERS FlexLexer * lex;                                               \
List(TSrvParsGlobalOpt) ParserOptStack;                                        \
/* List of parsed interfaces/IAs/Addresses, last    */                         \
/* interface/IA/address is just being parsing or have been just parsed */      \
/* FIXME:Don't forget to clear this lists in apropriate moment         */      \
List(TSrvCfgIface)          SrvCfgIfaceLst;                                    \
List(TSrvCfgAddrClass)  SrvCfgAddrClassLst;                                    \
/*Pointer to list which should contain either DNS servers or NTPServers*/      \
List(TIPv6Addr) PresentLst;                                                    \
/*Pointer to list which should contain: rejected clients, accepted clients */  \
/*or addressess ranges */                                                      \
List(TStationRange) PresentRangeLst;                                           \
/*method check whether interface with id=ifaceNr has been already declared */  \
bool CheckIsIface(int ifaceNr);                                                \
/*method check whether interface with id=ifaceName has been already declared*/ \
bool CheckIsIface(string ifaceName); \
void StartIfaceDeclaration(); \
bool EndIfaceDeclaration(); \
void StartClassDeclaration(); \
bool EndClassDeclaration(); \
virtual ~SrvParser();

// constructor
// <Linux>
%define CONSTRUCTOR_PARAM yyFlexLexer * lex
%define CONSTRUCTOR_CODE \
    ParserOptStack.append(new TSrvParsGlobalOpt()); \
    ParserOptStack.getLast()->setUnicast(false);    \
   this->lex = lex;
// </Linux>

%union    
{
    unsigned int ival;
  char                          *strval;
  struct                        SDuid
  {
    int                       length;
    char*                       duid;
    }                                   duidval;
  char                          addrval[16];
}

%token IFACE_, NO_CONFIG_,CLASS_, 
%token LOGNAME_, LOGLEVEL_, LOGMODE_, WORKDIR_
%token OPTION_, NTP_SERVER_,TIME_ZONE_,DNS_SERVER_,DOMAIN_
%token ACCEPT_ONLY_,REJECT_CLIENTS_,POOL_
%token T1_,T2_,PREF_TIME_,VALID_TIME_
%token UNICAST_,PREFERENCE_,RAPID_COMMIT_
%token IFACE_MAX_LEASE_, CLASS_MAX_LEASE_, CLNT_MAX_LEASE_

%token <strval>     STRING_
%token <ival>       HEXNUMBER_
%token <ival>       INTNUMBER_
%token <addrval>    IPV6ADDR_
%token <duidval>    DUID_
    
%type  <ival>       Number

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
//Deklaracja typu: iface 'eth0' { T1 10 T2 20 ... }
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
    SrvCfgIfaceLst.append(new TSrvCfgIface($2));
    //FIXME:used of char * should be always realeased
    delete [] $2;
    EndIfaceDeclaration();
}
/////////////////////////////////////////////////////////////////////////////
//Deklaracja typu: iface 5 { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number '{' 
{
    CheckIsIface($2);   //If no - everything is ok
    StartIfaceDeclaration();
}
InterfaceDeclarationsList '}'
{
    SrvCfgIfaceLst.append(new TSrvCfgIface($2));
    EndIfaceDeclaration();
}

InterfaceDeclarationsList
: InterfaceOptionDeclaration
| InterfaceDeclarationsList InterfaceOptionDeclaration
| ClassDeclaration
| InterfaceDeclarationsList ClassDeclaration
;


ClassDeclaration:
/////////////////////////////////////////////////////////////////////////////
//Deklaracja typu: CLASS { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
CLASS_ '{'
{ 
    StartClassDeclaration();
}
ClassOptionDeclarationsList '}'
{
    EndClassDeclaration();
}
;

ClassOptionDeclarationsList
: ClassOptionDeclaration
| ClassOptionDeclarationsList ClassOptionDeclaration
;

/////////////////////////////////////////////////////////////////////////////
// Now Options and their parameters
/////////////////////////////////////////////////////////////////////////////
Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;


/*ADDRESDUIDList
  : IPV6ADDR_   
  {
  PresentLst.append(SmartPtr<TStationID> (new TStationID($1)));
  }
  | DUID_       
  {
  PresentLst.append(SmartPtr<TStationID> (new TStationID($1.duid,$1.length))); 
  delete $1.duid;
  }
  | ADDRESDUIDList ',' IPV6ADDR_    
  {
  PresentLst.append(SmartPtr<TStationID> (new TStationID($3)));
  }
  | ADDRESDUIDList ',' DUID_        
  {
  PresentLst.append(SmartPtr<TStationID> (new TStationID($3.duid,$3.length)));
  delete $3.duid;
  }
  ;*/   

ADDRESSList
  : IPV6ADDR_   
  {
    PresentLst.append(new TIPv6Addr($1));
  }
  | ADDRESSList ',' IPV6ADDR_   
  {
    PresentLst.append(new TIPv6Addr($3));
  }
  ;

ADDRESSRangeList
    : IPV6ADDR_     
    {
        PresentRangeLst.append(new TStationRange(new TIPv6Addr($1),new TIPv6Addr($1)));
    }
    |   IPV6ADDR_ '-' IPV6ADDR_ 
    {
        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    }
    | ADDRESSRangeList ',' IPV6ADDR_ 
    {
        PresentRangeLst.append(new TStationRange(new TIPv6Addr($3),new TIPv6Addr($3)));
    }
    |   ADDRESSRangeList ',' IPV6ADDR_ '-' IPV6ADDR_ 
    {
        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    }
;

ADDRESSDUIDRangeList
: IPV6ADDR_     
{
    PresentRangeLst.append(new TStationRange(new TIPv6Addr($1),new TIPv6Addr($1)));
}
|   IPV6ADDR_ '-' IPV6ADDR_ 
{
    SmartPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
    SmartPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
    if (*addr1<=*addr2)
	PresentRangeLst.append(new TStationRange(addr1,addr2));
    else
	PresentRangeLst.append(new TStationRange(addr2,addr1));
}
| ADDRESSDUIDRangeList ',' IPV6ADDR_ 
{
    PresentRangeLst.append(new TStationRange(new TIPv6Addr($3),new TIPv6Addr($3)));
}
|   ADDRESSDUIDRangeList ',' IPV6ADDR_ '-' IPV6ADDR_ 
{
    SmartPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
    SmartPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
    if (*addr1<=*addr2)
	PresentRangeLst.append(new TStationRange(addr1,addr2));
    else
	PresentRangeLst.append(new TStationRange(addr2,addr1));
}
    
| DUID_ 
{
    PresentRangeLst.append(new TStationRange(new TDUID($1.duid,$1.length)));
    delete $1.duid;
}
| DUID_ '-' DUID_ 
{   
    SmartPtr<TDUID> duid1(new TDUID($1.duid,$1.length));
    SmartPtr<TDUID> duid2(new TDUID($3.duid,$3.length));
    
    if (*duid1<=*duid2)
	PresentRangeLst.append(new TStationRange(duid1,duid2));
    else
	PresentRangeLst.append(new TStationRange(duid2,duid1));
}
    | ADDRESSDUIDRangeList ',' DUID_ 
    {
        PresentRangeLst.append(new TStationRange(new TDUID($3.duid,$3.length)));
        delete $3.duid;
    }
    | ADDRESSDUIDRangeList ',' DUID_ '-' DUID_ 
    {
        SmartPtr<TDUID> duid2(new TDUID($3.duid,$3.length));
        SmartPtr<TDUID> duid1(new TDUID($5.duid,$5.length));
        if (*duid1<=*duid2)
            PresentRangeLst.append(new TStationRange(duid1,duid2));
        else
            PresentRangeLst.append(new TStationRange(duid2,duid1));
        delete $3.duid;
        delete $5.duid;
    }
    ;


RejectClientsOption
    : REJECT_CLIENTS_ 
    {
        PresentRangeLst.clear();    
    } ADDRESSDUIDRangeList
    {
        ParserOptStack.getLast()->setRejedClnt(&PresentRangeLst);
    }
  ;  

AcceptOnlyOption
    : ACCEPT_ONLY_
    {
        PresentRangeLst.clear();    
    } ADDRESSDUIDRangeList
    {
        ParserOptStack.getLast()->setAcceptClnt(&PresentRangeLst);
    }
  ;

PoolOption
    : POOL_
        {

            PresentRangeLst.clear();    
        } ADDRESSRangeList
        {
            ParserOptStack.getLast()->setPool(&PresentRangeLst);
        }
    ;
    
PreferredTimeOption
    : PREF_TIME_ Number 
    { 
        ParserOptStack.getLast()->setPrefBeg($2);
        ParserOptStack.getLast()->setPrefEnd($2);
    }
    | PREF_TIME_ Number '-' Number
    {
        ParserOptStack.getLast()->setPrefBeg($2);
        ParserOptStack.getLast()->setPrefEnd($4);   
    }
    ;
  
ValidTimeOption
    : VALID_TIME_ Number 
    { 
        ParserOptStack.getLast()->setValidBeg($2);
        ParserOptStack.getLast()->setValidEnd($2);
    }
    | VALID_TIME_ Number '-' Number
    {
        ParserOptStack.getLast()->setValidBeg($2);
        ParserOptStack.getLast()->setValidEnd($4);  
    }
    ;

T1Option
    : T1_ Number 
    {
        ParserOptStack.getLast()->setT1Beg($2); 
        ParserOptStack.getLast()->setT1End($2); 
    }
    | T1_ Number '-' Number
    {
        ParserOptStack.getLast()->setT1Beg($2); 
        ParserOptStack.getLast()->setT1End($4); 
    }
    ;

T2Option
    : T2_ Number 
    { 
        ParserOptStack.getLast()->setT2Beg($2); 
        ParserOptStack.getLast()->setT2End($2); 
    }
    | T2_ Number '-' Number
    {
        ParserOptStack.getLast()->setT2Beg($2); 
        ParserOptStack.getLast()->setT2End($4); 
    }
    ;

////////////////////////////////////////////////////////////////////////
/// DNS-server option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

DNSServerOption
:OPTION_ DNS_SERVER_ 
{
    PresentLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setDNSSrv(&PresentLst);
}
;

////////////////////////////////////////////////////////////////////////
/// DOMAIN option //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
DomainOption
: OPTION_ DOMAIN_ STRING_ 
{ 
    ParserOptStack.getLast()->setDomain($3);
}
;

////////////////////////////////////////////////////////////////////////
/// NTP-SERVER option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
NTPServerOption
:OPTION_ NTP_SERVER_ 
{
    PresentLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNTPSrv(&PresentLst);
}
;

////////////////////////////////////////////////////////////////////////
/// TIME-ZONE option ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
TimeZoneOption
: OPTION_ TIME_ZONE_ STRING_ 
{ 
    ParserOptStack.getLast()->setTimeZone($3); 
}
;

/*NISServerOption
  :NIS_SERVER_ STRING_
  { 
  ParserOptStack.getLast()->NISServer=$2;
  }
  ;*/

ClntMaxLeaseOption
: CLNT_MAX_LEASE_ Number 
{ 
    ParserOptStack.getLast()->setClntMaxLease($2);
}
;

ClassMaxLeaseOption
: CLASS_MAX_LEASE_ Number 
{
    ParserOptStack.getLast()->setClassMaxLease($2);
}
;

IfaceMaxLeaseOption
: IFACE_MAX_LEASE_ Number
{
    ParserOptStack.getLast()->setIfaceMaxLease($2);
}
;

UnicastAddressOption
: UNICAST_ IPV6ADDR_
{
    ParserOptStack.getLast()->setUnicast(true);
    ParserOptStack.getLast()->setAddress(new TIPv6Addr($2));
}
;

RapidCommitOption
:   RAPID_COMMIT_ Number 
{ 
    if ( ($2!=0) && ($2!=1)) {
	Log(Error) << "RAPID-COMMIT in line " << lex->lineno() << " must have 0 or 1 value." 
		   << LogEnd;
	YYABORT;
    }
    if (yyvsp[0].ival==1)
	ParserOptStack.getLast()->setRapidCommit(true); 
    else
	ParserOptStack.getLast()->setRapidCommit(false); 
}
;
    
PreferenceOption
: PREFERENCE_ Number 
{ 
    if (($2<0)||($2>255)) {
	Log(Error) << "Preference value (" << $2 << ") in line " << lex->lineno() 
		   << " is out of range [0..255]." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setPreference($2);    
}
;

LogLevelOption
: LOGLEVEL_ Number {
    logger::setLogLevel($2);
}
;

LogModeOption
: LOGMODE_ STRING_ {
    logger::setLogMode($2);
}

LogNameOption
: LOGNAME_ STRING_
{
    logger::setLogName($2);
}
;

WorkDirOption
:   WORKDIR_ STRING_
{
    ParserOptStack.getLast()->setWorkDir($2);
}
;

GlobalOptionDeclaration
: InterfaceOptionDeclaration
| LogModeOption
| LogLevelOption
| LogNameOption
| WorkDirOption
;

InterfaceOptionDeclaration
: ClassOptionDeclaration
| UnicastAddressOption
| PreferenceOption
| RapidCommitOption
| DNSServerOption
| NTPServerOption
| DomainOption
| TimeZoneOption
| IfaceMaxLeaseOption
| ClntMaxLeaseOption
;

ClassOptionDeclaration
: PreferredTimeOption
| ValidTimeOption
| PoolOption
| T1Option
| T2Option
//| NISServerOption
| RejectClientsOption
| AcceptOnlyOption
| ClassMaxLeaseOption
;

%%

/////////////////////////////////////////////////////////////////////////////
// programs section

//method check whether interface with id=ifaceNr has been 
//already declared
bool SrvParser::CheckIsIface(int ifaceNr)
{
  SmartPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) {
	Log(Error) << "Interface with ID=" << ifaceNr << " is already defined." << LogEnd;
	YYABORT;
    }
  return true;
};
    
//method check whether interface with id=ifaceName has been
//already declared 
bool SrvParser::CheckIsIface(string ifaceName)
{
  SmartPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) {
	Log(Error) << "Interface " << ifaceName << " is already defined." << LogEnd;
	YYABORT;
    }
  }
  return true;
};

//method creates new scope appropriately for interface options and declarations
//clears all lists except the list of interfaces and adds new group
void SrvParser::StartIfaceDeclaration()
{
    //Interface scope, so parameters associated with global scope are pushed on stack
    ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
    SrvCfgAddrClassLst.clear();
}

bool SrvParser::EndIfaceDeclaration()
{
    SmartPtr<TSrvCfgAddrClass> ptrAddrClass;
    if (!SrvCfgAddrClassLst.count()) {
	Log(Error) << "No address classes defined." << LogEnd;
	YYABORT;
    }
    SrvCfgAddrClassLst.first();
    while (ptrAddrClass=SrvCfgAddrClassLst.get())
	SrvCfgIfaceLst.getLast()->addAddrClass(ptrAddrClass);
    //setting interface options on the basis of just read information
    SrvCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    
    ParserOptStack.delLast();
    return true;
}   

void SrvParser::StartClassDeclaration()
{
  ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
}

bool SrvParser::EndClassDeclaration()
{
    if (!ParserOptStack.getLast()->countPool()) {
	Log(Error) << "No pools defined for this interface." << LogEnd;
        YYABORT;
    }
    SrvCfgAddrClassLst.append(new TSrvCfgAddrClass());
    //setting interface options on the basis of just read information
    SrvCfgAddrClassLst.getLast()->setOptions(ParserOptStack.getLast());
    ParserOptStack.delLast();
    return true;
}

namespace std {
extern yy_SrvParser_stype yylval;
};

int SrvParser::yylex()
{
    memset(&std::yylval,0, sizeof(std::yylval));
    memset(&this->yylval,0, sizeof(this->yylval));
    int x = this->lex->yylex();
    this->yylval=std::yylval;
    return x;
}

void SrvParser::yyerror(char *m)
{
    Log(Crit) << "Config parse error: line " << lex->lineno() 
              << ", unexpected [" << lex->YYText() << "] token." << LogEnd;
}

SrvParser::~SrvParser() {

}
