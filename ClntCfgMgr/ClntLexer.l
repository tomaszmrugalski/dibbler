%option noyywrap
%option yylineno

%{
#ifdef WIN32
#define strncasecmp _strnicmp
#endif

#include "ClntParser.h"
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

namespace std {
  unsigned ComBeg; //line, in which comment begins
  unsigned LftCnt; //how many signs : on the left side of :: sign was interpreted
  unsigned RgtCnt; //the same as above, but on the right side of ::
  char Address[16]; //address, which is analizying right now
  char AddrPart[16];
  unsigned intpos,pos;
  yy_ClntParser_stype yylval;
}
%}

%%

{EOL}*   ; // ignore end of line
[ \t]    ; // ignore TABs and spaces
iface                 { return ClntParser::IFACE_;}
no-config             { return ClntParser::NO_CONFIG_;}
address               { return ClntParser::ADDRESS_;}
string                { return ClntParser::STRING_KEYWORD_; }
address-list          { return ClntParser::ADDRESS_LIST_; }
name                  { return ClntParser::NAME_; }
ia                    { return ClntParser::IA_;}
ta                    { return ClntParser::TA_; }
iaid                  { return ClntParser::IAID_; }
no-ia                 { return ClntParser::STATELESS_;}
stateless             { return ClntParser::STATELESS_;}
log-level             { return ClntParser::LOGLEVEL_; }
log-mode              { return ClntParser::LOGMODE_; }
log-name              { return ClntParser::LOGNAME_; }
log-colors            { return ClntParser::LOGCOLORS_; }

work-dir              { return ClntParser::WORKDIR_;}
scripts-dir           { return ClntParser::SCRIPTS_DIR_; }
notify-scripts        { return ClntParser::NOTIFY_SCRIPTS_; }
prefered-lifetime     { return ClntParser::PREF_TIME_; }
valid-lifetime        { return ClntParser::VALID_TIME_; }
remote-autoconf       { return ClntParser::REMOTE_AUTOCONF_; }
t1                    { return ClntParser::T1_;}
t2                    { return ClntParser::T2_;}
option                { return ClntParser::OPTION_; }
dns-server            { return ClntParser::DNS_SERVER_;}
domain                { return ClntParser::DOMAIN_;}
ntp-server            { return ClntParser::NTP_SERVER_;}
time-zone             { return ClntParser::TIME_ZONE_;}
sip-server            { return ClntParser::SIP_SERVER_; }
sip-domain            { return ClntParser::SIP_DOMAIN_; }
fqdn                  { return ClntParser::FQDN_; }
fqdn-s                { return ClntParser::FQDN_S_; }
nis-server            { return ClntParser::NIS_SERVER_; }
nis-domain            { return ClntParser::NIS_DOMAIN_; }
nis\+-server          { return ClntParser::NISP_SERVER_; }
nis\+-domain          { return ClntParser::NISP_DOMAIN_; }
lifetime              { return ClntParser::LIFETIME_; }
reject-server         { return ClntParser::REJECT_SERVERS_;}
prefered-server       { return ClntParser::PREFERRED_SERVERS_;}
rapid-commit          { return ClntParser::RAPID_COMMIT_;}
reconfigure-accept    { return ClntParser::RECONFIGURE_; }
unicast               { return ClntParser::UNICAST_; }
strict-rfc-no-routing { return ClntParser::STRICT_RFC_NO_ROUTING_; }
prefix-delegation     { return ClntParser::PD_; }
pd                    { return ClntParser::PD_; }
prefix                { return ClntParser::PREFIX_; }
duid-type             { return ClntParser::DUID_TYPE_; }
DUID-LL               { return ClntParser::DUID_TYPE_LL_; }
DUID-LLT              { return ClntParser::DUID_TYPE_LLT_; }
DUID-EN               { return ClntParser::DUID_TYPE_EN_; }
vendor-spec           { return ClntParser::VENDOR_SPEC_; }
anonymous-inf-request { return ClntParser::ANON_INF_REQUEST_; }
insist-mode           { return ClntParser::INSIST_MODE_; }
inactive-mode         { return ClntParser::INACTIVE_MODE_; }
auth-enabled          { return ClntParser::AUTH_ENABLED_; }
auth-accept-methods   { return ClntParser::AUTH_ACCEPT_METHODS_; }
digest-none           { return ClntParser::DIGEST_NONE_; }
digest-plain          { return ClntParser::DIGEST_PLAIN_; }
digest-hmac-md5       { return ClntParser::DIGEST_HMAC_MD5_; }
digest-hmac-sha1      { return ClntParser::DIGEST_HMAC_SHA1_; }
digest-hmac-sha224    { return ClntParser::DIGEST_HMAC_SHA224_; }
digest-hmac-sha256    { return ClntParser::DIGEST_HMAC_SHA256_; }
digest-hmac-sha384    { return ClntParser::DIGEST_HMAC_SHA384_; }
digest-hmac-sha512    { return ClntParser::DIGEST_HMAC_SHA512_; }
skip-confirm          { return ClntParser::SKIP_CONFIRM_; }
aftr                  { return ClntParser::AFTR_; }

experimental          { return ClntParser::EXPERIMENTAL_; }
addr-params           { return ClntParser::ADDR_PARAMS_; }
request               { return ClntParser::REQUEST_; }

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

%{    //IPv6 address - various forms
%}
({hex1to4}:){7}{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} { 
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        return ClntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return ClntParser::IPV6ADDR_;
    }
}

('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return ClntParser::STRING_;
}

([a-zA-Z][a-zA-Z0-9\.-]+) {
    int len = strlen(yytext);
    if ( ( (len>2) && !strncasecmp("yes",yytext,3) ) ||
         ( (len>3) && !strncasecmp("true", yytext,4) )
       ) {
       yylval.ival = 1;
       return ClntParser::INTNUMBER_;
   }
    if ( ( (len>1) && !strncasecmp("no",yytext,2) ) ||
         ( (len>4) && !strncasecmp("false",yytext,5) )
       ) {
       yylval.ival = 0;
       return ClntParser::INTNUMBER_;
    }

    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return ClntParser::STRING_;
}

0x{hexdigit}+ {
    // DUID in 0x00010203 format
    int len;
    char * ptr;
    if (strlen(yytext)%2) {
        yytext[1]='0'; //if odd than no-meaning zero at the beginning
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
             { YYABORT; }
        } 
        if (isalpha(ptr[i])) {
            yylval.duidval.duid[i>>1]|=toupper(ptr[i])-'A'+10;
        } else {
            yylval.duidval.duid[i>>1]|=ptr[i]-'0';
        }
   }
   return ClntParser::DUID_;
}

{hexdigit}{2}(:{hexdigit}{2})+ {
   // DUID in 00:01:02:03 format
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
   
   return ClntParser::DUID_;
}

{hexnumber} { 
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",(unsigned int*)&(yylval.ival))) {
        Log(Crit) << "Hex number parsing [" << yytext << "] failed." << LogEnd;
        { YYABORT; }
    }
    return ClntParser::HEXNUMBER_;
}

{integer} { 
    if(!sscanf(yytext,"%u",(unsigned int*)&(yylval.ival))) { 
        Log(Crit) << "Integer parsing [" << yytext << "] failed." << LogEnd;
        { YYABORT; }
    }
    return ClntParser::INTNUMBER_;
}

.  {return yytext[0];} 

%%
