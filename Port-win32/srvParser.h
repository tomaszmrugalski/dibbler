#ifndef _SRVPARSER_H
#define _SRVPARSER_H

#include <cyacc.h>
#include <yacc.h>
#include "SrvParsGlobalOpt.h"
#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "DHCPConst.h"

/////////////////////////////////////////////////////////////////////////////
// SrvParser

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC SrvParser : public yyfparser {
public:
	SrvParser();

protected:
	void yytables();
	virtual void yyaction(int action);
#ifdef YYDEBUG
	void YYFAR* yyattribute1(int index) const;
	void yyinitdebug(void YYFAR** p, int count) const;
#endif

public:
#line 20 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

  public:
    //List of options in scope stack,the most fresh is last in the list
    List(TSrvParsGlobalOpt) ParserOptStack;
    //List of parsed interfaces/IAs/Addresses, last 
    //interface/IA/address is just being parsing or have been just parsed
    //FIXME:Don't forget to clear this lists in apropriate moment
    List(TSrvCfgIface)          SrvCfgIfaceLst;
    List(TSrvCfgAddrClass)  SrvCfgAddrClassLst;
    
    //Pointer to list which should contain either DNS servers or NTPServers 
    List(TIPv6Addr) PresentLst;
    
    //Pointer to list which should contain: rejected clients, accepted clients 
    //or addressess ranges 
    List(TStationRange) PresentRangeLst;
        
    //method check whether interface with id=ifaceNr has been 
    //already declared
    void CheckIsIface(int ifaceNr); 

    //method check whether interface with id=ifaceName has been
    //already declared 
    void CheckIsIface(string ifaceName);
    void StartIfaceDeclaration();
    void EndIfaceDeclaration();
    void StartClassDeclaration();
    void EndClassDeclaration();

#line 57 "C:\\Dyplom\\sources\\SrvParser\\srvParser.h"
};

#ifndef YYPARSENAME
#define YYPARSENAME SrvParser
#endif

#ifndef YYSTYPE
union tagYYSTYPE {
#line 63 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

  unsigned int          ival;
  char                          *strval;
  struct                        SDuid
  {
    int                       length;
    char*                       duid;
    }                                   duidval;
  char                          addrval[16];

#line 77 "C:\\Dyplom\\sources\\SrvParser\\srvParser.h"
};

#define YYSTYPE union tagYYSTYPE
#endif

#line 74 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

#ifndef YYSTYPE
#define YYSTYPE int
#endif

#line 89 "C:\\Dyplom\\sources\\SrvParser\\srvParser.h"
#define IFACE_ 257
#define NO_CONFIG_ 258
#define CLASS_ 259
#define LOGNAME_ 260
#define LOGLEVEL_ 261
#define WORKDIR_ 262
#define NTP_SERVER_ 263
#define TIME_ZONE_ 264
#define DNS_SERVER_ 265
#define DOMAIN_ 266
#define ACCEPT_ONLY_ 267
#define REJECT_CLIENTS_ 268
#define POOL_ 269
#define T1_ 270
#define T2_ 271
#define PREF_TIME_ 272
#define VALID_TIME_ 273
#define UNICAST_ 274
#define PREFERENCE_ 275
#define RAPID_COMMIT_ 276
#define MAX_LEASE_ 277
#define CLNT_MAX_LEASE_ 278
#define STRING_ 279
#define HEXNUMBER_ 280
#define INTNUMBER_ 281
#define IPV6ADDR_ 282
#define DUID_ 283
#endif
