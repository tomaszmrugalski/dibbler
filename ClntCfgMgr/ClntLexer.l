/* <Linux> */
%option noyywrap
%option yylineno
/* </Linux> */


%{
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

%{
  using namespace std;

namespace std {
  unsigned ComBeg; //line, in which comment begins
  unsigned LftCnt; //how many signs : on the left side of :: sign was interpreted
  unsigned RgtCnt; //the same as above, but on the right side of ::
  char Address[16]; //address, which is analizying right now
  char AddrPart[16];
  unsigned intpos,pos;
  unsigned analyzeBigPart(char* bigPart, unsigned length, char *dst);
  int decodeSmallPart(char *src, char* dst,int length);
  void decodeIntPart(char *src, char *dst);
  void resetAddress(char *tab);
  yy_clntParser_stype yylval;
};
%}

%%

  // place your Lex rules here
iface     {return clntParser::IFACE_;}
no-config {return clntParser::NO_CONFIG_;}
address   {return clntParser::ADDRES_;}
ia        {return clntParser::IA_;}
no-ia     {return clntParser::NOIA_;}

log-level {return clntParser::LOGLEVEL_; }
log-mode  {return clntParser::LOGMODE_; }
log-name  {return clntParser::LOGNAME_; }

work-dir  {return clntParser::WORKDIR_;}
option    {return clntParser::OPTION_; }

%{    //KEYWORDS FOR OPTIONS
  // [-|REQUEST|REQUIRE] OptionName [SEND|DEFAULT|SUPERSEDE|-] Value
  // [APPEND|PREPEND] OptionName Value
  
  //REQUEST - only request of option from a server - messages without will be
  //                    accepted (default)
%}
request {return clntParser::REQUEST_;}     
%{  //REQUIRE - messages from server must include this option, otherwise its 
  //                    message will be discarded by client
%}
require {return clntParser::REQUIRE_;} 

%{    //SEND -    only hint of option's value sent to server - if server dosen't 
    //              include it nothing happens(default)
%}
send {return clntParser::SEND_;}                                 
%{  //DEFAULT - default value of option. If server dosen't include this option 
  //                    this default value should be used
%}
default {return clntParser::DEFAULT_;} 
%{  //SUPERSEDE - whatever server returns overwrite returned value
%}
supersede {return clntParser::SUPERSEDE_;} 
%{  //APPEND - for options which can have multiple values, ordered in sequence
    //                 e.g. list of DNS servers, orders append list of options at the
    //               end of those returned by server
%}
append {return clntParser::APPEND_;} 
%{  //PREPPEND - for options which can have multiple values, ordered in sequence
  //                 e.g. list of DNS servers, orders append list of options at the
    //               beginning of those returned by server
%}
prepend {return clntParser::PREPEND_;}  


%{    //OPTIONS NAMES
%}
prefered-lifetime {return clntParser::PREF_TIME_;}
valid-lifetime  {return clntParser::VALID_TIME_;}
t1 {return clntParser::T1_;}
t2 {return clntParser::T2_;}

dns-server {return clntParser::DNS_SERVER_;}
domain {return clntParser::DOMAIN_;}

ntp-server {return clntParser::NTP_SERVER_;}
time-zone {return clntParser::TIME_ZONE_;}

reject-server    { return clntParser::REJECT_SERVERS_;}
prefered-server  { return clntParser::PREFERRED_SERVERS_;}
rapid-commit     { return clntParser::RAPID_COMMIT_;}

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
    //printf ("Niezamkniêty komentarz /* w linii:%d\n",ComBeg); 
  { YYABORT; }
}

%{    //IPv6 address - various forms
%}
({hex1to4}:){7}{hexdigit} {
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else
    /*analyzeBigPart(yytext,strlen(yytext)-2,Address);
    memcpy(yylval.addrval,Address,16);*/
        return clntParser::IPV6ADDR_;
}

(({hex1to4}:){1,6})?{hex1to4}"::"(({hex1to4}:){1,6})?{hex1to4} {
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else  
  /*pos=0;
  while ((yytext[pos]!=':')||(yytext[pos+1]!=':')) pos++;
  LftCnt=analyzeBigPart(yytext,pos,AddrPart);
  resetAddress(Address);
  memcpy(Address, AddrPart,16);
  pos+=2;
    RgtCnt=analyzeBigPart(yytext+pos,strlen(yytext+pos),AddrPart);
    if ((LftCnt+RgtCnt)>8)
      { YYABORT; } //to many address parts - raise exception
    memcpy( Address+16-(RgtCnt<<1), AddrPart,RgtCnt<<1);
    memcpy(yylval.addrval,Address,16);*/
    return clntParser::IPV6ADDR_;
}

"::"(({hex1to4}:){1,7})?{hex1to4} { 
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else  
  /*resetAddress(Address);
  RgtCnt=analyzeBigPart(yytext+2,strlen(yytext)-2,AddrPart);
    memcpy(Address+16-(RgtCnt<<1), AddrPart, RgtCnt<<1);
    memcpy(yylval.addrval,Address,16);*/
        return clntParser::IPV6ADDR_;
}

(({hex1to4}:){0,7})?{hex1to4}:: {
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else  
    /*analyzeBigPart(yytext,strlen(yytext)-2,Address);
    memcpy(yylval.addrval,Address,16);*/
        return clntParser::IPV6ADDR_;
}

"::" {
    /*resetAddress(Address);
    memcpy(yylval.addrval,Address,16);*/
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else  
        return clntParser::IPV6ADDR_;
}

(({hex1to4}:){1,5})?{hex1to4}"::"(({hex1to4}:){1,6})?{integer}"."{integer}"."{integer}"."{integer} {
  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else
  /*pos=0;
  while ((yytext[pos]!=':')||(yytext[pos+1]!=':')) pos++;
  LftCnt=analyzeBigPart(yytext,pos,AddrPart);
  resetAddress(Address);
  memcpy(Address, AddrPart,16);
  intpos=(pos+=2);
    while (yytext[intpos]!='.') intpos++;
    while (yytext[intpos]!=':') intpos--;
    if(intpos>pos)
    {
      RgtCnt=analyzeBigPart(yytext+pos,intpos-pos,AddrPart);
      if ((RgtCnt+LftCnt)>6) { YYABORT; }
      memcpy(Address+12-(RgtCnt<<1), AddrPart, RgtCnt<<1);  
    }
    decodeIntPart(yytext+intpos+1, Address+12);
    memcpy(yylval.addrval,Address,16);*/
      return clntParser::IPV6ADDR_;
}

"::"(({hex1to4}":"){1,6})?{integer}"."{integer}"."{integer}"."{integer} {

  if(!inet_pton6(yytext,yylval.addrval))
      { YYABORT; }
    else  
    /*resetAddress(Address);
  intpos=0;
    while (yytext[intpos]!='.') intpos++;
    while (yytext[intpos]!=':') intpos--;
    if (intpos>1)
    {
    RgtCnt=analyzeBigPart(yytext+2,intpos-2,AddrPart);
      memcpy(Address+12-(RgtCnt<<1), AddrPart, RgtCnt<<1);
    };
    decodeIntPart(yytext+intpos+1, Address+12);
    memcpy(yylval.addrval,Address,16);*/
      return clntParser::IPV6ADDR_;
}

%{  //STRING (interface identifier,dns server etc.)
  //FIXME: Remember to free memory in parser    
%}
('([^']|(''))*')|(\"[^\"]*\") {
    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
        return clntParser::STRING_;
}

([a-zA-Z][a-zA-Z0-9\.]+) {
    yylval.strval=new char[strlen(yytext)+1];
    strncpy(yylval.strval, yytext, strlen(yytext));
    yylval.strval[strlen(yytext)]=0;
    return clntParser::STRING_;
}

%{  //FIXME: Remember to free memory in parser
  //DUID
%}
\"{hexdigit}+\" {
    //every hex digit corresponds to half of byte, even at the begining
    //of string
  if (strlen(yytext)%2) 
        yytext[0]='0'; //if odd than no-meaning zero at the beginning
  else
        yytext++;
  //and now there is an even number of hex digits
    yylval.duidval.length=(strlen(yytext)-1)>>1;
  yylval.duidval.duid=new char[yylval.duidval.length];
    for (int i=0;i<(yylval.duidval.length<<1);i+=1)
    {
        yylval.duidval.duid[i>>1]<<=4;
        if (!isxdigit(yytext[i])) 
            { YYABORT; } //or raise exeception
        else
        if (isalpha(yytext[i]))
            yylval.duidval.duid[i>>1]|=toupper(yytext[i])-'A'+10;
        else
            yylval.duidval.duid[i>>1]|=yytext[i]-'0';
  }
    return clntParser::DUID_;
}

%{    //HEXADECIMAL NUMBER (interface id, option's values etc.)
%}
{hexnumber} {
    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival)))
      { YYABORT; }
    return clntParser::HEXNUMBER_;
}

%{    //DECIMAL NUMBER (as for HEXNUMBER)
%}
{integer} { 
    //FIXME:Check if number appropriate
    if(!sscanf(yytext,"%u",&(yylval.ival)))
      { YYABORT; }
    return clntParser::INTNUMBER_;
}

%{    //WHITE CHARS should be omitted
%}
[ \n\t] ; 


%{    //OTHER CHARS are direclty passed to parser
%}
.  {return yytext[0];}


%{    // place your Lex rules here
%}


%%

/////////////////////////////////////////////////////////////////////////////
// programs section

 /*void clntLexer::resetAddress(char *tab)
 {
  int i;
  for (i=0;i<16;i++) tab[i]=0;
 }

 void clntLexer::decodeIntPart(char *src, char *dst)
 {
  
  int i,obyte;
  int ipos=0;
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
 }

 int clntLexer::decodeSmallPart(char *src, char* dst,int length)
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

 unsigned clntLexer::analyzeBigPart(char* bigPart, unsigned length, char *dst)
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
*/
