#ifndef YY_clntParser_h_included
#define YY_clntParser_h_included
#define YY_USE_CLASS

#line 1 "../bison++/bison.h"
/* before anything */
#ifdef c_plusplus
 #ifndef __cplusplus
  #define __cplusplus
 #endif
#endif


 #line 8 "../bison++/bison.h"
#line 3 "ClntParser.y"

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
#include "ClntCfgGroup.h"
#include "Logger.h"

    using namespace std;
    
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
virtual ~clntParser();
#define YY_clntParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_clntParser_CONSTRUCTOR_CODE  \
   this->lex = lex; \
    ParserOptStack.append(new TClntParsGlobalOpt()); \
    ParserOptStack.getFirst()->setIAIDCnt(1);\
    ParserOptStack.getLast();

#line 65 "ClntParser.y"
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

#line 21 "../bison++/bison.h"
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
/*#ifdef YYSTYPE*/
  #ifndef YY_clntParser_STYPE
   #define YY_clntParser_STYPE YYSTYPE
  /* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
   /* use %define STYPE */
  #endif
/*#endif*/
 #ifdef YYDEBUG
  #ifndef YY_clntParser_DEBUG
   #define  YY_clntParser_DEBUG YYDEBUG
   /* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
   /* use %define DEBUG */
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

 #line 65 "../bison++/bison.h"

#line 65 "../bison++/bison.h"
/* YY_clntParser_PURE */
#endif


 #line 68 "../bison++/bison.h"

#line 68 "../bison++/bison.h"
/* prefix */

#ifndef YY_clntParser_DEBUG

 #line 71 "../bison++/bison.h"

#line 71 "../bison++/bison.h"
/* YY_clntParser_DEBUG */
#endif

#ifndef YY_clntParser_LSP_NEEDED

 #line 75 "../bison++/bison.h"

#line 75 "../bison++/bison.h"
 /* YY_clntParser_LSP_NEEDED*/
#endif

/* DEFAULT LTYPE*/
#ifdef YY_clntParser_LSP_NEEDED
 #ifndef YY_clntParser_LTYPE
  #ifndef BISON_YYLTYPE_ISDECLARED
   #define BISON_YYLTYPE_ISDECLARED
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
  #endif

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
  #ifndef yylval
   extern YY_clntParser_STYPE YY_clntParser_LVAL;
  #else
   #if yylval != YY_clntParser_LVAL
    extern YY_clntParser_STYPE YY_clntParser_LVAL;
   #else
    #warning "Namespace conflict, disabling some functionality (bison++ only)"
   #endif
  #endif
 #endif


 #line 169 "../bison++/bison.h"
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
#define	WORKDIR_	280
#define	RAPID_COMMIT_	281
#define	NOIA_	282
#define	OPTION_	283
#define	LOGNAME_	284
#define	LOGLEVEL_	285
#define	LOGMODE_	286
#define	STRING_	287
#define	HEXNUMBER_	288
#define	INTNUMBER_	289
#define	DUID_	290


#line 169 "../bison++/bison.h"
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
  
 #line 212 "../bison++/bison.h"
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
static const int WORKDIR_;
static const int RAPID_COMMIT_;
static const int NOIA_;
static const int OPTION_;
static const int LOGNAME_;
static const int LOGLEVEL_;
static const int LOGMODE_;
static const int STRING_;
static const int HEXNUMBER_;
static const int INTNUMBER_;
static const int DUID_;


#line 212 "../bison++/bison.h"
 /* decl const */
 #else
  enum YY_clntParser_ENUM_TOKEN { YY_clntParser_NULL_TOKEN=0
  
 #line 215 "../bison++/bison.h"
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
	,WORKDIR_=280
	,RAPID_COMMIT_=281
	,NOIA_=282
	,OPTION_=283
	,LOGNAME_=284
	,LOGLEVEL_=285
	,LOGMODE_=286
	,STRING_=287
	,HEXNUMBER_=288
	,INTNUMBER_=289
	,DUID_=290


#line 215 "../bison++/bison.h"
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
 /* Removed due to bison problems
 /#ifndef YYSTYPE
 / #define YYSTYPE YY_clntParser_STYPE
 /#endif*/

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

 #line 267 "../bison++/bison.h"
#endif
