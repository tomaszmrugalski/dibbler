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
  yy_clntParser_stype yylval;
};
%}

%%

{EOL}*   ; // ignore end of line
[ \t]    ; // ignore TABs and spaces
iface     {return clntParser::IFACE_;}
no-config {return clntParser::NO_CONFIG_;}
address   {return clntParser::ADDRES_;}
ia        {return clntParser::IA_;}
no-ia     {return clntParser::STATELESS_;}
stateless {return clntParser::STATELESS_;}

log-level {return clntParser::LOGLEVEL_; }
log-mode  {return clntParser::LOGMODE_; }
log-name  {return clntParser::LOGNAME_; }

work-dir  {return clntParser::WORKDIR_;}

prefered-lifetime { return clntParser::PREF_TIME_; }
valid-lifetime    { return clntParser::VALID_TIME_; }
t1                { return clntParser::T1_;}
t2                { return clntParser::T2_;}

option       {return clntParser::OPTION_; }
dns-server   { return clntParser::DNS_SERVER_;}
domain       { return clntParser::DOMAIN_;}
ntp-server   { return clntParser::NTP_SERVER_;}
time-zone    { return clntParser::TIME_ZONE_;}
sip-server   { return clntParser::SIP_SERVER_; }
sip-domain   { return clntParser::SIP_DOMAIN_; }
fqdn         { return clntParser::FQDN_; }
nis-server   { return clntParser::NIS_SERVER_; }
nis-domain   { return clntParser::NIS_DOMAIN_; }
nis\+-server { return clntParser::NISP_SERVER_; }
nis\+-domain { return clntParser::NISP_DOMAIN_; }
lifetime     { return clntParser::LIFETIME_; }

reject-server    { return clntParser::REJECT_SERVERS_;}
prefered-server  { return clntParser::PREFERRED_SERVERS_;}
rapid-commit     { return clntParser::RAPID_COMMIT_;}
unicast          { return clntParser::UNICAST_; }

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
        return clntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return clntParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} { 
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        return clntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return clntParser::IPV6ADDR_;
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return clntParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return clntParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        { YYABORT; }
    } else {
        return clntParser::IPV6ADDR_;
    }
}

('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return clntParser::STRING_;
}

([a-zA-Z][a-zA-Z0-9\.-]+) {
    int len = strlen(yytext);
    if ( ( (len>2) && !strncasecmp("yes",yytext,3) ) ||
         ( (len>3) && !strncasecmp("true", yytext,4) )
       ) {
       yylval.ival = 1;
       return clntParser::INTNUMBER_;
   }
    if ( ( (len>1) && !strncasecmp("no",yytext,2) ) ||
         ( (len>4) && !strncasecmp("false",yytext,5) )
       ) {
       yylval.ival = 0;
       return clntParser::INTNUMBER_;
    }

    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return clntParser::STRING_;
}

0x{hexdigit}+ {
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
   return clntParser::DUID_;
}

{hexnumber} { 
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival))) {
        Log(Crit) << "Hex number parsing [" << yytext << "] failed." << LogEnd;
        { YYABORT; }
    }
    return clntParser::HEXNUMBER_;
}

{integer} { 
    if(!sscanf(yytext,"%u",&(yylval.ival))) { 
        Log(Crit) << "Integer parsing [" << yytext << "] failed." << LogEnd;
        { YYABORT; }
    }
    return clntParser::INTNUMBER_;
}



.  {return yytext[0];} 



%%
