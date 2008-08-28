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
#include "Portable.h"
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
List(TSrvOptGeneric) ExtraOpts;                                                      \
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

#line 74 "SrvParser.y"
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
#define	IFACE_ID_ORDER_	261
#define	CLASS_	262
#define	TACLASS_	263
#define	LOGNAME_	264
#define	LOGLEVEL_	265
#define	LOGMODE_	266
#define	WORKDIR_	267
#define	OPTION_	268
#define	DNS_SERVER_	269
#define	DOMAIN_	270
#define	NTP_SERVER_	271
#define	TIME_ZONE_	272
#define	SIP_SERVER_	273
#define	SIP_DOMAIN_	274
#define	NIS_SERVER_	275
#define	NIS_DOMAIN_	276
#define	NISP_SERVER_	277
#define	NISP_DOMAIN_	278
#define	FQDN_	279
#define	LIFETIME_	280
#define	ACCEPT_ONLY_	281
#define	REJECT_CLIENTS_	282
#define	POOL_	283
#define	SHARE_	284
#define	T1_	285
#define	T2_	286
#define	PREF_TIME_	287
#define	VALID_TIME_	288
#define	UNICAST_	289
#define	PREFERENCE_	290
#define	RAPID_COMMIT_	291
#define	IFACE_MAX_LEASE_	292
#define	CLASS_MAX_LEASE_	293
#define	CLNT_MAX_LEASE_	294
#define	STATELESS_	295
#define	CACHE_SIZE_	296
#define	PDCLASS_	297
#define	PD_LENGTH_	298
#define	PD_POOL_	299
#define	VENDOR_SPEC_	300
#define	CLIENT_	301
#define	DUID_KEYWORD_	302
#define	REMOTE_ID_	303
#define	ADDRESS_	304
#define	GUESS_MODE_	305
#define	INACTIVE_MODE_	306
#define	EXPERIMENTAL_	307
#define	ADDR_PARAMS_	308
#define	TUNNEL_MODE_	309
#define	EXTRA_	310
#define	AUTH_METHOD_	311
#define	AUTH_LIFETIME_	312
#define	AUTH_KEY_LEN_	313
#define	DIGEST_NONE_	314
#define	DIGEST_PLAIN_	315
#define	DIGEST_HMAC_MD5_	316
#define	DIGEST_HMAC_SHA1_	317
#define	DIGEST_HMAC_SHA224_	318
#define	DIGEST_HMAC_SHA256_	319
#define	DIGEST_HMAC_SHA384_	320
#define	DIGEST_HMAC_SHA512_	321
#define	ACCEPT_LEASEQUERY_	322
#define	STRING_	323
#define	HEXNUMBER_	324
#define	INTNUMBER_	325
#define	IPV6ADDR_	326
#define	DUID_	327


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
static const int IFACE_ID_ORDER_;
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
static const int CLIENT_;
static const int DUID_KEYWORD_;
static const int REMOTE_ID_;
static const int ADDRESS_;
static const int GUESS_MODE_;
static const int INACTIVE_MODE_;
static const int EXPERIMENTAL_;
static const int ADDR_PARAMS_;
static const int TUNNEL_MODE_;
static const int EXTRA_;
static const int AUTH_METHOD_;
static const int AUTH_LIFETIME_;
static const int AUTH_KEY_LEN_;
static const int DIGEST_NONE_;
static const int DIGEST_PLAIN_;
static const int DIGEST_HMAC_MD5_;
static const int DIGEST_HMAC_SHA1_;
static const int DIGEST_HMAC_SHA224_;
static const int DIGEST_HMAC_SHA256_;
static const int DIGEST_HMAC_SHA384_;
static const int DIGEST_HMAC_SHA512_;
static const int ACCEPT_LEASEQUERY_;
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
	,IFACE_ID_ORDER_=261
	,CLASS_=262
	,TACLASS_=263
	,LOGNAME_=264
	,LOGLEVEL_=265
	,LOGMODE_=266
	,WORKDIR_=267
	,OPTION_=268
	,DNS_SERVER_=269
	,DOMAIN_=270
	,NTP_SERVER_=271
	,TIME_ZONE_=272
	,SIP_SERVER_=273
	,SIP_DOMAIN_=274
	,NIS_SERVER_=275
	,NIS_DOMAIN_=276
	,NISP_SERVER_=277
	,NISP_DOMAIN_=278
	,FQDN_=279
	,LIFETIME_=280
	,ACCEPT_ONLY_=281
	,REJECT_CLIENTS_=282
	,POOL_=283
	,SHARE_=284
	,T1_=285
	,T2_=286
	,PREF_TIME_=287
	,VALID_TIME_=288
	,UNICAST_=289
	,PREFERENCE_=290
	,RAPID_COMMIT_=291
	,IFACE_MAX_LEASE_=292
	,CLASS_MAX_LEASE_=293
	,CLNT_MAX_LEASE_=294
	,STATELESS_=295
	,CACHE_SIZE_=296
	,PDCLASS_=297
	,PD_LENGTH_=298
	,PD_POOL_=299
	,VENDOR_SPEC_=300
	,CLIENT_=301
	,DUID_KEYWORD_=302
	,REMOTE_ID_=303
	,ADDRESS_=304
	,GUESS_MODE_=305
	,INACTIVE_MODE_=306
	,EXPERIMENTAL_=307
	,ADDR_PARAMS_=308
	,TUNNEL_MODE_=309
	,EXTRA_=310
	,AUTH_METHOD_=311
	,AUTH_LIFETIME_=312
	,AUTH_KEY_LEN_=313
	,DIGEST_NONE_=314
	,DIGEST_PLAIN_=315
	,DIGEST_HMAC_MD5_=316
	,DIGEST_HMAC_SHA1_=317
	,DIGEST_HMAC_SHA224_=318
	,DIGEST_HMAC_SHA256_=319
	,DIGEST_HMAC_SHA384_=320
	,DIGEST_HMAC_SHA512_=321
	,ACCEPT_LEASEQUERY_=322
	,STRING_=323
	,HEXNUMBER_=324
	,INTNUMBER_=325
	,IPV6ADDR_=326
	,DUID_=327


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
