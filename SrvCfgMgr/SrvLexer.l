%{ /* -*- C++ -*- */

#include "SrvParserContext.h"

#ifdef WIN32
#define strncasecmp _strnicmp
#endif

using namespace std;

#include "SrvParser.h"
#include "Portable.h"
#define YYABORT yyterminate();

/* duplicate default definition from flex. This makes cppcheck
   check for defined YY_FATAL_ERROR() go away. */
#define YY_FATAL_ERROR(msg) SrvParserContext::fatal(msg)

using namespace dibbler;

%}

%option noyywrap
%option yylineno
%option nounput
%option batch
%option debug
%option noinput

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

%}

%%

{EOL}*   ; // ignore end of line
[ \t]    ; // ignore TABs and spaces

iface               { return SrvParser::make_IFACE_(driver.loc_); }
class               { return SrvParser::make_CLASS_(driver.loc_); }
ta-class            { return SrvParser::make_TACLASS_(driver.loc_);  }
stateless           { return SrvParser::make_STATELESS_(driver.loc_);  }
relay               { return SrvParser::make_RELAY_(driver.loc_);  }
interface-id        { return SrvParser::make_IFACE_ID_(driver.loc_);  }
interface-id-order  { return SrvParser::make_IFACE_ID_ORDER_(driver.loc_);  }

log-name            { return SrvParser::make_LOGNAME_(driver.loc_); }
log-level           { return SrvParser::make_LOGLEVEL_(driver.loc_); }
log-mode            { return SrvParser::make_LOGMODE_(driver.loc_);  }
log-colors          { return SrvParser::make_LOGCOLORS_(driver.loc_);  }

work-dir            { return SrvParser::make_WORKDIR_(driver.loc_); }

accept-only         { return SrvParser::make_ACCEPT_ONLY_(driver.loc_); }
reject-clients      { return SrvParser::make_REJECT_CLIENTS_(driver.loc_); }

T1                  { return SrvParser::make_T1_(driver.loc_); }
T2                  { return SrvParser::make_T2_(driver.loc_); }
preferred-lifetime  { return SrvParser::make_PREF_TIME_(driver.loc_); }
prefered-lifetime   { return SrvParser::make_PREF_TIME_(driver.loc_); }
valid-lifetime      { return SrvParser::make_VALID_TIME_(driver.loc_); }

drop-unicast        { return SrvParser::make_DROP_UNICAST_(driver.loc_);  }
unicast             { return SrvParser::make_UNICAST_(driver.loc_); }
preference          { return SrvParser::make_PREFERENCE_(driver.loc_); }
pool                { return SrvParser::make_POOL_(driver.loc_); }
share               { return SrvParser::make_SHARE_(driver.loc_); }
rapid-commit        { return SrvParser::make_RAPID_COMMIT_(driver.loc_); }
iface-max-lease     { return SrvParser::make_IFACE_MAX_LEASE_(driver.loc_);  }
class-max-lease     { return SrvParser::make_CLASS_MAX_LEASE_(driver.loc_);  }
client-max-lease    { return SrvParser::make_CLNT_MAX_LEASE_(driver.loc_);   }
client              { return SrvParser::make_CLIENT_(driver.loc_);  }
duid                { return SrvParser::make_DUID_KEYWORD_(driver.loc_);  }
remote-id           { return SrvParser::make_REMOTE_ID_(driver.loc_);  }
link-local					{ return SrvParser::make_LINK_LOCAL_(driver.loc_);  }
address             { return SrvParser::make_ADDRESS_(driver.loc_); }
prefix              { return SrvParser::make_PREFIX_(driver.loc_);  }
guess-mode          { return SrvParser::make_GUESS_MODE_(driver.loc_);  }

option              { return SrvParser::make_OPTION_(driver.loc_);  }
dns-server          { return SrvParser::make_DNS_SERVER_(driver.loc_); }
domain              { return SrvParser::make_DOMAIN_(driver.loc_); }
ntp-server          { return SrvParser::make_NTP_SERVER_(driver.loc_); }
time-zone           { return SrvParser::make_TIME_ZONE_(driver.loc_); }
sip-server          { return SrvParser::make_SIP_SERVER_(driver.loc_);  }
sip-domain          { return SrvParser::make_SIP_DOMAIN_(driver.loc_);  }
next-hop            { return SrvParser::make_NEXT_HOP_(driver.loc_);  }
subnet              { return SrvParser::make_SUBNET_(driver.loc_);  }
route               { return SrvParser::make_ROUTE_(driver.loc_);  }
fqdn                { return SrvParser::make_FQDN_(driver.loc_);  }
infinite            { return SrvParser::make_INFINITE_(driver.loc_);  }
accept-unknown-fqdn { return SrvParser::make_ACCEPT_UNKNOWN_FQDN_(driver.loc_);  }
fqdn-ddns-address   { return SrvParser::make_FQDN_DDNS_ADDRESS_(driver.loc_);  }
ddns-protocol       { return SrvParser::make_DDNS_PROTOCOL_(driver.loc_);  }
ddns-timeout        { return SrvParser::make_DDNS_TIMEOUT_(driver.loc_);  }
nis-server          { return SrvParser::make_NIS_SERVER_(driver.loc_);  }
nis-domain          { return SrvParser::make_NIS_DOMAIN_(driver.loc_);  }
nis\+-server        { return SrvParser::make_NISP_SERVER_(driver.loc_);  }
nis\+-domain        { return SrvParser::make_NISP_DOMAIN_(driver.loc_);  }
lifetime            { return SrvParser::make_LIFETIME_(driver.loc_);  }
cache-size          { return SrvParser::make_CACHE_SIZE_(driver.loc_);  }
pd-class	    { return SrvParser::make_PDCLASS_(driver.loc_);  }
pd-length	    { return SrvParser::make_PD_LENGTH_(driver.loc_);  }
pd-pool             { return SrvParser::make_PD_POOL_(driver.loc_); }
vendor-spec         { return SrvParser::make_VENDOR_SPEC_(driver.loc_);  }
script              { return SrvParser::make_SCRIPT_(driver.loc_);  }

experimental        { return SrvParser::make_EXPERIMENTAL_(driver.loc_);  }
addr-params         { return SrvParser::make_ADDR_PARAMS_(driver.loc_);  }
neighbors           { return SrvParser::make_REMOTE_AUTOCONF_NEIGHBORS_(driver.loc_);  }

aftr                { return SrvParser::make_AFTR_(driver.loc_);  }
inactive-mode       { return SrvParser::make_INACTIVE_MODE_(driver.loc_);  }
accept-leasequery   { return SrvParser::make_ACCEPT_LEASEQUERY_(driver.loc_);  }
bulk-leasequery-accept    { return SrvParser::make_BULKLQ_ACCEPT_(driver.loc_);  }
bulk-leasequery-tcp-port  { return SrvParser::make_BULKLQ_TCPPORT_(driver.loc_);  }
bulk-leasequery-max-conns { return SrvParser::make_BULKLQ_MAX_CONNS_(driver.loc_);  }
bulk-leasequery-timeout   { return SrvParser::make_BULKLQ_TIMEOUT_(driver.loc_);  }
auth-protocol       { return SrvParser::make_AUTH_PROTOCOL_(driver.loc_);  }
auth-algorithm      { return SrvParser::make_AUTH_ALGORITHM_(driver.loc_);  }
auth-replay         { return SrvParser::make_AUTH_REPLAY_(driver.loc_); }
auth-realm          { return SrvParser::make_AUTH_REALM_(driver.loc_);  }
auth-methods        { return SrvParser::make_AUTH_METHODS_(driver.loc_);  }
auth-required       { return SrvParser::make_AUTH_DROP_UNAUTH_(driver.loc_);  }
digest-none         { return SrvParser::make_DIGEST_NONE_(driver.loc_);  }
digest-plain        { return SrvParser::make_DIGEST_PLAIN_(driver.loc_);  }
digest-hmac-md5     { return SrvParser::make_DIGEST_HMAC_MD5_(driver.loc_);  }
hmac-md5            { return SrvParser::make_DIGEST_HMAC_MD5_(driver.loc_);  }
digest-hmac-sha1    { return SrvParser::make_DIGEST_HMAC_SHA1_(driver.loc_);  }
hmac-sha1           { return SrvParser::make_DIGEST_HMAC_SHA1_(driver.loc_);  }
digest-hmac-sha224  { return SrvParser::make_DIGEST_HMAC_SHA224_(driver.loc_);  }
hmac-sha224         { return SrvParser::make_DIGEST_HMAC_SHA224_(driver.loc_);  }
digest-hmac-sha256  { return SrvParser::make_DIGEST_HMAC_SHA256_(driver.loc_);  }
hmac-sha256         { return SrvParser::make_DIGEST_HMAC_SHA256_(driver.loc_);  }
digest-hmac-sha384  { return SrvParser::make_DIGEST_HMAC_SHA384_(driver.loc_);  }
hmac-sha384         { return SrvParser::make_DIGEST_HMAC_SHA384_(driver.loc_);  }
digest-hmac-sha512  { return SrvParser::make_DIGEST_HMAC_SHA512_(driver.loc_);  }
hmac-sha512         { return SrvParser::make_DIGEST_HMAC_SHA512_(driver.loc_);  }
key                 { return SrvParser::make_KEY_(driver.loc_);  }
secret              { return SrvParser::make_SECRET_(driver.loc_);  }
algorithm           { return SrvParser::make_ALGORITHM_(driver.loc_);  }
reconfigure-enabled { return SrvParser::make_RECONFIGURE_ENABLED_(driver.loc_);  }
fudge               { return SrvParser::make_FUDGE_(driver.loc_);  }
client-class        { return SrvParser::make_CLIENT_CLASS_(driver.loc_);  }
match-if            { return SrvParser::make_MATCH_IF_(driver.loc_);  }
==                  { return SrvParser::make_EQ_(driver.loc_);  }
and                 { return SrvParser::make_AND_(driver.loc_);  }
or                  { return SrvParser::make_OR_(driver.loc_);  }
client.vendor-spec.en    { return SrvParser::make_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_(driver.loc_);  }
client.vendor-spec.data   { return SrvParser::make_CLIENT_VENDOR_SPEC_DATA_(driver.loc_);  }
client.vendor-class.en    { return SrvParser::make_CLIENT_VENDOR_CLASS_EN_(driver.loc_);  }
client.vendor-class.data   { return SrvParser::make_CLIENT_VENDOR_CLASS_DATA_(driver.loc_);  }
allow		    { return SrvParser::make_ALLOW_(driver.loc_);  }
deny		    { return SrvParser::make_DENY_(driver.loc_);  }
substring	    { return SrvParser::make_SUBSTRING_(driver.loc_);  }
contain	    { return SrvParser::make_CONTAIN_(driver.loc_);  }
string              { return SrvParser::make_STRING_KEYWORD_(driver.loc_);  }
address-list        { return SrvParser::make_ADDRESS_LIST_(driver.loc_);  }
performance-mode    { return SrvParser::make_PERFORMANCE_MODE_(driver.loc_);  }

yes                 { return SrvParser::make_INTNUMBER_(1, driver.loc_); }
no                  { return SrvParser::make_INTNUMBER_(0, driver.loc_); }
true                { return SrvParser::make_INTNUMBER_(1, driver.loc_); }
false               { return SrvParser::make_INTNUMBER_(0, driver.loc_); }

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
	return SrvParser::make_IPV6ADDR_(string(yytext), driver.loc_); 
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(string(yytext), driver.loc_); 
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(string(yytext), driver.loc_); 
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(driver.loc_); 
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(driver.loc_); 
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(driver.loc_); 
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) {
	Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
	YYABORT;
    } else {
	return SrvParser::make_IPV6ADDR_(driver.loc_); 
    }
}

%{ //STRING (interface identifier,dns server etc.)
%}

('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return SrvParser::make_STRING_(driver.loc_); 
}

([a-zA-Z][a-zA-Z0-9\.-]+) {
    int len = strlen(yytext);
    if ( ( (len>2) && !strncasecmp("yes",yytext,3) ) ||
	 ( (len>3) && !strncasecmp("true", yytext,4) )
       ) {
       yylval.ival = 1;
       return SrvParser::make_INTNUMBER_(driver.loc_); 
   }
    if ( ( (len>1) && !strncasecmp("no",yytext,2) ) ||
	 ( (len>4) && !strncasecmp("false",yytext,5) )
       ) {
       yylval.ival = 0;
       return SrvParser::make_INTNUMBER_(driver.loc_); 
    }

    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return SrvParser::make_STRING_(driver.loc_); 
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
   return SrvParser::make_DUID_(driver.loc_); 
}

{hexdigit}{2}(:{hexdigit}{2})+ {
   int len = (strlen(yytext)+1)/3;
   char * pos = 0;
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

   return SrvParser::make_DUID_(driver.loc_); 
}

{hexnumber} {
    // HEX NUMBER
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%20x",&(yylval.ival))) {
      Log(Crit) << "Hex value [" << yytext << "] parsing failed." << LogEnd;
      YYABORT;
    }
    return SrvParser::make_HEXNUMBER_(driver.loc_); 
}

{integer} {
    // DECIMAL NUMBER
    if(!sscanf(yytext,"%20u",&(yylval.ival))) {
	Log(Crit) << "Decimal value [" << yytext << "] parsing failed." << LogEnd;
	YYABORT;
    }
    return SrvParser::make_INTNUMBER_(driver.loc_); 
}

.  { return yytext[0]; }


%%
