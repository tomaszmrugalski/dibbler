#ifndef _CLNTPARSER_H
#define _CLNTPARSER_H

#include <cyacc.h>
#include "Container.h"
#include "SmartPtr.h"
#include "ClntParsGlobalOpt.h"
#include "ClntCfgIface.h"
#include <yacc.h>

/////////////////////////////////////////////////////////////////////////////
// clntParser

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC clntParser : public yyfparser {
public:
	clntParser();

protected:
	void yytables();
	virtual void yyaction(int action);
#ifdef YYDEBUG
	void YYFAR* yyattribute1(int index) const;
	void yyinitdebug(void YYFAR** p, int count) const;
#endif

public:
#line 22 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"


public:
    //List of options in scope stack,the most fresh is last in the list
    TContainer<SmartPtr<TClntParsGlobalOpt> > ParserOptStack;
    
    //List of parsed interfaces/IAs/Addresses, last 
    //interface/IA/address is just being parsing or have been just parsed
    //FIXME:Don't forget to clear this lists in apropriate moment
    TContainer<SmartPtr<TClntCfgIface> > ClntCfgIfaceLst;
    TContainer<SmartPtr<TClntCfgGroup> > ClntCfgGroupLst;
    TContainer<SmartPtr<TClntCfgIA> >    ClntCfgIALst;
    TContainer<SmartPtr<TClntCfgAddr> >  ClntCfgAddrLst;
    
    //Pointer to list which should contain either rejected servers or 
    //preffered servers
    TContainer<SmartPtr<TStationID> > PresentStationLst;
    
    TContainer<SmartPtr<TIPv6Addr> > PresentAddrLst;
    //method check whether interface with id=ifaceNr has been 
    //already declared
    void CheckIsIface(int ifaceNr); 

    //method check whether interface with id=ifaceName has been
    //already declared 
    void CheckIsIface(string ifaceName);
    void StartIfaceDeclaration();
    void EndIfaceDeclaration();
    void EmptyIface();
    void StartIADeclaration(bool aggregation);
    void EndIADeclaration(long iaCnt);
    void EmptyIA();
    void EmptyAddr();

#line 62 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.h"
};

#ifndef YYPARSENAME
#define YYPARSENAME clntParser
#endif

#ifndef YYSTYPE
union tagYYSTYPE {
#line 70 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    int          ival;    
    char         *strval;  
    struct SDuid  {      
        int         length;    
        char*     duid;  
    } duidval;  
    char    addrval[16];  
    ESendOpt  SendOpt;  
    EReqOpt   ReqOpt; 

#line 83 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.h"
};

#define YYSTYPE union tagYYSTYPE
#endif

#line 82 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

#ifndef YYSTYPE
#define YYSTYPE int
#endif

#line 95 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.h"
#define T1_ 257
#define T2_ 258
#define PREF_TIME_ 259
#define DNS_SERVER_ 260
#define VALID_TIME_ 261
#define NTP_SERVER_ 262
#define DOMAIN_ 263
#define TIME_ZONE_ 264
#define IFACE_ 265
#define NO_CONFIG_ 266
#define REJECT_SERVERS_ 267
#define PREFERRED_SERVERS_ 268
#define REQUIRE_ 269
#define REQUEST_ 270
#define SEND_ 271
#define DEFAULT_ 272
#define SUPERSEDE_ 273
#define APPEND_ 274
#define PREPEND_ 275
#define IA_ 276
#define ADDRES_ 277
#define IPV6ADDR_ 278
#define LOGLEVEL_ 279
#define WORKDIR_ 280
#define RAPID_COMMIT_ 281
#define NOIA_ 282
#define STRING_ 283
#define HEXNUMBER_ 284
#define INTNUMBER_ 285
#define DUID_ 286
#endif
