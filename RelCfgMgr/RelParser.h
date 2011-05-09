#ifndef YY_RelParser_h_included
#define YY_RelParser_h_included
#define YY_USE_CLASS

#line 1 "../bison++/bison.h"
/* before anything */
#ifdef c_plusplus
 #ifndef __cplusplus
  #define __cplusplus
 #endif
#endif


 #line 8 "../bison++/bison.h"
#line 3 "RelParser.y"

#include <string.h>
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
#include "OptVendorData.h"
#include "DUID.h"
#include "Logger.h"
#include "Portable.h"

#define YY_USE_CLASS
#define YY_RelParser_MEMBERS  FlexLexer * lex;                                                     \
List(TRelParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */ \
List(TRelCfgIface) RelCfgIfaceLst;         /* list of RelCfg interfaces */           \
List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/ \
List(string) PresentStringLst;             /* string list */                         \
SPtr<TRelOptEcho> EchoOpt;                 /* echo request option */                 \
/*method check whether interface with id=ifaceNr has been already declared */        \
bool CheckIsIface(int ifaceNr);                                                      \
/*method check whether interface with id=ifaceName has been already declared*/       \
bool CheckIsIface(string ifaceName);                                                 \
void StartIfaceDeclaration();                                                        \
bool EndIfaceDeclaration();                                                          \
virtual ~RelParser();
#define YY_RelParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_RelParser_CONSTRUCTOR_CODE                                                           \
    ParserOptStack.append(new TRelParsGlobalOpt());                               \
    this->lex = lex;

#line 48 "RelParser.y"
typedef union    
{
    unsigned int ival;
    char *strval;
    char addrval[16];
    struct SDuid
    {
        int length;
        char* duid;
    } duidval;
} yy_RelParser_stype;
#define YY_RelParser_STYPE yy_RelParser_stype

#line 21 "../bison++/bison.h"
 /* %{ and %header{ and %union, during decl */
#ifndef YY_RelParser_COMPATIBILITY
 #ifndef YY_USE_CLASS
  #define  YY_RelParser_COMPATIBILITY 1
 #else
  #define  YY_RelParser_COMPATIBILITY 0
 #endif
#endif

#if YY_RelParser_COMPATIBILITY != 0
/* backward compatibility */
 #ifdef YYLTYPE
  #ifndef YY_RelParser_LTYPE
   #define YY_RelParser_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
  #endif
 #endif
/*#ifdef YYSTYPE*/
  #ifndef YY_RelParser_STYPE
   #define YY_RelParser_STYPE YYSTYPE
  /* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
   /* use %define STYPE */
  #endif
/*#endif*/
 #ifdef YYDEBUG
  #ifndef YY_RelParser_DEBUG
   #define  YY_RelParser_DEBUG YYDEBUG
   /* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
   /* use %define DEBUG */
  #endif
 #endif 
 /* use goto to be compatible */
 #ifndef YY_RelParser_USE_GOTO
  #define YY_RelParser_USE_GOTO 1
 #endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_RelParser_USE_GOTO
 #define YY_RelParser_USE_GOTO 0
#endif

#ifndef YY_RelParser_PURE

 #line 65 "../bison++/bison.h"

#line 65 "../bison++/bison.h"
/* YY_RelParser_PURE */
#endif


 #line 68 "../bison++/bison.h"

#line 68 "../bison++/bison.h"
/* prefix */

#ifndef YY_RelParser_DEBUG

 #line 71 "../bison++/bison.h"

#line 71 "../bison++/bison.h"
/* YY_RelParser_DEBUG */
#endif

#ifndef YY_RelParser_LSP_NEEDED

 #line 75 "../bison++/bison.h"

#line 75 "../bison++/bison.h"
 /* YY_RelParser_LSP_NEEDED*/
#endif

/* DEFAULT LTYPE*/
#ifdef YY_RelParser_LSP_NEEDED
 #ifndef YY_RelParser_LTYPE
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

  #define YY_RelParser_LTYPE yyltype
 #endif
#endif

/* DEFAULT STYPE*/
#ifndef YY_RelParser_STYPE
 #define YY_RelParser_STYPE int
#endif

/* DEFAULT MISCELANEOUS */
#ifndef YY_RelParser_PARSE
 #define YY_RelParser_PARSE yyparse
#endif

#ifndef YY_RelParser_LEX
 #define YY_RelParser_LEX yylex
#endif

#ifndef YY_RelParser_LVAL
 #define YY_RelParser_LVAL yylval
#endif

#ifndef YY_RelParser_LLOC
 #define YY_RelParser_LLOC yylloc
#endif

#ifndef YY_RelParser_CHAR
 #define YY_RelParser_CHAR yychar
#endif

#ifndef YY_RelParser_NERRS
 #define YY_RelParser_NERRS yynerrs
#endif

#ifndef YY_RelParser_DEBUG_FLAG
 #define YY_RelParser_DEBUG_FLAG yydebug
#endif

#ifndef YY_RelParser_ERROR
 #define YY_RelParser_ERROR yyerror
#endif

#ifndef YY_RelParser_PARSE_PARAM
 #ifndef __STDC__
  #ifndef __cplusplus
   #ifndef YY_USE_CLASS
    #define YY_RelParser_PARSE_PARAM
    #ifndef YY_RelParser_PARSE_PARAM_DEF
     #define YY_RelParser_PARSE_PARAM_DEF
    #endif
   #endif
  #endif
 #endif
 #ifndef YY_RelParser_PARSE_PARAM
  #define YY_RelParser_PARSE_PARAM void
 #endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

 #ifndef YY_RelParser_PURE
  #ifndef yylval
   extern YY_RelParser_STYPE YY_RelParser_LVAL;
  #else
   #if yylval != YY_RelParser_LVAL
    extern YY_RelParser_STYPE YY_RelParser_LVAL;
   #else
    #warning "Namespace conflict, disabling some functionality (bison++ only)"
   #endif
  #endif
 #endif


 #line 169 "../bison++/bison.h"
#define	IFACE_	258
#define	CLIENT_	259
#define	SERVER_	260
#define	UNICAST_	261
#define	MULTICAST_	262
#define	IFACE_ID_	263
#define	IFACE_ID_ORDER_	264
#define	LOGNAME_	265
#define	LOGLEVEL_	266
#define	LOGMODE_	267
#define	WORKDIR_	268
#define	DUID_	269
#define	OPTION_	270
#define	REMOTE_ID_	271
#define	ECHO_REQUEST_	272
#define	GUESS_MODE_	273
#define	STRING_	274
#define	HEXNUMBER_	275
#define	INTNUMBER_	276
#define	IPV6ADDR_	277


#line 169 "../bison++/bison.h"
 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
 #ifndef YY_RelParser_CLASS
  #define YY_RelParser_CLASS RelParser
 #endif

 #ifndef YY_RelParser_INHERIT
  #define YY_RelParser_INHERIT
 #endif

 #ifndef YY_RelParser_MEMBERS
  #define YY_RelParser_MEMBERS 
 #endif

 #ifndef YY_RelParser_LEX_BODY
  #define YY_RelParser_LEX_BODY  
 #endif

 #ifndef YY_RelParser_ERROR_BODY
  #define YY_RelParser_ERROR_BODY  
 #endif

 #ifndef YY_RelParser_CONSTRUCTOR_PARAM
  #define YY_RelParser_CONSTRUCTOR_PARAM
 #endif
 /* choose between enum and const */
 #ifndef YY_RelParser_USE_CONST_TOKEN
  #define YY_RelParser_USE_CONST_TOKEN 0
  /* yes enum is more compatible with flex,  */
  /* so by default we use it */ 
 #endif
 #if YY_RelParser_USE_CONST_TOKEN != 0
  #ifndef YY_RelParser_ENUM_TOKEN
   #define YY_RelParser_ENUM_TOKEN yy_RelParser_enum_token
  #endif
 #endif

class YY_RelParser_CLASS YY_RelParser_INHERIT
{
public: 
 #if YY_RelParser_USE_CONST_TOKEN != 0
  /* static const int token ... */
  
 #line 212 "../bison++/bison.h"
static const int IFACE_;
static const int CLIENT_;
static const int SERVER_;
static const int UNICAST_;
static const int MULTICAST_;
static const int IFACE_ID_;
static const int IFACE_ID_ORDER_;
static const int LOGNAME_;
static const int LOGLEVEL_;
static const int LOGMODE_;
static const int WORKDIR_;
static const int DUID_;
static const int OPTION_;
static const int REMOTE_ID_;
static const int ECHO_REQUEST_;
static const int GUESS_MODE_;
static const int STRING_;
static const int HEXNUMBER_;
static const int INTNUMBER_;
static const int IPV6ADDR_;


#line 212 "../bison++/bison.h"
 /* decl const */
 #else
  enum YY_RelParser_ENUM_TOKEN { YY_RelParser_NULL_TOKEN=0
  
 #line 215 "../bison++/bison.h"
	,IFACE_=258
	,CLIENT_=259
	,SERVER_=260
	,UNICAST_=261
	,MULTICAST_=262
	,IFACE_ID_=263
	,IFACE_ID_ORDER_=264
	,LOGNAME_=265
	,LOGLEVEL_=266
	,LOGMODE_=267
	,WORKDIR_=268
	,DUID_=269
	,OPTION_=270
	,REMOTE_ID_=271
	,ECHO_REQUEST_=272
	,GUESS_MODE_=273
	,STRING_=274
	,HEXNUMBER_=275
	,INTNUMBER_=276
	,IPV6ADDR_=277


#line 215 "../bison++/bison.h"
 /* enum token */
     }; /* end of enum declaration */
 #endif
public:
 int YY_RelParser_PARSE(YY_RelParser_PARSE_PARAM);
 virtual void YY_RelParser_ERROR(char *msg) YY_RelParser_ERROR_BODY;
 #ifdef YY_RelParser_PURE
  #ifdef YY_RelParser_LSP_NEEDED
   virtual int  YY_RelParser_LEX(YY_RelParser_STYPE *YY_RelParser_LVAL,YY_RelParser_LTYPE *YY_RelParser_LLOC) YY_RelParser_LEX_BODY;
  #else
   virtual int  YY_RelParser_LEX(YY_RelParser_STYPE *YY_RelParser_LVAL) YY_RelParser_LEX_BODY;
  #endif
 #else
  virtual int YY_RelParser_LEX() YY_RelParser_LEX_BODY;
  YY_RelParser_STYPE YY_RelParser_LVAL;
  #ifdef YY_RelParser_LSP_NEEDED
   YY_RelParser_LTYPE YY_RelParser_LLOC;
  #endif
  int YY_RelParser_NERRS;
  int YY_RelParser_CHAR;
 #endif
 #if YY_RelParser_DEBUG != 0
  public:
   int YY_RelParser_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
 #endif
public:
 YY_RelParser_CLASS(YY_RelParser_CONSTRUCTOR_PARAM);
public:
 YY_RelParser_MEMBERS 
};
/* other declare folow */
#endif


#if YY_RelParser_COMPATIBILITY != 0
 /* backward compatibility */
 /* Removed due to bison problems
 /#ifndef YYSTYPE
 / #define YYSTYPE YY_RelParser_STYPE
 /#endif*/

 #ifndef YYLTYPE
  #define YYLTYPE YY_RelParser_LTYPE
 #endif
 #ifndef YYDEBUG
  #ifdef YY_RelParser_DEBUG 
   #define YYDEBUG YY_RelParser_DEBUG
  #endif
 #endif

#endif
/* END */

 #line 267 "../bison++/bison.h"
#endif
