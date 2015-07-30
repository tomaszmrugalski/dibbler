%name SrvParser

%header{
#include <iostream>
#include <string>
#include <stdint.h>
#include <sstream>
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "SrvParser.h"
#include "SrvParsGlobalOpt.h"
#include "SrvParsClassOpt.h"
#include "SrvParsIfaceOpt.h"
#include "OptAddr.h"
#include "OptAddrLst.h"
#include "OptDomainLst.h"
#include "OptString.h"
#include "OptVendorSpecInfo.h"
#include "OptRtPrefix.h"
#include "SrvOptAddrParams.h"
#include "SrvCfgMgr.h"
#include "SrvCfgTA.h"
#include "SrvCfgPD.h"
#include "SrvCfgClientClass.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "SrvCfgOptions.h"
#include "DUID.h"
#include "Logger.h"
#include "FQDN.h"
#include "Key.h"
#include "Node.h"
#include "NodeConstant.h"
#include "NodeClientSpecific.h"
#include "NodeOperator.h"

using namespace std;

#define YY_USE_CLASS
%}

%{
#include "FlexLexer.h"
%}

// class definition
%define MEMBERS FlexLexer * lex;                                                     \
List(TSrvParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */ \
List(TSrvCfgIface) SrvCfgIfaceLst;         /* list of SrvCfg interfaces */           \
List(TSrvCfgAddrClass) SrvCfgAddrClassLst; /* list of SrvCfg address classes */      \
List(TSrvCfgTA) SrvCfgTALst;               /* list of SrvCfg TA objects */           \
List(TSrvCfgPD) SrvCfgPDLst;		   /* list of SrvCfg PD objects */           \
List(TSrvCfgClientClass) SrvCfgClientClassLst; /* list of SrvCfgClientClass objs */  \
List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/ \
List(std::string) PresentStringLst;             /* string list */                    \
List(Node) NodeClientClassLst;             /* Node list */                           \
List(TFQDN) PresentFQDNLst;                                                          \
SPtr<TIPv6Addr> addr;                                                                \
SPtr<TSIGKey> CurrentKey;                                                            \
DigestTypesLst DigestLst;                                                            \
List(THostRange) PresentRangeLst;                                                    \
List(THostRange) PDLst;                                                              \
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
SPtr<TOpt> nextHop;                                                                  \
virtual ~SrvParser();

// constructor
%define CONSTRUCTOR_PARAM yyFlexLexer * lex
%define CONSTRUCTOR_CODE                                                \
                     ParserOptStack.append(new TSrvParsGlobalOpt());    \
                     this->lex = lex;                                   \
                     CfgMgr = 0;                                        \
                     nextHop.reset();                                   \
                     yynerrs = 0;                                       \
                     yychar = 0;                                        \
                     PDPrefix = 0;

%union
{
    unsigned int ival;
    char *strval;
    struct SDuid
    {
	int length;
	char* duid;
    } duidval;
    char addrval[16];
}

%token IFACE_, RELAY_, IFACE_ID_, IFACE_ID_ORDER_, CLASS_, TACLASS_
%token LOGNAME_, LOGLEVEL_, LOGMODE_, LOGCOLORS_, WORKDIR_
%token OPTION_, DNS_SERVER_,DOMAIN_, NTP_SERVER_,TIME_ZONE_, SIP_SERVER_, SIP_DOMAIN_
%token NIS_SERVER_, NIS_DOMAIN_, NISP_SERVER_, NISP_DOMAIN_, LIFETIME_
%token FQDN_, ACCEPT_UNKNOWN_FQDN_, FQDN_DDNS_ADDRESS_, DDNS_PROTOCOL_, DDNS_TIMEOUT_
%token ACCEPT_ONLY_,REJECT_CLIENTS_,POOL_, SHARE_
%token T1_,T2_,PREF_TIME_,VALID_TIME_
%token UNICAST_, DROP_UNICAST_, PREFERENCE_,RAPID_COMMIT_
%token IFACE_MAX_LEASE_, CLASS_MAX_LEASE_, CLNT_MAX_LEASE_
%token STATELESS_
%token CACHE_SIZE_
%token PDCLASS_, PD_LENGTH_, PD_POOL_
%token SCRIPT_
%token VENDOR_SPEC_
%token CLIENT_, DUID_KEYWORD_, REMOTE_ID_, LINK_LOCAL_, ADDRESS_, PREFIX_, GUESS_MODE_
%token INACTIVE_MODE_
%token EXPERIMENTAL_, ADDR_PARAMS_, REMOTE_AUTOCONF_NEIGHBORS_
%token AFTR_, PERFORMANCE_MODE_
%token AUTH_PROTOCOL_, AUTH_ALGORITHM_, AUTH_REPLAY_, AUTH_METHODS_
%token AUTH_DROP_UNAUTH_, AUTH_REALM_
%token KEY_, SECRET_, ALGORITHM_, FUDGE_
%token DIGEST_NONE_, DIGEST_PLAIN_, DIGEST_HMAC_MD5_, DIGEST_HMAC_SHA1_, DIGEST_HMAC_SHA224_
%token DIGEST_HMAC_SHA256_, DIGEST_HMAC_SHA384_, DIGEST_HMAC_SHA512_
%token ACCEPT_LEASEQUERY_
%token BULKLQ_ACCEPT_, BULKLQ_TCPPORT_, BULKLQ_MAX_CONNS_, BULKLQ_TIMEOUT_
%token CLIENT_CLASS_
%token MATCH_IF_
%token EQ_, AND_, OR_
%token CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_
%token CLIENT_VENDOR_SPEC_DATA_
%token CLIENT_VENDOR_CLASS_EN_
%token CLIENT_VENDOR_CLASS_DATA_
%token RECONFIGURE_ENABLED_
%token ALLOW_
%token DENY_
%token SUBSTRING_, STRING_KEYWORD_, ADDRESS_LIST_
%token CONTAIN_
%token NEXT_HOP_, ROUTE_, INFINITE_
%token SUBNET_

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
: GlobalOption
| InterfaceDeclaration
| GlobalDeclarationList GlobalOption
| GlobalDeclarationList InterfaceDeclaration
;

GlobalOption
: InterfaceOptionDeclaration
| LogModeOption
| LogLevelOption
| LogNameOption
| LogColors
| WorkDirOption
| StatelessOption
| CacheSizeOption
| AuthProtocol
| AuthAlgorithm
| AuthReplay
| AuthRealm
| AuthMethods
| AuthDropUnauthenticated
| Experimental
| IfaceIDOrder
| FqdnDdnsAddress
| DdnsProtocol
| DdnsTimeout
| GuessMode
| ClientClass
| Key
| ScriptName
| PerformanceMode
| ReconfigureEnabled
| DropUnicast
;

InterfaceOptionDeclaration
: ClassOptionDeclaration
| RelayOption
| InterfaceIDOption
| AcceptLeaseQuery
| BulkLeaseQueryAccept
| BulkLeaseQueryTcpPort
| BulkLeaseQueryMaxConns
| BulkLeaseQueryTimeout
| UnicastAddressOption
| PreferenceOption
| RapidCommitOption
| IfaceMaxLeaseOption
| ClntMaxLeaseOption
| DNSServerOption
| DomainOption
| NTPServerOption
| TimeZoneOption
| SIPServerOption
| SIPDomainOption
| FQDNOption
| AcceptUnknownFQDN
| NISServerOption
| NISDomainOption
| NISPServerOption
| NISPDomainOption
| DsLiteAftrName
| LifetimeOption
| ExtraOption
| RemoteAutoconfNeighborsOption
| PDDeclaration
| VendorSpecOption
| Client
| InactiveMode
| Subnet
;

InterfaceDeclaration
/* iface eth0 { ... } */
:IFACE_ STRING_ '{'
{
    if (!StartIfaceDeclaration($2))
	YYABORT;
}
InterfaceDeclarationsList '}'
{
    //Information about new interface has been read
    //Add it to list of read interfaces
    delete [] $2;
    EndIfaceDeclaration();
}
/* iface 5 { ... } */
|IFACE_ Number '{'
{
    if (!StartIfaceDeclaration($2))
	YYABORT;
}
InterfaceDeclarationsList '}'
{
    EndIfaceDeclaration();
}

InterfaceDeclarationsList
: InterfaceOptionDeclaration
| InterfaceDeclarationsList InterfaceOptionDeclaration
| ClassDeclaration
| TAClassDeclaration
| NextHopDeclaration
| Route
| InterfaceDeclarationsList TAClassDeclaration
| InterfaceDeclarationsList ClassDeclaration
| InterfaceDeclarationsList NextHopDeclaration
| InterfaceDeclarationsList Route
;

Key
: KEY_ STRING_ '{'
{
    /// this is key object initialization part
    CurrentKey = new TSIGKey(string($2));
} KeyOptions
'}'
{
    /// check that both secret and algorithm keywords were defined.
    Log(Debug) << "Loaded key '" << CurrentKey->Name_ << "', base64len is "
	       << CurrentKey->getBase64Data().length() << ", rawlen is "
	       << CurrentKey->getPackedData().length() << "." << LogEnd;
    if (CurrentKey->getPackedData().length() == 0) {
	Log(Crit) << "Key " << CurrentKey->Name_ << " does not have secret specified." << LogEnd;
	YYABORT;
    }

    if ( (CurrentKey->Digest_ != DIGEST_HMAC_MD5) &&
	 (CurrentKey->Digest_ != DIGEST_HMAC_SHA1) &&
	 (CurrentKey->Digest_ != DIGEST_HMAC_SHA256) ) {
	Log(Crit) << "Invalid key type specified: only hmac-md5, hmac-sha1 and "
                  << "hmac-sha256 are supported." << LogEnd;
	YYABORT;
    }
#if !defined(MOD_SRV_DISABLE_DNSUPDATE) && !defined(MOD_CLNT_DISABLE_DNSUPDATE)
    CfgMgr->addKey( CurrentKey );
#else
    Log(Crit) << "DNS Update disabled at compilation time. Can't specify TSIG key."
              << LogEnd;
#endif
} ';'
;

KeyOptions
:KeyOption
|KeyOptions KeyOption
;

KeyOption
:KeyAlgorithm
|KeySecret
|KeyFudge
;

KeySecret
: SECRET_ STRING_ ';'
{
    // store the key in base64 encoded form
    CurrentKey->setData(string($2));
};

KeyFudge
: FUDGE_ Number ';'
{
    CurrentKey->Fudge_ = $2;
}

KeyAlgorithm
: ALGORITHM_ DIGEST_HMAC_SHA256_ ';' { CurrentKey->Digest_ = DIGEST_HMAC_SHA256; }
| ALGORITHM_ DIGEST_HMAC_SHA1_   ';' { CurrentKey->Digest_ = DIGEST_HMAC_SHA1;  }
| ALGORITHM_ DIGEST_HMAC_MD5_    ';' { CurrentKey->Digest_ = DIGEST_HMAC_MD5;  }
;
/// add other key types here

Client
: CLIENT_ DUID_KEYWORD_ DUID_ '{'
{
    ParserOptStack.append(new TSrvParsGlobalOpt());
    SPtr<TDUID> duid = new TDUID($3.duid,$3.length);
    ClientLst.append(new TSrvCfgOptions(duid));
} ClientOptions
'}'
{
    Log(Debug) << "Exception: DUID-based exception specified." << LogEnd;
    // copy all defined options
    ClientLst.getLast()->setOptions(ParserOptStack.getLast());
    ParserOptStack.delLast();
}

| CLIENT_ REMOTE_ID_ Number '-' DUID_ '{'
{
    ParserOptStack.append(new TSrvParsGlobalOpt());
    SPtr<TOptVendorData> remoteid = new TOptVendorData($3, $5.duid, $5.length, 0);
    ClientLst.append(new TSrvCfgOptions(remoteid));
} ClientOptions
'}'
{
    Log(Debug) << "Exception: RemoteID-based exception specified." << LogEnd;
    // copy all defined options
    ClientLst.getLast()->setOptions(ParserOptStack.getLast());
    ParserOptStack.delLast();
}

| CLIENT_ LINK_LOCAL_ IPV6ADDR_ '{'
{
		ParserOptStack.append(new TSrvParsGlobalOpt());
		SPtr<TIPv6Addr> clntaddr = new TIPv6Addr($3);
		ClientLst.append(new TSrvCfgOptions(clntaddr));
} ClientOptions
'}'
{
		Log(Debug) << "Exception: Link-local-based exception specified." << LogEnd;
		// copy all defined options
		ClientLst.getLast()->setOptions(ParserOptStack.getLast());
		ParserOptStack.delLast();
};

ClientOptions
: ClientOption
| ClientOptions ClientOption
;

ClientOption
: DNSServerOption
| DomainOption
| NTPServerOption
| TimeZoneOption
| SIPServerOption
| SIPDomainOption
| NISServerOption
| NISDomainOption
| NISPServerOption
| NISPDomainOption
| LifetimeOption
| VendorSpecOption
| ExtraOption
| DsLiteAftrName
| AddressReservation
| PrefixReservation
;

AddressReservation:
ADDRESS_ IPV6ADDR_
{
    addr = new TIPv6Addr($2);
    Log(Info) << "Exception: Address " << addr->getPlain() << " reserved." << LogEnd;
    ClientLst.getLast()->setAddr(addr);
};

PrefixReservation:
PREFIX_ IPV6ADDR_ '/' Number
{
    addr = new TIPv6Addr($2);
    Log(Info) << "Exception: Prefix " << addr->getPlain() << "/" << $4 << " reserved." << LogEnd;
    ClientLst.getLast()->setPrefix(addr, $4);
}

/* class { ... } */
ClassDeclaration:
CLASS_ '{'
{
    StartClassDeclaration();
}
ClassOptionDeclarationsList '}'
{
    if (!EndClassDeclaration())
	YYABORT;
}
;

ClassOptionDeclarationsList
: ClassOptionDeclaration
| ClassOptionDeclarationsList ClassOptionDeclaration
;

/* ta-class { ... } */
TAClassDeclaration
:TACLASS_ '{'
{
    StartTAClassDeclaration();
} TAClassOptionsList '}'
{
    if (!EndTAClassDeclaration())
	YYABORT;
}
;

TAClassOptionsList
: TAClassOption
| TAClassOptionsList TAClassOption
;

TAClassOption
: PreferredTimeOption
| ValidTimeOption
| PoolOption
| ClassMaxLeaseOption
| RejectClientsOption
| AcceptOnlyOption
| AllowClientClassDeclaration
| DenyClientClassDeclaration
;

 PDDeclaration
:PDCLASS_ '{'
{
    StartPDDeclaration();
} PDOptionsList '}'
{
    if (!EndPDDeclaration())
	YYABORT;
}
;

PDOptionsList
: PDOptions
| PDOptions PDOptionsList

PDOptions
: PDLength
| PDPoolOption
| ValidTimeOption
| PreferredTimeOption
| T1Option
| T2Option
| AllowClientClassDeclaration
| DenyClientClassDeclaration
;

////////////////////////////////////////////////////////////
/// Route Option ///////////////////////////////////////////
////////////////////////////////////////////////////////////

NextHopDeclaration:
NEXT_HOP_ IPV6ADDR_ '{'
{
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr($2);
    SPtr<TOpt> myNextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);
    nextHop = myNextHop; 
}
RouteList '}'
{
    ParserOptStack.getLast()->addExtraOption(nextHop, false);
    nextHop.reset();
}
| NEXT_HOP_ IPV6ADDR_
{
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr($2);
    SPtr<TOpt> myNextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);
    ParserOptStack.getLast()->addExtraOption(myNextHop, false);
}
;

RouteList
: Route
| RouteList Route
;

Route:
ROUTE_ IPV6ADDR_ '/' INTNUMBER_ LIFETIME_ INTNUMBER_ 
{
    SPtr<TIPv6Addr> prefix = new TIPv6Addr($2);
    SPtr<TOpt> rtPrefix = new TOptRtPrefix($6, $4, 42, prefix, NULL);
    if (nextHop)
        nextHop->addOption(rtPrefix);
    else
        ParserOptStack.getLast()->addExtraOption(rtPrefix, false);
}
| ROUTE_ IPV6ADDR_ '/' INTNUMBER_
{
    SPtr<TIPv6Addr> prefix = new TIPv6Addr($2);
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, $4, 42, prefix, NULL);
    if (nextHop)
        nextHop->addOption(rtPrefix);
    else
        ParserOptStack.getLast()->addExtraOption(rtPrefix, false);
}
| ROUTE_ IPV6ADDR_ '/' INTNUMBER_ LIFETIME_ INFINITE_
{
    SPtr<TIPv6Addr> prefix = new TIPv6Addr($2);
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, $4, 42, prefix, NULL);
    if (nextHop)
        nextHop->addOption(rtPrefix);
    else
        ParserOptStack.getLast()->addExtraOption(rtPrefix, false);
};

AuthProtocol
: AUTH_PROTOCOL_ STRING_ {

#ifndef MOD_DISABLE_AUTH
    if (!strcasecmp($2,"none")) {
        CfgMgr->setAuthProtocol(AUTH_PROTO_NONE);
        CfgMgr->setAuthAlgorithm(AUTH_ALGORITHM_NONE);
    } else if (!strcasecmp($2, "delayed")) {
        CfgMgr->setAuthProtocol(AUTH_PROTO_DELAYED);
    } else if (!strcasecmp($2, "reconfigure-key")) {
        CfgMgr->setAuthProtocol(AUTH_PROTO_RECONFIGURE_KEY);
        CfgMgr->setAuthAlgorithm(AUTH_ALGORITHM_RECONFIGURE_KEY);
    } else if (!strcasecmp($2, "dibbler")) {
        CfgMgr->setAuthProtocol(AUTH_PROTO_DIBBLER);
    } else {
        Log(Crit) << "Invalid auth-protocol parameter: " << string($2) << LogEnd;
        YYABORT;
    }
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
};

AuthAlgorithm
: AUTH_ALGORITHM_ STRING_ {
    Log(Crit) << "auth-algorithm secification is not supported yet." << LogEnd;
    YYABORT;
};

AuthReplay
: AUTH_REPLAY_ STRING_ {

#ifndef MOD_DISABLE_AUTH
    if (strcasecmp($2, "none")) {
        CfgMgr->setAuthReplay(AUTH_REPLAY_NONE);
    } else if (strcasecmp($2, "monotonic")) {
        CfgMgr->setAuthReplay(AUTH_REPLAY_MONOTONIC);
    } else {
        Log(Crit) << "Invalid auth-replay parameter: " << string($2) << LogEnd;
        YYABORT;
    }
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif

};

AuthRealm
: AUTH_REALM_ STRING_ {
#ifndef MOD_DISABLE_AUTH
    CfgMgr->setAuthRealm(std::string($2));
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
};

AuthMethods
: AUTH_METHODS_
{
    DigestLst.clear();
} DigestList {
#ifndef MOD_DISABLE_AUTH
    CfgMgr->setAuthDigests(DigestLst);
    CfgMgr->setAuthDropUnauthenticated(true);
    DigestLst.clear();
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}

DigestList
: Digest
| DigestList ',' Digest
;

Digest
: DIGEST_NONE_        { DigestLst.push_back(DIGEST_NONE); }
| DIGEST_PLAIN_       { DigestLst.push_back(DIGEST_PLAIN); }
| DIGEST_HMAC_MD5_    { DigestLst.push_back(DIGEST_HMAC_MD5); }
| DIGEST_HMAC_SHA1_   { DigestLst.push_back(DIGEST_HMAC_SHA1); }
| DIGEST_HMAC_SHA224_ { DigestLst.push_back(DIGEST_HMAC_SHA224); }
| DIGEST_HMAC_SHA256_ { DigestLst.push_back(DIGEST_HMAC_SHA256); }
| DIGEST_HMAC_SHA384_ { DigestLst.push_back(DIGEST_HMAC_SHA384); }
| DIGEST_HMAC_SHA512_ { DigestLst.push_back(DIGEST_HMAC_SHA512); }
;


AuthDropUnauthenticated
: AUTH_DROP_UNAUTH_ Number {
#ifndef MOD_DISABLE_AUTH
    CfgMgr->setAuthDropUnauthenticated($2);
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Now Options and their parameters
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////
// Parameters for FQDN Options                   //
///////////////////////////////////////////////////

FQDNList
: STRING_
{
    Log(Notice)<< "FQDN: The client "<<$1<<" has no address nor DUID"<<LogEnd;
    PresentFQDNLst.append(new TFQDN($1,false));
}
| STRING_ '-' DUID_
{
    /// @todo: Use SPtr()
    TDUID* duidNew = new TDUID($3.duid,$3.length);
    Log(Debug)<< "FQDN:" << $1 <<" reserved for DUID " << duidNew->getPlain()<<LogEnd;
    PresentFQDNLst.append(new TFQDN(duidNew, $1,false));
}
| STRING_ '-' IPV6ADDR_
{
    addr = new TIPv6Addr($3);
    Log(Debug)<< "FQDN:" << $1 <<" reserved for address "<<*addr<<LogEnd;
    PresentFQDNLst.append(new TFQDN(new TIPv6Addr($3), $1,false));
}
| FQDNList ',' STRING_
{
	Log(Debug) << "FQDN:"<<$3<<" has no reservations (is available to everyone)."<<LogEnd;
    PresentFQDNLst.append(new TFQDN($3,false));
}
| FQDNList ',' STRING_ '-' DUID_
{
    TDUID* duidNew = new TDUID($5.duid,$5.length);
    Log(Debug)<< "FQDN:" << $3 << " reserved for DUID "<< duidNew->getPlain() << LogEnd;
    PresentFQDNLst.append(new TFQDN( duidNew, $3,false));
}
| FQDNList ',' STRING_ '-' IPV6ADDR_
{
    addr = new TIPv6Addr($5);
    Log(Debug)<< "FQDN:" << $3<<" reserved for address "<< addr->getPlain() << LogEnd;
    PresentFQDNLst.append(new TFQDN(new TIPv6Addr($5), $3,false));
}
;

Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;

ADDRESSList
: IPV6ADDR_
{
    PresentAddrLst.append(new TIPv6Addr($1));
}
| ADDRESSList ',' IPV6ADDR_
{
    PresentAddrLst.append(new TIPv6Addr($3));
}
;

VendorSpecList
: Number '-' Number '-' DUID_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
	       << $3 << ", valuelen=" << $5.length << LogEnd;

    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    $5.duid, $5.length, 0), false);
}
| Number '-' Number '-' IPV6ADDR_ 
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($5));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
               << $3 << ", value=" << addr->getPlain() << LogEnd;
    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    new TIPv6Addr($5), 0), false);
}
| Number '-' Number '-' STRING_ 
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
	       << $3 << ", valuelen=" << strlen($5) << LogEnd;

    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    $5, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' DUID_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
	       << $5 << ", valuelen=" << $7.length << LogEnd;
    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    $7.duid, $7.length, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' IPV6ADDR_ 
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($7));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
               << $5 << ", value=" << addr->getPlain() << LogEnd;
    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    addr, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' STRING_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
	       << $5 << ", valuelen=" << strlen($7) << LogEnd;
    ParserOptStack.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    $7, 0), false);
}
;

StringList
: STRING_ { PresentStringLst.append(SPtr<string> (new string($1))); }
| StringList ',' STRING_ { PresentStringLst.append(SPtr<string> (new string($3))); }
;

ADDRESSRangeList
    : IPV6ADDR_
    {
	PresentRangeLst.append(new THostRange(new TIPv6Addr($1),new TIPv6Addr($1)));
    }
    |   IPV6ADDR_ '-' IPV6ADDR_
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
	if (*addr1<=*addr2)
	    PresentRangeLst.append(new THostRange(addr1,addr2));
	else
	    PresentRangeLst.append(new THostRange(addr2,addr1));
    }
    |  IPV6ADDR_ '/' INTNUMBER_
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr($1));
	int prefix = $3;
	if ( (prefix<1) || (prefix>128)) {
	    Log(Crit) << "Invalid prefix defined: " << prefix << " in line " << lex->lineno()
		      << ". Allowed range: 1..128." << LogEnd;
	    YYABORT;
	}
	SPtr<TIPv6Addr> addr1 = this->getRangeMin($1, prefix);
	SPtr<TIPv6Addr> addr2 = this->getRangeMax($1, prefix);
	if (*addr1<=*addr2)
	    PresentRangeLst.append(new THostRange(addr1,addr2));
	else
	    PresentRangeLst.append(new THostRange(addr2,addr1));
    }
    | ADDRESSRangeList ',' IPV6ADDR_
    {
	PresentRangeLst.append(new THostRange(new TIPv6Addr($3),new TIPv6Addr($3)));
    }
    |   ADDRESSRangeList ',' IPV6ADDR_ '-' IPV6ADDR_
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
	if (*addr1<=*addr2)
	    PresentRangeLst.append(new THostRange(addr1,addr2));
	else
	    PresentRangeLst.append(new THostRange(addr2,addr1));
    }
;

PDRangeList
:   IPV6ADDR_ '/' INTNUMBER_
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr($1));
	int prefix = $3;
	if ( (prefix<1) || (prefix>128)) {
	    Log(Crit) << "Invalid prefix defined: " << prefix << " in line " << lex->lineno()
		      << ". Allowed range: 1..128." << LogEnd;
	    YYABORT;
	}

	SPtr<TIPv6Addr> addr1 = this->getRangeMin($1, prefix);
	SPtr<TIPv6Addr> addr2 = this->getRangeMax($1, prefix);
	SPtr<THostRange> range;
	if (*addr1<=*addr2)
	    range = new THostRange(addr1,addr2);
	else
	    range = new THostRange(addr2,addr1);
	range->setPrefixLength(prefix);
	PDLst.append(range);
    }
;

ADDRESSDUIDRangeList
: IPV6ADDR_
{
    PresentRangeLst.append(new THostRange(new TIPv6Addr($1),new TIPv6Addr($1)));
}
|   IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
    if (*addr1<=*addr2)
	PresentRangeLst.append(new THostRange(addr1,addr2));
    else
	PresentRangeLst.append(new THostRange(addr2,addr1));
}
| ADDRESSDUIDRangeList ',' IPV6ADDR_
{
    PresentRangeLst.append(new THostRange(new TIPv6Addr($3),new TIPv6Addr($3)));
}
|   ADDRESSDUIDRangeList ',' IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
    if (*addr1<=*addr2)
	PresentRangeLst.append(new THostRange(addr1,addr2));
    else
	PresentRangeLst.append(new THostRange(addr2,addr1));
}
| DUID_
{
    SPtr<TDUID> duid(new TDUID($1.duid, $1.length));
    PresentRangeLst.append(new THostRange(duid, duid));
    delete $1.duid;
}
| DUID_ '-' DUID_
{
    SPtr<TDUID> duid1(new TDUID($1.duid,$1.length));
    SPtr<TDUID> duid2(new TDUID($3.duid,$3.length));

    if (*duid1<=*duid2)
	PresentRangeLst.append(new THostRange(duid1,duid2));
    else
	PresentRangeLst.append(new THostRange(duid2,duid1));

    /// @todo: delete [] $1.duid; delete [] $3.duid?
}
| ADDRESSDUIDRangeList ',' DUID_
{
    SPtr<TDUID> duid(new TDUID($3.duid, $3.length));
    PresentRangeLst.append(new THostRange(duid, duid));
    delete $3.duid;
}
| ADDRESSDUIDRangeList ',' DUID_ '-' DUID_
{
    SPtr<TDUID> duid2(new TDUID($3.duid,$3.length));
    SPtr<TDUID> duid1(new TDUID($5.duid,$5.length));
    if (*duid1<=*duid2)
	PresentRangeLst.append(new THostRange(duid1,duid2));
    else
	PresentRangeLst.append(new THostRange(duid2,duid1));
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

PDPoolOption
: PD_POOL_
{
} PDRangeList
{
    ParserOptStack.getLast()->setPool(&PresentRangeLst/*PDList*/);
}
;
PDLength
: PD_LENGTH_ Number
{
    if ( (($2) > 128) || (($2) < 1) ) {
        Log(Crit) << "Invalid pd-length:" << $2 << ", allowed range is 1..128."
                  << LogEnd;
        YYABORT;
    }
   this->PDPrefix = $2;
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

ShareOption
: SHARE_ Number
{
    int x=$2;
    if ( (x<1) || (x>1000)) {
	Log(Crit) << "Invalid share value: " << x << " in line " << lex->lineno()
		  << ". Allowed range: 1..1000." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setShare(x);
}

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

AddrParams
: ADDR_PARAMS_ Number
{
    if (!ParserOptStack.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'addr-params' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }
    int bitfield = ADDRPARAMS_MASK_PREFIX;
    Log(Warning) << "Experimental addr-params added (prefix=" << $2
                 << ", bitfield=" << bitfield << ")." << LogEnd;
    ParserOptStack.getLast()->setAddrParams($2,bitfield);
};

DsLiteAftrName
: OPTION_ AFTR_ STRING_
{
    SPtr<TOpt> tunnelName = new TOptDomainLst(OPTION_AFTR_NAME, $3, 0);
    Log(Debug) << "Enabling DS-Lite tunnel option, AFTR name=" << $3 << LogEnd;
    ParserOptStack.getLast()->addExtraOption(tunnelName, false);
};

ExtraOption
:OPTION_ Number DUID_KEYWORD_ DUID_
{
    SPtr<TOpt> opt = new TOptGeneric($2, $4.duid, $4.length, 0);
    ParserOptStack.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", length="
               << $4.length << LogEnd;
}
|OPTION_ Number ADDRESS_ IPV6ADDR_
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($4));

    SPtr<TOpt> opt = new TOptAddr($2, addr, 0);
    ParserOptStack.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", address=" << addr->getPlain() << LogEnd;
}
|OPTION_ Number ADDRESS_LIST_
{
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> opt = new TOptAddrLst($2, PresentAddrLst, 0);
    ParserOptStack.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", address count="
               << PresentAddrLst.count() << LogEnd;
}
|OPTION_ Number STRING_KEYWORD_ STRING_
{
    SPtr<TOpt> opt = new TOptString($2, string($4), 0);
    ParserOptStack.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", string=" << $4 << LogEnd;
};

RemoteAutoconfNeighborsOption
:OPTION_ REMOTE_AUTOCONF_NEIGHBORS_
{
    if (!ParserOptStack.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'remote autoconf neighbors' defined, but "
		  << "experimental features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> opt = new TOptAddrLst(OPTION_NEIGHBORS, PresentAddrLst, 0);
    ParserOptStack.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Remote autoconf neighbors enabled (" << PresentAddrLst.count()
	       << " neighbors defined.)" << LogEnd;
}


IfaceMaxLeaseOption
: IFACE_MAX_LEASE_ Number
{
    ParserOptStack.getLast()->setIfaceMaxLease($2);
}
;

UnicastAddressOption
: UNICAST_ IPV6ADDR_
{
    ParserOptStack.getLast()->setUnicast(new TIPv6Addr($2));
}
;

DropUnicast
: DROP_UNICAST_
{
    CfgMgr->dropUnicast(true);
}

RapidCommitOption
:   RAPID_COMMIT_ Number
{
    if ( ($2!=0) && ($2!=1)) {
	Log(Crit) << "RAPID-COMMIT  parameter in line " << lex->lineno()
                  << " must have 0 or 1 value." << LogEnd;
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
	Log(Crit) << "Preference value (" << $2 << ") in line " << lex->lineno()
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

LogColors
: LOGCOLORS_ Number
{
    logger::setColors($2==1);
}

WorkDirOption
:   WORKDIR_ STRING_
{
    ParserOptStack.getLast()->setWorkDir($2);
}
;

StatelessOption
: STATELESS_
{
    ParserOptStack.getLast()->setStateless(true);
}
;

GuessMode
: GUESS_MODE_
{
    Log(Info) << "Guess-mode enabled: relay interfaces may be loosely "
              << "defined (matching interface-id is not mandatory)." << LogEnd;
    ParserOptStack.getLast()->setGuessMode(true);
};

ScriptName
: SCRIPT_ STRING_
{
    CfgMgr->setScriptName($2);
};

PerformanceMode
: PERFORMANCE_MODE_ Number
{
    if (!ParserOptStack.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'performance-mode' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    CfgMgr->setPerformanceMode($2);
};

ReconfigureEnabled
: RECONFIGURE_ENABLED_ Number
{
    switch ($2) {
    case 0:
    case 1:
        CfgMgr->setReconfigureSupport($2);
        break;
    default:
        Log(Crit) << "Invalid reconfigure-enabled value " << $2
                  << ", only 0 and 1 are supported." << LogEnd;
        YYABORT;
    }
};


InactiveMode
: INACTIVE_MODE_
{
    ParserOptStack.getLast()->setInactiveMode(true);
};

Experimental
: EXPERIMENTAL_
{
    Log(Crit) << "Experimental features are allowed." << LogEnd;
    ParserOptStack.getLast()->setExperimental(true);
};

IfaceIDOrder
:IFACE_ID_ORDER_ STRING_
{
    if (!strncasecmp($2,"before",6))
    {
		ParserOptStack.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_BEFORE);
    } else
    if (!strncasecmp($2,"after",5))
    {
		ParserOptStack.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_AFTER);
    } else
    if (!strncasecmp($2,"omit",4))
    {
		ParserOptStack.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_NONE);
    } else
    {
		Log(Crit) << "Invalid interface-id-order specified. Allowed "
                          << "values: before, after, omit" << LogEnd;
		YYABORT;
    }
};

CacheSizeOption
: CACHE_SIZE_ Number
{
    ParserOptStack.getLast()->setCacheSize($2);
}
;

////////////////////////////////////////////////////////////////////////
/// LEASE-QUERY (regular and bulk) /////////////////////////////////////
////////////////////////////////////////////////////////////////////////

AcceptLeaseQuery
: ACCEPT_LEASEQUERY_
{
    ParserOptStack.getLast()->setLeaseQuerySupport(true);

}
| ACCEPT_LEASEQUERY_ Number
{
    switch ($2) {
    case 0:
		ParserOptStack.getLast()->setLeaseQuerySupport(false);
		break;
    case 1:
		ParserOptStack.getLast()->setLeaseQuerySupport(true);
		break;
    default:
		Log(Crit) << "Invalid value of accept-leasequery specifed. Allowed "
                          << "values: 0, 1, yes, no, true, false" << LogEnd;
		YYABORT;
    }
};

BulkLeaseQueryAccept
: BULKLQ_ACCEPT_ Number
{
    if ($2!=0 && $2!=1) {
	Log(Error) << "Invalid bulk-leasequery-accept value: " << ($2)
		   << ", 0 or 1 expected." << LogEnd;
	YYABORT;
    }
    CfgMgr->bulkLQAccept( (bool) $2);
};

BulkLeaseQueryTcpPort
: BULKLQ_TCPPORT_ Number
{
    CfgMgr->bulkLQTcpPort( $2 );
}

BulkLeaseQueryMaxConns
: BULKLQ_MAX_CONNS_ Number
{
    CfgMgr->bulkLQMaxConns( $2 );
};

BulkLeaseQueryTimeout
: BULKLQ_TIMEOUT_ Number
{
    CfgMgr->bulkLQTimeout( $2 );
};

////////////////////////////////////////////////////////////////////////
/// RELAY //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
RelayOption
:RELAY_ STRING_
{
    ParserOptStack.getLast()->setRelayName($2);
}
|RELAY_ Number
{
    ParserOptStack.getLast()->setRelayID($2);
}
;

InterfaceIDOption
:IFACE_ID_ Number
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2, 0);
    ParserOptStack.getLast()->setRelayInterfaceID(id);
}
|IFACE_ID_ DUID_
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2.duid, $2.length, 0);
    ParserOptStack.getLast()->setRelayInterfaceID(id);
}
|IFACE_ID_ STRING_
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2, strlen($2), 0);
    ParserOptStack.getLast()->setRelayInterfaceID(id);
}
;

Subnet
:SUBNET_ IPV6ADDR_ '/' Number
{
    int prefix = $4;
    if ( (prefix<1) || (prefix>128) ) {
        Log(Crit) << "Invalid (1..128 allowed) prefix used: " << prefix
                  << " in subnet definition in line " << lex->lineno() << LogEnd;
        YYABORT;
    }
    SPtr<TIPv6Addr> min = getRangeMin($2, prefix);
    SPtr<TIPv6Addr> max = getRangeMax($2, prefix);
    SrvCfgIfaceLst.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "/" << $4
               << " on " << SrvCfgIfaceLst.getLast()->getFullName() << LogEnd;
}|SUBNET_ IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> min = new TIPv6Addr($2);
    SPtr<TIPv6Addr> max = new TIPv6Addr($4);
    SrvCfgIfaceLst.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "-" << max->getPlain()
               << "on " << SrvCfgIfaceLst.getLast()->getFullName() << LogEnd;
}

ClassOptionDeclaration
: PreferredTimeOption
| ValidTimeOption
| PoolOption
| ShareOption
| T1Option
| T2Option
| RejectClientsOption
| AcceptOnlyOption
| ClassMaxLeaseOption
| AddrParams
| AllowClientClassDeclaration
| DenyClientClassDeclaration
;

AllowClientClassDeclaration
: ALLOW_ STRING_
{
    SPtr<TSrvCfgClientClass> clntClass;
    bool found = false;
    SrvCfgClientClassLst.first();
    while (clntClass = SrvCfgClientClassLst.get())
    {
	if (clntClass->getClassName() == string($2))
	    found = true;
    }
    if (!found)
    {
	Log(Crit) << "Line " << lex->lineno()
		  << ": Unable to use class " << string($2) << ", no such class defined." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setAllowClientClass(string($2));

    int deny = ParserOptStack.getLast()->getDenyClientClassString().count();

    if (deny)
    {
	Log(Crit) << "Line " << lex->lineno() << ": Unable to define both allow and deny lists for this client class." << LogEnd;
	YYABORT;
    }

}

DenyClientClassDeclaration
: DENY_ STRING_
{
    SPtr<TSrvCfgClientClass> clntClass;
    bool found = false;
    SrvCfgClientClassLst.first();
    while (clntClass = SrvCfgClientClassLst.get())
    {
	if (clntClass->getClassName() == string($2))
	    found = true;
    }
    if (!found)
    {
	Log(Crit) << "Line " << lex->lineno()
		  << ": Unable to use class " << string($2) << ", no such class defined." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setDenyClientClass(string($2));

    int allow = ParserOptStack.getLast()->getAllowClientClassString().count();

    if (allow)
    {
	Log(Crit) << "Line " << lex->lineno() << ": Unable to define both allow and deny lists for this client class." << LogEnd;
	YYABORT;
    }

}

////////////////////////////////////////////////////////////////////////
/// DNS-server option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

DNSServerOption
:OPTION_ DNS_SERVER_
{
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_DNS_SERVERS, PresentAddrLst, NULL);
    ParserOptStack.getLast()->addExtraOption(nis_servers, false);
}
;

////////////////////////////////////////////////////////////////////////
/// DOMAIN option //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
DomainOption
: OPTION_ DOMAIN_ {
    PresentStringLst.clear();
} StringList
{
    SPtr<TOpt> domains = new TOptDomainLst(OPTION_DOMAIN_LIST, PresentStringLst, NULL);
    ParserOptStack.getLast()->addExtraOption(domains, false);
}
;

////////////////////////////////////////////////////////////////////////
/// NTP-SERVER option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
NTPServerOption
:OPTION_ NTP_SERVER_
{
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> ntp_servers = new TOptAddrLst(OPTION_SNTP_SERVERS, PresentAddrLst, NULL);
    ParserOptStack.getLast()->addExtraOption(ntp_servers, false);
    // ParserOptStack.getLast()->setNTPServerLst(&PresentAddrLst);
}
;

////////////////////////////////////////////////////////////////////////
/// TIME-ZONE option ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
TimeZoneOption
: OPTION_ TIME_ZONE_ STRING_
{
    SPtr<TOpt> timezone = new TOptString(OPTION_NEW_TZDB_TIMEZONE, string($3), NULL);
    ParserOptStack.getLast()->addExtraOption(timezone, false);
    // ParserOptStack.getLast()->setTimezone($3);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPServerOption
:OPTION_ SIP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> sip_servers = new TOptAddrLst(OPTION_SIP_SERVER_A, PresentAddrLst, NULL);
    ParserOptStack.getLast()->addExtraOption(sip_servers, false);
    // ParserOptStack.getLast()->setSIPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPDomainOption
:OPTION_ SIP_DOMAIN_ {
    PresentStringLst.clear();
} StringList
{
    SPtr<TOpt> sip_domains = new TOptDomainLst(OPTION_SIP_SERVER_D, PresentStringLst, NULL);
    ParserOptStack.getLast()->addExtraOption(sip_domains, false);
    //ParserOptStack.getLast()->setSIPDomainLst(&PresentStringLst);
}
;

//////////////////////////////////////////////////////////////////////
//FQDN option/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

FQDNOption
:OPTION_ FQDN_
{
    PresentFQDNLst.clear();
    Log(Debug)   << "No FQDNMode found, setting default mode 2 (all updates "
                 "executed by server)." << LogEnd;
    Log(Warning) << "revDNS zoneroot lenght not found, dynamic revDNS update "
                 "will not be possible." << LogEnd;
    ParserOptStack.getLast()->setFQDNMode(2);
    ParserOptStack.getLast()->setRevDNSZoneRootLength(0);
} FQDNList
{
    ParserOptStack.getLast()->setFQDNLst(&PresentFQDNLst);
}
|OPTION_ FQDN_ INTNUMBER_
{
    PresentFQDNLst.clear();
    Log(Debug)  << "FQDN: Setting update mode to " << $3;
    switch ($3) {
    case 0:
	Log(Cont) << "(no updates)" << LogEnd;
	break;
    case 1:
	Log(Cont) << "(client will update AAAA, server will update PTR)" << LogEnd;
	break;
    case 2:
	Log(Cont) << "(server will update both AAAA and PTR)" << LogEnd;
	break;
    default:
	Log(Cont) << LogEnd;
	Log(Crit) << "FQDN: Invalid mode. Only 0-2 are supported." << LogEnd;
        YYABORT;
    }
    Log(Warning)<< "FQDN: RevDNS zoneroot lenght not specified, dynamic revDNS update will not be possible." << LogEnd;
    ParserOptStack.getLast()->setFQDNMode($3);
    ParserOptStack.getLast()->setRevDNSZoneRootLength(0);
} FQDNList
{
    ParserOptStack.getLast()->setFQDNLst(&PresentFQDNLst);

}
|OPTION_ FQDN_ INTNUMBER_ INTNUMBER_
{
    PresentFQDNLst.clear();
    Log(Debug) << "FQDN: Setting update mode to " << $3;
    switch ($3) {
    case 0:
	Log(Cont) << "(no updates)" << LogEnd;
	break;
    case 1:
	Log(Cont) << "(client will update AAAA, server will update PTR)" << LogEnd;
	break;
    case 2:
	Log(Cont) << "(server will update both AAAA and PTR)" << LogEnd;
	break;
    default:
	Log(Cont) << LogEnd;
	Log(Crit) << "FQDN: Invalid mode. Only 0-2 are supported." << LogEnd;
        YYABORT;
    }

    Log(Debug) << "FQDN: RevDNS zoneroot lenght set to " << $4 <<LogEnd;
    if ( ($4 < 0) || ($4 > 128) ) {
	Log(Crit) << "FQDN: Invalid zoneroot length specified:" << $4
                  << ". Value 0-128 expected." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setFQDNMode($3);
    ParserOptStack.getLast()->setRevDNSZoneRootLength($4);
} FQDNList
{
    ParserOptStack.getLast()->setFQDNLst(&PresentFQDNLst);

}
;

AcceptUnknownFQDN
:ACCEPT_UNKNOWN_FQDN_ Number STRING_
{
    ParserOptStack.getLast()->setUnknownFQDN(EUnknownFQDNMode($2), string($3) );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << $2
               << ", domain=" << $3 << "." << LogEnd;
}
|ACCEPT_UNKNOWN_FQDN_ Number
{
    ParserOptStack.getLast()->setUnknownFQDN(EUnknownFQDNMode($2), string("") );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << $2
               << ", no domain." << LogEnd;
}
;

FqdnDdnsAddress
:FQDN_DDNS_ADDRESS_ IPV6ADDR_
{
    addr = new TIPv6Addr($2);
    CfgMgr->setDDNSAddress(addr);
    Log(Info) << "FQDN: DDNS updates will be performed to " << addr->getPlain() << "." << LogEnd;
};

DdnsProtocol
:DDNS_PROTOCOL_ STRING_
{
    if (!strcasecmp($2,"tcp"))
	CfgMgr->setDDNSProtocol(TCfgMgr::DNSUPDATE_TCP);
    else if (!strcasecmp($2,"udp"))
	CfgMgr->setDDNSProtocol(TCfgMgr::DNSUPDATE_UDP);
    else if (!strcasecmp($2,"any"))
	CfgMgr->setDDNSProtocol(TCfgMgr::DNSUPDATE_ANY);
    else {
        Log(Crit) << "Invalid ddns-protocol specifed:" << ($2) 
                  << ", supported values are tcp, udp, any." << LogEnd;
        YYABORT;
    }
    Log(Debug) << "DDNS: Setting protocol to " << ($2) << LogEnd;
};

DdnsTimeout
:DDNS_TIMEOUT_ Number
{
    Log(Debug) << "DDNS: Setting timeout to " << $2 << "ms." << LogEnd;
    CfgMgr->setDDNSTimeout($2);
}

//////////////////////////////////////////////////////////////////////
//NIS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISServerOption
:OPTION_ NIS_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_NIS_SERVERS, PresentAddrLst, NULL);
    ParserOptStack.getLast()->addExtraOption(nis_servers, false);
    ///ParserOptStack.getLast()->setNISServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-SERVER option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPServerOption
: OPTION_ NISP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> nisp_servers = new TOptAddrLst(OPTION_NISP_SERVERS, PresentAddrLst, NULL);
    ParserOptStack.getLast()->addExtraOption(nisp_servers, false);
    // ParserOptStack.getLast()->setNISPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NIS-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISDomainOption
:OPTION_ NIS_DOMAIN_ STRING_
{
    SPtr<TOpt> nis_domain = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, string($3), NULL);
    ParserOptStack.getLast()->addExtraOption(nis_domain, false);
    // ParserOptStack.getLast()->setNISDomain($3);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-DOMAIN option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPDomainOption
:OPTION_ NISP_DOMAIN_ STRING_
{
    SPtr<TOpt> nispdomain = new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, string($3), NULL);
    ParserOptStack.getLast()->addExtraOption(nispdomain, false);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-DOMAIN option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
LifetimeOption
:OPTION_ LIFETIME_ Number
{
    SPtr<TOpt> lifetime = new TOptInteger(OPTION_INFORMATION_REFRESH_TIME,
                                          OPTION_INFORMATION_REFRESH_TIME_LEN, 
                                          (uint32_t)($3), NULL);
    ParserOptStack.getLast()->addExtraOption(lifetime, false);
    //ParserOptStack.getLast()->setLifetime($3);
}
;

VendorSpecOption
:OPTION_ VENDOR_SPEC_ {
} VendorSpecList
{
    // ParserOptStack.getLast()->setVendorSpec(VendorSpec);
    // Log(Debug) << "Vendor-spec parsing finished" << LogEnd;
};


ClientClass
:CLIENT_CLASS_ STRING_ '{'
{
    Log(Notice) << "ClientClass found, name: " << string($2) << LogEnd;
} ClientClassDecleration   '}'
{
    SPtr<Node> cond =  NodeClientClassLst.getLast();
    SrvCfgClientClassLst.append( new TSrvCfgClientClass(string($2),cond));
    NodeClientClassLst.delLast();
}
;


ClientClassDecleration
: MATCH_IF_ Condition
{
}
;

Condition
: | '(' Expr CONTAIN_ Expr ')'
{
    SPtr<Node> r =  NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    SPtr<Node> l = NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    NodeClientClassLst.append(new NodeOperator(NodeOperator::OPERATOR_CONTAIN,l,r));
}
| '(' Expr EQ_ Expr ')'
{
    SPtr<Node> l =  NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    SPtr<Node> r = NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();

    NodeClientClassLst.append(new NodeOperator(NodeOperator::OPERATOR_EQUAL,l,r));
}
| '(' Condition  AND_  Condition ')'
{
    SPtr<Node> l =  NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    SPtr<Node> r = NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    NodeClientClassLst.append(new NodeOperator(NodeOperator::OPERATOR_AND,l,r));

}
| '(' Condition  OR_  Condition ')'
{
    SPtr<Node> l =  NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    SPtr<Node> r = NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    NodeClientClassLst.append(new NodeOperator(NodeOperator::OPERATOR_OR,l,r));
}
;

Expr
:CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_
{
    NodeClientClassLst.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM));
}
|CLIENT_VENDOR_SPEC_DATA_
{
    NodeClientClassLst.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_DATA));
}
| CLIENT_VENDOR_CLASS_EN_
{
    NodeClientClassLst.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM));
}
| CLIENT_VENDOR_CLASS_DATA_
{
    NodeClientClassLst.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_DATA));
}
| STRING_
{
    // Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    NodeClientClassLst.append(new NodeConstant(string($1)));
}
|Number
{
    //Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    stringstream convert;
    string snum;
    convert<<$1;
    convert>>snum;
    NodeClientClassLst.append(new NodeConstant(snum));
}
| SUBSTRING_ '(' Expr ',' Number ',' Number  ')'
{
    SPtr<Node> l =  NodeClientClassLst.getLast();
    NodeClientClassLst.delLast();
    NodeClientClassLst.append(new NodeOperator(NodeOperator::OPERATOR_SUBSTRING,l, $5,$7));
}
;
%%

/////////////////////////////////////////////////////////////////////////////
// programs section

/**
 * method check whether interface with id=ifaceNr has been already declared
 *
 * @param ifaceNr
 *
 * @return true if interface was not declared
 */
bool SrvParser::IfaceDefined(int ifaceNr)
{
  SPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) {
	Log(Crit) << "Interface with ID=" << ifaceNr << " is already defined." << LogEnd;
	return false;
    }
  return true;
}

/**
 * check whether interface with id=ifaceName has been already declared
 *
 * @param ifaceName
 *
 * @return true, if defined, false otherwise
 */
bool SrvParser::IfaceDefined(string ifaceName)
{
  SPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) {
	Log(Crit) << "Interface " << ifaceName << " is already defined." << LogEnd;
	return false;
    }
  }
  return true;
}

/**
 * method creates new option for just started interface scope
 * clears all lists except the list of interfaces and adds new group
 *
 */
bool SrvParser::StartIfaceDeclaration(string ifaceName)
{
    if (!IfaceDefined(ifaceName))
	return false;

    SrvCfgIfaceLst.append(new TSrvCfgIface(ifaceName));

    // create new option (representing this interface) on the parser stack
    ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
    SrvCfgAddrClassLst.clear();
    ClientLst.clear();

    return true;
}

/**
 * method creates new option for just started interface scope
 * clears all lists except the list of interfaces and adds new group
 *
 */
bool SrvParser::StartIfaceDeclaration(int ifindex)
{
    if (!IfaceDefined(ifindex))
	return false;

    SrvCfgIfaceLst.append(new TSrvCfgIface(ifindex));

    // create new option (representing this interface) on the parser stack
    ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
    SrvCfgAddrClassLst.clear();
    ClientLst.clear();

    return true;
}


/**
 * this method is called after inteface declaration has ended. It creates
 * new interface representation used in SrvCfgMgr. Also removes corresponding
 * element from the parser stack
 *
 * @return true if everything is ok
 */
bool SrvParser::EndIfaceDeclaration()
{
    // get this interface object
    SPtr<TSrvCfgIface> iface = SrvCfgIfaceLst.getLast();

    // set its options
    SrvCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());

    // copy all IA objects
    SPtr<TSrvCfgAddrClass> ptrAddrClass;
    SrvCfgAddrClassLst.first();
    while (ptrAddrClass=SrvCfgAddrClassLst.get())
	iface->addAddrClass(ptrAddrClass);
    SrvCfgAddrClassLst.clear();

    // copy all TA objects
    SPtr<TSrvCfgTA> ta;
    SrvCfgTALst.first();
    while (ta=SrvCfgTALst.get())
	iface->addTA(ta);
    SrvCfgTALst.clear();

    SPtr<TSrvCfgPD> pd;
    SrvCfgPDLst.first();
    while (pd=SrvCfgPDLst.get())
	iface->addPD(pd);
    SrvCfgPDLst.clear();

    iface->addClientExceptionsLst(ClientLst);

    // remove last option (representing this interface) from the parser stack
    ParserOptStack.delLast();

    return true;
}

void SrvParser::StartClassDeclaration()
{
    ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
    SrvCfgAddrClassLst.append(new TSrvCfgAddrClass());
}

/**
 * this method is adds new object representig just parsed IA class.
 *
 * @return true if everything works ok.
 */
bool SrvParser::EndClassDeclaration()
{
    if (!ParserOptStack.getLast()->countPool()) {
	Log(Crit) << "No pools defined for this class." << LogEnd;
	return false;
    }
    //setting interface options on the basis of just read information
    SrvCfgAddrClassLst.getLast()->setOptions(ParserOptStack.getLast());
    ParserOptStack.delLast();

    return true;
}


/**
 * Just add global options
 *
 */
void SrvParser::StartTAClassDeclaration()
{
  ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
}

bool SrvParser::EndTAClassDeclaration()
{
    if (!ParserOptStack.getLast()->countPool()) {
	Log(Crit) << "No pools defined for this ta-class." << LogEnd;
	return false;
    }
    // create new object representing just parsed TA and add it to the list
    SPtr<TSrvCfgTA> ptrTA = new TSrvCfgTA();
    ptrTA->setOptions(ParserOptStack.getLast());
    SrvCfgTALst.append(ptrTA);

    // remove temporary parser object for this (just finished) scope
    ParserOptStack.delLast();
    return true;
}

void SrvParser::StartPDDeclaration()
{
    ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
    this->PDLst.clear();
    this->PDPrefix = 0;
}

bool SrvParser::EndPDDeclaration()
{
    if (!this->PDLst.count()) {
	Log(Crit) << "No PD pools defined ." << LogEnd;
	return false;
    }
    if (!this->PDPrefix) {
	Log(Crit) << "PD prefix length not defined or set to 0." << LogEnd;
	return false;
    }

    int len = 0;
    this->PDLst.first();
    while ( SPtr<THostRange> pool = PDLst.get() ) {
	if (!len)
	    len = pool->getPrefixLength();
	if (len!=pool->getPrefixLength()) {
	    Log(Crit) << "Prefix pools with different lengths are not supported. "
                "Make sure that all 'pd-pool' uses the same prefix length." << LogEnd;
	    return false;
	}
    }
    if (len>PDPrefix) {
	Log(Crit) << "Clients are supposed to get /" << this->PDPrefix << " prefixes,"
                  << "but pd-pool(s) are only /" << len << " long." << LogEnd;
	return false;
    }
    if (len==PDPrefix) {
	Log(Warning) << "Prefix pool /" << PDPrefix << " defined and clients are "
            "supposed to get /" << len << " prefixes. Only ONE client will get "
            "prefix" << LogEnd;
    }

    SPtr<TSrvCfgPD> ptrPD = new TSrvCfgPD();
    ParserOptStack.getLast()->setPool(&this->PDLst);
    if (!ptrPD->setOptions(ParserOptStack.getLast(), this->PDPrefix))
	return false;
    SrvCfgPDLst.append(ptrPD);

    // remove temporary parser object for this (just finished) scope
    ParserOptStack.delLast();
    return true;
}

namespace std {
    extern yy_SrvParser_stype yylval;
}

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
    ParserOptStack.clear();
    SrvCfgIfaceLst.clear();
    SrvCfgAddrClassLst.clear();
    SrvCfgTALst.clear();
    PresentAddrLst.clear();
    PresentStringLst.clear();
    PresentRangeLst.clear();
}

static char bitMask[]= { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

SPtr<TIPv6Addr> SrvParser::getRangeMin(char * addrPacked, int prefix) {
    char packed[16];
    char mask;
    memcpy(packed, addrPacked, 16);
    if (prefix%8!=0) {
	mask = bitMask[prefix%8];
	packed[prefix/8] = packed[prefix/8] & mask;
	prefix = (prefix/8 + 1)*8;
    }
    for (int i=prefix/8;i<16; i++) {
	packed[i]=0;
    }
    return new TIPv6Addr(packed, false);
}

SPtr<TIPv6Addr> SrvParser::getRangeMax(char * addrPacked, int prefix){
    char packed[16];
    char mask;
    memcpy(packed, addrPacked,16);
    if (prefix%8!=0) {
	mask = bitMask[prefix%8];
	packed[prefix/8] = packed[prefix/8] | ~mask;
	prefix = (prefix/8 + 1)*8;
    }
    for (int i=prefix/8;i<16; i++) {
	packed[i]=0xff;
    }

    return new TIPv6Addr(packed, false);
}
