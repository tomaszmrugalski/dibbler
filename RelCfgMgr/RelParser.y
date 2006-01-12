%name RelParser

%header{
#include <iostream>
#include <string>
#include <malloc.h>
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "RelParser.h"
#include "RelParsGlobalOpt.h"
#include "RelParsIfaceOpt.h"
#include "RelCfgIface.h"
#include "DUID.h"
#include "Logger.h"

#define YY_USE_CLASS
%}

%{
#include "FlexLexer.h"
%}

// class definition
%define MEMBERS FlexLexer * lex;                                                     \
List(TRelParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */ \
List(TRelCfgIface) RelCfgIfaceLst;         /* list of RelCfg interfaces */           \
List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/ \
List(string) PresentStringLst;             /* string list */                         \
/*method check whether interface with id=ifaceNr has been already declared */        \
bool CheckIsIface(int ifaceNr);                                                      \
/*method check whether interface with id=ifaceName has been already declared*/       \
bool CheckIsIface(string ifaceName);                                                 \
void StartIfaceDeclaration();                                                        \
bool EndIfaceDeclaration();                                                          \
virtual ~RelParser();

// constructor
%define CONSTRUCTOR_PARAM yyFlexLexer * lex
%define CONSTRUCTOR_CODE                                                          \
    ParserOptStack.append(new TRelParsGlobalOpt());                               \
    this->lex = lex;

%union    
{
    unsigned int ival;
    char *strval;
    char addrval[16];
}

%token IFACE_, CLIENT_, SERVER_, UNICAST_, MULTICAST_, IFACE_ID_
%token LOGNAME_, LOGLEVEL_, LOGMODE_, WORKDIR_

%token <strval>     STRING_
%token <ival>       HEXNUMBER_
%token <ival>       INTNUMBER_
%token <addrval>    IPV6ADDR_
%type  <ival>       Number

%%

/////////////////////////////////////////////////////////////////////////////
// rules section
/////////////////////////////////////////////////////////////////////////////

Grammar
: GlobalList
;

GlobalList
: GlobalOptionsList IfaceList
;

GlobalOptionsList
: GlobalOption
| GlobalOptionsList GlobalOption
;

GlobalOption
: LogModeOption
| LogLevelOption
| LogNameOption
| WorkDirOption
;

IfaceList
: Iface
| IfaceList Iface
;

IfaceOptionList
: IfaceOptions
| IfaceOptionList IfaceOptions
;

IfaceOptions
: ClientUnicastOption
| ServerUnicastOption
| ClientMulticastOption
| ServerMulticastOption
| IfaceIDOption
;

Iface
:IFACE_ STRING_ '{' 
{
    CheckIsIface(string($2)); //If no - everything is ok
    StartIfaceDeclaration();
}
IfaceOptionList '}'
{
    //Information about new interface has been read
    //Add it to list of read interfaces
    RelCfgIfaceLst.append(new TRelCfgIface($2));
    delete [] $2;
    EndIfaceDeclaration();
}
|IFACE_ Number '{' 
{
    CheckIsIface($2);   //If no - everything is ok
    StartIfaceDeclaration();
}
IfaceOptionList '}'
{
    RelCfgIfaceLst.append(new TRelCfgIface($2));
    EndIfaceDeclaration();
}


/////////////////////////////////////////////////////////////////////////////
// Now Options and their parameters
/////////////////////////////////////////////////////////////////////////////
Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;

ServerUnicastOption
: SERVER_ UNICAST_ IPV6ADDR_
{
    ParserOptStack.getLast()->setServerUnicast(new TIPv6Addr($3));
}
;

ClientUnicastOption
: CLIENT_ UNICAST_ IPV6ADDR_
{
    ParserOptStack.getLast()->setClientUnicast(new TIPv6Addr($3));
}
;

ServerMulticastOption
: SERVER_ MULTICAST_ Number
{ 
    ParserOptStack.getLast()->setServerMulticast($3);
}
| SERVER_ MULTICAST_
{
    ParserOptStack.getLast()->setServerMulticast(true);
}
;

ClientMulticastOption
:   CLIENT_ MULTICAST_ Number
{ 
    ParserOptStack.getLast()->setClientMulticast($3);
}
| CLIENT_ MULTICAST_
{
    ParserOptStack.getLast()->setClientMulticast(true);
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

IfaceIDOption
:IFACE_ID_ Number
{
    ParserOptStack.getLast()->setInterfaceID($2);
}
;


%%

/////////////////////////////////////////////////////////////////////////////
// programs section

//method check whether interface with id=ifaceNr has been 
//already declared
bool RelParser::CheckIsIface(int ifaceNr)
{
  SmartPtr<TRelCfgIface> ptr;
  RelCfgIfaceLst.first();
  while (ptr=RelCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) {
	Log(Crit) << "Interface with ID=" << ifaceNr << " is already defined." << LogEnd;
	YYABORT;
    }
  return true;
}
    
//method check whether interface with id=ifaceName has been
//already declared 
bool RelParser::CheckIsIface(string ifaceName)
{
  SmartPtr<TRelCfgIface> ptr;
  RelCfgIfaceLst.first();
  while (ptr=RelCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) {
	Log(Crit) << "Interface " << ifaceName << " is already defined." << LogEnd;
	YYABORT;
    }
  }
  return true;
}

//method creates new scope appropriately for interface options and declarations
//clears all lists except the list of interfaces and adds new group
void RelParser::StartIfaceDeclaration()
{
    //Interface scope, so parameters associated with global scope are pushed on stack
    ParserOptStack.append(new TRelParsGlobalOpt(*ParserOptStack.getLast()));
}

bool RelParser::EndIfaceDeclaration()
{
    //setting interface options on the basis of just read information
    RelCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    ParserOptStack.delLast();
    return true;
}   

namespace std {
    extern yy_RelParser_stype yylval;
}

int RelParser::yylex()
{
    memset(&std::yylval,0, sizeof(std::yylval));
    memset(&this->yylval,0, sizeof(this->yylval));
    int x = this->lex->yylex();
    this->yylval=std::yylval;
    return x;
}

void RelParser::yyerror(char *m)
{
    Log(Crit) << "Config parse error: line " << lex->lineno() 
              << ", unexpected [" << lex->YYText() << "] token." << LogEnd;
}

RelParser::~RelParser() {
    
}
