#include <clex.h>

#line 1 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

/****************************************************************************
lexer.l
ParserWizard generated Lex file.

Date: 30 czerwca 2003
****************************************************************************/

#include "srvparser.h"
#include "Portable.h"

#line 16 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
// repeated because of possible precompiled header
#include <clex.h>

#include "srvLexer.h"

/////////////////////////////////////////////////////////////////////////////
// constructor

YYLEXNAME::YYLEXNAME()
{
	yytables();
#line 47 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

    // place any extra initialisation code here

#line 32 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
}

#ifndef YYTEXT_SIZE
#define YYTEXT_SIZE 100
#endif
#ifndef YYUNPUT_SIZE
#define YYUNPUT_SIZE YYTEXT_SIZE
#endif

// backwards compatability with lex
#ifdef input
int YYLEXNAME::yyinput()
{
	return input();
}
#else
#define input yyinput
#endif

#ifdef output
void YYLEXNAME::yyoutput(int ch)
{
	output(ch);
}
#else
#define output yyoutput
#endif

#ifdef unput
void YYLEXNAME::yyunput(int ch)
{
	unput(ch);
}
#else
#define unput yyunput
#endif

#ifndef YYNBORLANDWARN
#ifdef __BORLANDC__
#pragma warn -rch		// <warning: unreachable code> off
#endif
#endif

int YYLEXNAME::yyaction(int action)
{
#line 58 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

// extract yylval for use later on in actions
YYSTYPE& yylval = *(YYSTYPE*)yyparserptr->yylvalptr;

#line 83 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
	yyreturnflg = 1;
	switch (action) {
	case 1:
		{
#line 67 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return IFACE_;
#line 90 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 2:
		{
#line 68 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return NO_CONFIG_;
#line 97 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 3:
		{
#line 69 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return CLASS_;
#line 104 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 4:
		{
#line 71 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return LOGNAME_;
#line 111 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 5:
		{
#line 72 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return LOGLEVEL_;
#line 118 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 6:
		{
#line 73 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return WORKDIR_;
#line 125 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 7:
		{
#line 75 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return DNS_SERVER_;
#line 132 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 8:
		{
#line 76 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return DOMAIN_;
#line 139 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 9:
		{
#line 78 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return NTP_SERVER_;
#line 146 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 10:
		{
#line 79 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return TIME_ZONE_;
#line 153 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
#line 81 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
 //[Nn][Ii][Ss]-[Ss][Ee][Rr][Vv][Ee][Rr] {return NIS_SERVER_;}
#line 158 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
	case 11:
		{
#line 83 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return ACCEPT_ONLY_;
#line 163 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 12:
		{
#line 84 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return REJECT_CLIENTS_;
#line 170 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 13:
		{
#line 86 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return T1_;
#line 177 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 14:
		{
#line 87 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return T2_;
#line 184 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 15:
		{
#line 89 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return PREF_TIME_;
#line 191 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 16:
		{
#line 90 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return VALID_TIME_;
#line 198 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 17:
		{
#line 92 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return UNICAST_;
#line 205 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 18:
		{
#line 93 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return PREFERENCE_;
#line 212 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 19:
		{
#line 94 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return POOL_;
#line 219 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 20:
		{
#line 95 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return RAPID_COMMIT_;
#line 226 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 21:
		{
#line 96 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return MAX_LEASE_;
#line 233 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 22:
		{
#line 97 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return CLNT_MAX_LEASE_;
#line 240 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 23:
		{
#line 99 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
yylval.ival=1; return INTNUMBER_;
#line 247 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 24:
		{
#line 100 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
yylval.ival=0; return INTNUMBER_;
#line 254 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 25:
		{
#line 101 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
yylval.ival=1; return INTNUMBER_;
#line 261 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 26:
		{
#line 102 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
yylval.ival=0; return INTNUMBER_;
#line 268 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 27:
		{
#line 105 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
;
#line 275 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 28:
		{
#line 107 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
;
#line 282 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 29:
		{
#line 109 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  BEGIN(COMMENT);
  ComBeg=yylineno; 

#line 292 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 30:
		{
#line 115 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
BEGIN(INITIAL);
#line 299 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 31:
		{
#line 116 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
;
#line 306 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 32:
		{
#line 117 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

//  printf ("Niezamkniêty komentarz /* w linii:%d\n",ComBeg); 
  yyabort();

#line 316 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
#line 122 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
    //IPv6 address - various forms
#line 321 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
	case 33:
		{
#line 123 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
    else
    /*analyzeBigPart(yytext,strlen(yytext)-2,Address);
    memcpy(yylval.addrval,Address,16);*/
        return IPV6ADDR_;

#line 333 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 34:
		{
#line 132 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
    else  
  /*pos=0;
  while ((yytext[pos]!=':')||(yytext[pos+1]!=':')) pos++;
  LftCnt=analyzeBigPart(yytext,pos,AddrPart);
  resetAddress(Address);
  memcpy(Address, AddrPart,16);
  pos+=2;
    RgtCnt=analyzeBigPart(yytext+pos,strlen(yytext+pos),AddrPart);
    if ((LftCnt+RgtCnt)>8)
      yyabort(); //to many address parts - raise exception
    memcpy( Address+16-(RgtCnt<<1), AddrPart,RgtCnt<<1);
    memcpy(yylval.addrval,Address,16);*/
    return IPV6ADDR_;

#line 356 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 35:
		{
#line 150 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
 
  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
    else  
  /*resetAddress(Address);
  RgtCnt=analyzeBigPart(yytext+2,strlen(yytext)-2,AddrPart);
    memcpy(Address+16-(RgtCnt<<1), AddrPart, RgtCnt<<1);
    memcpy(yylval.addrval,Address,16);*/
        return IPV6ADDR_;

#line 372 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 36:
		{
#line 161 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
    else  
    /*analyzeBigPart(yytext,strlen(yytext)-2,Address);
    memcpy(yylval.addrval,Address,16);*/
        return IPV6ADDR_;

#line 386 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 37:
		{
#line 170 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

    /*resetAddress(Address);
    memcpy(yylval.addrval,Address,16);*/
  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
    else  
        return IPV6ADDR_;

#line 400 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 38:
		{
#line 179 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
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
      if ((RgtCnt+LftCnt)>6) yyabort();
      memcpy(Address+12-(RgtCnt<<1), AddrPart, RgtCnt<<1);  
    }
    decodeIntPart(yytext+intpos+1, Address+12);
    memcpy(yylval.addrval,Address,16);*/
      return IPV6ADDR_;

#line 428 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 39:
		{
#line 202 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"


  if(!inet_pton6(yytext,yylval.addrval))
      yyabort();
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
      return IPV6ADDR_;

#line 452 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 40:
		{
#line 223 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

    yylval.strval=new char[strlen(yytext)-1];
    strncpy(yylval.strval, yytext+1, strlen(yytext)-2);
    yylval.strval[strlen(yytext)-2]=0;
    return STRING_;

#line 464 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 41:
		{
#line 231 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

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
            yyabort(); //or raise exeception
        else
        if (isalpha(yytext[i]))
            yylval.duidval.duid[i>>1]|=toupper(yytext[i])-'A'+10;
        else
            yylval.duidval.duid[i>>1]|=yytext[i]-'0';
   }
   return DUID_;

#line 493 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 42:
		{
#line 256 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

    yytext[strlen(yytext)-1]='\n';
    if(!sscanf(yytext,"%x",&(yylval.ival)))
      yyabort();
    return HEXNUMBER_;

#line 505 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 43:
		{
#line 264 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
 
    //FIXME:Check if number appropriate
    if(!sscanf(yytext,"%u",&(yylval.ival)))
      yyabort();
    return INTNUMBER_;

#line 517 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 44:
		{
#line 272 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
;
#line 524 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	case 45:
		{
#line 275 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"
return yytext[0];
#line 531 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"
		}
		break;
	default:
		yyassert(0);
		break;
	}
	yyreturnflg = 0;
	return 0;
}

#ifndef YYNBORLANDWARN
#ifdef __BORLANDC__
#pragma warn .rch		// <warning: unreachable code> to the old state
#endif
#endif
#line 278 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"


/////////////////////////////////////////////////////////////////////////////
// programs section

 void SrvLexer::resetAddress(char *tab)
 {
  int i;
  for (i=0;i<16;i++) tab[i]=0;
 }

 void SrvLexer::decodeIntPart(char *src, char *dst)
 {
  
  int i,obyte;
  int ipos=0;
    for (i=0;i<4;i++)
    {
      obyte=src[ipos]-'0';
      while((src[++ipos]!='.')&&(ipos<strlen(src)))
      {
        obyte=obyte*10+src[ipos]-'0';
        if (obyte>255) yyabort();
      }
      Address[12+i]=obyte;
      ipos++;
    }
 }

 int SrvLexer::decodeSmallPart(char *src, char* dst,int length)
 {
  int int16=0;
  int i;
  if (length>4)
    yyabort();//there is too many digits abort and raise exception
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

 unsigned SrvLexer::analyzeBigPart(char* bigPart, unsigned length, char *dst)
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

#line 620 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.cpp"

void YYLEXNAME::yytables()
{
	yystext_size = YYTEXT_SIZE;
	yysunput_size = YYUNPUT_SIZE;

	static const yyctransition_t YYNEARFAR YYBASED_CODE ctransition[] = {
		{ 0, 0, -1 },
		{ 9, 9, 29 },
		{ 10, 10, 30 },
		{ 32, 32, 29 },
		{ 34, 34, 28 },
		{ 35, 35, 22 },
		{ 39, 39, 27 },
		{ 47, 47, 23 },
		{ 48, 57, 24 },
		{ 58, 58, 26 },
		{ 65, 65, 14 },
		{ 66, 66, 25 },
		{ 67, 67, 9 },
		{ 68, 68, 12 },
		{ 69, 69, 25 },
		{ 70, 70, 21 },
		{ 73, 73, 7 },
		{ 76, 76, 10 },
		{ 77, 77, 19 },
		{ 78, 78, 8 },
		{ 80, 80, 16 },
		{ 82, 82, 15 },
		{ 84, 84, 13 },
		{ 85, 85, 18 },
		{ 86, 86, 17 },
		{ 87, 87, 11 },
		{ 89, 89, 20 },
		{ 97, 97, 14 },
		{ 98, 98, 25 },
		{ 99, 99, 9 },
		{ 100, 100, 12 },
		{ 101, 101, 25 },
		{ 102, 102, 21 },
		{ 105, 105, 7 },
		{ 108, 108, 10 },
		{ 109, 109, 19 },
		{ 110, 110, 8 },
		{ 112, 112, 16 },
		{ 114, 114, 15 },
		{ 116, 116, 13 },
		{ 117, 117, 18 },
		{ 118, 118, 17 },
		{ 119, 119, 11 },
		{ 121, 121, 20 },
		{ 0, 0, -1 },
		{ 70, 70, 32 },
		{ 102, 102, 32 },
		{ 0, 0, -1 },
		{ 79, 79, 33 },
		{ 84, 84, 34 },
		{ 111, 111, 33 },
		{ 116, 116, 34 },
		{ 0, 0, -1 },
		{ 76, 76, 35 },
		{ 108, 108, 35 },
		{ 0, 0, -1 },
		{ 79, 79, 39 },
		{ 111, 111, 39 },
		{ 0, 0, -1 },
		{ 79, 79, 40 },
		{ 111, 111, 40 },
		{ 0, 0, -1 },
		{ 78, 78, 41 },
		{ 79, 79, 42 },
		{ 110, 110, 41 },
		{ 111, 111, 42 },
		{ 0, 0, -1 },
		{ 49, 49, 44 },
		{ 50, 50, 45 },
		{ 73, 73, 43 },
		{ 82, 82, 46 },
		{ 105, 105, 43 },
		{ 114, 114, 46 },
		{ 0, 0, -1 },
		{ 67, 67, 47 },
		{ 99, 99, 47 },
		{ 0, 0, -1 },
		{ 65, 65, 49 },
		{ 69, 69, 48 },
		{ 97, 97, 49 },
		{ 101, 101, 48 },
		{ 0, 0, -1 },
		{ 79, 79, 51 },
		{ 82, 82, 50 },
		{ 111, 111, 51 },
		{ 114, 114, 50 },
		{ 0, 0, -1 },
		{ 65, 65, 52 },
		{ 97, 97, 52 },
		{ 0, 0, -1 },
		{ 78, 78, 53 },
		{ 110, 110, 53 },
		{ 0, 0, -1 },
		{ 65, 65, 54 },
		{ 97, 97, 54 },
		{ 0, 0, -1 },
		{ 69, 69, 55 },
		{ 101, 101, 55 },
		{ 0, 0, -1 },
		{ 65, 65, 56 },
		{ 97, 97, 56 },
		{ 0, 0, -1 },
		{ 42, 42, 59 },
		{ 47, 47, 58 },
		{ 0, 0, -1 },
		{ 48, 57, 60 },
		{ 0, 0, -1 },
		{ 48, 57, 36 },
		{ 65, 70, 36 },
		{ 97, 102, 36 },
		{ 0, 0, -1 },
		{ 58, 58, 61 },
		{ 0, 0, -1 },
		{ 34, 34, 0 },
		{ 0, 0, -1 },
		{ 65, 65, 65 },
		{ 97, 97, 65 },
		{ 0, 0, -1 },
		{ 45, 45, 66 },
		{ 0, 0, -1 },
		{ 80, 80, 67 },
		{ 112, 112, 67 },
		{ 0, 0, -1 },
		{ 65, 65, 68 },
		{ 73, 73, 69 },
		{ 97, 97, 68 },
		{ 105, 105, 69 },
		{ 0, 0, -1 },
		{ 76, 76, 0 },
		{ 108, 108, 88 },
		{ 0, 0, -1 },
		{ 48, 57, 71 },
		{ 65, 70, 71 },
		{ 97, 102, 71 },
		{ 0, 0, -1 },
		{ 71, 71, 73 },
		{ 103, 103, 73 },
		{ 0, 0, -1 },
		{ 82, 82, 74 },
		{ 114, 114, 74 },
		{ 0, 0, -1 },
		{ 83, 83, 75 },
		{ 115, 115, 75 },
		{ 0, 0, -1 },
		{ 77, 77, 76 },
		{ 109, 109, 76 },
		{ 0, 0, -1 },
		{ 77, 77, 77 },
		{ 109, 109, 77 },
		{ 0, 0, -1 },
		{ 85, 85, 78 },
		{ 117, 117, 78 },
		{ 0, 0, -1 },
		{ 69, 69, 79 },
		{ 101, 101, 79 },
		{ 0, 0, -1 },
		{ 74, 74, 80 },
		{ 106, 106, 80 },
		{ 0, 0, -1 },
		{ 80, 80, 81 },
		{ 112, 112, 81 },
		{ 0, 0, -1 },
		{ 69, 69, 82 },
		{ 101, 101, 82 },
		{ 0, 0, -1 },
		{ 79, 79, 83 },
		{ 111, 111, 83 },
		{ 0, 0, -1 },
		{ 76, 76, 84 },
		{ 108, 108, 84 },
		{ 0, 0, -1 },
		{ 73, 73, 85 },
		{ 105, 105, 85 },
		{ 0, 0, -1 },
		{ 88, 88, 86 },
		{ 120, 120, 86 },
		{ 0, 0, -1 },
		{ 83, 83, 87 },
		{ 115, 115, 87 },
		{ 0, 0, -1 },
		{ 48, 57, 70 },
		{ 76, 76, 88 },
		{ 108, 108, 88 },
		{ 0, 0, -1 },
		{ 10, 10, 0 },
		{ 0, 0, -1 },
		{ 10, 10, 0 },
		{ 0, 0, -1 },
		{ 48, 57, 89 },
		{ 65, 70, 70 },
		{ 97, 102, 70 },
		{ 0, 0, -1 },
		{ 48, 57, 90 },
		{ 65, 70, 91 },
		{ 97, 102, 91 },
		{ 0, 0, -1 },
		{ 39, 39, 63 },
		{ 0, 0, -1 },
		{ 39, 39, 62 },
		{ 0, 0, -1 },
		{ 34, 34, 92 },
		{ 48, 57, 64 },
		{ 65, 70, 64 },
		{ 97, 102, 64 },
		{ 0, 0, -1 },
		{ 67, 67, 93 },
		{ 99, 99, 93 },
		{ 0, 0, -1 },
		{ 67, 67, 94 },
		{ 99, 99, 94 },
		{ 0, 0, -1 },
		{ 45, 45, 95 },
		{ 0, 0, -1 },
		{ 83, 83, 96 },
		{ 115, 115, 96 },
		{ 0, 0, -1 },
		{ 69, 69, 97 },
		{ 101, 101, 97 },
		{ 0, 0, -1 },
		{ 80, 80, 0 },
		{ 112, 112, 109 },
		{ 0, 0, -1 },
		{ 48, 57, 99 },
		{ 65, 70, 99 },
		{ 97, 102, 99 },
		{ 0, 0, -1 },
		{ 48, 57, 101 },
		{ 65, 70, 102 },
		{ 97, 102, 102 },
		{ 0, 0, -1 },
		{ 45, 45, 103 },
		{ 0, 0, -1 },
		{ 75, 75, 104 },
		{ 107, 107, 104 },
		{ 0, 0, -1 },
		{ 45, 45, 105 },
		{ 0, 0, -1 },
		{ 65, 65, 106 },
		{ 97, 97, 106 },
		{ 0, 0, -1 },
		{ 69, 69, 107 },
		{ 101, 101, 107 },
		{ 0, 0, -1 },
		{ 69, 69, 108 },
		{ 101, 101, 108 },
		{ 0, 0, -1 },
		{ 48, 57, 98 },
		{ 80, 80, 109 },
		{ 112, 112, 109 },
		{ 0, 0, -1 },
		{ 69, 69, 110 },
		{ 101, 101, 110 },
		{ 0, 0, -1 },
		{ 73, 73, 111 },
		{ 105, 105, 111 },
		{ 0, 0, -1 },
		{ 70, 70, 112 },
		{ 102, 102, 112 },
		{ 0, 0, -1 },
		{ 76, 76, 113 },
		{ 108, 108, 113 },
		{ 0, 0, -1 },
		{ 73, 73, 114 },
		{ 105, 105, 114 },
		{ 0, 0, -1 },
		{ 67, 67, 115 },
		{ 99, 99, 115 },
		{ 0, 0, -1 },
		{ 45, 45, 116 },
		{ 0, 0, -1 },
		{ 83, 83, 117 },
		{ 115, 115, 117 },
		{ 0, 0, -1 },
		{ 48, 57, 118 },
		{ 65, 70, 98 },
		{ 97, 102, 98 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 119 },
		{ 0, 0, -1 },
		{ 48, 57, 120 },
		{ 65, 70, 120 },
		{ 97, 102, 120 },
		{ 0, 0, -1 },
		{ 69, 69, 123 },
		{ 101, 101, 123 },
		{ 0, 0, -1 },
		{ 79, 79, 124 },
		{ 111, 111, 124 },
		{ 0, 0, -1 },
		{ 83, 83, 125 },
		{ 115, 115, 125 },
		{ 0, 0, -1 },
		{ 83, 83, 126 },
		{ 115, 115, 126 },
		{ 0, 0, -1 },
		{ 78, 78, 127 },
		{ 110, 110, 127 },
		{ 0, 0, -1 },
		{ 58, 58, 37 },
		{ 0, 0, -1 },
		{ 48, 57, 129 },
		{ 65, 70, 129 },
		{ 97, 102, 129 },
		{ 0, 0, -1 },
		{ 48, 57, 130 },
		{ 65, 70, 130 },
		{ 97, 102, 130 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 131 },
		{ 0, 0, -1 },
		{ 48, 57, 132 },
		{ 65, 70, 132 },
		{ 97, 102, 132 },
		{ 0, 0, -1 },
		{ 76, 76, 136 },
		{ 78, 78, 135 },
		{ 108, 108, 136 },
		{ 110, 110, 135 },
		{ 0, 0, -1 },
		{ 45, 45, 137 },
		{ 0, 0, -1 },
		{ 83, 83, 138 },
		{ 115, 115, 138 },
		{ 0, 0, -1 },
		{ 73, 73, 139 },
		{ 105, 105, 139 },
		{ 0, 0, -1 },
		{ 45, 45, 140 },
		{ 0, 0, -1 },
		{ 84, 84, 141 },
		{ 116, 116, 141 },
		{ 0, 0, -1 },
		{ 67, 67, 142 },
		{ 99, 99, 142 },
		{ 0, 0, -1 },
		{ 68, 68, 143 },
		{ 100, 100, 143 },
		{ 0, 0, -1 },
		{ 69, 69, 144 },
		{ 101, 101, 144 },
		{ 0, 0, -1 },
		{ 68, 68, 145 },
		{ 100, 100, 145 },
		{ 0, 0, -1 },
		{ 65, 65, 146 },
		{ 97, 97, 146 },
		{ 0, 0, -1 },
		{ 76, 76, 147 },
		{ 108, 108, 147 },
		{ 0, 0, -1 },
		{ 69, 69, 148 },
		{ 101, 101, 148 },
		{ 0, 0, -1 },
		{ 58, 58, 37 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 150 },
		{ 0, 0, -1 },
		{ 48, 57, 151 },
		{ 65, 70, 151 },
		{ 97, 102, 151 },
		{ 0, 0, -1 },
		{ 48, 57, 152 },
		{ 65, 70, 153 },
		{ 97, 102, 153 },
		{ 0, 0, -1 },
		{ 46, 46, 0 },
		{ 0, 0, -1 },
		{ 78, 78, 155 },
		{ 110, 110, 155 },
		{ 0, 0, -1 },
		{ 69, 69, 156 },
		{ 101, 101, 156 },
		{ 0, 0, -1 },
		{ 84, 84, 157 },
		{ 116, 116, 157 },
		{ 0, 0, -1 },
		{ 48, 57, 128 },
		{ 0, 0, -1 },
		{ 48, 57, 158 },
		{ 65, 70, 158 },
		{ 97, 102, 158 },
		{ 0, 0, -1 },
		{ 48, 57, 159 },
		{ 65, 70, 159 },
		{ 97, 102, 159 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 161 },
		{ 0, 0, -1 },
		{ 48, 57, 162 },
		{ 65, 70, 162 },
		{ 97, 102, 162 },
		{ 0, 0, -1 },
		{ 48, 57, 163 },
		{ 65, 70, 164 },
		{ 97, 102, 164 },
		{ 0, 0, -1 },
		{ 46, 46, 0 },
		{ 0, 0, -1 },
		{ 65, 65, 166 },
		{ 97, 97, 166 },
		{ 0, 0, -1 },
		{ 69, 69, 167 },
		{ 101, 101, 167 },
		{ 0, 0, -1 },
		{ 68, 68, 168 },
		{ 100, 100, 168 },
		{ 0, 0, -1 },
		{ 69, 69, 169 },
		{ 101, 101, 169 },
		{ 0, 0, -1 },
		{ 78, 78, 170 },
		{ 110, 110, 170 },
		{ 0, 0, -1 },
		{ 90, 90, 171 },
		{ 122, 122, 171 },
		{ 0, 0, -1 },
		{ 45, 45, 172 },
		{ 0, 0, -1 },
		{ 84, 84, 173 },
		{ 116, 116, 173 },
		{ 0, 0, -1 },
		{ 67, 67, 174 },
		{ 99, 99, 174 },
		{ 0, 0, -1 },
		{ 82, 82, 175 },
		{ 114, 114, 175 },
		{ 0, 0, -1 },
		{ 45, 45, 176 },
		{ 0, 0, -1 },
		{ 83, 83, 177 },
		{ 115, 115, 177 },
		{ 0, 0, -1 },
		{ 69, 69, 178 },
		{ 101, 101, 178 },
		{ 0, 0, -1 },
		{ 48, 57, 149 },
		{ 65, 70, 128 },
		{ 97, 102, 128 },
		{ 104, 104, 38 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 179 },
		{ 0, 0, -1 },
		{ 48, 57, 180 },
		{ 65, 70, 180 },
		{ 97, 102, 180 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 181 },
		{ 0, 0, -1 },
		{ 48, 57, 182 },
		{ 65, 70, 182 },
		{ 97, 102, 182 },
		{ 0, 0, -1 },
		{ 46, 46, 184 },
		{ 48, 57, 154 },
		{ 0, 0, -1 },
		{ 70, 70, 185 },
		{ 102, 102, 185 },
		{ 0, 0, -1 },
		{ 82, 82, 186 },
		{ 114, 114, 186 },
		{ 0, 0, -1 },
		{ 45, 45, 187 },
		{ 0, 0, -1 },
		{ 58, 58, 100 },
		{ 0, 0, -1 },
		{ 48, 57, 188 },
		{ 65, 70, 188 },
		{ 97, 102, 188 },
		{ 0, 0, -1 },
		{ 48, 57, 189 },
		{ 65, 70, 189 },
		{ 97, 102, 189 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 190 },
		{ 0, 0, -1 },
		{ 48, 57, 191 },
		{ 65, 70, 191 },
		{ 97, 102, 191 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 192 },
		{ 0, 0, -1 },
		{ 48, 57, 193 },
		{ 65, 70, 193 },
		{ 97, 102, 193 },
		{ 0, 0, -1 },
		{ 46, 46, 195 },
		{ 48, 57, 165 },
		{ 0, 0, -1 },
		{ 77, 77, 196 },
		{ 109, 109, 196 },
		{ 0, 0, -1 },
		{ 86, 86, 197 },
		{ 118, 118, 197 },
		{ 0, 0, -1 },
		{ 73, 73, 198 },
		{ 105, 105, 198 },
		{ 0, 0, -1 },
		{ 82, 82, 199 },
		{ 114, 114, 199 },
		{ 0, 0, -1 },
		{ 79, 79, 200 },
		{ 111, 111, 200 },
		{ 0, 0, -1 },
		{ 79, 79, 201 },
		{ 111, 111, 201 },
		{ 0, 0, -1 },
		{ 45, 45, 202 },
		{ 0, 0, -1 },
		{ 79, 79, 203 },
		{ 111, 111, 203 },
		{ 0, 0, -1 },
		{ 69, 69, 204 },
		{ 101, 101, 204 },
		{ 0, 0, -1 },
		{ 76, 76, 205 },
		{ 108, 108, 205 },
		{ 0, 0, -1 },
		{ 84, 84, 206 },
		{ 116, 116, 206 },
		{ 0, 0, -1 },
		{ 65, 65, 207 },
		{ 97, 97, 207 },
		{ 0, 0, -1 },
		{ 58, 58, 121 },
		{ 0, 0, -1 },
		{ 58, 58, 121 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 209 },
		{ 0, 0, -1 },
		{ 48, 57, 210 },
		{ 65, 70, 210 },
		{ 97, 102, 210 },
		{ 0, 0, -1 },
		{ 48, 57, 211 },
		{ 65, 70, 212 },
		{ 97, 102, 212 },
		{ 0, 0, -1 },
		{ 46, 46, 0 },
		{ 0, 0, -1 },
		{ 73, 73, 214 },
		{ 105, 105, 214 },
		{ 0, 0, -1 },
		{ 86, 86, 215 },
		{ 118, 118, 215 },
		{ 0, 0, -1 },
		{ 77, 77, 216 },
		{ 109, 109, 216 },
		{ 0, 0, -1 },
		{ 48, 57, 217 },
		{ 65, 70, 217 },
		{ 97, 102, 217 },
		{ 0, 0, -1 },
		{ 48, 57, 218 },
		{ 65, 70, 218 },
		{ 97, 102, 218 },
		{ 0, 0, -1 },
		{ 58, 58, 133 },
		{ 0, 0, -1 },
		{ 58, 58, 133 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 221 },
		{ 0, 0, -1 },
		{ 48, 57, 222 },
		{ 65, 70, 222 },
		{ 97, 102, 222 },
		{ 0, 0, -1 },
		{ 48, 57, 223 },
		{ 65, 70, 224 },
		{ 97, 102, 224 },
		{ 0, 0, -1 },
		{ 46, 46, 0 },
		{ 0, 0, -1 },
		{ 69, 69, 226 },
		{ 101, 101, 226 },
		{ 0, 0, -1 },
		{ 69, 69, 227 },
		{ 101, 101, 227 },
		{ 0, 0, -1 },
		{ 82, 82, 228 },
		{ 114, 114, 228 },
		{ 0, 0, -1 },
		{ 86, 86, 229 },
		{ 118, 118, 229 },
		{ 0, 0, -1 },
		{ 78, 78, 230 },
		{ 110, 110, 230 },
		{ 0, 0, -1 },
		{ 78, 78, 231 },
		{ 110, 110, 231 },
		{ 0, 0, -1 },
		{ 67, 67, 232 },
		{ 99, 99, 232 },
		{ 0, 0, -1 },
		{ 77, 77, 233 },
		{ 109, 109, 233 },
		{ 0, 0, -1 },
		{ 68, 68, 234 },
		{ 78, 78, 235 },
		{ 100, 100, 234 },
		{ 110, 110, 235 },
		{ 0, 0, -1 },
		{ 73, 73, 236 },
		{ 105, 105, 236 },
		{ 0, 0, -1 },
		{ 83, 83, 237 },
		{ 115, 115, 237 },
		{ 0, 0, -1 },
		{ 58, 58, 383 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 238 },
		{ 0, 0, -1 },
		{ 48, 57, 239 },
		{ 65, 70, 239 },
		{ 97, 102, 239 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 240 },
		{ 0, 0, -1 },
		{ 48, 57, 241 },
		{ 65, 70, 241 },
		{ 97, 102, 241 },
		{ 0, 0, -1 },
		{ 46, 46, 243 },
		{ 48, 57, 213 },
		{ 0, 0, -1 },
		{ 71, 71, 244 },
		{ 103, 103, 244 },
		{ 0, 0, -1 },
		{ 69, 69, 245 },
		{ 101, 101, 245 },
		{ 0, 0, -1 },
		{ 65, 65, 246 },
		{ 97, 97, 246 },
		{ 0, 0, -1 },
		{ 58, 58, 160 },
		{ 0, 0, -1 },
		{ 48, 57, 247 },
		{ 65, 70, 247 },
		{ 97, 102, 247 },
		{ 0, 0, -1 },
		{ 48, 57, 248 },
		{ 65, 70, 248 },
		{ 97, 102, 248 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 249 },
		{ 0, 0, -1 },
		{ 48, 57, 250 },
		{ 65, 70, 250 },
		{ 97, 102, 250 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 251 },
		{ 0, 0, -1 },
		{ 48, 57, 252 },
		{ 65, 70, 252 },
		{ 97, 102, 252 },
		{ 0, 0, -1 },
		{ 46, 46, 254 },
		{ 48, 57, 225 },
		{ 0, 0, -1 },
		{ 76, 76, 255 },
		{ 108, 108, 255 },
		{ 0, 0, -1 },
		{ 69, 69, 256 },
		{ 101, 101, 256 },
		{ 0, 0, -1 },
		{ 69, 69, 257 },
		{ 101, 101, 257 },
		{ 0, 0, -1 },
		{ 76, 76, 258 },
		{ 108, 108, 258 },
		{ 0, 0, -1 },
		{ 76, 76, 259 },
		{ 108, 108, 259 },
		{ 0, 0, -1 },
		{ 77, 77, 260 },
		{ 109, 109, 260 },
		{ 0, 0, -1 },
		{ 45, 45, 261 },
		{ 0, 0, -1 },
		{ 67, 67, 262 },
		{ 99, 99, 262 },
		{ 0, 0, -1 },
		{ 70, 70, 263 },
		{ 102, 102, 263 },
		{ 0, 0, -1 },
		{ 69, 69, 264 },
		{ 101, 101, 264 },
		{ 0, 0, -1 },
		{ 58, 58, 183 },
		{ 0, 0, -1 },
		{ 58, 58, 183 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 265 },
		{ 0, 0, -1 },
		{ 48, 57, 266 },
		{ 65, 70, 266 },
		{ 97, 102, 266 },
		{ 0, 0, -1 },
		{ 48, 57, 267 },
		{ 65, 70, 268 },
		{ 97, 102, 268 },
		{ 0, 0, -1 },
		{ 82, 82, 270 },
		{ 114, 114, 270 },
		{ 0, 0, -1 },
		{ 88, 88, 271 },
		{ 120, 120, 271 },
		{ 0, 0, -1 },
		{ 48, 57, 272 },
		{ 65, 70, 272 },
		{ 97, 102, 272 },
		{ 0, 0, -1 },
		{ 48, 57, 273 },
		{ 65, 70, 273 },
		{ 97, 102, 273 },
		{ 0, 0, -1 },
		{ 58, 58, 194 },
		{ 0, 0, -1 },
		{ 58, 58, 194 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 275 },
		{ 0, 0, -1 },
		{ 48, 57, 276 },
		{ 65, 70, 276 },
		{ 97, 102, 276 },
		{ 0, 0, -1 },
		{ 48, 57, 277 },
		{ 65, 70, 278 },
		{ 97, 102, 278 },
		{ 0, 0, -1 },
		{ 82, 82, 280 },
		{ 114, 114, 280 },
		{ 0, 0, -1 },
		{ 89, 89, 281 },
		{ 121, 121, 281 },
		{ 0, 0, -1 },
		{ 73, 73, 282 },
		{ 105, 105, 282 },
		{ 0, 0, -1 },
		{ 73, 73, 283 },
		{ 105, 105, 283 },
		{ 0, 0, -1 },
		{ 76, 76, 284 },
		{ 108, 108, 284 },
		{ 0, 0, -1 },
		{ 69, 69, 285 },
		{ 101, 101, 285 },
		{ 0, 0, -1 },
		{ 69, 69, 286 },
		{ 101, 101, 286 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 287 },
		{ 0, 0, -1 },
		{ 48, 57, 288 },
		{ 65, 70, 288 },
		{ 97, 102, 288 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 289 },
		{ 0, 0, -1 },
		{ 48, 57, 290 },
		{ 65, 70, 290 },
		{ 97, 102, 290 },
		{ 0, 0, -1 },
		{ 48, 57, 269 },
		{ 0, 0, -1 },
		{ 45, 45, 292 },
		{ 0, 0, -1 },
		{ 58, 58, 219 },
		{ 0, 0, -1 },
		{ 48, 57, 293 },
		{ 65, 70, 293 },
		{ 97, 102, 293 },
		{ 0, 0, -1 },
		{ 48, 57, 294 },
		{ 65, 70, 294 },
		{ 97, 102, 294 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 295 },
		{ 0, 0, -1 },
		{ 48, 57, 296 },
		{ 65, 70, 296 },
		{ 97, 102, 296 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 297 },
		{ 0, 0, -1 },
		{ 48, 57, 298 },
		{ 65, 70, 298 },
		{ 97, 102, 298 },
		{ 0, 0, -1 },
		{ 48, 57, 279 },
		{ 0, 0, -1 },
		{ 69, 69, 300 },
		{ 101, 101, 300 },
		{ 0, 0, -1 },
		{ 84, 84, 301 },
		{ 116, 116, 301 },
		{ 0, 0, -1 },
		{ 73, 73, 302 },
		{ 105, 105, 302 },
		{ 0, 0, -1 },
		{ 84, 84, 303 },
		{ 116, 116, 303 },
		{ 0, 0, -1 },
		{ 58, 58, 242 },
		{ 0, 0, -1 },
		{ 58, 58, 242 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 304 },
		{ 0, 0, -1 },
		{ 48, 57, 305 },
		{ 65, 70, 305 },
		{ 97, 102, 305 },
		{ 0, 0, -1 },
		{ 48, 57, 306 },
		{ 65, 70, 307 },
		{ 97, 102, 307 },
		{ 0, 0, -1 },
		{ 76, 76, 308 },
		{ 108, 108, 308 },
		{ 0, 0, -1 },
		{ 48, 57, 309 },
		{ 65, 70, 309 },
		{ 97, 102, 309 },
		{ 0, 0, -1 },
		{ 48, 57, 310 },
		{ 65, 70, 310 },
		{ 97, 102, 310 },
		{ 0, 0, -1 },
		{ 58, 58, 253 },
		{ 0, 0, -1 },
		{ 58, 58, 253 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 312 },
		{ 0, 0, -1 },
		{ 48, 57, 313 },
		{ 65, 70, 313 },
		{ 97, 102, 313 },
		{ 0, 0, -1 },
		{ 48, 57, 314 },
		{ 65, 70, 315 },
		{ 97, 102, 315 },
		{ 0, 0, -1 },
		{ 78, 78, 316 },
		{ 110, 110, 316 },
		{ 0, 0, -1 },
		{ 70, 70, 317 },
		{ 102, 102, 317 },
		{ 0, 0, -1 },
		{ 73, 73, 318 },
		{ 105, 105, 318 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 319 },
		{ 0, 0, -1 },
		{ 48, 57, 320 },
		{ 65, 70, 320 },
		{ 97, 102, 320 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 321 },
		{ 0, 0, -1 },
		{ 48, 57, 322 },
		{ 65, 70, 322 },
		{ 97, 102, 322 },
		{ 0, 0, -1 },
		{ 69, 69, 324 },
		{ 101, 101, 324 },
		{ 0, 0, -1 },
		{ 58, 58, 274 },
		{ 0, 0, -1 },
		{ 48, 57, 325 },
		{ 65, 70, 325 },
		{ 97, 102, 325 },
		{ 0, 0, -1 },
		{ 48, 57, 326 },
		{ 58, 58, 72 },
		{ 65, 70, 326 },
		{ 97, 102, 326 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 327 },
		{ 0, 0, -1 },
		{ 48, 57, 328 },
		{ 65, 70, 328 },
		{ 97, 102, 328 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 329 },
		{ 0, 0, -1 },
		{ 48, 57, 330 },
		{ 65, 70, 330 },
		{ 97, 102, 330 },
		{ 0, 0, -1 },
		{ 84, 84, 332 },
		{ 116, 116, 332 },
		{ 0, 0, -1 },
		{ 69, 69, 333 },
		{ 101, 101, 333 },
		{ 0, 0, -1 },
		{ 77, 77, 334 },
		{ 109, 109, 334 },
		{ 0, 0, -1 },
		{ 58, 58, 291 },
		{ 0, 0, -1 },
		{ 58, 58, 291 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 335 },
		{ 0, 0, -1 },
		{ 48, 57, 336 },
		{ 65, 70, 336 },
		{ 97, 102, 336 },
		{ 0, 0, -1 },
		{ 48, 57, 337 },
		{ 65, 70, 338 },
		{ 97, 102, 338 },
		{ 0, 0, -1 },
		{ 65, 65, 339 },
		{ 97, 97, 339 },
		{ 0, 0, -1 },
		{ 48, 57, 340 },
		{ 65, 70, 340 },
		{ 97, 102, 340 },
		{ 0, 0, -1 },
		{ 48, 57, 341 },
		{ 65, 70, 341 },
		{ 97, 102, 341 },
		{ 0, 0, -1 },
		{ 58, 58, 299 },
		{ 0, 0, -1 },
		{ 58, 58, 299 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 343 },
		{ 0, 0, -1 },
		{ 48, 57, 344 },
		{ 65, 70, 344 },
		{ 97, 102, 344 },
		{ 0, 0, -1 },
		{ 48, 57, 345 },
		{ 65, 70, 346 },
		{ 97, 102, 346 },
		{ 0, 0, -1 },
		{ 83, 83, 347 },
		{ 115, 115, 347 },
		{ 0, 0, -1 },
		{ 84, 84, 348 },
		{ 116, 116, 348 },
		{ 0, 0, -1 },
		{ 69, 69, 349 },
		{ 101, 101, 349 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 350 },
		{ 0, 0, -1 },
		{ 48, 57, 351 },
		{ 65, 70, 351 },
		{ 97, 102, 351 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 352 },
		{ 0, 0, -1 },
		{ 48, 57, 353 },
		{ 65, 70, 353 },
		{ 97, 102, 353 },
		{ 0, 0, -1 },
		{ 83, 83, 355 },
		{ 115, 115, 355 },
		{ 0, 0, -1 },
		{ 58, 58, 311 },
		{ 0, 0, -1 },
		{ 48, 57, 356 },
		{ 65, 70, 356 },
		{ 97, 102, 356 },
		{ 0, 0, -1 },
		{ 48, 57, 357 },
		{ 58, 58, 358 },
		{ 65, 70, 357 },
		{ 97, 102, 357 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 359 },
		{ 0, 0, -1 },
		{ 48, 57, 360 },
		{ 65, 70, 360 },
		{ 97, 102, 360 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 361 },
		{ 0, 0, -1 },
		{ 48, 57, 362 },
		{ 65, 70, 362 },
		{ 97, 102, 362 },
		{ 0, 0, -1 },
		{ 73, 73, 364 },
		{ 105, 105, 364 },
		{ 0, 0, -1 },
		{ 58, 58, 323 },
		{ 0, 0, -1 },
		{ 58, 58, 323 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 365 },
		{ 0, 0, -1 },
		{ 48, 57, 366 },
		{ 65, 70, 366 },
		{ 97, 102, 366 },
		{ 0, 0, -1 },
		{ 48, 57, 367 },
		{ 65, 70, 368 },
		{ 97, 102, 368 },
		{ 0, 0, -1 },
		{ 69, 69, 369 },
		{ 101, 101, 369 },
		{ 0, 0, -1 },
		{ 48, 57, 370 },
		{ 65, 70, 370 },
		{ 97, 102, 370 },
		{ 0, 0, -1 },
		{ 48, 57, 371 },
		{ 65, 70, 371 },
		{ 97, 102, 371 },
		{ 0, 0, -1 },
		{ 48, 57, 373 },
		{ 65, 70, 373 },
		{ 97, 102, 373 },
		{ 0, 0, -1 },
		{ 58, 58, 331 },
		{ 0, 0, -1 },
		{ 58, 58, 331 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 374 },
		{ 0, 0, -1 },
		{ 48, 57, 375 },
		{ 65, 70, 375 },
		{ 97, 102, 375 },
		{ 0, 0, -1 },
		{ 48, 57, 376 },
		{ 0, 0, -1 },
		{ 77, 77, 378 },
		{ 109, 109, 378 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 379 },
		{ 0, 0, -1 },
		{ 48, 57, 380 },
		{ 65, 70, 380 },
		{ 97, 102, 380 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 381 },
		{ 0, 0, -1 },
		{ 48, 57, 382 },
		{ 65, 70, 382 },
		{ 97, 102, 382 },
		{ 0, 0, -1 },
		{ 58, 58, 342 },
		{ 0, 0, -1 },
		{ 48, 57, 384 },
		{ 65, 70, 384 },
		{ 97, 102, 384 },
		{ 0, 0, -1 },
		{ 48, 57, 386 },
		{ 65, 70, 386 },
		{ 97, 102, 386 },
		{ 0, 0, -1 },
		{ 48, 57, 387 },
		{ 65, 70, 387 },
		{ 97, 102, 387 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 389 },
		{ 0, 0, -1 },
		{ 48, 57, 390 },
		{ 65, 70, 390 },
		{ 97, 102, 390 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 391 },
		{ 0, 0, -1 },
		{ 48, 57, 392 },
		{ 65, 70, 392 },
		{ 97, 102, 392 },
		{ 0, 0, -1 },
		{ 69, 69, 393 },
		{ 101, 101, 393 },
		{ 0, 0, -1 },
		{ 58, 58, 354 },
		{ 0, 0, -1 },
		{ 58, 58, 354 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 394 },
		{ 0, 0, -1 },
		{ 48, 57, 395 },
		{ 65, 70, 395 },
		{ 97, 102, 395 },
		{ 0, 0, -1 },
		{ 48, 57, 396 },
		{ 65, 70, 396 },
		{ 97, 102, 396 },
		{ 0, 0, -1 },
		{ 48, 57, 397 },
		{ 65, 70, 397 },
		{ 97, 102, 397 },
		{ 0, 0, -1 },
		{ 48, 57, 398 },
		{ 65, 70, 398 },
		{ 97, 102, 398 },
		{ 0, 0, -1 },
		{ 48, 57, 400 },
		{ 65, 70, 400 },
		{ 97, 102, 400 },
		{ 0, 0, -1 },
		{ 48, 57, 401 },
		{ 65, 70, 401 },
		{ 97, 102, 401 },
		{ 0, 0, -1 },
		{ 58, 58, 363 },
		{ 0, 0, -1 },
		{ 58, 58, 363 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 402 },
		{ 0, 0, -1 },
		{ 48, 57, 403 },
		{ 65, 70, 403 },
		{ 97, 102, 403 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 404 },
		{ 0, 0, -1 },
		{ 48, 57, 405 },
		{ 65, 70, 405 },
		{ 97, 102, 405 },
		{ 0, 0, -1 },
		{ 48, 57, 406 },
		{ 65, 70, 406 },
		{ 97, 102, 406 },
		{ 0, 0, -1 },
		{ 58, 58, 372 },
		{ 0, 0, -1 },
		{ 48, 57, 407 },
		{ 65, 70, 407 },
		{ 97, 102, 407 },
		{ 0, 0, -1 },
		{ 58, 58, 385 },
		{ 0, 0, -1 },
		{ 48, 57, 408 },
		{ 65, 70, 408 },
		{ 97, 102, 408 },
		{ 0, 0, -1 },
		{ 48, 57, 409 },
		{ 65, 70, 409 },
		{ 97, 102, 409 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 411 },
		{ 0, 0, -1 },
		{ 48, 57, 412 },
		{ 65, 70, 412 },
		{ 97, 102, 412 },
		{ 0, 0, -1 },
		{ 46, 46, 122 },
		{ 48, 57, 208 },
		{ 0, 0, -1 },
		{ 58, 58, 383 },
		{ 0, 0, -1 },
		{ 48, 57, 413 },
		{ 65, 70, 413 },
		{ 97, 102, 413 },
		{ 0, 0, -1 },
		{ 48, 57, 414 },
		{ 65, 70, 414 },
		{ 97, 102, 414 },
		{ 0, 0, -1 },
		{ 58, 58, 388 },
		{ 0, 0, -1 },
		{ 48, 57, 415 },
		{ 65, 70, 415 },
		{ 97, 102, 415 },
		{ 0, 0, -1 },
		{ 48, 57, 416 },
		{ 65, 70, 416 },
		{ 97, 102, 416 },
		{ 0, 0, -1 },
		{ 46, 46, 134 },
		{ 48, 57, 220 },
		{ 0, 0, -1 },
		{ 48, 57, 417 },
		{ 65, 70, 417 },
		{ 97, 102, 417 },
		{ 0, 0, -1 },
		{ 58, 58, 399 },
		{ 0, 0, -1 },
		{ 48, 57, 418 },
		{ 65, 70, 418 },
		{ 97, 102, 418 },
		{ 0, 0, -1 },
		{ 48, 57, 419 },
		{ 65, 70, 419 },
		{ 97, 102, 419 },
		{ 0, 0, -1 },
		{ 58, 58, 410 },
		{ 0, 0, -1 },
		{ 48, 57, 421 },
		{ 65, 70, 421 },
		{ 97, 102, 421 },
		{ 0, 0, -1 },
		{ 48, 57, 422 },
		{ 65, 70, 422 },
		{ 97, 102, 422 },
		{ 0, 0, -1 },
		{ 48, 57, 423 },
		{ 65, 70, 423 },
		{ 97, 102, 423 },
		{ 0, 0, -1 },
		{ 48, 57, 424 },
		{ 65, 70, 424 },
		{ 97, 102, 424 },
		{ 0, 0, -1 },
		{ 58, 58, 420 },
		{ 0, 0, -1 },
		{ 48, 57, 426 },
		{ 65, 70, 426 },
		{ 97, 102, 426 },
		{ 0, 0, -1 },
		{ 48, 57, 427 },
		{ 65, 70, 427 },
		{ 97, 102, 427 },
		{ 0, 0, -1 },
		{ 48, 57, 428 },
		{ 65, 70, 428 },
		{ 97, 102, 428 },
		{ 0, 0, -1 },
		{ 48, 57, 429 },
		{ 65, 70, 429 },
		{ 97, 102, 429 },
		{ 0, 0, -1 },
		{ 58, 58, 425 },
		{ 0, 0, -1 },
		{ 48, 57, 431 },
		{ 65, 70, 431 },
		{ 97, 102, 431 },
		{ 0, 0, -1 },
		{ 48, 57, 432 },
		{ 65, 70, 432 },
		{ 97, 102, 432 },
		{ 0, 0, -1 },
		{ 48, 57, 433 },
		{ 65, 70, 433 },
		{ 97, 102, 433 },
		{ 0, 0, -1 },
		{ 48, 57, 434 },
		{ 65, 70, 434 },
		{ 97, 102, 434 },
		{ 0, 0, -1 },
		{ 58, 58, 430 },
		{ 0, 0, -1 },
		{ 48, 57, 436 },
		{ 65, 70, 436 },
		{ 97, 102, 436 },
		{ 0, 0, -1 },
		{ 48, 57, 377 },
		{ 65, 70, 377 },
		{ 97, 102, 377 },
		{ 0, 0, -1 },
		{ 48, 57, 437 },
		{ 65, 70, 437 },
		{ 97, 102, 437 },
		{ 0, 0, -1 },
		{ 58, 58, 435 },
		{ 0, 0, -1 },
		{ 42, 42, 438 },
		{ 0, 0, -1 },
		{ 47, 47, 440 },
		{ 0, 0, -1 }
	};
	yyctransition = ctransition;

	static const yymatch_t YYNEARFAR YYBASED_CODE match[] = {
		0,
		27,
		45,
		0,
		43,
		45,
		0,
		44,
		45,
		0
	};
	yymatch = match;

	static const yystate_t YYNEARFAR YYBASED_CODE state[] = {
		{ 0, 0, 0 },
		{ -31, 1, 0 },
		{ 1, 0, 0 },
		{ -439, 1295, 32 },
		{ 3, 0, 32 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 302, 45, 45 },
		{ 333, 48, 45 },
		{ 25, 53, 45 },
		{ 174, 56, 45 },
		{ 174, 59, 45 },
		{ 25, 62, 45 },
		{ 348, 67, 45 },
		{ 25, 74, 45 },
		{ 378, 77, 45 },
		{ 256, 82, 45 },
		{ 324, 87, 45 },
		{ 300, 90, 45 },
		{ 324, 93, 45 },
		{ 378, 96, 45 },
		{ 25, 99, 45 },
		{ 57, 0, -1 },
		{ 438, 102, 45 },
		{ 25, 105, -4 },
		{ 118, 107, 45 },
		{ 437, 111, 45 },
		{ 62, 0, 45 },
		{ 64, 113, 45 },
		{ 0, 0, -7 },
		{ 0, 0, 44 },
		{ 0, 0, 45 },
		{ 324, 115, 0 },
		{ 271, 118, 24 },
		{ 49, 120, 0 },
		{ 348, 123, 0 },
		{ 56, 128, 0 },
		{ 311, 131, 0 },
		{ 0, 0, 42 },
		{ 214, 135, 0 },
		{ 256, 138, 0 },
		{ 339, 141, 0 },
		{ 364, 144, 0 },
		{ 364, 147, 0 },
		{ 0, 0, 13 },
		{ 0, 0, 14 },
		{ 0, 150, 0 },
		{ 36, 153, 0 },
		{ 0, 156, 0 },
		{ 0, 159, 0 },
		{ 378, 162, 0 },
		{ 174, 165, 0 },
		{ 292, 168, 0 },
		{ 348, 171, 0 },
		{ 246, 174, 0 },
		{ 339, 177, 0 },
		{ 60, 180, 0 },
		{ -57, 184, 27 },
		{ -58, 186, 28 },
		{ 0, 0, 29 },
		{ 118, 188, 43 },
		{ 435, 192, 37 },
		{ -62, 196, 0 },
		{ 0, 198, 40 },
		{ 435, 200, 0 },
		{ 235, 205, 0 },
		{ 235, 208, 0 },
		{ 271, 211, 0 },
		{ 339, 213, 0 },
		{ 378, 216, 0 },
		{ 79, 219, 0 },
		{ 158, 222, 0 },
		{ 435, 226, 36 },
		{ 271, 230, 0 },
		{ 0, 232, 0 },
		{ 271, 235, 0 },
		{ 324, 237, 0 },
		{ 378, 240, 0 },
		{ 378, 243, 0 },
		{ 89, 246, 0 },
		{ 378, 250, 0 },
		{ 348, 253, 0 },
		{ 302, 256, 0 },
		{ 292, 259, 0 },
		{ 348, 262, 0 },
		{ 235, 265, 0 },
		{ 271, 268, 0 },
		{ 0, 0, 23 },
		{ 339, 270, 0 },
		{ 118, 273, 43 },
		{ 91, 277, 35 },
		{ 180, 280, 35 },
		{ 0, 0, 41 },
		{ 378, 284, 0 },
		{ 174, 287, 0 },
		{ 339, 290, 0 },
		{ 339, 293, 0 },
		{ 300, 296, 0 },
		{ 128, 299, 0 },
		{ 158, 301, 0 },
		{ 311, 305, 0 },
		{ 102, 309, 34 },
		{ 191, 312, 34 },
		{ 300, 316, 0 },
		{ 271, 321, 0 },
		{ 339, 323, 0 },
		{ 348, 326, 0 },
		{ 271, 329, 0 },
		{ 0, 0, 25 },
		{ 333, 331, 0 },
		{ 235, 334, 0 },
		{ 137, 337, 0 },
		{ 378, 340, 0 },
		{ 0, 0, 19 },
		{ 137, 343, 0 },
		{ 324, 346, 0 },
		{ 292, 349, 0 },
		{ 378, 352, 0 },
		{ 149, 355, 43 },
		{ 120, 357, 35 },
		{ 180, 360, 35 },
		{ 435, 364, 0 },
		{ 154, 368, 0 },
		{ 0, 0, 1 },
		{ 300, 370, 0 },
		{ 378, 373, 0 },
		{ 0, 0, 3 },
		{ 333, 376, 0 },
		{ 149, 379, 0 },
		{ 158, 381, 0 },
		{ 217, 385, 0 },
		{ 132, 389, 34 },
		{ 191, 392, 34 },
		{ 435, 396, 0 },
		{ 165, 400, 0 },
		{ 324, 402, 0 },
		{ 378, 405, 0 },
		{ 0, 408, 0 },
		{ 378, 411, 0 },
		{ 300, 414, 0 },
		{ 0, 417, 0 },
		{ 271, 420, 0 },
		{ 333, 422, 0 },
		{ 235, 425, 0 },
		{ 256, 428, 0 },
		{ 271, 431, 0 },
		{ 339, 433, 0 },
		{ 378, 436, 0 },
		{ 0, 0, 26 },
		{ 435, 439, 43 },
		{ 151, 444, 35 },
		{ 180, 447, 35 },
		{ 153, 451, 35 },
		{ 239, 454, 35 },
		{ 411, 458, 0 },
		{ 302, 461, 0 },
		{ 256, 464, 0 },
		{ 271, 467, 0 },
		{ 437, 469, 0 },
		{ 217, 471, 0 },
		{ 311, 475, 0 },
		{ 162, 479, 34 },
		{ 191, 482, 34 },
		{ 164, 486, 34 },
		{ 250, 489, 34 },
		{ 411, 493, 0 },
		{ 364, 496, 0 },
		{ 199, 499, 0 },
		{ 348, 502, 0 },
		{ 256, 505, 0 },
		{ 0, 0, 8 },
		{ 174, 508, 0 },
		{ 174, 511, 0 },
		{ 271, 514, 0 },
		{ 0, 516, 0 },
		{ 378, 519, 0 },
		{ 292, 522, 0 },
		{ 333, 525, 0 },
		{ 324, 528, 0 },
		{ 404, 531, 35 },
		{ 437, 533, 35 },
		{ 182, 535, 35 },
		{ 239, 538, 35 },
		{ 435, 542, 0 },
		{ 213, 546, 0 },
		{ 348, 548, 0 },
		{ 199, 551, 0 },
		{ 364, 554, 0 },
		{ 217, 557, 0 },
		{ 272, 561, 0 },
		{ 411, 565, 34 },
		{ 437, 567, 34 },
		{ 193, 569, 34 },
		{ 250, 572, 34 },
		{ 435, 576, 0 },
		{ 225, 580, 0 },
		{ 378, 582, 0 },
		{ 378, 585, 0 },
		{ 256, 588, 0 },
		{ 0, 591, 0 },
		{ 300, 594, 0 },
		{ 300, 597, 0 },
		{ 235, 600, 0 },
		{ 364, 603, 0 },
		{ 300, 606, 0 },
		{ 348, 611, 0 },
		{ 0, 0, 17 },
		{ 339, 614, 0 },
		{ 404, 617, 0 },
		{ 210, 619, 35 },
		{ 239, 622, 35 },
		{ 212, 626, 35 },
		{ 288, 629, 35 },
		{ 411, 633, 0 },
		{ 0, 636, 0 },
		{ 378, 639, 0 },
		{ 324, 642, 0 },
		{ 437, 645, 0 },
		{ 272, 647, 0 },
		{ 311, 651, 0 },
		{ 411, 0, 0 },
		{ 222, 655, 34 },
		{ 250, 658, 34 },
		{ 224, 662, 34 },
		{ 296, 665, 34 },
		{ 411, 669, 0 },
		{ 0, 0, 4 },
		{ 292, 672, 0 },
		{ 0, 0, 6 },
		{ 378, 675, 0 },
		{ 378, 678, 0 },
		{ 292, 681, 0 },
		{ 292, 684, 0 },
		{ 364, 687, 0 },
		{ 271, 690, 0 },
		{ 0, 692, 0 },
		{ 302, 695, 0 },
		{ 378, 698, 0 },
		{ 404, 701, 35 },
		{ 437, 703, 35 },
		{ 241, 705, 35 },
		{ 288, 708, 35 },
		{ 435, 712, 0 },
		{ 269, 0, 0 },
		{ 0, 0, 2 },
		{ 256, 716, 0 },
		{ 0, 719, 0 },
		{ 272, 722, 0 },
		{ 309, 726, 0 },
		{ 411, 730, 34 },
		{ 437, 732, 34 },
		{ 252, 734, 34 },
		{ 296, 737, 34 },
		{ 435, 741, 0 },
		{ 279, 0, 0 },
		{ 0, 0, 5 },
		{ 0, 745, 0 },
		{ 0, 0, 10 },
		{ 0, 748, 0 },
		{ 348, 751, 0 },
		{ 348, 754, 0 },
		{ 292, 757, 0 },
		{ 378, 760, 0 },
		{ 378, 763, 0 },
		{ 0, 0, 21 },
		{ 266, 766, 35 },
		{ 288, 769, 35 },
		{ 268, 773, 35 },
		{ 320, 776, 35 },
		{ 279, 780, 39 },
		{ 0, 0, 9 },
		{ 0, 782, 0 },
		{ 437, 784, 0 },
		{ 309, 786, 0 },
		{ 311, 790, 0 },
		{ 276, 794, 34 },
		{ 296, 797, 34 },
		{ 278, 801, 34 },
		{ 328, 804, 34 },
		{ 195, 808, 38 },
		{ 0, 0, 7 },
		{ 0, 0, 11 },
		{ 378, 810, 0 },
		{ 333, 813, 0 },
		{ 348, 816, 0 },
		{ 0, 0, 18 },
		{ 333, 819, 0 },
		{ 404, 822, 35 },
		{ 437, 824, 35 },
		{ 290, 826, 35 },
		{ 320, 829, 35 },
		{ 435, 833, 0 },
		{ 0, 837, 0 },
		{ 309, 840, 0 },
		{ 340, 844, 0 },
		{ 411, 848, 34 },
		{ 437, 850, 34 },
		{ 298, 852, 34 },
		{ 328, 855, 34 },
		{ 435, 859, 0 },
		{ 0, 863, 0 },
		{ 0, 0, 20 },
		{ 0, 866, 0 },
		{ 348, 869, 0 },
		{ 305, 872, 35 },
		{ 320, 875, 35 },
		{ 307, 879, 35 },
		{ 351, 882, 35 },
		{ 378, 886, 0 },
		{ 437, 889, 0 },
		{ 340, 891, 0 },
		{ 437, 895, 0 },
		{ 313, 900, 34 },
		{ 328, 903, 34 },
		{ 315, 907, 34 },
		{ 360, 910, 34 },
		{ 333, 914, 0 },
		{ 378, 917, 0 },
		{ 364, 920, 0 },
		{ 404, 923, 35 },
		{ 437, 925, 35 },
		{ 322, 927, 35 },
		{ 351, 930, 35 },
		{ 435, 934, 0 },
		{ 0, 938, 0 },
		{ 340, 941, 0 },
		{ 370, 945, 0 },
		{ 411, 949, 34 },
		{ 437, 951, 34 },
		{ 330, 953, 34 },
		{ 360, 956, 34 },
		{ 435, 960, 0 },
		{ 339, 964, 0 },
		{ 0, 967, 0 },
		{ 378, 970, 0 },
		{ 336, 973, 35 },
		{ 351, 976, 35 },
		{ 338, 980, 35 },
		{ 380, 983, 35 },
		{ 0, 987, 0 },
		{ 437, 990, 0 },
		{ 370, 992, 0 },
		{ 437, 996, 0 },
		{ 344, 1001, 34 },
		{ 360, 1004, 34 },
		{ 346, 1008, 34 },
		{ 390, 1011, 34 },
		{ 0, 0, 12 },
		{ 0, 1015, 0 },
		{ 0, 0, 16 },
		{ 404, 1018, 35 },
		{ 437, 1020, 35 },
		{ 353, 1022, 35 },
		{ 380, 1025, 35 },
		{ 435, 1029, 0 },
		{ 378, 1033, 0 },
		{ 370, 1036, 0 },
		{ 397, 1040, 33 },
		{ 435, 1044, 36 },
		{ 411, 1048, 34 },
		{ 437, 1050, 34 },
		{ 362, 1052, 34 },
		{ 390, 1055, 34 },
		{ 435, 1059, 0 },
		{ 0, 1061, 0 },
		{ 366, 1064, 35 },
		{ 380, 1067, 35 },
		{ 368, 1071, 35 },
		{ 405, 1074, 35 },
		{ 0, 0, 22 },
		{ 437, 1078, 0 },
		{ 397, 1080, 0 },
		{ 399, 1084, 0 },
		{ 408, 1088, 34 },
		{ 375, 1092, 34 },
		{ 390, 1095, 34 },
		{ 377, 1099, 34 },
		{ 435, 1102, 34 },
		{ 0, 1106, 0 },
		{ 404, 1109, 35 },
		{ 437, 1111, 35 },
		{ 382, 1113, 35 },
		{ 405, 1116, 35 },
		{ 435, 1120, 0 },
		{ 397, 1124, 0 },
		{ 0, 0, 36 },
		{ 414, 1128, 0 },
		{ 408, 1132, 34 },
		{ 435, 1136, 0 },
		{ 411, 1140, 34 },
		{ 437, 1142, 34 },
		{ 392, 1144, 34 },
		{ 435, 1147, 34 },
		{ 0, 0, 15 },
		{ 395, 1151, 35 },
		{ 405, 1154, 35 },
		{ 435, 1158, 35 },
		{ 437, 1162, 0 },
		{ 414, 1164, 0 },
		{ 437, 1168, 0 },
		{ 408, 1170, 34 },
		{ 418, 1174, 34 },
		{ 403, 1178, 34 },
		{ 435, 1181, 34 },
		{ 405, 1185, 35 },
		{ 437, 1188, 35 },
		{ 435, 1190, 35 },
		{ 414, 1194, 0 },
		{ 437, 1198, 34 },
		{ 418, 1200, 34 },
		{ 435, 1204, 0 },
		{ 208, 1208, 34 },
		{ 0, 0, 34 },
		{ 435, 1211, 35 },
		{ 437, 1215, 0 },
		{ 418, 1217, 34 },
		{ 423, 1221, 34 },
		{ 0, 0, 35 },
		{ 437, 1225, 34 },
		{ 423, 1227, 34 },
		{ 435, 1231, 0 },
		{ 423, 1235, 34 },
		{ 428, 1239, 34 },
		{ 437, 1243, 34 },
		{ 428, 1245, 34 },
		{ 435, 1249, 0 },
		{ 428, 1253, 34 },
		{ 433, 1257, 34 },
		{ 437, 1261, 34 },
		{ 433, 1263, 34 },
		{ 435, 1267, 0 },
		{ 433, 1271, 34 },
		{ 437, 1275, 34 },
		{ 437, 1279, 34 },
		{ 437, 1281, 34 },
		{ 378, 1285, 0 },
		{ 437, 1289, 34 },
		{ 0, 1293, 34 },
		{ 0, 1297, 31 },
		{ 0, 0, 31 },
		{ 0, 0, 30 }
	};
	yystate = state;

	static const yybackup_t YYNEARFAR YYBASED_CODE backup[] = {
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};
	yybackup = backup;
}
