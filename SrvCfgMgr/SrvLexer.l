/* <Linux> */
%option noyywrap
%option yylineno
/* </Linux> */

%{
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

%{
using namespace std;
  unsigned ComBeg; //line, in which comment begins
  unsigned LftCnt; //how many chars : on the left side of '::' char was interpreted
  unsigned RgtCnt; //the same as above, but on the right side of '::'
  char Address[16]; //address, which is analizing right now
  char AddrPart[16];
  unsigned intpos,pos;
  unsigned analyzeBigPart(char* bigPart, unsigned length, char *dst);
  int decodeSmallPart(char *src, char* dst,int length);
  bool decodeIntPart(char *src, char *dst);
  void resetAddress(char *tab);
namespace std{
  yy_SrvParser_stype yylval;
};
%}

%%

iface               { return SrvParser::IFACE_;}
no-config           { return SrvParser::NO_CONFIG_;}
class               { return SrvParser::CLASS_;}

log-name            { return SrvParser::LOGNAME_;}
log-level           { return SrvParser::LOGLEVEL_;}
log-mode            { return SrvParser::LOGMODE_; }

work-dir            { return SrvParser::WORKDIR_;}

option              { return SrvParser::OPTION_; }
dns-server          { return SrvParser::DNS_SERVER_;}
domain              { return SrvParser::DOMAIN_;}
ntp-server          { return SrvParser::NTP_SERVER_;}
time-zone           { return SrvParser::TIME_ZONE_;}

accept-only         { return SrvParser::ACCEPT_ONLY_;}
reject-clients      { return SrvParser::REJECT_CLIENTS_;}

T1                  { return SrvParser::T1_;}
T2                  { return SrvParser::T2_;}
prefered-lifetime   { return SrvParser::PREF_TIME_;}
valid-lifetime      { return SrvParser::VALID_TIME_;}

unicast             { return SrvParser::UNICAST_;}
preference          { return SrvParser::PREFERENCE_;}
pool                { return SrvParser::POOL_;}
rapid-commit        { return SrvParser::RAPID_COMMIT_;}
iface-max-lease     { return SrvParser::IFACE_MAX_LEASE_; }
class-max-lease     { return SrvParser::CLASS_MAX_LEASE_; }
client-max-lease    { return SrvParser::CLNT_MAX_LEASE_;  }

yes {yylval.ival=1; return SrvParser::INTNUMBER_;}
no         {yylval.ival=0; return SrvParser::INTNUMBER_;}
true    {yylval.ival=1; return SrvParser::INTNUMBER_;}
false {yylval.ival=0; return SrvParser::INTNUMBER_;}

%{ //[Nn][Ii][Ss]-[Ss][Ee][Rr][Vv][Ee][Rr] {return SrvParser::NIS_SERVER_;}
%}
%{ //COMMENTS
%}

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
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}:){1,7})?{hex1to4} { 
    if(!inet_pton6(yytext,yylval.addrval)) { 
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){0,7})?{hex1to4}:: {
    if(!inet_pton6(yytext,yylval.addrval)) { 
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::" {
    if(!inet_pton6(yytext,yylval.addrval)) { 
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
	YYABORT; 
    } else {
	return SrvParser::IPV6ADDR_;
    }
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
    if(!inet_pton6(yytext,yylval.addrval)) { 
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

([a-zA-Z][a-zA-Z0-9\.]+) {
    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return SrvParser::STRING_;
}

%{ 
//DUID
%}
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
             Log(Error) << "DUID parsing failed." << LogEnd;
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

%{ //HEXADECIMAL NUMBER (interface id, option's values etc.)
%}
{hexnumber} {
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival)))
      { YYABORT; }
    return SrvParser::HEXNUMBER_;
}

%{ //DECIMAL NUMBER (as for HEXNUMBER)
%}
{integer} { 
    //FIXME:Check if number appropriate
    if(!sscanf(yytext,"%u",&(yylval.ival)))
      { YYABORT; }
    return SrvParser::INTNUMBER_;
}

%{ //WHITE CHARS should be omitted
%}
[ \n\t] ; 

%{ //OTHER CHARS are directly passed to parser
%}
.  {return yytext[0];}


%%

/////////////////////////////////////////////////////////////////////////////
// programs section

 void resetAddress(char *tab)
 {
  int i;
  for (i=0;i<16;i++) tab[i]=0;
 }

 bool decodeIntPart(char *src, char *dst)
 {
  
  int i,obyte;
  unsigned int ipos=0;
    for (i=0;i<4;i++)
    {
      obyte=src[ipos]-'0';
      while((src[++ipos]!='.')&&(ipos<strlen(src)))
      {
        obyte=obyte*10+src[ipos]-'0';
        if (obyte>255) { YYABORT; }
      }
      Address[12+i]=obyte;
      ipos++;
    }
    return true;
 }

 int decodeSmallPart(char *src, char* dst,int length)
 {
  int int16=0;
  int i;
  if (length>4)
    { YYABORT; }//there is too many digits abort and raise exception
  for (i=0; i<length; i++) 
  {
    if (!isxdigit(src[i])) 
      return 0; //or raise exeception
    else
      if (isalpha(src[i]))
        int16=(int16<<4)+toupper(src[i])-55;
      else

        int16=(int16<<4)+src[i]-48;
  }
  dst[1]=*((char*)(&int16));
  dst[0]=*(((char*)(&int16))+1);
  return 1;
 }

 unsigned analyzeBigPart(char* bigPart, unsigned length, char *dst)
 {
  unsigned srcStart,dstStart,pos;
  srcStart=pos=0;
  dstStart=0;
  resetAddress(dst);
  do
  {
    //extracting part of string to next ':' sign or its end
    while((pos<length)&&(bigPart[pos]!=':')) pos++;
    //decode this part to binary form
    decodeSmallPart(bigPart+srcStart,dst+dstStart,pos-srcStart);
    //bigPart[pos]!=':'?(pos-srcStart-1):
    //next two bytes found
    dstStart+=2;
    //omission of : sign    
    srcStart=++pos;
  }while(pos<=length);
  return (dstStart)>>1;
 }
