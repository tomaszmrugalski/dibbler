#ifndef YY_clntParser_h_included
#define YY_clntParser_h_included
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
    #include "DHCPConst.h"
    #include "SmartPtr.h"
    #include "Container.h"
    #include "ClntParser.h"
    #include "ClntParsGlobalOpt.h"
    #include "ClntCfgIface.h"
    #include "ClntCfgAddr.h"
    #include "ClntCfgIA.h"
    #include "ClntCfgGroup.h"

    #define YY_USE_CLASS
#define YY_clntParser_MEMBERS  yyFlexLexer * lex; \
/*List of options in scope stack,the most fresh is last in the list*/ \
TContainer<SmartPtr<TClntParsGlobalOpt> > ParserOptStack; \
/*List of parsed interfaces/IAs/Addresses, last */ \
/*interface/IA/address is just being parsing or have been just parsed*/ \
/*FIXME:Don't forget to clear this lists in apropriate moment*/ \
TContainer<SmartPtr<TClntCfgIface> > ClntCfgIfaceLst; \
TContainer<SmartPtr<TClntCfgGroup> > ClntCfgGroupLst; \
TContainer<SmartPtr<TClntCfgIA> >    ClntCfgIALst;    \
TContainer<SmartPtr<TClntCfgAddr> >  ClntCfgAddrLst;  \
/*Pointer to list which should contain either rejected servers or */ \
/*preffered servers*/  \
TContainer<SmartPtr<TStationID> > PresentStationLst;\
TContainer<SmartPtr<TIPv6Addr> > PresentAddrLst; \
/*method check whether interface with id=ifaceNr has been */ \
/*already declared */ \
bool CheckIsIface(int ifaceNr); \
/* method check whether interface with id=ifaceName has been already declared */ \
bool CheckIsIface(string ifaceName); \
void StartIfaceDeclaration(); \
bool EndIfaceDeclaration(); \
void EmptyIface(); \
void StartIADeclaration(bool aggregation); \
void EndIADeclaration(long iaCnt); \
void EmptyIA(); \
void EmptyAddr(); \

#define YY_clntParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_clntParser_CONSTRUCTOR_CODE  \
   this->lex = lex; \
    ParserOptStack.append(new TClntParsGlobalOpt()); \
    ParserOptStack.getFirst()->setIAIDCnt(1);

typedef union    
{
    int          ival;    
    char         *strval;  
    struct SDuid  {      
        int         length;    
        char*     duid;  
    } duidval;  
    char    addrval[16];  
    ESendOpt  SendOpt;  
    EReqOpt   ReqOpt; 
} yy_clntParser_stype;
#define YY_clntParser_STYPE yy_clntParser_stype
 /* %{ and %header{ and %union, during decl */
#ifndef YY_clntParser_COMPATIBILITY
#ifndef YY_USE_CLASS
#define  YY_clntParser_COMPATIBILITY 1
#else
#define  YY_clntParser_COMPATIBILITY 0
#endif
#endif

#if YY_clntParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YYLTYPE
#ifndef YY_clntParser_LTYPE
#define YY_clntParser_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_clntParser_STYPE 
#define YY_clntParser_STYPE YYSTYPE
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
/* use %define STYPE */
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_clntParser_DEBUG
#define  YY_clntParser_DEBUG YYDEBUG
/* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
/* use %define DEBUG */
#endif
#endif
#ifdef YY_clntParser_STYPE
#ifndef yystype
#define yystype YY_clntParser_STYPE
#endif
#endif
/* use goto to be compatible */
#ifndef YY_clntParser_USE_GOTO
#define YY_clntParser_USE_GOTO 1
#endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_clntParser_USE_GOTO
#define YY_clntParser_USE_GOTO 0
#endif

#ifndef YY_clntParser_PURE
/* YY_clntParser_PURE */
#endif
/* prefix */
#ifndef YY_clntParser_DEBUG
/* YY_clntParser_DEBUG */
#endif
#ifndef YY_clntParser_LSP_NEEDED
 /* YY_clntParser_LSP_NEEDED*/
#endif
/* DEFAULT LTYPE*/
#ifdef YY_clntParser_LSP_NEEDED
#ifndef YY_clntParser_LTYPE
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

#define YY_clntParser_LTYPE yyltype
#endif
#endif
/* DEFAULT STYPE*/
#ifndef YY_clntParser_STYPE
#define YY_clntParser_STYPE int
#endif
/* DEFAULT MISCELANEOUS */
#ifndef YY_clntParser_PARSE
#define YY_clntParser_PARSE yyparse
#endif
#ifndef YY_clntParser_LEX
#define YY_clntParser_LEX yylex
#endif
#ifndef YY_clntParser_LVAL
#define YY_clntParser_LVAL yylval
#endif
#ifndef YY_clntParser_LLOC
#define YY_clntParser_LLOC yylloc
#endif
#ifndef YY_clntParser_CHAR
#define YY_clntParser_CHAR yychar
#endif
#ifndef YY_clntParser_NERRS
#define YY_clntParser_NERRS yynerrs
#endif
#ifndef YY_clntParser_DEBUG_FLAG
#define YY_clntParser_DEBUG_FLAG yydebug
#endif
#ifndef YY_clntParser_ERROR
#define YY_clntParser_ERROR yyerror
#endif

#ifndef YY_clntParser_PARSE_PARAM
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
#define YY_clntParser_PARSE_PARAM
#ifndef YY_clntParser_PARSE_PARAM_DEF
#define YY_clntParser_PARSE_PARAM_DEF
#endif
#endif
#endif
#endif
#ifndef YY_clntParser_PARSE_PARAM
#define YY_clntParser_PARSE_PARAM void
#endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

#ifndef YY_clntParser_PURE
extern YY_clntParser_STYPE YY_clntParser_LVAL;
#endif

#define	T1_	258
#define	T2_	259
#define	PREF_TIME_	260
#define	DNS_SERVER_	261
#define	VALID_TIME_	262
#define	NTP_SERVER_	263
#define	DOMAIN_	264
#define	TIME_ZONE_	265
#define	IFACE_	266
#define	NO_CONFIG_	267
#define	REJECT_SERVERS_	268
#define	PREFERRED_SERVERS_	269
#define	REQUIRE_	270
#define	REQUEST_	271
#define	SEND_	272
#define	DEFAULT_	273
#define	SUPERSEDE_	274
#define	APPEND_	275
#define	PREPEND_	276
#define	IA_	277
#define	ADDRES_	278
#define	IPV6ADDR_	279
#define	LOGLEVEL_	280
#define	WORKDIR_	281
#define	RAPID_COMMIT_	282
#define	NOIA_	283
#define	STRING_	284
#define	HEXNUMBER_	285
#define	INTNUMBER_	286
#define	DUID_	287

 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
#ifndef YY_clntParser_CLASS
#define YY_clntParser_CLASS clntParser
#endif

#ifndef YY_clntParser_INHERIT
#define YY_clntParser_INHERIT
#endif
#ifndef YY_clntParser_MEMBERS
#define YY_clntParser_MEMBERS 
#endif
#ifndef YY_clntParser_LEX_BODY
#define YY_clntParser_LEX_BODY  
#endif
#ifndef YY_clntParser_ERROR_BODY
#define YY_clntParser_ERROR_BODY  
#endif
#ifndef YY_clntParser_CONSTRUCTOR_PARAM
#define YY_clntParser_CONSTRUCTOR_PARAM
#endif
/* choose between enum and const */
#ifndef YY_clntParser_USE_CONST_TOKEN
#define YY_clntParser_USE_CONST_TOKEN 0
/* yes enum is more compatible with flex,  */
/* so by default we use it */ 
#endif
#if YY_clntParser_USE_CONST_TOKEN != 0
#ifndef YY_clntParser_ENUM_TOKEN
#define YY_clntParser_ENUM_TOKEN yy_clntParser_enum_token
#endif
#endif

class YY_clntParser_CLASS YY_clntParser_INHERIT
{
public: 
#if YY_clntParser_USE_CONST_TOKEN != 0
/* static const int token ... */
static const int T1_;
static const int T2_;
static const int PREF_TIME_;
static const int DNS_SERVER_;
static const int VALID_TIME_;
static const int NTP_SERVER_;
static const int DOMAIN_;
static const int TIME_ZONE_;
static const int IFACE_;
static const int NO_CONFIG_;
static const int REJECT_SERVERS_;
static const int PREFERRED_SERVERS_;
static const int REQUIRE_;
static const int REQUEST_;
static const int SEND_;
static const int DEFAULT_;
static const int SUPERSEDE_;
static const int APPEND_;
static const int PREPEND_;
static const int IA_;
static const int ADDRES_;
static const int IPV6ADDR_;
static const int LOGLEVEL_;
static const int WORKDIR_;
static const int RAPID_COMMIT_;
static const int NOIA_;
static const int STRING_;
static const int HEXNUMBER_;
static const int INTNUMBER_;
static const int DUID_;

 /* decl const */
#else
enum YY_clntParser_ENUM_TOKEN { YY_clntParser_NULL_TOKEN=0
	,T1_=258
	,T2_=259
	,PREF_TIME_=260
	,DNS_SERVER_=261
	,VALID_TIME_=262
	,NTP_SERVER_=263
	,DOMAIN_=264
	,TIME_ZONE_=265
	,IFACE_=266
	,NO_CONFIG_=267
	,REJECT_SERVERS_=268
	,PREFERRED_SERVERS_=269
	,REQUIRE_=270
	,REQUEST_=271
	,SEND_=272
	,DEFAULT_=273
	,SUPERSEDE_=274
	,APPEND_=275
	,PREPEND_=276
	,IA_=277
	,ADDRES_=278
	,IPV6ADDR_=279
	,LOGLEVEL_=280
	,WORKDIR_=281
	,RAPID_COMMIT_=282
	,NOIA_=283
	,STRING_=284
	,HEXNUMBER_=285
	,INTNUMBER_=286
	,DUID_=287

 /* enum token */
     }; /* end of enum declaration */
#endif
public:
 int YY_clntParser_PARSE(YY_clntParser_PARSE_PARAM);
 virtual void YY_clntParser_ERROR(char *msg) YY_clntParser_ERROR_BODY;
#ifdef YY_clntParser_PURE
#ifdef YY_clntParser_LSP_NEEDED
 virtual int  YY_clntParser_LEX(YY_clntParser_STYPE *YY_clntParser_LVAL,YY_clntParser_LTYPE *YY_clntParser_LLOC) YY_clntParser_LEX_BODY;
#else
 virtual int  YY_clntParser_LEX(YY_clntParser_STYPE *YY_clntParser_LVAL) YY_clntParser_LEX_BODY;
#endif
#else
 virtual int YY_clntParser_LEX() YY_clntParser_LEX_BODY;
 YY_clntParser_STYPE YY_clntParser_LVAL;
#ifdef YY_clntParser_LSP_NEEDED
 YY_clntParser_LTYPE YY_clntParser_LLOC;
#endif
 int YY_clntParser_NERRS;
 int YY_clntParser_CHAR;
#endif
#if YY_clntParser_DEBUG != 0
public:
 int YY_clntParser_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
#endif
public:
 YY_clntParser_CLASS(YY_clntParser_CONSTRUCTOR_PARAM);
public:
 YY_clntParser_MEMBERS 
};
/* other declare folow */
#endif


#if YY_clntParser_COMPATIBILITY != 0
/* backward compatibility */
#ifndef YYSTYPE
#define YYSTYPE YY_clntParser_STYPE
#endif

#ifndef YYLTYPE
#define YYLTYPE YY_clntParser_LTYPE
#endif
#ifndef YYDEBUG
#ifdef YY_clntParser_DEBUG 
#define YYDEBUG YY_clntParser_DEBUG
#endif
#endif

#endif
/* END */
#endif
