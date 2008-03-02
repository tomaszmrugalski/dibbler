%option noyywrap
%option yylineno

%{
#ifdef WIN32
#define strncasecmp _strnicmp
#endif

#include "RelParser.h"
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
  yy_RelParser_stype yylval;
}
%}

%%

{EOL}*   ; // ignore end of line
[ \t]    ; // ignore TABs and spaces

iface               { return RelParser::IFACE_;}
client              { return RelParser::CLIENT_;}
server              { return RelParser::SERVER_; }
unicast             { return RelParser::UNICAST_; }
multicast           { return RelParser::MULTICAST_; }
interface-id        { return RelParser::IFACE_ID_; }
interface-id-order  { return RelParser::IFACE_ID_ORDER_; }
guess-mode          { return RelParser::GUESS_MODE_; }
option              { return RelParser::OPTION_; }
remote-id           { return RelParser::REMOTE_ID_; }
echo-request        { return RelParser::ECHO_REQUEST_; }

log-name            { return RelParser::LOGNAME_;}
log-level           { return RelParser::LOGLEVEL_;}
log-mode            { return RelParser::LOGMODE_; }

work-dir            { return RelParser::WORKDIR_;}

yes                 { yylval.ival=1; return RelParser::INTNUMBER_;}
no                  { yylval.ival=0; return RelParser::INTNUMBER_;}
true                { yylval.ival=1; return RelParser::INTNUMBER_;}
false               { yylval.ival=0; return RelParser::INTNUMBER_;}

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
        return RelParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} { 
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
        Log(Crit) << "Invalid address format: [" << yytext << "]" << LogEnd;
        YYABORT; 
    } else {
        return RelParser::IPV6ADDR_;
    }
}

('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return RelParser::STRING_;
}

([a-zA-Z][a-zA-Z0-9\.-]+) {
    int len = strlen(yytext);
    if ( ( (len>2) && !strncasecmp("yes",yytext,3) ) ||
         ( (len>3) && !strncasecmp("true", yytext,4) )
       ) {
       yylval.ival = 1;
       return RelParser::INTNUMBER_;
   }
    if ( ( (len>1) && !strncasecmp("no",yytext,2) ) ||
         ( (len>4) && !strncasecmp("false",yytext,5) )
       ) {
       yylval.ival = 0;
       return RelParser::INTNUMBER_;
    }

    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return RelParser::STRING_;
}

{hexnumber} {
    // HEX NUMBER
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival))) {
      Log(Crit) << "Hex value [" << yytext << " parsing failed." << LogEnd; 
      YYABORT; 
    }
    return RelParser::HEXNUMBER_;
}

{integer} { 
    if(!sscanf(yytext,"%u",&(yylval.ival))) { 
        Log(Crit) << "Decimal value [" << yytext << " parsing failed." << LogEnd; 
        YYABORT; 
    }
    return RelParser::INTNUMBER_;
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
   return RelParser::DUID_;
}

.  { return yytext[0]; } 


%%
