%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.0"
%defines
%define api.parser.class {SrvParser}
%define api.namespace {dibbler}
%define parse.assert

 // %define api.value.type union
     
 //%define api.token.constructor
 //%define api.value.type variant


 // this will be inserted in the .h file.
%code requires {
class SrvParserContext;

}
     
// this will be inserted in the .cc file.
%code {
#include <iostream>
#include <string>
#include <stdint.h>
#include <sstream>
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "SrvParser.h"
// #include "SrvParsGlobalOpt.h"
#include "SrvParsClassOpt.h"
#include "SrvParsIfaceOpt.h"
#include "OptAddr.h"
//#include "OptAddrLst.h"
#include "OptDomainLst.h"
#include "OptString.h"
#include "OptVendorSpecInfo.h"
#include "OptRtPrefix.h"
#include "SrvOptAddrParams.h"
//#include "SrvCfgMgr.h"
//#include "SrvCfgTA.h"
//#include "SrvCfgPD.h"
#include "SrvCfgClientClass.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "SrvCfgOptions.h"
#include "DUID.h"
#include "Logger.h"
#include "FQDN.h"
//#include "Key.h"
#include "Node.h"
#include "NodeConstant.h"
#include "NodeClientSpecific.h"
#include "NodeOperator.h"


using namespace std;

//#define YY_USE_CLASS


//#include <FlexLexer.h>

}

// class definition

// Define constructor parameters
%param { SrvParserContext& ctx }

%locations
%define parse.trace
%define parse.error verbose
%code
{
#include "SrvParserContext.h"
}


//%define CONSTRUCTOR_CODE                                                \
//                     ParserOptStack.append(new TSrvParsGlobalOpt());    \
//                     this->lex = lex;                                   \
//                     CfgMgr = 0;                                        \
//                     nextHop.reset();                                   \
//                     yynerrs = 0;                                       \
//                     yychar = 0;                                        \
//                     PDPrefix = 0;

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

%define api.token.prefix {TOKEN_}

%token IFACE_
%token RELAY_
%token IFACE_ID_
%token IFACE_ID_ORDER_
%token CLASS_
%token TACLASS_
%token LOGNAME_
%token LOGLEVEL_
%token LOGMODE_
%token LOGCOLORS_
%token WORKDIR_
%token OPTION_
%token DNS_SERVER_
%token DOMAIN_
%token NTP_SERVER_
%token TIME_ZONE_
%token SIP_SERVER_
%token SIP_DOMAIN_
%token NIS_SERVER_
%token NIS_DOMAIN_
%token NISP_SERVER_
%token NISP_DOMAIN_
%token LIFETIME_
%token FQDN_
%token ACCEPT_UNKNOWN_FQDN_
%token FQDN_DDNS_ADDRESS_
%token DDNS_PROTOCOL_
%token DDNS_TIMEOUT_
%token ACCEPT_ONLY_
%token REJECT_CLIENTS_
%token POOL_
%token SHARE_
%token T1_
%token T2_
%token PREF_TIME_
%token VALID_TIME_
%token UNICAST_
%token DROP_UNICAST_
%token PREFERENCE_
%token RAPID_COMMIT_
%token IFACE_MAX_LEASE_
%token CLASS_MAX_LEASE_
%token CLNT_MAX_LEASE_
%token STATELESS_
%token CACHE_SIZE_
%token PDCLASS_
%token PD_LENGTH_
%token PD_POOL_
%token SCRIPT_
%token VENDOR_SPEC_
%token CLIENT_
%token DUID_KEYWORD_
%token REMOTE_ID_
%token LINK_LOCAL_
%token ADDRESS_
%token PREFIX_
%token GUESS_MODE_
%token INACTIVE_MODE_
%token EXPERIMENTAL_
%token ADDR_PARAMS_
%token REMOTE_AUTOCONF_NEIGHBORS_
%token AFTR_
%token PERFORMANCE_MODE_
%token AUTH_PROTOCOL_
%token AUTH_ALGORITHM_
%token AUTH_REPLAY_
%token AUTH_METHODS_
%token AUTH_DROP_UNAUTH_
%token AUTH_REALM_
%token KEY_
%token SECRET_
%token ALGORITHM_
%token FUDGE_
%token DIGEST_NONE_
%token DIGEST_PLAIN_
%token DIGEST_HMAC_MD5_
%token DIGEST_HMAC_SHA1_
%token DIGEST_HMAC_SHA224_
%token DIGEST_HMAC_SHA256_
%token DIGEST_HMAC_SHA384_
%token DIGEST_HMAC_SHA512_
%token ACCEPT_LEASEQUERY_
%token BULKLQ_ACCEPT_
%token BULKLQ_TCPPORT_
%token BULKLQ_MAX_CONNS_
%token BULKLQ_TIMEOUT_
%token CLIENT_CLASS_
%token MATCH_IF_
%token EQ_
%token AND_
%token OR_
%token CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_
%token CLIENT_VENDOR_SPEC_DATA_
%token CLIENT_VENDOR_CLASS_EN_
%token CLIENT_VENDOR_CLASS_DATA_
%token RECONFIGURE_ENABLED_
%token ALLOW_
%token DENY_
%token SUBSTRING_
%token STRING_KEYWORD_
%token ADDRESS_LIST_
%token CONTAIN_
%token NEXT_HOP_
%token ROUTE_
%token INFINITE_
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
    if (!ctx.StartIfaceDeclaration($2)) {
	YYABORT;
    }
}
InterfaceDeclarationsList '}'
{
    //Information about new interface has been read
    //Add it to list of read interfaces
    delete [] $2;
    ctx.EndIfaceDeclaration();
}
/* iface 5 { ... } */
|IFACE_ Number '{'
{
    if (!ctx.StartIfaceDeclaration($2)) {
	YYABORT;
    }
}
InterfaceDeclarationsList '}'
{
    ctx.EndIfaceDeclaration();
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
    ctx.CurrentKey_ = new TSIGKey(string($2));
} KeyOptions
'}'
{
    /// check that both secret and algorithm keywords were defined.
    Log(Debug) << "Loaded key '" << ctx.CurrentKey_->Name_ << "', base64len is "
	       << ctx.CurrentKey_->getBase64Data().length() << ", rawlen is "
	       << ctx.CurrentKey_->getPackedData().length() << "." << LogEnd;
    if (ctx.CurrentKey_->getPackedData().length() == 0) {
	Log(Crit) << "Key " << ctx.CurrentKey_->Name_ << " does not have secret specified." << LogEnd;
	YYABORT;
    }

    if ( (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_MD5) &&
	 (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_SHA1) &&
	 (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_SHA256) ) {
	Log(Crit) << "Invalid key type specified: only hmac-md5, hmac-sha1 and "
                  << "hmac-sha256 are supported." << LogEnd;
	YYABORT;
    }
#if !defined(MOD_SRV_DISABLE_DNSUPDATE) && !defined(MOD_CLNT_DISABLE_DNSUPDATE)
    ctx.CfgMgr_->addKey( ctx.CurrentKey_ );
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
    ctx.CurrentKey_->setData(string($2));
};

KeyFudge
: FUDGE_ Number ';'
{
    ctx.CurrentKey_->Fudge_ = $2;
}

KeyAlgorithm
: ALGORITHM_ DIGEST_HMAC_SHA256_ ';' { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_SHA256; }
| ALGORITHM_ DIGEST_HMAC_SHA1_   ';' { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_SHA1;  }
| ALGORITHM_ DIGEST_HMAC_MD5_    ';' { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_MD5;  }
;
/// add other key types here

Client
: CLIENT_ DUID_KEYWORD_ DUID_ '{'
{
    ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
    SPtr<TDUID> duid = new TDUID($3.duid,$3.length);
    ctx.ClientLst_.append(new TSrvCfgOptions(duid));
} ClientOptions
'}'
{
    Log(Debug) << "Exception: DUID-based exception specified." << LogEnd;
    // copy all defined options
    ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
    ctx.ParserOptStack_.delLast();
}

| CLIENT_ REMOTE_ID_ Number '-' DUID_ '{'
{
    ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
    SPtr<TOptVendorData> remoteid = new TOptVendorData($3, $5.duid, $5.length, 0);
    ctx.ClientLst_.append(new TSrvCfgOptions(remoteid));
} ClientOptions
'}'
{
    Log(Debug) << "Exception: RemoteID-based exception specified." << LogEnd;
    // copy all defined options
    ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
    ctx.ParserOptStack_.delLast();
}

| CLIENT_ LINK_LOCAL_ IPV6ADDR_ '{'
{
		ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
		SPtr<TIPv6Addr> clntaddr = new TIPv6Addr($3);
		ctx.ClientLst_.append(new TSrvCfgOptions(clntaddr));
} ClientOptions
'}'
{
		Log(Debug) << "Exception: Link-local-based exception specified." << LogEnd;
		// copy all defined options
		ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
		ctx.ParserOptStack_.delLast();
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
    ctx.addr_ = new TIPv6Addr($2);
    Log(Info) << "Exception: Address " << ctx.addr_->getPlain() << " reserved." << LogEnd;
    ctx.ClientLst_.getLast()->setAddr(ctx.addr_);
};

PrefixReservation:
PREFIX_ IPV6ADDR_ '/' Number
{
    ctx.addr_ = new TIPv6Addr($2);
    Log(Info) << "Exception: Prefix " << ctx.addr_->getPlain() << "/" << $4 << " reserved." << LogEnd;
    ctx.ClientLst_.getLast()->setPrefix(ctx.addr_, $4);
}

/* class { ... } */
ClassDeclaration:
CLASS_ '{'
{
    ctx.StartClassDeclaration();
}
ClassOptionDeclarationsList '}'
{
    if (!ctx.EndClassDeclaration()) {
	YYABORT;
    }
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
    ctx.StartTAClassDeclaration();
} TAClassOptionsList '}'
{
    if (!ctx.EndTAClassDeclaration()) {
	YYABORT;
    }
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
    ctx.StartPDDeclaration();
} PDOptionsList '}'
{
    if (!ctx.EndPDDeclaration()) {
	YYABORT;
    }
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
    ctx.nextHop = myNextHop; 
}
RouteList '}'
{
    ctx.ParserOptStack_.getLast()->addExtraOption(ctx.nextHop, false);
    ctx.nextHop.reset();
}
| NEXT_HOP_ IPV6ADDR_
{
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr($2);
    SPtr<TOpt> myNextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(myNextHop, false);
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
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
}
| ROUTE_ IPV6ADDR_ '/' INTNUMBER_
{
    SPtr<TIPv6Addr> prefix = new TIPv6Addr($2);
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, $4, 42, prefix, NULL);
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
}
| ROUTE_ IPV6ADDR_ '/' INTNUMBER_ LIFETIME_ INFINITE_
{
    SPtr<TIPv6Addr> prefix = new TIPv6Addr($2);
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, $4, 42, prefix, NULL);
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
};

AuthProtocol
: AUTH_PROTOCOL_ STRING_ {

#ifndef MOD_DISABLE_AUTH
    if (!strcasecmp($2,"none")) {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_NONE);
        ctx.CfgMgr_->setAuthAlgorithm(AUTH_ALGORITHM_NONE);
    } else if (!strcasecmp($2, "delayed")) {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_DELAYED);
    } else if (!strcasecmp($2, "reconfigure-key")) {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_RECONFIGURE_KEY);
        ctx.CfgMgr_->setAuthAlgorithm(AUTH_ALGORITHM_RECONFIGURE_KEY);
    } else if (!strcasecmp($2, "dibbler")) {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_DIBBLER);
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
        ctx.CfgMgr_->setAuthReplay(AUTH_REPLAY_NONE);
    } else if (strcasecmp($2, "monotonic")) {
        ctx.CfgMgr_->setAuthReplay(AUTH_REPLAY_MONOTONIC);
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
    ctx.CfgMgr_->setAuthRealm(string($2));
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
};

AuthMethods
: AUTH_METHODS_
{
    ctx.DigestLst_.clear();
} DigestList {
#ifndef MOD_DISABLE_AUTH
    ctx.CfgMgr_->setAuthDigests(ctx.DigestLst_);
    ctx.CfgMgr_->setAuthDropUnauthenticated(true);
    ctx.DigestLst_.clear();
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}

DigestList
: Digest
| DigestList ',' Digest
;

Digest
: DIGEST_NONE_        { ctx.DigestLst_.push_back(DIGEST_NONE); }
| DIGEST_PLAIN_       { ctx.DigestLst_.push_back(DIGEST_PLAIN); }
| DIGEST_HMAC_MD5_    { ctx.DigestLst_.push_back(DIGEST_HMAC_MD5); }
| DIGEST_HMAC_SHA1_   { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA1); }
| DIGEST_HMAC_SHA224_ { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA224); }
| DIGEST_HMAC_SHA256_ { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA256); }
| DIGEST_HMAC_SHA384_ { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA384); }
| DIGEST_HMAC_SHA512_ { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA512); }
;


AuthDropUnauthenticated
: AUTH_DROP_UNAUTH_ Number {
#ifndef MOD_DISABLE_AUTH
    ctx.CfgMgr_->setAuthDropUnauthenticated($2);
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
    ctx.PresentFQDNLst_.append(new TFQDN($1,false));
}
| STRING_ '-' DUID_
{
    /// @todo: Use SPtr()
    TDUID* duidNew = new TDUID($3.duid,$3.length);
    Log(Debug)<< "FQDN:" << $1 <<" reserved for DUID " << duidNew->getPlain()<<LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(duidNew, $1,false));
}
| STRING_ '-' IPV6ADDR_
{
    ctx.addr_= new TIPv6Addr($3);
    Log(Debug)<< "FQDN:" << $1 << " reserved for address " << ctx.addr_->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(new TIPv6Addr($3), $1,false));
}
| FQDNList ',' STRING_
{
	Log(Debug) << "FQDN:"<<$3<<" has no reservations (is available to everyone)."<<LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN($3,false));
}
| FQDNList ',' STRING_ '-' DUID_
{
    TDUID* duidNew = new TDUID($5.duid,$5.length);
    Log(Debug)<< "FQDN:" << $3 << " reserved for DUID "<< duidNew->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN( duidNew, $3,false));
}
| FQDNList ',' STRING_ '-' IPV6ADDR_
{
    ctx.addr_ = new TIPv6Addr($5);
    Log(Debug)<< "FQDN:" << $3<<" reserved for address "<< ctx.addr_->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(new TIPv6Addr($5), $3,false));
}
;

Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;

ADDRESSList
: IPV6ADDR_
{
    ctx.PresentAddrLst_.append(new TIPv6Addr($1));
}
| ADDRESSList ',' IPV6ADDR_
{
    ctx.PresentAddrLst_.append(new TIPv6Addr($3));
}
;

VendorSpecList
: Number '-' Number '-' DUID_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
	       << $3 << ", valuelen=" << $5.length << LogEnd;

    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    $5.duid, $5.length, 0), false);
}
| Number '-' Number '-' IPV6ADDR_ 
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($5));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
               << $3 << ", value=" << addr->getPlain() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    new TIPv6Addr($5), 0), false);
}
| Number '-' Number '-' STRING_ 
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $1 << ", optionCode: "
	       << $3 << ", valuelen=" << strlen($5) << LogEnd;

    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1, $3,
								    $5, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' DUID_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
	       << $5 << ", valuelen=" << $7.length << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    $7.duid, $7.length, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' IPV6ADDR_ 
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($7));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
               << $5 << ", value=" << addr->getPlain() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    addr, 0), false);
}
| VendorSpecList ',' Number '-' Number '-' STRING_
{
    Log(Debug) << "Vendor-spec defined: Enterprise: " << $3 << ", optionCode: "
	       << $5 << ", valuelen=" << strlen($7) << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3, $5,
								    $7, 0), false);
}
;

StringList
: STRING_ { ctx.PresentStringLst_.append(SPtr<string> (new string($1))); }
| StringList ',' STRING_ { ctx.PresentStringLst_.append(SPtr<string> (new string($3))); }
;

ADDRESSRangeList
    : IPV6ADDR_
    {
	ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr($1),new TIPv6Addr($1)));
    }
    |   IPV6ADDR_ '-' IPV6ADDR_
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
    |  IPV6ADDR_ '/' INTNUMBER_
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr($1));
	int prefix = $3;
	if ( (prefix<1) || (prefix>128) ) {
	    error(@3, "Invalid prefix defined: " + to_string(prefix) + ". Allowed range: 1..128.");
	    YYABORT;
	}
	SPtr<TIPv6Addr> addr1 = ctx.getRangeMin($1, prefix);
	SPtr<TIPv6Addr> addr2 = ctx.getRangeMax($1, prefix);
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
    | ADDRESSRangeList ',' IPV6ADDR_
    {
	ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr($3),new TIPv6Addr($3)));
    }
    |   ADDRESSRangeList ',' IPV6ADDR_ '-' IPV6ADDR_
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
;

PDRangeList
:   IPV6ADDR_ '/' INTNUMBER_
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr($1));
	int prefix = $3;
	if ( (prefix<1) || (prefix>128)) {
	    error(@3, "Invalid prefix defined: " + to_string(prefix) + ". Allowed range: 1..128.");
            YYABORT;
	}

	SPtr<TIPv6Addr> addr1 = ctx.getRangeMin($1, prefix);
	SPtr<TIPv6Addr> addr2 = ctx.getRangeMax($1, prefix);
	SPtr<THostRange> range;
	if (*addr1<=*addr2)
	    range = new THostRange(addr1,addr2);
	else
	    range = new THostRange(addr2,addr1);
	range->setPrefixLength(prefix);
	ctx.PDLst_.append(range);
    }
;

ADDRESSDUIDRangeList
: IPV6ADDR_
{
    ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr($1),new TIPv6Addr($1)));
}
|   IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> addr1(new TIPv6Addr($1));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr($3));
    if (*addr1<=*addr2)
	ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
    else
	ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
}
| ADDRESSDUIDRangeList ',' IPV6ADDR_
{
    ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr($3),new TIPv6Addr($3)));
}
|   ADDRESSDUIDRangeList ',' IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> addr1(new TIPv6Addr($3));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr($5));
    if (*addr1<=*addr2)
	ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
    else
	ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
}
| DUID_
{
    SPtr<TDUID> duid(new TDUID($1.duid, $1.length));
    ctx.PresentRangeLst_.append(new THostRange(duid, duid));
    delete $1.duid;
}
| DUID_ '-' DUID_
{
    SPtr<TDUID> duid1(new TDUID($1.duid,$1.length));
    SPtr<TDUID> duid2(new TDUID($3.duid,$3.length));

    if (*duid1<=*duid2)
	ctx.PresentRangeLst_.append(new THostRange(duid1,duid2));
    else
	ctx.PresentRangeLst_.append(new THostRange(duid2,duid1));

    /// @todo: delete [] $1.duid; delete [] $3.duid?
}
| ADDRESSDUIDRangeList ',' DUID_
{
    SPtr<TDUID> duid(new TDUID($3.duid, $3.length));
    ctx.PresentRangeLst_.append(new THostRange(duid, duid));
    delete $3.duid;
}
| ADDRESSDUIDRangeList ',' DUID_ '-' DUID_
{
    SPtr<TDUID> duid2(new TDUID($3.duid,$3.length));
    SPtr<TDUID> duid1(new TDUID($5.duid,$5.length));
    if (*duid1<=*duid2)
	ctx.PresentRangeLst_.append(new THostRange(duid1,duid2));
    else
	ctx.PresentRangeLst_.append(new THostRange(duid2,duid1));
    delete $3.duid;
    delete $5.duid;
}
;

RejectClientsOption
: REJECT_CLIENTS_
{
    ctx.PresentRangeLst_.clear();
} ADDRESSDUIDRangeList
{
    ctx.ParserOptStack_.getLast()->setRejedClnt(&ctx.PresentRangeLst_);
}
;

AcceptOnlyOption
: ACCEPT_ONLY_
{
    ctx.PresentRangeLst_.clear();
} ADDRESSDUIDRangeList
{
    ctx.ParserOptStack_.getLast()->setAcceptClnt(&ctx.PresentRangeLst_);
}
;

PoolOption
: POOL_
{
    ctx.PresentRangeLst_.clear();
} ADDRESSRangeList
{
    ctx.ParserOptStack_.getLast()->setPool(&ctx.PresentRangeLst_);
}
;

PDPoolOption
: PD_POOL_
{
} PDRangeList
{
    ctx.ParserOptStack_.getLast()->setPool(&ctx.PresentRangeLst_/*PDList*/);
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
   ctx.PDPrefix_ = $2;
}
;

PreferredTimeOption
: PREF_TIME_ Number
{
    ctx.ParserOptStack_.getLast()->setPrefBeg($2);
    ctx.ParserOptStack_.getLast()->setPrefEnd($2);
}
| PREF_TIME_ Number '-' Number
{
    ctx.ParserOptStack_.getLast()->setPrefBeg($2);
    ctx.ParserOptStack_.getLast()->setPrefEnd($4);
}
;

ValidTimeOption
: VALID_TIME_ Number
{
    ctx.ParserOptStack_.getLast()->setValidBeg($2);
    ctx.ParserOptStack_.getLast()->setValidEnd($2);
}
| VALID_TIME_ Number '-' Number
{
    ctx.ParserOptStack_.getLast()->setValidBeg($2);
    ctx.ParserOptStack_.getLast()->setValidEnd($4);
}
;

ShareOption
: SHARE_ Number
{
    int x=$2;
    if ( (x<1) || (x>1000)) {
	error(@2, "Invalid share value: " + to_string(x) + ". Allowed range: 1..1000.");
        YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setShare(x);
}

T1Option
: T1_ Number
{
    ctx.ParserOptStack_.getLast()->setT1Beg($2);
    ctx.ParserOptStack_.getLast()->setT1End($2);
}
| T1_ Number '-' Number
{
    ctx.ParserOptStack_.getLast()->setT1Beg($2);
    ctx.ParserOptStack_.getLast()->setT1End($4);
}
;

T2Option
: T2_ Number
{
    ctx.ParserOptStack_.getLast()->setT2Beg($2);
    ctx.ParserOptStack_.getLast()->setT2End($2);
}
| T2_ Number '-' Number
{
    ctx.ParserOptStack_.getLast()->setT2Beg($2);
    ctx.ParserOptStack_.getLast()->setT2End($4);
}
;

ClntMaxLeaseOption
: CLNT_MAX_LEASE_ Number
{
    ctx.ParserOptStack_.getLast()->setClntMaxLease($2);
}
;

ClassMaxLeaseOption
: CLASS_MAX_LEASE_ Number
{
    ctx.ParserOptStack_.getLast()->setClassMaxLease($2);
}
;

AddrParams
: ADDR_PARAMS_ Number
{
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'addr-params' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }
    int bitfield = ADDRPARAMS_MASK_PREFIX;
    Log(Warning) << "Experimental addr-params added (prefix=" << $2
                 << ", bitfield=" << bitfield << ")." << LogEnd;
    ctx.ParserOptStack_.getLast()->setAddrParams($2,bitfield);
};

DsLiteAftrName
: OPTION_ AFTR_ STRING_
{
    SPtr<TOpt> tunnelName = new TOptDomainLst(OPTION_AFTR_NAME, $3, 0);
    Log(Debug) << "Enabling DS-Lite tunnel option, AFTR name=" << $3 << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(tunnelName, false);
};

ExtraOption
:OPTION_ Number DUID_KEYWORD_ DUID_
{
    SPtr<TOpt> opt = new TOptGeneric($2, $4.duid, $4.length, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", length="
               << $4.length << LogEnd;
}
|OPTION_ Number ADDRESS_ IPV6ADDR_
{
    SPtr<TIPv6Addr> addr(new TIPv6Addr($4));

    SPtr<TOpt> opt = new TOptAddr($2, addr, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", address=" << addr->getPlain() << LogEnd;
}
|OPTION_ Number ADDRESS_LIST_
{
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> opt = new TOptAddrLst($2, ctx.PresentAddrLst_, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", address count="
               << ctx.PresentAddrLst_.count() << LogEnd;
}
|OPTION_ Number STRING_KEYWORD_ STRING_
{
    SPtr<TOpt> opt = new TOptString($2, string($4), 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << $2 << ", string=" << $4 << LogEnd;
};

RemoteAutoconfNeighborsOption
:OPTION_ REMOTE_AUTOCONF_NEIGHBORS_
{
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'remote autoconf neighbors' defined, but "
		  << "experimental features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> opt = new TOptAddrLst(OPTION_NEIGHBORS, ctx.PresentAddrLst_, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Remote autoconf neighbors enabled (" << ctx.PresentAddrLst_.count()
	       << " neighbors defined.)" << LogEnd;
}


IfaceMaxLeaseOption
: IFACE_MAX_LEASE_ Number
{
    ctx.ParserOptStack_.getLast()->setIfaceMaxLease($2);
}
;

UnicastAddressOption
: UNICAST_ IPV6ADDR_
{
    ctx.ParserOptStack_.getLast()->setUnicast(new TIPv6Addr($2));
}
;

DropUnicast
: DROP_UNICAST_
{
    ctx.CfgMgr_->dropUnicast(true);
}

RapidCommitOption
:   RAPID_COMMIT_ Number
{
    if ( ($2!=0) && ($2!=1)) {
	error(@2, "RAPID-COMMIT parameter specified " + to_string($2) + " must have 0 or 1 value.");
        YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setRapidCommit($2 == 1);
}
;

PreferenceOption
: PREFERENCE_ Number
{
    int x = $2;
    if ( (x<0) || (x>255) ) {
	error(@2, "Preference value (" + to_string(x) + " is out of range [0..255].");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setPreference(x);
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
    ctx.ParserOptStack_.getLast()->setWorkDir($2);
}
;

StatelessOption
: STATELESS_
{
    ctx.ParserOptStack_.getLast()->setStateless(true);
}
;

GuessMode
: GUESS_MODE_
{
    Log(Info) << "Guess-mode enabled: relay interfaces may be loosely "
              << "defined (matching interface-id is not mandatory)." << LogEnd;
    ctx.ParserOptStack_.getLast()->setGuessMode(true);
};

ScriptName
: SCRIPT_ STRING_
{
    ctx.CfgMgr_->setScriptName($2);
};

PerformanceMode
: PERFORMANCE_MODE_ Number
{
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'performance-mode' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    ctx.CfgMgr_->setPerformanceMode($2);
};

ReconfigureEnabled
: RECONFIGURE_ENABLED_ Number
{
    switch ($2) {
    case 0:
    case 1:
        ctx.CfgMgr_->setReconfigureSupport($2);
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
    ctx.ParserOptStack_.getLast()->setInactiveMode(true);
};

Experimental
: EXPERIMENTAL_
{
    Log(Crit) << "Experimental features are allowed." << LogEnd;
    ctx.ParserOptStack_.getLast()->setExperimental(true);
};

IfaceIDOrder
:IFACE_ID_ORDER_ STRING_
{
    if (!strncasecmp($2,"before",6))
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_BEFORE);
    } else
    if (!strncasecmp($2,"after",5))
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_AFTER);
    } else
    if (!strncasecmp($2,"omit",4))
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_NONE);
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
    ctx.ParserOptStack_.getLast()->setCacheSize($2);
}
;

////////////////////////////////////////////////////////////////////////
/// LEASE-QUERY (regular and bulk) /////////////////////////////////////
////////////////////////////////////////////////////////////////////////

AcceptLeaseQuery
: ACCEPT_LEASEQUERY_
{
    ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(true);

}
| ACCEPT_LEASEQUERY_ Number
{
    switch ($2) {
    case 0:
		ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(false);
		break;
    case 1:
		ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(true);
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
    ctx.CfgMgr_->bulkLQAccept( (bool) $2);
};

BulkLeaseQueryTcpPort
: BULKLQ_TCPPORT_ Number
{
    ctx.CfgMgr_->bulkLQTcpPort( $2 );
}

BulkLeaseQueryMaxConns
: BULKLQ_MAX_CONNS_ Number
{
    ctx.CfgMgr_->bulkLQMaxConns( $2 );
};

BulkLeaseQueryTimeout
: BULKLQ_TIMEOUT_ Number
{
    ctx.CfgMgr_->bulkLQTimeout( $2 );
};

////////////////////////////////////////////////////////////////////////
/// RELAY //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
RelayOption
:RELAY_ STRING_
{
    ctx.ParserOptStack_.getLast()->setRelayName($2);
}
|RELAY_ Number
{
    ctx.ParserOptStack_.getLast()->setRelayID($2);
}
;

InterfaceIDOption
:IFACE_ID_ Number
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2, 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
|IFACE_ID_ DUID_
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2.duid, $2.length, 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
|IFACE_ID_ STRING_
{
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID($2, strlen($2), 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
;

Subnet
:SUBNET_ IPV6ADDR_ '/' Number
{
    int prefix = $4;
    if ( (prefix<1) || (prefix>128) ) {
        error(@4, "Invalid (1..128 allowed) prefix used: "+ to_string(prefix)
              + " in subnet definition.");
        YYABORT;
    }
    SPtr<TIPv6Addr> min = ctx.getRangeMin($2, prefix);
    SPtr<TIPv6Addr> max = ctx.getRangeMax($2, prefix);
    ctx.SrvCfgIfaceLst_.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "/" << $4
               << " on " << ctx.SrvCfgIfaceLst_.getLast()->getFullName() << LogEnd;
}|SUBNET_ IPV6ADDR_ '-' IPV6ADDR_
{
    SPtr<TIPv6Addr> min = new TIPv6Addr($2);
    SPtr<TIPv6Addr> max = new TIPv6Addr($4);
    ctx.SrvCfgIfaceLst_.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "-" << max->getPlain()
               << "on " << ctx.SrvCfgIfaceLst_.getLast()->getFullName() << LogEnd;
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
    ctx.SrvCfgClientClassLst_.first();
    while (clntClass = ctx.SrvCfgClientClassLst_.get())
    {
	if (clntClass->getClassName() == string($2))
	    found = true;
    }
    if (!found)
    {
	error(@2, ": Unable to use class '" + string($2) + "', no such class defined.");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setAllowClientClass(string($2));

    int deny = ctx.ParserOptStack_.getLast()->getDenyClientClassString().count();

    if (deny)
    {
	error(@1, "Unable to define both allow and deny lists for this client class.");
	YYABORT;
    }

}

DenyClientClassDeclaration
: DENY_ STRING_
{
    SPtr<TSrvCfgClientClass> clntClass;
    bool found = false;
    ctx.SrvCfgClientClassLst_.first();
    while (clntClass = ctx.SrvCfgClientClassLst_.get())
    {
	if (clntClass->getClassName() == string($2))
	    found = true;
    }
    if (!found)
    {
	error(@2, " Unable to use class " + string($2) + ", no such class defined.");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setDenyClientClass(string($2));

    int allow = ctx.ParserOptStack_.getLast()->getAllowClientClassString().count();

    if (allow)
    {
	error(@1, "Unable to define both allow and deny lists for this client class.");
	YYABORT;
    }

}

////////////////////////////////////////////////////////////////////////
/// DNS-server option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

DNSServerOption
:OPTION_ DNS_SERVER_
{
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_DNS_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_servers, false);
}
;

////////////////////////////////////////////////////////////////////////
/// DOMAIN option //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
DomainOption
: OPTION_ DOMAIN_ {
    ctx.PresentStringLst_.clear();
} StringList
{
    SPtr<TOpt> domains = new TOptDomainLst(OPTION_DOMAIN_LIST, ctx.PresentStringLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(domains, false);
}
;

////////////////////////////////////////////////////////////////////////
/// NTP-SERVER option //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
NTPServerOption
:OPTION_ NTP_SERVER_
{
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> ntp_servers = new TOptAddrLst(OPTION_SNTP_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(ntp_servers, false);
    // ctx.ParserOptStack_.getLast()->setNTPServerLst(&ctx.PresentAddrLst);
}
;

////////////////////////////////////////////////////////////////////////
/// TIME-ZONE option ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
TimeZoneOption
: OPTION_ TIME_ZONE_ STRING_
{
    SPtr<TOpt> timezone = new TOptString(OPTION_NEW_TZDB_TIMEZONE, string($3), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(timezone, false);
    // ctx.ParserOptStack_.getLast()->setTimezone($3);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPServerOption
:OPTION_ SIP_SERVER_ {
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> sip_servers = new TOptAddrLst(OPTION_SIP_SERVER_A, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(sip_servers, false);
    // ctx.ParserOptStack_.getLast()->setSIPServerLst(&ctx.PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPDomainOption
:OPTION_ SIP_DOMAIN_ {
    ctx.PresentStringLst_.clear();
} StringList
{
    SPtr<TOpt> sip_domains = new TOptDomainLst(OPTION_SIP_SERVER_D, ctx.PresentStringLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(sip_domains, false);
    //ctx.ParserOptStack_.getLast()->setSIPDomainLst(&ctx.PresentStringLst_);
}
;

//////////////////////////////////////////////////////////////////////
//FQDN option/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

FQDNOption
:OPTION_ FQDN_
{
    ctx.PresentFQDNLst_.clear();
    Log(Debug)   << "No FQDNMode found, setting default mode 2 (all updates "
                 "executed by server)." << LogEnd;
    Log(Warning) << "revDNS zoneroot lenght not found, dynamic revDNS update "
                 "will not be possible." << LogEnd;
    ctx.ParserOptStack_.getLast()->setFQDNMode(2);
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength(0);
} FQDNList
{
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);
}
|OPTION_ FQDN_ INTNUMBER_
{
    ctx.PresentFQDNLst_.clear();
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
    ctx.ParserOptStack_.getLast()->setFQDNMode($3);
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength(0);
} FQDNList
{
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);

}
|OPTION_ FQDN_ INTNUMBER_ INTNUMBER_
{
    ctx.PresentFQDNLst_.clear();
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
    ctx.ParserOptStack_.getLast()->setFQDNMode($3);
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength($4);
} FQDNList
{
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);

}
;

AcceptUnknownFQDN
:ACCEPT_UNKNOWN_FQDN_ Number STRING_
{
    ctx.ParserOptStack_.getLast()->setUnknownFQDN(EUnknownFQDNMode($2), string($3) );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << $2
               << ", domain=" << $3 << "." << LogEnd;
}
|ACCEPT_UNKNOWN_FQDN_ Number
{
    ctx.ParserOptStack_.getLast()->setUnknownFQDN(EUnknownFQDNMode($2), string("") );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << $2
               << ", no domain." << LogEnd;
}
;

FqdnDdnsAddress
:FQDN_DDNS_ADDRESS_ IPV6ADDR_
{
    ctx.addr_ = new TIPv6Addr($2);
    ctx.CfgMgr_->setDDNSAddress(ctx.addr_);
    Log(Info) << "FQDN: DDNS updates will be performed to " << ctx.addr_->getPlain() << "." << LogEnd;
};

DdnsProtocol
:DDNS_PROTOCOL_ STRING_
{
    if (!strcasecmp($2,"tcp"))
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_TCP);
    else if (!strcasecmp($2,"udp"))
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_UDP);
    else if (!strcasecmp($2,"any"))
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_ANY);
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
    ctx.CfgMgr_->setDDNSTimeout($2);
}

//////////////////////////////////////////////////////////////////////
//NIS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISServerOption
:OPTION_ NIS_SERVER_ {
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_NIS_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_servers, false);
    ///ctx.ParserOptStack_.getLast()->setNISServerLst(&ctx.PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-SERVER option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPServerOption
: OPTION_ NISP_SERVER_ {
    ctx.PresentAddrLst_.clear();
} ADDRESSList
{
    SPtr<TOpt> nisp_servers = new TOptAddrLst(OPTION_NISP_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nisp_servers, false);
    // ctx.ParserOptStack_.getLast()->setNISPServerLst(&ctx.PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NIS-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISDomainOption
:OPTION_ NIS_DOMAIN_ STRING_
{
    SPtr<TOpt> nis_domain = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, string($3), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_domain, false);
    // ctx.ParserOptStack_.getLast()->setNISDomain($3);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-DOMAIN option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPDomainOption
:OPTION_ NISP_DOMAIN_ STRING_
{
    SPtr<TOpt> nispdomain = new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, string($3), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nispdomain, false);
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
    ctx.ParserOptStack_.getLast()->addExtraOption(lifetime, false);
    //ctx.ParserOptStack_.getLast()->setLifetime($3);
}
;

VendorSpecOption
:OPTION_ VENDOR_SPEC_ {
} VendorSpecList
{
    // ctx.ParserOptStack_.getLast()->setVendorSpec(VendorSpec);
    // Log(Debug) << "Vendor-spec parsing finished" << LogEnd;
};


ClientClass
:CLIENT_CLASS_ STRING_ '{'
{
    Log(Notice) << "ClientClass found, name: " << string($2) << LogEnd;
} ClientClassDecleration   '}'
{
    SPtr<Node> cond =  ctx.NodeClientClassLst_.getLast();
    ctx.SrvCfgClientClassLst_.append( new TSrvCfgClientClass(string($2),cond));
    ctx.NodeClientClassLst_.delLast();
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
    SPtr<Node> r =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> l = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_CONTAIN,l,r));
}
| '(' Expr EQ_ Expr ')'
{
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();

    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_EQUAL,l,r));
}
| '(' Condition  AND_  Condition ')'
{
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_AND,l,r));

}
| '(' Condition  OR_  Condition ')'
{
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_OR,l,r));
}
;

Expr
:CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_
{
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM));
}
|CLIENT_VENDOR_SPEC_DATA_
{
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_DATA));
}
| CLIENT_VENDOR_CLASS_EN_
{
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM));
}
| CLIENT_VENDOR_CLASS_DATA_
{
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_DATA));
}
| STRING_
{
    // Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    ctx.NodeClientClassLst_.append(new NodeConstant(string($1)));
}
|Number
{
    //Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    stringstream convert;
    string snum;
    convert<<$1;
    convert>>snum;
    ctx.NodeClientClassLst_.append(new NodeConstant(snum));
}
| SUBSTRING_ '(' Expr ',' Number ',' Number  ')'
{
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_SUBSTRING,l, $5,$7));
}
;
%%

void
dibbler::SrvParser::error(const location_type& loc,
                          const std::string& what)
{
    ctx.error(loc, what);
}


