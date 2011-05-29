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
#include "OptAddr.h"
#include "OptAddrLst.h"
#include "OptString.h"
#include "SrvCfgMgr.h"
#include "SrvCfgTA.h"
#include "SrvCfgPD.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "SrvCfgOptions.h"
#include "DUID.h"
#include "Logger.h"
#include "FQDN.h"
#include "OptVendorSpecInfo.h"
#include "SrvOptAddrParams.h"
#include "Portable.h"
#include "SrvCfgClientClass.h"
#include "Node.h"
#include "NodeConstant.h"
#include "NodeClientSpecific.h"
#include "NodeOperator.h"
#include "CfgMgr.h"
#include <sstream>

#define YY_USE_CLASS
#define YY_SrvParser_MEMBERS  FlexLexer * lex;                                                     \
List(TSrvParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */ \
List(TSrvCfgIface) SrvCfgIfaceLst;         /* list of SrvCfg interfaces */           \
List(TSrvCfgAddrClass) SrvCfgAddrClassLst; /* list of SrvCfg address classes */      \
List(TSrvCfgTA) SrvCfgTALst;               /* list of SrvCfg TA objects */           \
List(TSrvCfgPD) SrvCfgPDLst;		   /* list of SrvCfg PD objects */           \
List(TSrvCfgClientClass) SrvCfgClientClassLst; /* list of SrvCfgClientClass objs */  \
List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/ \
List(string) PresentStringLst;             /* string list */                         \
List(Node) NodeClientClassLst;             /* Node list */                           \
List(TFQDN) PresentFQDNLst;                                                          \
SPtr<TIPv6Addr> addr;                                                                \
List(TStationRange) PresentRangeLst;                                                 \
List(TStationRange) PDLst;                                                           \
List(TSrvCfgOptions) ClientLst;                                                      \
int PDPrefix;                                                                        \
bool IfaceDefined(int ifaceNr);                                                      \
bool IfaceDefined(string ifaceName);                                                 \
bool StartIfaceDeclaration(string iface);                                            \
bool StartIfaceDeclaration(int ifindex);                                             \
bool EndIfaceDeclaration();                                                          \
void StartClassDeclaration();                                                        \
bool EndClassDeclaration();                                                          \
SPtr<TIPv6Addr> getRangeMin(char * addrPacked, int prefix);                          \
SPtr<TIPv6Addr> getRangeMax(char * addrPacked, int prefix);                          \
void StartTAClassDeclaration();                                                      \
bool EndTAClassDeclaration();                                                        \
void StartPDDeclaration();                                                           \
bool EndPDDeclaration();                                                             \
TSrvCfgMgr * CfgMgr;                                                                 \
virtual ~SrvParser();
#define YY_SrvParser_CONSTRUCTOR_PARAM  yyFlexLexer * lex
#define YY_SrvParser_CONSTRUCTOR_CODE                                                           \
    ParserOptStack.append(new TSrvParsGlobalOpt());                               \
    this->lex = lex;

#line 83 "SrvParser.y"
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
#define	LOGCOLORS_	267
#define	WORKDIR_	268
#define	OPTION_	269
#define	DNS_SERVER_	270
#define	DOMAIN_	271
#define	NTP_SERVER_	272
#define	TIME_ZONE_	273
#define	SIP_SERVER_	274
#define	SIP_DOMAIN_	275
#define	NIS_SERVER_	276
#define	NIS_DOMAIN_	277
#define	NISP_SERVER_	278
#define	NISP_DOMAIN_	279
#define	LIFETIME_	280
#define	FQDN_	281
#define	ACCEPT_UNKNOWN_FQDN_	282
#define	FQDN_DDNS_ADDRESS_	283
#define	DDNS_PROTOCOL_	284
#define	DDNS_TIMEOUT_	285
#define	ACCEPT_ONLY_	286
#define	REJECT_CLIENTS_	287
#define	POOL_	288
#define	SHARE_	289
#define	T1_	290
#define	T2_	291
#define	PREF_TIME_	292
#define	VALID_TIME_	293
#define	UNICAST_	294
#define	PREFERENCE_	295
#define	RAPID_COMMIT_	296
#define	IFACE_MAX_LEASE_	297
#define	CLASS_MAX_LEASE_	298
#define	CLNT_MAX_LEASE_	299
#define	STATELESS_	300
#define	CACHE_SIZE_	301
#define	PDCLASS_	302
#define	PD_LENGTH_	303
#define	PD_POOL_	304
#define	VENDOR_SPEC_	305
#define	CLIENT_	306
#define	DUID_KEYWORD_	307
#define	REMOTE_ID_	308
#define	ADDRESS_	309
#define	GUESS_MODE_	310
#define	INACTIVE_MODE_	311
#define	EXPERIMENTAL_	312
#define	ADDR_PARAMS_	313
#define	REMOTE_AUTOCONF_NEIGHBORS_	314
#define	AFTR_	315
#define	AUTH_METHOD_	316
#define	AUTH_LIFETIME_	317
#define	AUTH_KEY_LEN_	318
#define	DIGEST_NONE_	319
#define	DIGEST_PLAIN_	320
#define	DIGEST_HMAC_MD5_	321
#define	DIGEST_HMAC_SHA1_	322
#define	DIGEST_HMAC_SHA224_	323
#define	DIGEST_HMAC_SHA256_	324
#define	DIGEST_HMAC_SHA384_	325
#define	DIGEST_HMAC_SHA512_	326
#define	ACCEPT_LEASEQUERY_	327
#define	BULKLQ_ACCEPT_	328
#define	BULKLQ_TCPPORT_	329
#define	BULKLQ_MAX_CONNS_	330
#define	BULKLQ_TIMEOUT_	331
#define	CLIENT_CLASS_	332
#define	MATCH_IF_	333
#define	EQ_	334
#define	AND_	335
#define	OR_	336
#define	CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_	337
#define	CLIENT_VENDOR_SPEC_DATA_	338
#define	CLIENT_VENDOR_CLASS_EN_	339
#define	CLIENT_VENDOR_CLASS_DATA_	340
#define	ALLOW_	341
#define	DENY_	342
#define	SUBSTRING_	343
#define	STRING_KEYWORD_	344
#define	ADDRESS_LIST_	345
#define	CONTAIN_	346
#define	STRING_	347
#define	HEXNUMBER_	348
#define	INTNUMBER_	349
#define	IPV6ADDR_	350
#define	DUID_	351


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
static const int LOGCOLORS_;
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
static const int LIFETIME_;
static const int FQDN_;
static const int ACCEPT_UNKNOWN_FQDN_;
static const int FQDN_DDNS_ADDRESS_;
static const int DDNS_PROTOCOL_;
static const int DDNS_TIMEOUT_;
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
static const int REMOTE_AUTOCONF_NEIGHBORS_;
static const int AFTR_;
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
static const int BULKLQ_ACCEPT_;
static const int BULKLQ_TCPPORT_;
static const int BULKLQ_MAX_CONNS_;
static const int BULKLQ_TIMEOUT_;
static const int CLIENT_CLASS_;
static const int MATCH_IF_;
static const int EQ_;
static const int AND_;
static const int OR_;
static const int CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_;
static const int CLIENT_VENDOR_SPEC_DATA_;
static const int CLIENT_VENDOR_CLASS_EN_;
static const int CLIENT_VENDOR_CLASS_DATA_;
static const int ALLOW_;
static const int DENY_;
static const int SUBSTRING_;
static const int STRING_KEYWORD_;
static const int ADDRESS_LIST_;
static const int CONTAIN_;
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
	,LOGCOLORS_=267
	,WORKDIR_=268
	,OPTION_=269
	,DNS_SERVER_=270
	,DOMAIN_=271
	,NTP_SERVER_=272
	,TIME_ZONE_=273
	,SIP_SERVER_=274
	,SIP_DOMAIN_=275
	,NIS_SERVER_=276
	,NIS_DOMAIN_=277
	,NISP_SERVER_=278
	,NISP_DOMAIN_=279
	,LIFETIME_=280
	,FQDN_=281
	,ACCEPT_UNKNOWN_FQDN_=282
	,FQDN_DDNS_ADDRESS_=283
	,DDNS_PROTOCOL_=284
	,DDNS_TIMEOUT_=285
	,ACCEPT_ONLY_=286
	,REJECT_CLIENTS_=287
	,POOL_=288
	,SHARE_=289
	,T1_=290
	,T2_=291
	,PREF_TIME_=292
	,VALID_TIME_=293
	,UNICAST_=294
	,PREFERENCE_=295
	,RAPID_COMMIT_=296
	,IFACE_MAX_LEASE_=297
	,CLASS_MAX_LEASE_=298
	,CLNT_MAX_LEASE_=299
	,STATELESS_=300
	,CACHE_SIZE_=301
	,PDCLASS_=302
	,PD_LENGTH_=303
	,PD_POOL_=304
	,VENDOR_SPEC_=305
	,CLIENT_=306
	,DUID_KEYWORD_=307
	,REMOTE_ID_=308
	,ADDRESS_=309
	,GUESS_MODE_=310
	,INACTIVE_MODE_=311
	,EXPERIMENTAL_=312
	,ADDR_PARAMS_=313
	,REMOTE_AUTOCONF_NEIGHBORS_=314
	,AFTR_=315
	,AUTH_METHOD_=316
	,AUTH_LIFETIME_=317
	,AUTH_KEY_LEN_=318
	,DIGEST_NONE_=319
	,DIGEST_PLAIN_=320
	,DIGEST_HMAC_MD5_=321
	,DIGEST_HMAC_SHA1_=322
	,DIGEST_HMAC_SHA224_=323
	,DIGEST_HMAC_SHA256_=324
	,DIGEST_HMAC_SHA384_=325
	,DIGEST_HMAC_SHA512_=326
	,ACCEPT_LEASEQUERY_=327
	,BULKLQ_ACCEPT_=328
	,BULKLQ_TCPPORT_=329
	,BULKLQ_MAX_CONNS_=330
	,BULKLQ_TIMEOUT_=331
	,CLIENT_CLASS_=332
	,MATCH_IF_=333
	,EQ_=334
	,AND_=335
	,OR_=336
	,CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_=337
	,CLIENT_VENDOR_SPEC_DATA_=338
	,CLIENT_VENDOR_CLASS_EN_=339
	,CLIENT_VENDOR_CLASS_DATA_=340
	,ALLOW_=341
	,DENY_=342
	,SUBSTRING_=343
	,STRING_KEYWORD_=344
	,ADDRESS_LIST_=345
	,CONTAIN_=346
	,STRING_=347
	,HEXNUMBER_=348
	,INTNUMBER_=349
	,IPV6ADDR_=350
	,DUID_=351


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
