#ifndef YY_SrvParser_h_included
#define YY_SrvParser_h_included
#define YY_USE_CLASS

#line 1 "../bison++/bison.h"
/* before anything */
#ifdef c_plusplus
 #ifndef __cplusplus
  #define __cplusplus
 #endif
#endif


 #line 8 "../bison++/bison.h"
#line 3 "SrvParser.y"

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
#include "SrvCfgTA.h"
#include "SrvCfgPD.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "SrvCfgOptions.h"
#include "DUID.h"
#include "Logger.h"
#include "FQDN.h"
#include "SrvOptVendorSpec.h"
#include "SrvOptAddrParams.h"
#define YY_USE_CLASS
#define YY_SrvParser_MEMBERS  FlexLexer * lex;                                                     \
List(TSrvParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */ \
List(TSrvCfgIface) SrvCfgIfaceLst;         /* list of SrvCfg interfaces */           \
List(TSrvCfgAddrClass) SrvCfgAddrClassLst; /* list of SrvCfg address classes */      \
List(TSrvCfgTA) SrvCfgTALst;               /* list of SrvCfg TA objects */           \
List(TSrvCfgPD) SrvCfgPDLst;		   /* list of SrvCfg PD objects */           \
List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/ \
List(string) PresentStringLst;             /* string list */                         \
List(TFQDN) PresentFQDNLst;                                                          \
SmartPtr<TDUID> duidNew;                                                             \
SmartPtr<TIPv6Addr> addr;                                                            \
List(TStationRange) PresentRangeLst;                                                 \
List(TStationRange) PDLst;                                                           \
int VendorEnterpriseNumber;                                                          \
List(TSrvOptVendorSpec) VendorSpec;			                             \
List(TSrvCfgOptions) ClientLst;                                                      \
int PDPrefix;                                                                        \
/*method check whether interface with id=ifaceNr has been already declared */        \
bool CheckIsIface(int ifaceNr);                                                      \
/*method check whether interface with id=ifaceName has been already declared*/       \
bool CheckIsIface(string ifaceName);                                                 \
void StartIfaceDeclaration();                                                        \
bool EndIfaceDeclaration();                                                          \
void StartClassDeclaration();                                                        \
bool EndClassDeclaration();                                                          \
SmartPtr<TIPv6Addr> getRangeMin(char * addrPacked, int prefix);                      \
SmartPtr<TIPv6Addr> getRangeMax(char * addrPacked, int prefix);                      \
void StartTAClassDeclaration();                                                      \
bool EndTAClassDeclaration();                                                        \
void StartPDDeclaration();                                                           \
bool EndPDDeclaration();                                                             \
virtual ~SrvParser();
#define YY_SrvParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_SrvParser_CONSTRUCTOR_CODE                                                           \
    ParserOptStack.append(new TSrvParsGlobalOpt());                               \
    ParserOptStack.getLast()->setUnicast(false);                                  \
    this->lex = lex;

#line 72 "SrvParser.y"
typedef union    
{
    unsigned int ival;
    char *strval;
    struct SDuid
    {
        int length;
        char* duid;
    } duidval;
    char addrval[16];
} yy_SrvParser_stype;
#define YY_SrvParser_STYPE yy_SrvParser_stype

#line 21 "../bison++/bison.h"
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
/*#ifdef YYSTYPE*/
  #ifndef YY_SrvParser_STYPE
   #define YY_SrvParser_STYPE YYSTYPE
  /* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
   /* use %define STYPE */
  #endif
/*#endif*/
 #ifdef YYDEBUG
  #ifndef YY_SrvParser_DEBUG
   #define  YY_SrvParser_DEBUG YYDEBUG
   /* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
   /* use %define DEBUG */
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

 #line 65 "../bison++/bison.h"

#line 65 "../bison++/bison.h"
/* YY_SrvParser_PURE */
#endif


 #line 68 "../bison++/bison.h"

#line 68 "../bison++/bison.h"
/* prefix */

#ifndef YY_SrvParser_DEBUG

 #line 71 "../bison++/bison.h"
#define YY_SrvParser_DEBUG 1

#line 71 "../bison++/bison.h"
/* YY_SrvParser_DEBUG */
#endif

#ifndef YY_SrvParser_LSP_NEEDED

 #line 75 "../bison++/bison.h"

#line 75 "../bison++/bison.h"
 /* YY_SrvParser_LSP_NEEDED*/
#endif

/* DEFAULT LTYPE*/
#ifdef YY_SrvParser_LSP_NEEDED
 #ifndef YY_SrvParser_LTYPE
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
  #ifndef yylval
   extern YY_SrvParser_STYPE YY_SrvParser_LVAL;
  #else
   #if yylval != YY_SrvParser_LVAL
    extern YY_SrvParser_STYPE YY_SrvParser_LVAL;
   #else
    #warning "Namespace conflict, disabling some functionality (bison++ only)"
   #endif
  #endif
 #endif


 #line 169 "../bison++/bison.h"
#define	IFACE_	258
#define	RELAY_	259
#define	IFACE_ID_	260
#define	CLASS_	261
#define	TACLASS_	262
#define	LOGNAME_	263
#define	LOGLEVEL_	264
#define	LOGMODE_	265
#define	WORKDIR_	266
#define	OPTION_	267
#define	DNS_SERVER_	268
#define	DOMAIN_	269
#define	NTP_SERVER_	270
#define	TIME_ZONE_	271
#define	SIP_SERVER_	272
#define	SIP_DOMAIN_	273
#define	NIS_SERVER_	274
#define	NIS_DOMAIN_	275
#define	NISP_SERVER_	276
#define	NISP_DOMAIN_	277
#define	FQDN_	278
#define	LIFETIME_	279
#define	ACCEPT_ONLY_	280
#define	REJECT_CLIENTS_	281
#define	POOL_	282
#define	SHARE_	283
#define	T1_	284
#define	T2_	285
#define	PREF_TIME_	286
#define	VALID_TIME_	287
#define	UNICAST_	288
#define	PREFERENCE_	289
#define	RAPID_COMMIT_	290
#define	IFACE_MAX_LEASE_	291
#define	CLASS_MAX_LEASE_	292
#define	CLNT_MAX_LEASE_	293
#define	STATELESS_	294
#define	CACHE_SIZE_	295
#define	PDCLASS_	296
#define	PD_LENGTH_	297
#define	PD_POOL_	298
#define	VENDOR_SPEC_	299
#define	AUTH_	300
#define	DIGEST_NONE_	301
#define	DIGEST_HMAC_SHA1_	302
#define	CLIENT_	303
#define	EXPERIMENTAL_	304
#define	ADDR_PARAMS_	305
#define	STRING_	306
#define	HEXNUMBER_	307
#define	INTNUMBER_	308
#define	IPV6ADDR_	309
#define	DUID_	310


#line 169 "../bison++/bison.h"
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
  
 #line 212 "../bison++/bison.h"
static const int IFACE_;
static const int RELAY_;
static const int IFACE_ID_;
static const int CLASS_;
static const int TACLASS_;
static const int LOGNAME_;
static const int LOGLEVEL_;
static const int LOGMODE_;
static const int WORKDIR_;
static const int OPTION_;
static const int DNS_SERVER_;
static const int DOMAIN_;
static const int NTP_SERVER_;
static const int TIME_ZONE_;
static const int SIP_SERVER_;
static const int SIP_DOMAIN_;
static const int NIS_SERVER_;
static const int NIS_DOMAIN_;
static const int NISP_SERVER_;
static const int NISP_DOMAIN_;
static const int FQDN_;
static const int LIFETIME_;
static const int ACCEPT_ONLY_;
static const int REJECT_CLIENTS_;
static const int POOL_;
static const int SHARE_;
static const int T1_;
static const int T2_;
static const int PREF_TIME_;
static const int VALID_TIME_;
static const int UNICAST_;
static const int PREFERENCE_;
static const int RAPID_COMMIT_;
static const int IFACE_MAX_LEASE_;
static const int CLASS_MAX_LEASE_;
static const int CLNT_MAX_LEASE_;
static const int STATELESS_;
static const int CACHE_SIZE_;
static const int PDCLASS_;
static const int PD_LENGTH_;
static const int PD_POOL_;
static const int VENDOR_SPEC_;
static const int AUTH_;
static const int DIGEST_NONE_;
static const int DIGEST_HMAC_SHA1_;
static const int CLIENT_;
static const int EXPERIMENTAL_;
static const int ADDR_PARAMS_;
static const int STRING_;
static const int HEXNUMBER_;
static const int INTNUMBER_;
static const int IPV6ADDR_;
static const int DUID_;


#line 212 "../bison++/bison.h"
 /* decl const */
 #else
  enum YY_SrvParser_ENUM_TOKEN { YY_SrvParser_NULL_TOKEN=0
  
 #line 215 "../bison++/bison.h"
	,IFACE_=258
	,RELAY_=259
	,IFACE_ID_=260
	,CLASS_=261
	,TACLASS_=262
	,LOGNAME_=263
	,LOGLEVEL_=264
	,LOGMODE_=265
	,WORKDIR_=266
	,OPTION_=267
	,DNS_SERVER_=268
	,DOMAIN_=269
	,NTP_SERVER_=270
	,TIME_ZONE_=271
	,SIP_SERVER_=272
	,SIP_DOMAIN_=273
	,NIS_SERVER_=274
	,NIS_DOMAIN_=275
	,NISP_SERVER_=276
	,NISP_DOMAIN_=277
	,FQDN_=278
	,LIFETIME_=279
	,ACCEPT_ONLY_=280
	,REJECT_CLIENTS_=281
	,POOL_=282
	,SHARE_=283
	,T1_=284
	,T2_=285
	,PREF_TIME_=286
	,VALID_TIME_=287
	,UNICAST_=288
	,PREFERENCE_=289
	,RAPID_COMMIT_=290
	,IFACE_MAX_LEASE_=291
	,CLASS_MAX_LEASE_=292
	,CLNT_MAX_LEASE_=293
	,STATELESS_=294
	,CACHE_SIZE_=295
	,PDCLASS_=296
	,PD_LENGTH_=297
	,PD_POOL_=298
	,VENDOR_SPEC_=299
	,AUTH_=300
	,DIGEST_NONE_=301
	,DIGEST_HMAC_SHA1_=302
	,CLIENT_=303
	,EXPERIMENTAL_=304
	,ADDR_PARAMS_=305
	,STRING_=306
	,HEXNUMBER_=307
	,INTNUMBER_=308
	,IPV6ADDR_=309
	,DUID_=310


#line 215 "../bison++/bison.h"
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
 /* Removed due to bison problems
 /#ifndef YYSTYPE
 / #define YYSTYPE YY_SrvParser_STYPE
 /#endif*/

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

 #line 267 "../bison++/bison.h"
#endif
