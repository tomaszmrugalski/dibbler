%option noyywrap
%option yylineno

%{
#ifdef WIN32
#define strncasecmp _strnicmp
#endif

#include "SrvParser.h"
#include "Portable.h"
#define YYABORT yyterminate();
%}

%x COMMENT
%x ADDR

hexdigit [0-9A-Fa-f]
hexnumber {hexdigit}+h
letter [a-zA-Z]
cipher [0-9]
integer {cipher}+
curly_op [{]
curly_cl [}]
hex1to4  {hexdigit}{1,4}
CR \r
LF \n
EOL (({CR}{LF}?)|{LF})

%{
using namespace std;
  unsigned ComBeg;    // line, in which comment begins
  unsigned LftCnt;    // how many chars : on the left side of '::' char was interpreted
  unsigned RgtCnt;    // the same as above, but on the right side of '::'
  char Address[16];   // address, which is analizing right now
  char AddrPart[16];
  unsigned intpos,pos;

namespace std{
  yy_SrvParser_stype yylval;
}
%}

%%

{EOL}*   ; // ignore end of line
[ \t]    ; // ignore TABs and spaces

iface               { return SrvParser::IFACE_;}
class               { return SrvParser::CLASS_;}
ta-class            { return SrvParser::TACLASS_; }
stateless           { return SrvParser::STATELESS_; }
relay               { return SrvParser::RELAY_; }
interface-id        { return SrvParser::IFACE_ID_; }
interface-id-order  { return SrvParser::IFACE_ID_ORDER_; }

log-name            { return SrvParser::LOGNAME_;}
log-level           { return SrvParser::LOGLEVEL_;}
log-mode            { return SrvParser::LOGMODE_; }
log-colors          { return SrvParser::LOGCOLORS_; }

work-dir            { return SrvParser::WORKDIR_;}

accept-only         { return SrvParser::ACCEPT_ONLY_;}
reject-clients      { return SrvParser::REJECT_CLIENTS_;}

T1                  { return SrvParser::T1_;}
T2                  { return SrvParser::T2_;}
prefered-lifetime   { return SrvParser::PREF_TIME_;}
valid-lifetime      { return SrvParser::VALID_TIME_;}

unicast             { return SrvParser::UNICAST_;}
preference          { return SrvParser::PREFERENCE_;}
pool                { return SrvParser::POOL_;}
share               { return SrvParser::SHARE_;}
rapid-commit        { return SrvParser::RAPID_COMMIT_;}
iface-max-lease     { return SrvParser::IFACE_MAX_LEASE_; }
class-max-lease     { return SrvParser::CLASS_MAX_LEASE_; }
client-max-lease    { return SrvParser::CLNT_MAX_LEASE_;  }
client              { return SrvParser::CLIENT_; }
duid                { return SrvParser::DUID_KEYWORD_; }
remote-id           { return SrvParser::REMOTE_ID_; }
address             { return SrvParser::ADDRESS_;}
guess-mode          { return SrvParser::GUESS_MODE_; }

option              { return SrvParser::OPTION_; }
dns-server          { return SrvParser::DNS_SERVER_;}
domain              { return SrvParser::DOMAIN_;}
ntp-server          { return SrvParser::NTP_SERVER_;}
time-zone           { return SrvParser::TIME_ZONE_;}
sip-server          { return SrvParser::SIP_SERVER_; }
sip-domain          { return SrvParser::SIP_DOMAIN_; }
fqdn                { return SrvParser::FQDN_; }
accept-unknown-fqdn { return SrvParser::ACCEPT_UNKNOWN_FQDN_; }
fqdn-ddns-address   { return SrvParser::FQDN_DDNS_ADDRESS_; }
ddns-protocol       { return SrvParser::DDNS_PROTOCOL_; }
ddns-timeout        { return SrvParser::DDNS_TIMEOUT_; }
nis-server          { return SrvParser::NIS_SERVER_; }
nis-domain          { return SrvParser::NIS_DOMAIN_; }
nis\+-server        { return SrvParser::NISP_SERVER_; }
nis\+-domain        { return SrvParser::NISP_DOMAIN_; }
lifetime            { return SrvParser::LIFETIME_; }
cache-size          { return SrvParser::CACHE_SIZE_; }
pd-class	    { return SrvParser::PDCLASS_; }
pd-length	    { return SrvParser::PD_LENGTH_; }
pd-pool             { return SrvParser::PD_POOL_;}
vendor-spec         { return SrvParser::VENDOR_SPEC_; }

experimental        { return SrvParser::EXPERIMENTAL_; }
addr-params         { return SrvParser::ADDR_PARAMS_; }
neighbors           { return SrvParser::REMOTE_AUTOCONF_NEIGHBORS_; }

aftr                { return SrvParser::AFTR_; }
inactive-mode       { return SrvParser::INACTIVE_MODE_; }
accept-leasequery   { return SrvParser::ACCEPT_LEASEQUERY_; }
bulk-leasequery-accept    { return SrvParser::BULKLQ_ACCEPT_; }
bulk-leasequery-tcp-port  { return SrvParser::BULKLQ_TCPPORT_; }
bulk-leasequery-max-conns { return SrvParser::BULKLQ_MAX_CONNS_; }
bulk-leasequery-timeout   { return SrvParser::BULKLQ_TIMEOUT_; }
auth-method         { return SrvParser::AUTH_METHOD_; }
auth-lifetime       { return SrvParser::AUTH_LIFETIME_; }
auth-key-len        { return SrvParser::AUTH_KEY_LEN_; }
digest-none         { return SrvParser::DIGEST_NONE_; }
digest-plain        { return SrvParser::DIGEST_PLAIN_; }
digest-hmac-md5     { return SrvParser::DIGEST_HMAC_MD5_; }
digest-hmac-sha1    { return SrvParser::DIGEST_HMAC_SHA1_; }
digest-hmac-sha224  { return SrvParser::DIGEST_HMAC_SHA224_; }
digest-hmac-sha256  { return SrvParser::DIGEST_HMAC_SHA256_; }
digest-hmac-sha384  { return SrvParser::DIGEST_HMAC_SHA384_; }
digest-hmac-sha512  { return SrvParser::DIGEST_HMAC_SHA512_; }
client-class        { return SrvParser::CLIENT_CLASS_; }
match-if            { return SrvParser::MATCH_IF_; }
==                  { return SrvParser::EQ_; }
and                  { return SrvParser::AND_; }
or                  { return SrvParser::OR_; }
client.vendor-spec.en    { return SrvParser::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_; }
client.vendor-spec.data   { return SrvParser::CLIENT_VENDOR_SPEC_DATA_; }
client.vendor-class.en    { return SrvParser::CLIENT_VENDOR_CLASS_EN_; }
client.vendor-class.data   { return SrvParser::CLIENT_VENDOR_CLASS_DATA_; }
allow		    { return SrvParser::ALLOW_; }
deny		    { return SrvParser::DENY_; }
substring	    { return SrvParser::SUBSTRING_; }
contain	    { return SrvParser::CONTAIN_; }
string              { return SrvParser::STRING_KEYWORD_; }
address-list        { return SrvParser::ADDRESS_LIST_; }


yes                 { yylval.ival=1; return SrvParser::INTNUMBER_;}
no                  { yylval.ival=0; return SrvParser::INTNUMBER_;}
true                { yylval.ival=1; return SrvParser::INTNUMBER_;}
false               { yylval.ival=0; return SrvParser::INTNUMBER_;}

#.* ;

"//"(.*) ;

"/*" {
  BEGIN(COMMENT);
  ComBeg=yylineno;
}

<COMMENT>"*/" BEGIN(INITIAL);
<COMMENT>.|"\n" ;
<COMMENT><<EOF>> {
    Log(Crit) << "Comment not closed. (/* in line " << ComBeg << LogEnd;
  { YYABORT; }
}

%{ //IPv6 address - various forms
%}
({hex1to4}:){7}{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

%{ //STRING (interface identifier,dns server etc.)
%}

('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return SrvParser::STRING_;
}

([a-zA-Z][a-zA-Z0-9\.-]+) {
    int len = strlen(yytext);
    if ( ( (len>2) && !strncasecmp("yes",yytext,3) ) ||
	 ( (len>3) && !strncasecmp("true", yytext,4) )
       ) {
       yylval.ival = 1;
       return SrvParser::INTNUMBER_;
   }
    if ( ( (len>1) && !strncasecmp("no",yytext,2) ) ||
	 ( (len>4) && !strncasecmp("false",yytext,5) )
       ) {
       yylval.ival = 0;
       return SrvParser::INTNUMBER_;
    }

    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return SrvParser::STRING_;
}

0x{hexdigit}+ {
    // DUID
    int len;
    char * ptr;
    if (strlen(yytext)%2) {
	yytext[1]='0'; //if odd then no-meaning zero at the beginning
	len = strlen(yytext)-1;
	ptr = yytext+1;
    } else {
	len = strlen(yytext)-2;
	ptr = yytext+2;
    }

    //and now there is an even number of hex digits
    yylval.duidval.length = len >> 1;
    yylval.duidval.duid   = new char[len >> 1];
    for (int i=0 ; i<len; i++)
    {
	yylval.duidval.duid[i>>1]<<=4;
	if (!isxdigit(ptr[i])) {
	     Log(Crit) << "DUID parsing failed (" << yytext << ")." << LogEnd;
	     YYABORT;
	}
	if (isalpha(ptr[i])) {
	    yylval.duidval.duid[i>>1]|=toupper(ptr[i])-'A'+10;
	} else {
	    yylval.duidval.duid[i>>1]|=ptr[i]-'0';
	}
   }
   return SrvParser::DUID_;
}

{hexdigit}{2}(:{hexdigit}{2})+ {
   int len = (strlen(yytext)+1)/3;
   char * pos = 0;
   // Log(Crit) << "#### Detected [" << yytext << "] as DUID [len=" << len << "]" << LogEnd;
   yylval.duidval.length = len;
   yylval.duidval.duid   = new char[len];

   int i=0;
   for (pos = yytext; pos<=yytext+strlen(yytext)-2; pos+=3)
   {
      char x;
      if (isalpha(*pos))
	x = (toupper(*pos)-'A' + 10);
      else
	x = *pos-'0';
      x *= 16;

      if (isalpha(*(pos+1)))
	x += (toupper(*(pos+1))-'A' + 10);
      else
	x += *(pos+1) - '0';
      yylval.duidval.duid[i] = x;
      i++;
   }

   return SrvParser::DUID_;
}

{hexnumber} {
    // HEX NUMBER
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival))) {
      Log(Crit) << "Hex value [" << yytext << " parsing failed." << LogEnd;
      YYABORT;
    }
    return SrvParser::HEXNUMBER_;
}

{integer} {
    // DECIMAL NUMBER
    if(!sscanf(yytext,"%u",&(yylval.ival))) {
	Log(Crit) << "Decimal value [" << yytext << " parsing failed." << LogEnd;
	YYABORT;
    }
    return SrvParser::INTNUMBER_;
}

.  { return yytext[0]; }


%%
