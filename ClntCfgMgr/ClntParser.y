%name ClntParser

%header{
#include <iostream>
#include <string>
#include <vector>
#include <malloc.h>
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "ClntParser.h"
#include "ClntParsGlobalOpt.h"
#include "ClntCfgIface.h"
#include "ClntCfgAddr.h"
#include "ClntCfgIA.h"
#include "ClntCfgTA.h"
#include "ClntCfgPD.h"
#include "ClntCfgMgr.h"
#include "Logger.h"
#include "OptGeneric.h"
#include "OptAddr.h"
#include "OptAddrLst.h"
#include "OptString.h"

using namespace std;

#define YY_USE_CLASS
%}

%{
#include "FlexLexer.h"
%}
// --- CLASS MEMBERS ---

%define MEMBERS yyFlexLexer * Lex_;                                         \
/*List of options in scope stack,the most fresh is last in the list*/       \
List(TClntParsGlobalOpt) ParserOptStack;                                    \
/*List of parsed interfaces/IAs/Addresses, last */                          \
/*interface/IA/address is just being parsing or have been just parsed*/     \
List(TClntCfgIface) ClntCfgIfaceLst;                                        \
List(TClntCfgIA)    ClntCfgIALst;                                           \
List(TClntCfgTA)    ClntCfgTALst;                                           \
List(TClntCfgPD)    ClntCfgPDLst;                                           \
List(TClntCfgAddr)  ClntCfgAddrLst;                                         \
DigestTypesLst      DigestLst;                                              \
/*Pointer to list which should contain either rejected servers or */        \
/*preffered servers*/                                                       \
List(THostID) PresentStationLst;                                            \
List(TIPv6Addr) PresentAddrLst;                                             \
List(TClntCfgPrefix) PrefixLst;                                             \
List(std::string) PresentStringLst;                                         \
List(TOptVendorSpecInfo) VendorSpec;                                        \
bool IfaceDefined(int ifaceNr);                                             \
bool IfaceDefined(const std::string& ifaceName);                            \
bool StartIfaceDeclaration(const std::string& ifaceName);                   \
bool StartIfaceDeclaration(int ifindex);                                    \
bool EndIfaceDeclaration();                                                 \
void EmptyIface();                                                          \
bool StartIADeclaration(bool aggregation);                                  \
void EndIADeclaration();                                                    \
bool StartPDDeclaration();                                                  \
bool EndPDDeclaration();                                                    \
void EmptyIA();                                                             \
void EmptyAddr();                                                           \
TClntCfgMgr * CfgMgr;                                                       \
bool iaidSet;                                                               \
unsigned int iaid;                                                          \
unsigned int AddrCount_;                                                    \
virtual ~ClntParser();                                                      \
EDUIDType DUIDType;                                                         \
int DUIDEnterpriseNumber;                                                   \
SPtr<TDUID> DUIDEnterpriseID;


%define CONSTRUCTOR_PARAM yyFlexLexer * lex
%define CONSTRUCTOR_CODE                                                    \
    Lex_ = lex;                                                             \
    ParserOptStack.append(new TClntParsGlobalOpt());                        \
    ParserOptStack.getFirst()->setIAIDCnt(1);                               \
    ParserOptStack.getLast();                                               \
    DUIDType = DUID_TYPE_NOT_DEFINED;                                       \
    DUIDEnterpriseID.reset();                                               \
    AddrCount_ = 0;                                                         \
    CfgMgr = 0;                                                             \
    iaidSet = false;                                                        \
    iaid = 0xffffffff;                                                      \
    DUIDEnterpriseNumber = -1;                                              \
    yynerrs = 0;                                                            \
    yychar = 0;

%union
{
    int ival;
    char *strval;
    struct SDuid  {
	int length;
	char* duid;
    } duidval;
    char addrval[16];
}

%{
namespace std
{
    extern yy_ClntParser_stype yylval;
}
%}

%token T1_,T2_,PREF_TIME_,DNS_SERVER_,VALID_TIME_, UNICAST_
%token NTP_SERVER_, DOMAIN_, TIME_ZONE_, SIP_SERVER_, SIP_DOMAIN_
%token NIS_SERVER_, NISP_SERVER_, NIS_DOMAIN_, NISP_DOMAIN_
%token FQDN_, FQDN_S_, DDNS_PROTOCOL_, DDNS_TIMEOUT_
%token LIFETIME_, VENDOR_SPEC_
%token IFACE_,NO_CONFIG_,REJECT_SERVERS_,PREFERRED_SERVERS_
%token IA_,TA_,IAID_,ADDRESS_KEYWORD_, NAME_, IPV6ADDR_,WORKDIR_, RAPID_COMMIT_
%token OPTION_, SCRIPT_
%token LOGNAME_, LOGLEVEL_, LOGMODE_, LOGCOLORS_
%token <strval>     STRING_
%token <ival>       HEXNUMBER_
%token <ival>       INTNUMBER_
%token <addrval>    IPV6ADDR_
%token <duidval>    DUID_
%token STRICT_RFC_NO_ROUTING_
%token SKIP_CONFIRM_, OBEY_RA_BITS_
%token PD_, PREFIX_, DOWNLINK_PREFIX_IFACES_
%token DUID_TYPE_, DUID_TYPE_LLT_, DUID_TYPE_LL_, DUID_TYPE_EN_
%token AUTH_METHODS_, AUTH_PROTOCOL_, AUTH_ALGORITHM_, AUTH_REPLAY_, AUTH_REALM_
%token DIGEST_NONE_, DIGEST_PLAIN_, DIGEST_HMAC_MD5_, DIGEST_HMAC_SHA1_, DIGEST_HMAC_SHA224_
%token DIGEST_HMAC_SHA256_, DIGEST_HMAC_SHA384_, DIGEST_HMAC_SHA512_
%token STATELESS_, ANON_INF_REQUEST_, INSIST_MODE_, INACTIVE_MODE_
%token EXPERIMENTAL_, ADDR_PARAMS_, REMOTE_AUTOCONF_
%token AFTR_
%token ROUTING_, BIND_TO_ADDR_
%token ADDRESS_LIST_KEYWORD_, STRING_KEYWORD_, DUID_KEYWORD_, HEX_KEYWORD_
%token RECONFIGURE_
%type  <ival> Number

%%

/////////////////////////////////////////////////////////////////////////////
// rules section
/////////////////////////////////////////////////////////////////////////////

Grammar
: GlobalDeclarationList
|
;

GlobalDeclarationList
: GlobalOptionDeclaration
| InterfaceDeclaration
| GlobalDeclarationList GlobalOptionDeclaration
| GlobalDeclarationList InterfaceDeclaration
;

GlobalOptionDeclaration
: InterfaceOptionDeclaration
| LogModeOption
| LogNameOption
| LogLevelOption
| LogColors
| WorkDirOption
| DuidTypeOption
| StrictRfcNoRoutingOption
| ScriptName
| DdnsProtocol
| DdnsTimeout
| AuthAcceptMethods
| AuthProtocol
| AuthAlgorithm
| AuthReplay
| AuthRealm
| AnonInfRequest
| InactiveMode
| InsistMode
| FQDNBits
| Experimental
| SkipConfirm
| ReconfigureAccept
| DownlinkPrefixInterfaces
| ObeyRaBits
;

InterfaceOptionDeclaration
: IAOptionDeclaration
| Routing
| StatelessMode
| UnicastOption
| DNSServerOption
| DomainOption
| NTPServerOption
| TimeZoneOption
| SIPServerOption
| SIPDomainOption
| FQDNOption
| NISServerOption
| NISPServerOption
| NISDomainOption
| NISPDomainOption
| LifetimeOption
| VendorSpecOption
| DsLiteTunnelOption
| RejectServersOption
| PreferServersOption
| ExtraOption
| ExperimentalRemoteAutoconf
| BindToAddress
;

IAOptionDeclaration
: T1Option
| T2Option
| RapidCommitOption
| AddressParameter
| ExperimentalAddrParams
;

DownlinkPrefixInterfaces
: DOWNLINK_PREFIX_IFACES_ {
    PresentStringLst.clear();
} StringList {
    CfgMgr->setDownlinkPrefixIfaces(PresentStringLst);
}

InterfaceDeclaration
/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
:IFACE_ STRING_ '{'
{
    if (!StartIfaceDeclaration($2))
	YYABORT;
}
InterfaceDeclarationsList '}'
{
    delete [] $2;
    if (!EndIfaceDeclaration())
	YYABORT;
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 { T1 10 T2 20 ... }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number '{'
{
    if (!IfaceDefined($2))
	YYABORT;
    if (!StartIfaceDeclaration($2))
	YYABORT;
}
InterfaceDeclarationsList '}'
{
    if (!EndIfaceDeclaration())
	YYABORT;
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' { }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ STRING_ '{' '}'
{
    if (!IfaceDefined(string($2)))
	YYABORT;
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    delete [] $2;
    EmptyIface();
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 { }
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number '{' '}'
{
    if (!IfaceDefined($2))
	YYABORT;
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    EmptyIface();
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 'eth0' no-config
/////////////////////////////////////////////////////////////////////////////
|IFACE_ STRING_ NO_CONFIG_
{
    if (!IfaceDefined(string($2)))
	YYABORT;
    ClntCfgIfaceLst.append(new TClntCfgIface($2));
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->setNoConfig();
    delete $2;
}

/////////////////////////////////////////////////////////////////////////////
//Declaration: iface 5 no-config
/////////////////////////////////////////////////////////////////////////////
|IFACE_ Number NO_CONFIG_
{
    if (!IfaceDefined($2))
	YYABORT;
    ClntCfgIfaceLst.append(SPtr<TClntCfgIface> (new TClntCfgIface($2)) );
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->setNoConfig();
}
;

////////////////////////////////////////////////////////////////////////////
//Interface specific declarations //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
InterfaceDeclarationsList
: InterfaceOptionDeclaration
| InterfaceDeclarationsList InterfaceOptionDeclaration
| IADeclaration
| InterfaceDeclarationsList IADeclaration
| TADeclaration
| InterfaceDeclarationsList TADeclaration
| PDDeclaration
| InterfaceDeclarationsList PDDeclaration
;

TADeclaration
/////////////////////////////////////////////////////////////////////////////
// TA options
/////////////////////////////////////////////////////////////////////////////
:TA_
{
    if (!ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Attempted to use TA (stateful option) in stateless mode." << LogEnd;
        YYABORT;
    }

    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
}
|TA_ '{'
{
    if (!ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Attempted to use TA (stateful option) in stateless mode." << LogEnd;
        YYABORT;
    }

    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
    this->iaidSet = false;
}
TADeclarationList
{
    if (this->iaidSet)
	this->ClntCfgTALst.getLast()->setIAID(this->iaid);
}
'}'
|TA_ '{' '}'
{
    if (!ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Attempted to use TA (stateful option) in stateless mode." << LogEnd;
        YYABORT;
    }

    this->ClntCfgTALst.append( new TClntCfgTA() ); // append new TA
}
;

TADeclarationList
:TADeclarationList IAID
|IAID
;

IAID
:IAID_ Number
{
    this->iaidSet = true;
    this->iaid = $2;
    Log(Crit) << "IAID=" << this->iaid << " parsed." << LogEnd;
}
;

IADeclaration
/////////////////////////////////////////////////////////////////////////////
// ia { options-inside }
/////////////////////////////////////////////////////////////////////////////
:IA_ '{'
{
    if (!StartIADeclaration(false)) {
        YYABORT;
    }
}
IADeclarationList '}'
{
    EndIADeclaration();
}

|IA_ Number '{'
{
    if (!StartIADeclaration(false)) {
        YYABORT;
    }
    this->iaid = $2;
}
IADeclarationList '}'
{
    EndIADeclaration();
    Log(Info) << "Setting IAID to " << this->iaid << LogEnd;
    ClntCfgIALst.getLast()->setIAID(this->iaid);
}

/////////////////////////////////////////////////////////////////////////////
// ia { }
/////////////////////////////////////////////////////////////////////////////
|IA_ '{' '}'
{
    if (!StartIADeclaration(true)) {
        YYABORT;
    }
    EndIADeclaration();
}

/////////////////////////////////////////////////////////////////////////////
// ia (1 address is added by default)
/////////////////////////////////////////////////////////////////////////////
|IA_
{
    if (!StartIADeclaration(true)) {
        YYABORT;
    }
    EndIADeclaration();
}

|IA_ Number
{
    if (!StartIADeclaration(true)) {
        YYABORT;
    }
    EndIADeclaration();
    Log(Info) << "Setting IAID to " << $2 << LogEnd;
    ClntCfgIALst.getLast()->setIAID($2);
}

;

IADeclarationList
:IAOptionDeclaration
|IADeclarationList IAOptionDeclaration
|ADDRESDeclaration
|IADeclarationList ADDRESDeclaration
;

// This covers the following declarations:
// 1. address (send an empty address)
// 2. address 2001:db8::1 (send this specific address)
// 3. address 5 (send 5 addresses)
// 4. address { ... } (send an empty address with the following parameters)
// 5. address 2001:d8b::1 { ... } (spend specific address with the following parameters)
// 6. address 5 { ... } (send 5 addresses with the following parameters)
ADDRESDeclaration

// 1. address (send an empty address)
: ADDRESS_KEYWORD_
{
    EmptyAddr();
}

// 2. address 2001:db8::1 (send this specific address)
| ADDRESS_KEYWORD_ IPV6ADDR_ {
    ClntCfgAddrLst.append(new TClntCfgAddr(new TIPv6Addr($2)));
    ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
}

// 3. address 5 (send 5 addresses)
|ADDRESS_KEYWORD_ Number
{
    for (int i = 0; i < $2; i++) {
        EmptyAddr();
    }
}

// 4. address { ... } (send an empty address with the following parameters)
| ADDRESS_KEYWORD_ '{'
{
    // Get last context
    SPtr<TClntParsGlobalOpt> globalOpt = ParserOptStack.getLast();

    // Create new context based on the current one
    SPtr<TClntParsGlobalOpt> newOpt = new TClntParsGlobalOpt(*globalOpt);

    // Add this new context to the contexts stack
    ParserOptStack.append(newOpt);
}
AddressParametersList '}'
{
    EmptyAddr(); // Create an empty address
    ParserOptStack.delLast(); // Delete new context
}

// 5. address 2001:d8b::1 { ... } (spend specific address with the following parameters)
| ADDRESS_KEYWORD_ IPV6ADDR_ '{'
{
    // We need to store just one address, but let's use PresentAddrLst
    // We'll need that address to create an actual object when the context is closed
    PresentAddrLst.clear();
    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr($2)));

    SPtr<TClntParsGlobalOpt> globalOpt = ParserOptStack.getLast();
    SPtr<TClntParsGlobalOpt> newOpt = new TClntParsGlobalOpt(*globalOpt);
    ParserOptStack.append(newOpt);
}
AddressParametersList '}'
{
    ClntCfgAddrLst.append(new TClntCfgAddr(PresentAddrLst.getLast()));
    ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
    if (ParserOptStack.count())
	ParserOptStack.delLast();
    PresentAddrLst.clear();
}

// 6. address 5 { ... } (send 5 addresses with the following parameters)
|ADDRESS_KEYWORD_ Number '{'
{
    //In this agregated declaration no address hints are allowed
    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ParserOptStack.getLast()->setAddrHint(false);

    AddrCount_ = $2;
}
AddressParametersList '}'
{
    for (unsigned int i = 0; i < AddrCount_; i++) {
        EmptyAddr();
        ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
    }
    ParserOptStack.delLast();
    AddrCount_ = 0;
}
;

AddressParametersList
:  AddressParameter
|  AddressParametersList AddressParameter
;

AddressParameter
: PreferredTimeOption
| ValidTimeOption
;

LogLevelOption
: LOGLEVEL_ Number
{
    if ( ($2<1) || ($2>8) ) {
	Log(Crit) << "Invalid loglevel specified: " << $2 << ". Allowed range: 1-8." << LogEnd;
    }
    logger::setLogLevel($2);
}
;

LogModeOption
: LOGMODE_ STRING_ {
    logger::setLogMode($2);
}

LogNameOption
: LOGNAME_ STRING_ {
    logger::setLogName($2);
}

LogColors
: LOGCOLORS_ Number
{
    logger::setColors($2==1);
}

DuidTypeOption
: DUID_TYPE_ DUID_TYPE_LLT_  { this->DUIDType  = DUID_TYPE_LLT;}
| DUID_TYPE_ DUID_TYPE_LL_   { this->DUIDType  = DUID_TYPE_LL; }
| DUID_TYPE_ DUID_TYPE_EN_ Number DUID_ {
  this->DUIDType       = DUID_TYPE_EN;
  this->DUIDEnterpriseNumber = $3;
  this->DUIDEnterpriseID     = new TDUID($4.duid, $4.length);
}
;

StatelessMode
:   STATELESS_
{
    if (!ClntCfgIALst.empty()) {
        Log(Crit) << "Attempting to enable statelss, but IA (stateful option) is already defined." << LogEnd;
        YYABORT;
    }

    if (!ClntCfgTALst.empty()) {
        Log(Crit) << "Attempting to enable statelss, but TA (stateful option) is already defined." << LogEnd;
        YYABORT;
    }

    if (!ClntCfgPDLst.empty()) {
        Log(Crit) << "Attempting to enable statelss, but PD (stateful option) is already defined." << LogEnd;
        YYABORT;
    }

    ParserOptStack.getLast()->setStateful(false);
}
;

WorkDirOption
:   WORKDIR_ STRING_
{
    ParserOptStack.getLast()->setWorkDir($2);
}
;

StrictRfcNoRoutingOption
: STRICT_RFC_NO_ROUTING_
{
    Log(Warning) << "strict-rfc-no-routing has changed in 1.0.0RC2: it now takes one argument: "
                 << " 0 (address configured with guessed /64 prefix length that may be wrong in "
                 << "some cases; dibbler clients prior to 1.0.0RC2 used this) or 1 (address "
                 << "configured with /128, as RFC specifies); the default has changed in 1.0.0RC2: "
                 << "Dibbler is now RFC conformant." << LogEnd;
}
| STRICT_RFC_NO_ROUTING_ Number
{
    switch ($2) {
    case 0:
        // This is pre 1.0.0RC2 behaviour
        Log(Warning) << "Strict-rfc-no-routing disabled: addresses will be "
                     << "configured with /64 prefix." << LogEnd;
        ParserOptStack.getLast()->setOnLinkPrefixLength(64);
        break;
    case 1:
        // The default is now /128 anyway, so this is a no-op
        Log(Warning) << "Strict-rfc-no-routing enabled (it is the default): "
                     << "addresses will be configured with /128 prefix." << LogEnd;
        break;
    default:
        Log(Crit) << "Invalid parameter passed to strict-rfc-no-routing: " << $2
                  << ", only 0 or 1" << LogEnd;
        YYABORT;
    }
}
;

ScriptName
: SCRIPT_ STRING_
{
    CfgMgr->setScript($2);
}

AuthAcceptMethods
: AUTH_METHODS_
{
    DigestLst.clear();
} DigestList {
#ifndef MOD_DISABLE_AUTH
    CfgMgr->setAuthAcceptMethods(DigestLst);
    DigestLst.clear();
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}

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
#ifndef MOD_DISABLE_AUTH
    Log(Crit) << "auth-algorithm selection is not supported yet." << LogEnd;
    YYABORT;
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
};
| AuthReplay

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

AnonInfRequest
: ANON_INF_REQUEST_
{
    ParserOptStack.getLast()->setAnonInfRequest(true);
};

InactiveMode
: INACTIVE_MODE_
{
    ParserOptStack.getLast()->setInactiveMode(true);
};

InsistMode
: INSIST_MODE_
{
    ParserOptStack.getLast()->setInsistMode(true);
};

Experimental
: EXPERIMENTAL_
{
    Log(Crit) << "Experimental features are allowed." << LogEnd;
    ParserOptStack.getLast()->setExperimental();
};

RejectServersOption
:REJECT_SERVERS_
{
    //ParserOptStack.getLast()->clearRejedSrv();
    PresentStationLst.clear();
} ADDRESDUIDList
{
    ParserOptStack.getLast()->setRejedSrvLst(&PresentStationLst);
}
;

PreferServersOption
:PREFERRED_SERVERS_
{
    PresentStationLst.clear();
} ADDRESDUIDList{
    ParserOptStack.getLast()->setPrefSrvLst(&PresentStationLst);
}
;

BindToAddress
:BIND_TO_ADDR_ IPV6ADDR_
{
    ClntCfgIfaceLst.getLast()->setBindToAddr(SPtr<TIPv6Addr>(new TIPv6Addr($2)));
}

PreferredTimeOption
:PREF_TIME_ Number
{
    ParserOptStack.getLast()->setPref($2);
}
;

RapidCommitOption
:   RAPID_COMMIT_ Number
{
    ParserOptStack.getLast()->setRapidCommit($2);
}
;

ExperimentalAddrParams
:   ADDR_PARAMS_
{
	if (!ParserOptStack.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'addr-params' defined, but experimental features are disabled."
		  << "Add 'experimental' in global section of client.conf to enable it." << LogEnd;
	YYABORT;
    }
    ParserOptStack.getLast()->setAddrParams(true);
};

ExperimentalRemoteAutoconf
:   REMOTE_AUTOCONF_
{
    if (!ParserOptStack.getLast()->getExperimental()) {
	Log(Crit) << "Experimental remote autoconfiguration feature defined, but experimental"
	" features are disabled. Add 'experimental' in global section of client.conf "
	"to enable it." << LogEnd;
	YYABORT;
    }
#ifdef MOD_REMOTE_AUTOCONF
    CfgMgr->setRemoteAutoconf(true);
#else
	Log(Error) << "Remote autoconf support not compiled in." << LogEnd;
#endif
};

ObeyRaBits
: OBEY_RA_BITS_
{
    Log(Debug) << "Obeying Router Advertisement (M, O) bits." << LogEnd;
    CfgMgr->obeyRaBits(true);
}

SkipConfirm
: SKIP_CONFIRM_
{
    Log(Debug) << "CONFIRM support disabled (skip-confirm in client.conf)." << LogEnd;
    ParserOptStack.getLast()->setConfirm(false);
};

ReconfigureAccept
: RECONFIGURE_ Number
{
    Log(Debug) << "Reconfigure accept " << (($2>0)?"enabled":"disabled") << "." << LogEnd;
    CfgMgr->setReconfigure($2);
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


ValidTimeOption
:VALID_TIME_ Number
{
    ParserOptStack.getLast()->setValid($2);
}
;

T1Option
:T1_ Number
{
    ParserOptStack.getLast()->setT1($2);
}
;

T2Option
:T2_ Number
{
    ParserOptStack.getLast()->setT2($2);
}
;

PDDeclaration
:PD_
{
    Log(Debug) << "Prefix delegation option (no parameters) found." << LogEnd;
    if (!StartPDDeclaration()) {
        YYABORT;
    }
    EndPDDeclaration();
}
|PD_ '{' '}'
{
    Log(Debug) << "Prefix delegation option (empty scope) found." << LogEnd;
    if (!StartPDDeclaration()) {
        YYABORT;
    }
    EndPDDeclaration();
}
|PD_ '{'
{
    Log(Debug) << "Prefix delegation option (with scope) found." << LogEnd;
    if (!StartPDDeclaration()) {
        YYABORT;
    }
}
PDOptionsList '}'
{
    EndPDDeclaration();
}
|PD_ Number 
{
    Log(Debug) << "Prefix delegation option (with IAID set to " << $2 << " found." << LogEnd;
    if (!StartPDDeclaration()) {
        YYABORT;
    }
    EndPDDeclaration();
    ClntCfgPDLst.getLast()->setIAID($2);
}
|PD_ Number '{'
{
    if (!StartPDDeclaration()) {
        YYABORT;
    }
    this->iaid = $2;
}
PDOptionsList '}'
{
    EndPDDeclaration();
    ClntCfgPDLst.getLast()->setIAID($2);
}
;

PDOptionsList
: PDOption
| PDOptionsList PDOption
;

PDOption
: Prefix
| T1Option
| T2Option
;



Prefix
: PREFIX_ IPV6ADDR_ '/' Number
{
    SPtr<TIPv6Addr> addr = new TIPv6Addr($2);
    SPtr<TClntCfgPrefix> prefix = new TClntCfgPrefix(addr, ($4));
    PrefixLst.append(prefix);
    Log(Debug) << "PD: Adding single prefix " << addr->getPlain() << "/" << ($4) << "." << LogEnd;
}
| PREFIX_
{
    Log(Debug) << "PD: Adding single prefix." << LogEnd;
    SPtr<TClntCfgPrefix> prefix = new TClntCfgPrefix(new TIPv6Addr("::",true), 0);
    PrefixLst.append(prefix);
}

| PREFIX_ '{' '}'
{
    Log(Debug) << "PD: Adding single prefix." << LogEnd;
    SPtr<TClntCfgPrefix> prefix = new TClntCfgPrefix(new TIPv6Addr("::",true), 0);
    PrefixLst.append(prefix);
}

| PREFIX_ '{'
{
}
PrefixOptionsList '}'
{
    Log(Debug) << "PD: Adding single (any) prefix." << LogEnd;
    SPtr<TClntCfgPrefix> prefix = new TClntCfgPrefix(new TIPv6Addr("::",true), 0);
    prefix->setOptions(ParserOptStack.getLast());
    PrefixLst.append(prefix);
}

| PREFIX_ IPV6ADDR_ '/' Number '{'
{
    SPtr<TIPv6Addr> addr = new TIPv6Addr($2);
    SPtr<TClntCfgPrefix> prefix = new TClntCfgPrefix(addr, ($4));
    PrefixLst.append(prefix);
    Log(Debug) << "PD: Adding single prefix " << addr->getPlain() << "/" << ($4) << "." << LogEnd;
}
PrefixOptionsList '}'
{
    PrefixLst.getLast()->setOptions(ParserOptStack.getLast());
}
;

PrefixOptionsList
: PrefixOption
| PrefixOptionsList PrefixOption
;

PrefixOption
: ValidTimeOption
| PreferredTimeOption
;

UnicastOption
:UNICAST_ Number
{
    switch($2) {
    case 0:
	ParserOptStack.getLast()->setUnicast(false);
	break;
    case 1:
	ParserOptStack.getLast()->setUnicast(true);
	break;
    default:
	Log(Error) << "Invalid parameter (" << $2 << ") passed to unicast in line "
		   << Lex_->YYText() << "." << LogEnd;
	return 1;
    }
}
;

Routing
:ROUTING_ Number
{
    switch($2) {
    case 0:
        ClntCfgIfaceLst.getLast()->setRouting(false);
        break;
    case 1:
        ClntCfgIfaceLst.getLast()->setRouting(true);
        break;
    default:
        Log(Error) << "Invalid parameter (" << $2 << ") passed to routing in line "
                   << Lex_->YYText() << "." << LogEnd;
        return 1;
    }
}

ADDRESDUIDList
: IPV6ADDR_
{
    PresentStationLst.append(SPtr<THostID> (new THostID(new TIPv6Addr($1))));
}
| DUID_
{
    PresentStationLst.append(SPtr<THostID> (new THostID(new TDUID($1.duid,$1.length))));
}
| ADDRESDUIDList ',' IPV6ADDR_
{
    PresentStationLst.append(SPtr<THostID> (new THostID(new TIPv6Addr($3))));
}
| ADDRESDUIDList ',' DUID_
{
    PresentStationLst.append(SPtr<THostID> (new THostID( new TDUID($3.duid,$3.length))));
}
;

ADDRESSList
: IPV6ADDR_   {PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr($1)));}
| ADDRESSList ',' IPV6ADDR_   {PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr($3)));}
;

StringList
: STRING_ { PresentStringLst.append(SPtr<string> (new string($1))); }
| StringList ',' STRING_ { PresentStringLst.append(SPtr<string> (new string($3))); }

Number
:  HEXNUMBER_ {$$=$1;}
|  INTNUMBER_ {$$=$1;}
;

//////////////////////////////////////////////////////////////////////
//DNS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DNSServerOption
:OPTION_ DNS_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setDNSServerLst(&PresentAddrLst);
}
|OPTION_ DNS_SERVER_
{
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setDNSServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//DOMAIN option///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DomainOption
:OPTION_ DOMAIN_
{
    PresentStringLst.clear();
    ParserOptStack.getLast()->setDomainLst(&PresentStringLst);
}
|OPTION_ DOMAIN_ {
    PresentStringLst.clear();
} StringList
{
    ParserOptStack.getLast()->setDomainLst(&PresentStringLst);
}
;

//////////////////////////////////////////////////////////////////////
//NTP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NTPServerOption
:OPTION_ NTP_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNTPServerLst(&PresentAddrLst);
}
|OPTION_ NTP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNTPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//TIMEZONE option/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TimeZoneOption
: OPTION_ TIME_ZONE_
{
    ParserOptStack.getLast()->setTimezone(string(""));
  }
|OPTION_ TIME_ZONE_ STRING_
{
    ParserOptStack.getLast()->setTimezone($3);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPServerOption
:OPTION_ SIP_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setSIPServerLst(&PresentAddrLst);
}
|OPTION_ SIP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setSIPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//SIP-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SIPDomainOption
:OPTION_ SIP_DOMAIN_
{
    PresentStringLst.clear();
    ParserOptStack.getLast()->setSIPDomainLst(&PresentStringLst);
}
|OPTION_ SIP_DOMAIN_ {
    PresentStringLst.clear();
} StringList
{
    ParserOptStack.getLast()->setSIPDomainLst(&PresentStringLst);
}
;

//////////////////////////////////////////////////////////////////////
//FQDN option/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
FQDNOption
:OPTION_ FQDN_
{
	char hostname[255];
	if (get_hostname(hostname, 255) == LOWLEVEL_NO_ERROR) {
	    ParserOptStack.getLast()->setFQDN(string(hostname));
	} else {
	    ParserOptStack.getLast()->setFQDN(string(""));
	}
}
|OPTION_ FQDN_ STRING_
{
    ParserOptStack.getLast()->setFQDN($3);
}
;

FQDNBits
:OPTION_ FQDN_S_ Number
{
    if ($3!=0 && $3!=1) {
	Log(Crit) << "Invalid FQDN S bit value: " << $3 << ", expected 0 or 1." << LogEnd;
	YYABORT;
    }

    Log(Info) << "Setting FQDN S bit to " << $3 << LogEnd;
    ParserOptStack.getLast()->setFQDNFlagS($3);
};

//////////////////////////////////////////////////////////////////////
//NIS-SERVER option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISServerOption
:OPTION_ NIS_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNISServerLst(&PresentAddrLst);
}
|OPTION_ NIS_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNISServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NISP-SERVER option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPServerOption
: OPTION_ NISP_SERVER_
{
    PresentAddrLst.clear();
//    PresentAddrLst.append(SPtr<TIPv6Addr> (new TIPv6Addr()));
    ParserOptStack.getLast()->setNISPServerLst(&PresentAddrLst);
}
| OPTION_ NISP_SERVER_ {
    PresentAddrLst.clear();
} ADDRESSList
{
    ParserOptStack.getLast()->setNISPServerLst(&PresentAddrLst);
}
;

//////////////////////////////////////////////////////////////////////
//NIS-DOMAIN option///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISDomainOption
:OPTION_ NIS_DOMAIN_
{
    ParserOptStack.getLast()->setNISDomain("");
}
|OPTION_ NIS_DOMAIN_ STRING_
{
    ParserOptStack.getLast()->setNISDomain($3);
}
;


//////////////////////////////////////////////////////////////////////
//NISP-DOMAIN option//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NISPDomainOption
:OPTION_ NISP_DOMAIN_
{
    ParserOptStack.getLast()->setNISPDomain("");
}
|OPTION_ NISP_DOMAIN_ STRING_
{
    ParserOptStack.getLast()->setNISPDomain($3);
}
;

//////////////////////////////////////////////////////////////////////
//LIFETIME option/////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
LifetimeOption
:OPTION_ LIFETIME_
{
    if (ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Information refresh time (lifetime) option can only be used in stateless mode." << LogEnd;
        YYABORT;
    }
    ParserOptStack.getLast()->setLifetime();
}
;

VendorSpecOption
:OPTION_ VENDOR_SPEC_
{
    Log(Debug) << "VendorSpec defined (no details)." << LogEnd;
    ParserOptStack.getLast()->setVendorSpec();
}
|OPTION_ VENDOR_SPEC_ VendorSpecList
{
    ParserOptStack.getLast()->setVendorSpec();
    Log(Debug) << "VendorSpec defined (multiple times)." << LogEnd;
}
;

VendorSpecList
: Number                     { VendorSpec.append( new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1,0,0,0,0) ); }
| Number '-' Number          { VendorSpec.append( new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $1,$3,0,0,0) ); }
| VendorSpecList ',' Number  { VendorSpec.append( new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3,0,0,0,0) ); }
| VendorSpecList ',' Number '-' Number { VendorSpec.append( new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, $3,$5,0,0,0) ); }
;

DsLiteTunnelOption
: OPTION_ AFTR_
{
    ClntCfgIfaceLst.getLast()->addExtraOption(OPTION_AFTR_NAME, TOpt::Layout_String, false);
}
;

ExtraOption
:OPTION_ Number HEX_KEYWORD_ DUID_
{
    // option 123 hex 0x1234abcd
    SPtr<TOpt> opt = new TOptGeneric($2, $4.duid, $4.length, 0);
    ClntCfgIfaceLst.getLast()->addExtraOption(opt, TOpt::Layout_Duid, true);
    Log(Debug) << "Will send option " << $2 << " (hex data, len" << $4.length << ")" << LogEnd;
}
|OPTION_ Number ADDRESS_KEYWORD_ IPV6ADDR_
{
    // option 123 address 2001:db8::1
    SPtr<TIPv6Addr> addr(new TIPv6Addr($4));

    SPtr<TOpt> opt = new TOptAddr($2, addr, 0);
    ClntCfgIfaceLst.getLast()->addExtraOption(opt, TOpt::Layout_Addr, true);
    Log(Debug) << "Will send option " << $2 << " (address " << addr->getPlain() << ")" << LogEnd;
}
|OPTION_ Number ADDRESS_LIST_KEYWORD_
{
    // option 123 address-list 2001:db8::1,2001:db8::cafe
    PresentAddrLst.clear();
} ADDRESSList
{
    SPtr<TOpt> opt = new TOptAddrLst($2, PresentAddrLst, 0);
    ClntCfgIfaceLst.getLast()->addExtraOption(opt, TOpt::Layout_AddrLst, true);
    Log(Debug) << "Will send option " << $2 << " (address list, containing "
	       << PresentAddrLst.count() << " addresses)." << LogEnd;
}
|OPTION_ Number STRING_KEYWORD_ STRING_
{
    // option 123 string "foobar"
    SPtr<TOpt> opt = new TOptString($2, string($4), 0);
    ClntCfgIfaceLst.getLast()->addExtraOption(opt, TOpt::Layout_String, true);
    Log(Debug) << "Will send option " << $2 << " (string " << $4 << ")" << LogEnd;
}
|OPTION_ Number HEX_KEYWORD_
{
    // just request option 123 and interpret responses as hex
    Log(Debug) << "Will request option " << $2 << " and iterpret response as hex." << LogEnd;
    ClntCfgIfaceLst.getLast()->addExtraOption($2, TOpt::Layout_Duid, false);
}
|OPTION_ Number ADDRESS_KEYWORD_
{
    // just request this option and expect OptAddr layout
    Log(Debug) << "Will request option " << $2 
               << " and interpret response as IPv6 address." << LogEnd;
    ClntCfgIfaceLst.getLast()->addExtraOption($2, TOpt::Layout_Addr, false);
}
|OPTION_ Number STRING_KEYWORD_
{
    // just request this option and expect OptString layout
    Log(Debug) << "Will request option " << $2 << " and interpret response as a string." << LogEnd;
    ClntCfgIfaceLst.getLast()->addExtraOption($2, TOpt::Layout_String, false);
}
|OPTION_ Number ADDRESS_LIST_KEYWORD_
{
    // just request this option and expect OptAddrLst layout
    Log(Debug) << "Will request option " << $2
               << " and interpret response as an address list." << LogEnd;
    ClntCfgIfaceLst.getLast()->addExtraOption($2, TOpt::Layout_AddrLst, false);
};

%%

/////////////////////////////////////////////////////////////////////////////
// programs section
/////////////////////////////////////////////////////////////////////////////


/**
 * method check whether interface with id=ifaceNr has been
 * already declared.
 *
 * @param ifindex interface index of the checked interface
 *
 * @return true if not declared.
 */
bool ClntParser::IfaceDefined(int ifindex)
{
  SPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
  {
      if ((ptr->getID())==ifindex) {
	  Log(Crit) << "Interface with ifindex=" << ifindex << " is already defined." << LogEnd;
	  return false;
      }
  }
  return true;
}

//method check whether interface with id=ifaceName has been
//already declared
/**
 * method check whether interface with specified name has been
 * already declared.
 *
 * @param ifaceName name of the checked interface
 *
 * @return true if not declared.
 */
bool ClntParser::IfaceDefined(const std::string& ifaceName)
{
  SPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
  {
      if (ptr->getName()==ifaceName) {
	  Log(Crit) << "Interface " << ifaceName << " is already defined." << LogEnd;
	  return false;
      }
  };
  return true;
}

/**
 * creates new scope appropriately for interface options and declarations
 * clears all lists except the list of interfaces and adds new group
 */
bool ClntParser::StartIfaceDeclaration(const std::string& ifaceName)
{
    if (!IfaceDefined(ifaceName))
	return false;

    ClntCfgIfaceLst.append(new TClntCfgIface(ifaceName));

    //Interface scope, so parameters associated with global scope are pushed on stack
    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ClntCfgIALst.clear();
    ClntCfgAddrLst.clear();
    this->VendorSpec.clear();
    return true;
}

/**
 * creates new scope appropriately for interface options and declarations
 * clears all lists except the list of interfaces and adds new group
 */
bool ClntParser::StartIfaceDeclaration(int ifindex)
{
    if (!IfaceDefined(ifindex))
	return false;

    ClntCfgIfaceLst.append(new TClntCfgIface(ifindex));

    //Interface scope, so parameters associated with global scope are pushed on stack
    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ClntCfgIALst.clear();
    ClntCfgAddrLst.clear();
    this->VendorSpec.clear();
    return true;
}

bool ClntParser::EndIfaceDeclaration()
{
    SPtr<TClntCfgIface> iface = ClntCfgIfaceLst.getLast();
    if (!iface) {
	Log(Crit) << "Internal error: Interface not found. Something is wrong. Very wrong." << LogEnd;
	return false;
    }

    // set interface options on the basis of just read information
    // preferred-server and rejected-servers are also copied here
    if (VendorSpec.count())
	ParserOptStack.getLast()->setVendorSpec(VendorSpec);
    iface->setOptions(ParserOptStack.getLast());
    iface->setOnLinkPrefixLength(ParserOptStack.getLast()->getOnLinkPrefixLength());

    if ( (iface->stateless()) && (ClntCfgIALst.count()) ) {
	Log(Crit) << "Interface " << iface->getFullName() << " is configured stateless, "
	    " but has " << ClntCfgIALst.count() << " IA(s) defined." << LogEnd;
	return false;
    }

    if ( (iface->stateless()) && (ClntCfgTALst.count()) ) {
	Log(Crit) << "Interface " << iface->getFullName() << " is configured stateless, "
	    " but has TA defined." << LogEnd;
	return false;
    }

    // add all IAs to the interface
    SPtr<TClntCfgIA> ia;
    ClntCfgIALst.first();
    while (ia=ClntCfgIALst.get()) {
	ClntCfgIfaceLst.getLast()->addIA(ia);
    }

    //add all TAs to the interface
    SPtr<TClntCfgTA> ptrTA;
    ClntCfgTALst.first();
    while ( ptrTA = ClntCfgTALst.get() ) {
	iface->addTA(ptrTA);
    }

    //add all PDs to the interface
    SPtr<TClntCfgPD> pd;
    ClntCfgPDLst.first();
    while (pd = ClntCfgPDLst.get() ) {
	iface->addPD(pd);
    }

    //restore global options
    ParserOptStack.delLast();
    ClntCfgIALst.clear();
    return true;
}

void ClntParser::EmptyIface()
{
    //set iface options on the basis of recent information
    ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
    //add one IA with one address to this iface
    EmptyIA();
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIfaceLst.getLast()->addIA(ClntCfgIALst.getLast());
}

/// method creates new scope appropriately for interface options and declarations
/// clears list of addresses
///
/// @param aggregation - does this IA contains suboptions ( ia { ... } )
/// @return true if creation was successful
bool ClntParser::StartIADeclaration(bool aggregation)
{
    if (!ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Attempted to use IA (stateful option) in stateless mode." << LogEnd;
        return (false);
    }

    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ParserOptStack.getLast()->setAddrHint(!aggregation);
    ClntCfgAddrLst.clear();

    return (true);
}

/**
 * Inbelivable piece of crap code. If you read this, rewrite this code immediately.
 *
 */
void ClntParser::EndIADeclaration()
{
    if(!ClntCfgAddrLst.count()) {
	EmptyIA();
    } else {
	SPtr<TClntCfgIA> ia = new TClntCfgIA();
	ClntCfgIALst.append(ia);
	SPtr<TClntCfgAddr> ptr;
	ClntCfgAddrLst.first();
	while(ptr=ClntCfgAddrLst.get())
	    ia->addAddr(ptr);
    }

    //set proper options specific for this IA
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());

    ClntCfgAddrLst.clear();
    ParserOptStack.delLast();

    //this IA matches with previous ones and can be grouped with them
    //so it's should be left on the list and be appended with them to present list
}

/// @brief creates PD context
///
/// @return true if initialization was successful
bool ClntParser::StartPDDeclaration()
{
    if (!ParserOptStack.getLast()->getStateful()) {
        Log(Crit) << "Attempted to use PD (stateful option) in stateless mode." << LogEnd;
        return (false);
    }

    ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    ClntCfgAddrLst.clear();
    PrefixLst.clear();
    return (true);
}

bool ClntParser::EndPDDeclaration()
{
    SPtr<TClntCfgPD> pd = new TClntCfgPD();
    pd->setOptions(ParserOptStack.getLast());

    // copy all defined prefixes
    PrefixLst.first();
    SPtr<TClntCfgPrefix> prefix;
    while (prefix = PrefixLst.get()) {
	pd->addPrefix(prefix);
    }

    PrefixLst.clear();

    ClntCfgPDLst.append(pd);
    ParserOptStack.delLast();
    return true;
}

/**
 * method adds 1 IA object (containing 1 address) to the ClntCfgIA list.
 * Both objects' properties are set to last parsed values
 *
 */
void ClntParser::EmptyIA()
{
    EmptyAddr();
    ClntCfgIALst.append(new TClntCfgIA());
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());

    // Commented out: by default sent empty IA, without any addresses
    // ClntCfgIALst.getLast()->addAddr(ClntCfgAddrLst.getLast());
}

/**
 * method adds empty address to the ClntCfgAddrList list and sets
 * its properties to last parsed values
 *
 */
void ClntParser::EmptyAddr()
{
    ClntCfgAddrLst.append(new TClntCfgAddr());
    ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
}


int ClntParser::yylex()
{
    memset(&std::yylval,0, sizeof(std::yylval));
    memset(&this->yylval,0, sizeof(this->yylval));
    int x = Lex_->yylex();
    this->yylval=std::yylval;
    return x;
}

/**
 * This method is called when parsing error is detected.
 *
 * @param m - first invalid character
 */
void ClntParser::yyerror(char *m)
{
    Log(Crit) << "Config parse error: line " << Lex_->lineno()
	      << ", unexpected [" << Lex_->YYText() << "] token." << LogEnd;
}

/**
 * Desctructor. Just cleans things up
 *
 */

ClntParser::~ClntParser() {
    this->ClntCfgIfaceLst.clear();
    this->ClntCfgIALst.clear();
    this->ClntCfgTALst.clear();
    this->ClntCfgAddrLst.clear();
}
