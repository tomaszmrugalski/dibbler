#ifndef YY_SrvParser_h_included
#define YY_SrvParser_h_included
/* before anything */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif
#ifdef __cplusplus
#ifndef YY_USE_CLASS
#define YY_USE_CLASS
#endif
#else
#endif
#include <stdio.h>

#include <iostream>
#include <string>
#include "FlexLexer.h"
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
#include "SrvCfgMgr.h"
#include "DUID.h"
#include "Logger.h"

#define YY_USE_CLASS
#define YY_SrvParser_MEMBERS  FlexLexer * lex;                                                   \
List(TSrvParsGlobalOpt) ParserOptStack;                                            \
/* List of parsed interfaces/IAs/Addresses, last    */                            \
/* interface/IA/address is just being parsing or have been just parsed */   \
/* FIXME:Don't forget to clear this lists in apropriate moment         */   \
List(TSrvCfgIface)          SrvCfgIfaceLst;   \
List(TSrvCfgAddrClass)  SrvCfgAddrClassLst;   \
/*Pointer to list which should contain either DNS servers or NTPServers*/   \
List(TIPv6Addr) PresentLst;  \
/*Pointer to list which should contain: rejected clients, accepted clients */ \
/*or addressess ranges */ \
List(TStationRange) PresentRangeLst; \
/*method check whether interface with id=ifaceNr has been already declared */ \
bool CheckIsIface(int ifaceNr);  \
/*method check whether interface with id=ifaceName has been already declared*/ \
bool CheckIsIface(string ifaceName); \
void StartIfaceDeclaration(); \
bool EndIfaceDeclaration(); \
void StartClassDeclaration(); \
bool EndClassDeclaration();
#define YY_SrvParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_SrvParser_CONSTRUCTOR_CODE  \
    ParserOptStack.append(new TSrvParsGlobalOpt()); \
    ParserOptStack.getLast()->setUnicast(false);    \
   this->lex = lex;

typedef union    
{
    unsigned int ival;
  char                          *strval;
  struct                        SDuid
  {
    int                       length;
    char*                       duid;
    }                                   duidval;
  char                          addrval[16];
} yy_SrvParser_stype;
#define YY_SrvParser_STYPE yy_SrvParser_stype
 /* %{ and %header{ and %union, during decl */
#ifndef YY_SrvParser_COMPATIBILITY
#ifndef YY_USE_CLASS
#define  YY_SrvParser_COMPATIBILITY 1
#else
#define  YY_SrvParser_COMPATIBILITY 0
#endif
#endif

#if YY_SrvParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YYLTYPE
#ifndef YY_SrvParser_LTYPE
#define YY_SrvParser_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_SrvParser_STYPE 
#define YY_SrvParser_STYPE YYSTYPE
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
/* use %define STYPE */
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_SrvParser_DEBUG
#define  YY_SrvParser_DEBUG YYDEBUG
/* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
/* use %define DEBUG */
#endif
#endif
#ifdef YY_SrvParser_STYPE
#ifndef yystype
#define yystype YY_SrvParser_STYPE
#endif
#endif
/* use goto to be compatible */
#ifndef YY_SrvParser_USE_GOTO
#define YY_SrvParser_USE_GOTO 1
#endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_SrvParser_USE_GOTO
#define YY_SrvParser_USE_GOTO 0
#endif

#ifndef YY_SrvParser_PURE
/* YY_SrvParser_PURE */
#endif
/* prefix */
#ifndef YY_SrvParser_DEBUG
/* YY_SrvParser_DEBUG */
#endif
#ifndef YY_SrvParser_LSP_NEEDED
 /* YY_SrvParser_LSP_NEEDED*/
#endif
/* DEFAULT LTYPE*/
#ifdef YY_SrvParser_LSP_NEEDED
#ifndef YY_SrvParser_LTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YY_SrvParser_LTYPE yyltype
#endif
#endif
/* DEFAULT STYPE*/
#ifndef YY_SrvParser_STYPE
#define YY_SrvParser_STYPE int
#endif
/* DEFAULT MISCELANEOUS */
#ifndef YY_SrvParser_PARSE
#define YY_SrvParser_PARSE yyparse
#endif
#ifndef YY_SrvParser_LEX
#define YY_SrvParser_LEX yylex
#endif
#ifndef YY_SrvParser_LVAL
#define YY_SrvParser_LVAL yylval
#endif
#ifndef YY_SrvParser_LLOC
#define YY_SrvParser_LLOC yylloc
#endif
#ifndef YY_SrvParser_CHAR
#define YY_SrvParser_CHAR yychar
#endif
#ifndef YY_SrvParser_NERRS
#define YY_SrvParser_NERRS yynerrs
#endif
#ifndef YY_SrvParser_DEBUG_FLAG
#define YY_SrvParser_DEBUG_FLAG yydebug
#endif
#ifndef YY_SrvParser_ERROR
#define YY_SrvParser_ERROR yyerror
#endif

#ifndef YY_SrvParser_PARSE_PARAM
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
#define YY_SrvParser_PARSE_PARAM
#ifndef YY_SrvParser_PARSE_PARAM_DEF
#define YY_SrvParser_PARSE_PARAM_DEF
#endif
#endif
#endif
#endif
#ifndef YY_SrvParser_PARSE_PARAM
#define YY_SrvParser_PARSE_PARAM void
#endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

#ifndef YY_SrvParser_PURE
extern YY_SrvParser_STYPE YY_SrvParser_LVAL;
#endif

#define	IFACE_	258
#define	NO_CONFIG_	259
#define	CLASS_	260
#define	LOGNAME_	261
#define	LOGLEVEL_	262
#define	WORKDIR_	263
#define	NTP_SERVER_	264
#define	TIME_ZONE_	265
#define	DNS_SERVER_	266
#define	DOMAIN_	267
#define	ACCEPT_ONLY_	268
#define	REJECT_CLIENTS_	269
#define	POOL_	270
#define	T1_	271
#define	T2_	272
#define	PREF_TIME_	273
#define	VALID_TIME_	274
#define	UNICAST_	275
#define	PREFERENCE_	276
#define	RAPID_COMMIT_	277
#define	MAX_LEASE_	278
#define	CLNT_MAX_LEASE_	279
#define	STRING_	280
#define	HEXNUMBER_	281
#define	INTNUMBER_	282
#define	IPV6ADDR_	283
#define	DUID_	284

 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
#ifndef YY_SrvParser_CLASS
#define YY_SrvParser_CLASS SrvParser
#endif

#ifndef YY_SrvParser_INHERIT
#define YY_SrvParser_INHERIT
#endif
#ifndef YY_SrvParser_MEMBERS
#define YY_SrvParser_MEMBERS 
#endif
#ifndef YY_SrvParser_LEX_BODY
#define YY_SrvParser_LEX_BODY  
#endif
#ifndef YY_SrvParser_ERROR_BODY
#define YY_SrvParser_ERROR_BODY  
#endif
#ifndef YY_SrvParser_CONSTRUCTOR_PARAM
#define YY_SrvParser_CONSTRUCTOR_PARAM
#endif
/* choose between enum and const */
#ifndef YY_SrvParser_USE_CONST_TOKEN
#define YY_SrvParser_USE_CONST_TOKEN 0
/* yes enum is more compatible with flex,  */
/* so by default we use it */ 
#endif
#if YY_SrvParser_USE_CONST_TOKEN != 0
#ifndef YY_SrvParser_ENUM_TOKEN
#define YY_SrvParser_ENUM_TOKEN yy_SrvParser_enum_token
#endif
#endif

class YY_SrvParser_CLASS YY_SrvParser_INHERIT
{
public: 
#if YY_SrvParser_USE_CONST_TOKEN != 0
/* static const int token ... */
static const int IFACE_;
static const int NO_CONFIG_;
static const int CLASS_;
static const int LOGNAME_;
static const int LOGLEVEL_;
static const int WORKDIR_;
static const int NTP_SERVER_;
static const int TIME_ZONE_;
static const int DNS_SERVER_;
static const int DOMAIN_;
static const int ACCEPT_ONLY_;
static const int REJECT_CLIENTS_;
static const int POOL_;
static const int T1_;
static const int T2_;
static const int PREF_TIME_;
static const int VALID_TIME_;
static const int UNICAST_;
static const int PREFERENCE_;
static const int RAPID_COMMIT_;
static const int MAX_LEASE_;
static const int CLNT_MAX_LEASE_;
static const int STRING_;
static const int HEXNUMBER_;
static const int INTNUMBER_;
static const int IPV6ADDR_;
static const int DUID_;

 /* decl const */
#else
enum YY_SrvParser_ENUM_TOKEN { YY_SrvParser_NULL_TOKEN=0
	,IFACE_=258
	,NO_CONFIG_=259
	,CLASS_=260
	,LOGNAME_=261
	,LOGLEVEL_=262
	,WORKDIR_=263
	,NTP_SERVER_=264
	,TIME_ZONE_=265
	,DNS_SERVER_=266
	,DOMAIN_=267
	,ACCEPT_ONLY_=268
	,REJECT_CLIENTS_=269
	,POOL_=270
	,T1_=271
	,T2_=272
	,PREF_TIME_=273
	,VALID_TIME_=274
	,UNICAST_=275
	,PREFERENCE_=276
	,RAPID_COMMIT_=277
	,MAX_LEASE_=278
	,CLNT_MAX_LEASE_=279
	,STRING_=280
	,HEXNUMBER_=281
	,INTNUMBER_=282
	,IPV6ADDR_=283
	,DUID_=284

 /* enum token */
     }; /* end of enum declaration */
#endif
public:
 int YY_SrvParser_PARSE(YY_SrvParser_PARSE_PARAM);
 virtual void YY_SrvParser_ERROR(char *msg) YY_SrvParser_ERROR_BODY;
#ifdef YY_SrvParser_PURE
#ifdef YY_SrvParser_LSP_NEEDED
 virtual int  YY_SrvParser_LEX(YY_SrvParser_STYPE *YY_SrvParser_LVAL,YY_SrvParser_LTYPE *YY_SrvParser_LLOC) YY_SrvParser_LEX_BODY;
#else
 virtual int  YY_SrvParser_LEX(YY_SrvParser_STYPE *YY_SrvParser_LVAL) YY_SrvParser_LEX_BODY;
#endif
#else
 virtual int YY_SrvParser_LEX() YY_SrvParser_LEX_BODY;
 YY_SrvParser_STYPE YY_SrvParser_LVAL;
#ifdef YY_SrvParser_LSP_NEEDED
 YY_SrvParser_LTYPE YY_SrvParser_LLOC;
#endif
 int YY_SrvParser_NERRS;
 int YY_SrvParser_CHAR;
#endif
#if YY_SrvParser_DEBUG != 0
public:
 int YY_SrvParser_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
#endif
public:
 YY_SrvParser_CLASS(YY_SrvParser_CONSTRUCTOR_PARAM);
public:
 YY_SrvParser_MEMBERS 
};
/* other declare folow */
#endif


#if YY_SrvParser_COMPATIBILITY != 0
/* backward compatibility */
#ifndef YYSTYPE
#define YYSTYPE YY_SrvParser_STYPE
#endif

#ifndef YYLTYPE
#define YYLTYPE YY_SrvParser_LTYPE
#endif
#ifndef YYDEBUG
#ifdef YY_SrvParser_DEBUG 
#define YYDEBUG YY_SrvParser_DEBUG
#endif
#endif

#endif
/* END */
#endif
