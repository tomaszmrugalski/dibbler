#include <cyacc.h>

#line 1 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

/****************************************************************************
parser.y
ParserWizard generated YACC file.

Date: 30 czerwca 2003
****************************************************************************/

//TParsServAddr

#line 15 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
// repeated because of possible precompiled header
#include <cyacc.h>

#include "srvParser.h"

/////////////////////////////////////////////////////////////////////////////
// constructor

YYPARSENAME::YYPARSENAME()
{
	yytables();
#line 51 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

    // place any extra initialisation code here
    ParserOptStack.append(new TSrvParsGlobalOpt());
    ParserOptStack.getLast()->setUnicast(false);

#line 33 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
}

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#ifndef YYSTACK_SIZE
#define YYSTACK_SIZE 100
#endif

// yyattribute
#ifdef YYDEBUG
void YYFAR* YYPARSENAME::yyattribute1(int index) const
{
	YYSTYPE YYFAR* p = &((YYSTYPE YYFAR*)yyattributestackptr)[yytop + index];
	return p;
}
#define yyattribute(index) (*(YYSTYPE YYFAR*)yyattribute1(index))
#else
#define yyattribute(index) (((YYSTYPE YYFAR*)yyattributestackptr)[yytop + (index)])
#endif

#ifdef YYDEBUG
void YYPARSENAME::yyinitdebug(void YYFAR** p, int count) const
{
	yyassert(p != NULL);
	yyassert(count >= 1);

	YYSTYPE YYFAR** p1 = (YYSTYPE YYFAR**)p;
	for (int i = 0; i < count; i++) {
		p1[i] = &((YYSTYPE YYFAR*)yyattributestackptr)[yytop + i - (count - 1)];
	}
}
#endif

void YYPARSENAME::yyaction(int action)
{
	switch (action) {
	case 0:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 120 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

      CheckIsIface(string(yyattribute(2 - 3).strval)); //If no - everything is ok
      StartIfaceDeclaration();
  
#line 83 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 1:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[7];
			yyinitdebug((void YYFAR**)yya, 7);
#endif
			{
#line 125 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

    //Information about new interface has been read
    //Add it to list of read interfaces
      SrvCfgIfaceLst.append(new TSrvCfgIface(yyattribute(2 - 6).strval));
      //FIXME:used of char * should be always realeased
      delete yyattribute(2 - 6).strval;
      EndIfaceDeclaration();
  
#line 103 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 2:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 137 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        CheckIsIface(yyattribute(2 - 3).ival);   //If no - everything is ok
        StartIfaceDeclaration();
  
#line 119 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 3:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[7];
			yyinitdebug((void YYFAR**)yya, 7);
#endif
			{
#line 142 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

      SrvCfgIfaceLst.append(new TSrvCfgIface(yyattribute(2 - 6).ival));
      EndIfaceDeclaration();
  
#line 135 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 4:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 150 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

      CheckIsIface(string(yyattribute(2 - 3).strval));
      SrvCfgIfaceLst.append(new TSrvCfgIface(yyattribute(2 - 3).strval));
      SrvCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
      SrvCfgIfaceLst.getLast()->setNoConfig();
    
#line 153 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 5:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 160 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        CheckIsIface(yyattribute(2 - 3).ival);
      SrvCfgIfaceLst.append(new TSrvCfgIface(yyattribute(2 - 3).ival));
      SrvCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
        SrvCfgIfaceLst.getLast()->setNoConfig();
    
#line 171 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 6:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 181 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        StartClassDeclaration();
    
#line 186 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 7:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 185 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

            EndClassDeclaration();
        
#line 201 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 8:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 201 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=yyattribute(1 - 1).ival;
#line 214 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 9:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 202 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=yyattribute(1 - 1).ival;
#line 227 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 10:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 229 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

    PresentLst.append(new TIPv6Addr(yyattribute(1 - 1).addrval));
  
#line 242 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 11:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 233 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

    PresentLst.append(new TIPv6Addr(yyattribute(3 - 3).addrval));
  
#line 257 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 12:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 240 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TIPv6Addr(yyattribute(1 - 1).addrval),new TIPv6Addr(yyattribute(1 - 1).addrval)));
    
#line 272 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 13:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 244 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr(yyattribute(1 - 3).addrval));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr(yyattribute(3 - 3).addrval));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    
#line 292 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 14:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 253 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TIPv6Addr(yyattribute(3 - 3).addrval),new TIPv6Addr(yyattribute(3 - 3).addrval)));
    
#line 307 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 15:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 257 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr(yyattribute(3 - 5).addrval));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr(yyattribute(5 - 5).addrval));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    
#line 327 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 16:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 269 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TIPv6Addr(yyattribute(1 - 1).addrval),new TIPv6Addr(yyattribute(1 - 1).addrval)));
    
#line 342 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 17:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 273 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr(yyattribute(1 - 3).addrval));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr(yyattribute(3 - 3).addrval));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    
#line 362 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 18:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 282 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TIPv6Addr(yyattribute(3 - 3).addrval),new TIPv6Addr(yyattribute(3 - 3).addrval)));
    
#line 377 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 19:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 286 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        SmartPtr<TIPv6Addr> addr1(new TIPv6Addr(yyattribute(3 - 5).addrval));
        SmartPtr<TIPv6Addr> addr2(new TIPv6Addr(yyattribute(5 - 5).addrval));
        if (*addr1<=*addr2)
            PresentRangeLst.append(new TStationRange(addr1,addr2));
        else
            PresentRangeLst.append(new TStationRange(addr2,addr1));
    
#line 397 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 20:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 296 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TDUID(yyattribute(1 - 1).duidval.duid,yyattribute(1 - 1).duidval.length)));
        delete yyattribute(1 - 1).duidval.duid;
    
#line 413 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 21:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 301 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
   
        SmartPtr<TDUID> duid1(new TDUID(yyattribute(1 - 3).duidval.duid,yyattribute(1 - 3).duidval.length));
        SmartPtr<TDUID> duid2(new TDUID(yyattribute(3 - 3).duidval.duid,yyattribute(3 - 3).duidval.length));
        
        if (*duid1<=*duid2)
            PresentRangeLst.append(new TStationRange(duid1,duid2));
        else
            PresentRangeLst.append(new TStationRange(duid2,duid1));
        delete yyattribute(1 - 3).duidval.duid;
        delete yyattribute(3 - 3).duidval.duid;
    
#line 436 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 22:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 313 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.append(new TStationRange(new TDUID(yyattribute(3 - 3).duidval.duid,yyattribute(3 - 3).duidval.length)));
        delete yyattribute(3 - 3).duidval.duid;
    
#line 452 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 23:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 318 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        SmartPtr<TDUID> duid2(new TDUID(yyattribute(3 - 5).duidval.duid,yyattribute(3 - 5).duidval.length));
        SmartPtr<TDUID> duid1(new TDUID(yyattribute(5 - 5).duidval.duid,yyattribute(5 - 5).duidval.length));
        if (*duid1<=*duid2)
            PresentRangeLst.append(new TStationRange(duid1,duid2));
        else
            PresentRangeLst.append(new TStationRange(duid2,duid1));
        delete yyattribute(3 - 5).duidval.duid;
        delete yyattribute(5 - 5).duidval.duid;
    
#line 474 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 24:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 333 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.clear();    
    
#line 489 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 25:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 336 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setRejedClnt(&PresentRangeLst);
    
#line 504 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 26:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 343 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentRangeLst.clear();    
    
#line 519 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 27:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 346 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setAcceptClnt(&PresentRangeLst);
    
#line 534 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 28:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 353 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"


            PresentRangeLst.clear();    
        
#line 550 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 29:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 357 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

            ParserOptStack.getLast()->setPool(&PresentRangeLst);
        
#line 565 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 30:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 364 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setPrefBeg(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setPrefEnd(yyattribute(2 - 2).ival);
    
#line 581 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 31:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 369 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setPrefBeg(yyattribute(2 - 4).ival);
        ParserOptStack.getLast()->setPrefEnd(yyattribute(4 - 4).ival);   
    
#line 597 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 32:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 377 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setValidBeg(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setValidEnd(yyattribute(2 - 2).ival);
    
#line 613 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 33:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 382 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setValidBeg(yyattribute(2 - 4).ival);
        ParserOptStack.getLast()->setValidEnd(yyattribute(4 - 4).ival);  
    
#line 629 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 34:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 390 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setT1Beg(yyattribute(2 - 2).ival); 
        ParserOptStack.getLast()->setT1End(yyattribute(2 - 2).ival); 
    
#line 645 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 35:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 395 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setT1Beg(yyattribute(2 - 4).ival); 
        ParserOptStack.getLast()->setT1End(yyattribute(4 - 4).ival); 
    
#line 661 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 36:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 403 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setT2Beg(yyattribute(2 - 2).ival); 
        ParserOptStack.getLast()->setT2End(yyattribute(2 - 2).ival); 
    
#line 677 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 37:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 408 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setT2Beg(yyattribute(2 - 4).ival); 
        ParserOptStack.getLast()->setT2End(yyattribute(4 - 4).ival); 
    
#line 693 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 38:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 416 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        PresentLst.clear();
  
#line 708 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 39:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 419 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

            ParserOptStack.getLast()->setDNSSrv(&PresentLst);
  
#line 723 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 40:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 426 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

    PresentLst.clear();
  
#line 738 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 41:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 429 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setNTPSrv(&PresentLst);
    
#line 753 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 42:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 443 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setDomain(yyattribute(2 - 2).strval);
    
#line 768 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 43:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 450 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setTimeZone(yyattribute(2 - 2).strval); 
    
#line 783 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 44:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 457 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setMaxClientLease(yyattribute(2 - 2).ival);    
    
#line 798 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 45:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 464 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setUnicast(true);
        ParserOptStack.getLast()->setAddress(new TIPv6Addr(yyattribute(2 - 2).addrval));
    
#line 814 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 46:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 472 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        if((yyattribute(2 - 2).ival&&(!ParserOptStack.getLast()->getUnicast() ))||(yyattribute(2 - 2).ival>1))
            yyabort(); //there should be declared prior unicast address
        ParserOptStack.getLast()->setUnicast(yyattribute(2 - 2).ival);
    
#line 831 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 47:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 481 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        ParserOptStack.getLast()->setRapidCommit(yyattribute(2 - 2).ival); 
    
#line 846 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 48:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 488 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"
 
        if ((yyattribute(2 - 2).ival<0)||(yyattribute(2 - 2).ival>255))
            yyabort(); //FIXME:Exception or what kind of notification
        ParserOptStack.getLast()->setPreference(yyattribute(2 - 2).ival);    
    
#line 863 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 49:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 497 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setMaxLeases(yyattribute(2 - 2).ival);
    
#line 878 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 50:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 503 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        if (yyattribute(2 - 2).ival<5)
            ParserOptStack.getLast()->setLogLevel(yyattribute(2 - 2).ival);
        else
            yyabort();
    
#line 896 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 51:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 513 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setLogName(yyattribute(2 - 2).strval);
    
#line 911 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	case 52:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 520 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"

        ParserOptStack.getLast()->setWorkDir(yyattribute(2 - 2).strval);
    
#line 926 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
			}
		}
		break;
	default:
		yyassert(0);
		break;
	}
}
#line 557 "C:\\Dyplom\\sources\\SrvParser\\srvParser.y"


/////////////////////////////////////////////////////////////////////////////
// programs section

//method check whether interface with id=ifaceNr has been 
//already declared
void SrvParser::CheckIsIface(int ifaceNr)
{
  SmartPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) yyabort();
};
    
//method check whether interface with id=ifaceName has been
//already declared 
void SrvParser::CheckIsIface(string ifaceName)
{
  SmartPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst.first();
  while (ptr=SrvCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) yyabort();
  };
};

//method creates new scope appropriately for interface options and declarations
//clears all lists except the list of interfaces and adds new group
void SrvParser::StartIfaceDeclaration()
{
  //Interface scope, so parameters associated with global scope are pushed on stack
  ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
  SrvCfgAddrClassLst.clear();

}

void SrvParser::EndIfaceDeclaration()
{

  SmartPtr<TSrvCfgAddrClass> ptrAddrClass;
  if (!SrvCfgAddrClassLst.count())
    yyabort();
  SrvCfgAddrClassLst.first();
    while (ptrAddrClass=SrvCfgAddrClassLst.get())
      SrvCfgIfaceLst.getLast()->addAddrClass(ptrAddrClass);
  //setting interface options on the basis of just read information
  SrvCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
  //FIXED:Here should be add list of Groups to this iface and here it is
  
  //if (groupsCnt==0)
  //  EmptyIface();  //add one IA with one address to this iface
  //restore global options
  ParserOptStack.delLast();
    
}   

void SrvParser::StartClassDeclaration()
{
  ParserOptStack.append(new TSrvParsGlobalOpt(*ParserOptStack.getLast()));
}

void SrvParser::EndClassDeclaration()
{
    if (!ParserOptStack.getLast()->countPool())
        yyabort();
    SrvCfgAddrClassLst.append(new TSrvCfgAddrClass());
  //setting interface options on the basis of just read information
  SrvCfgAddrClassLst.getLast()->setOptions(ParserOptStack.getLast());
  //FIXED:Here should be add list of Groups to this iface and here it is
  ParserOptStack.delLast();
}

#line 1010 "C:\\Dyplom\\sources\\SrvParser\\srvParser.cpp"
void YYPARSENAME::yytables()
{
	yyattribute_size = sizeof(YYSTYPE);
	yysstack_size = YYSTACK_SIZE;

#ifdef YYDEBUG
	static const yysymbol_t YYNEARFAR YYBASED_CODE symbol[] = {
		{ "$end", 0 },
		{ "\',\'", 44 },
		{ "\'-\'", 45 },
		{ "\'{\'", 123 },
		{ "\'}\'", 125 },
		{ "error", 256 },
		{ "IFACE_", 257 },
		{ "NO_CONFIG_", 258 },
		{ "CLASS_", 259 },
		{ "LOGNAME_", 260 },
		{ "LOGLEVEL_", 261 },
		{ "WORKDIR_", 262 },
		{ "NTP_SERVER_", 263 },
		{ "TIME_ZONE_", 264 },
		{ "DNS_SERVER_", 265 },
		{ "DOMAIN_", 266 },
		{ "ACCEPT_ONLY_", 267 },
		{ "REJECT_CLIENTS_", 268 },
		{ "POOL_", 269 },
		{ "T1_", 270 },
		{ "T2_", 271 },
		{ "PREF_TIME_", 272 },
		{ "VALID_TIME_", 273 },
		{ "UNICAST_", 274 },
		{ "PREFERENCE_", 275 },
		{ "RAPID_COMMIT_", 276 },
		{ "MAX_LEASE_", 277 },
		{ "CLNT_MAX_LEASE_", 278 },
		{ "STRING_", 279 },
		{ "HEXNUMBER_", 280 },
		{ "INTNUMBER_", 281 },
		{ "IPV6ADDR_", 282 },
		{ "DUID_", 283 },
		{ NULL, 0 }
	};
	yysymbol = symbol;

	static const char* const YYNEARFAR YYBASED_CODE rule[] = {
		"$accept: Grammar",
		"Grammar: GlobalDeclarationList",
		"Grammar:",
		"GlobalDeclarationList: GlobalOptionDeclaration",
		"GlobalDeclarationList: InterfaceDeclaration",
		"GlobalDeclarationList: GlobalDeclarationList GlobalOptionDeclaration",
		"GlobalDeclarationList: GlobalDeclarationList InterfaceDeclaration",
		"$$1:",
		"InterfaceDeclaration: IFACE_ STRING_ \'{\' $$1 InterfaceDeclarationsList \'}\'",
		"$$2:",
		"InterfaceDeclaration: IFACE_ Number \'{\' $$2 InterfaceDeclarationsList \'}\'",
		"InterfaceDeclaration: IFACE_ STRING_ NO_CONFIG_",
		"InterfaceDeclaration: IFACE_ Number NO_CONFIG_",
		"InterfaceDeclarationsList: InterfaceOptionDeclaration",
		"InterfaceDeclarationsList: InterfaceDeclarationsList InterfaceOptionDeclaration",
		"InterfaceDeclarationsList: ClassDeclaration",
		"InterfaceDeclarationsList: InterfaceDeclarationsList ClassDeclaration",
		"$$3:",
		"ClassDeclaration: CLASS_ \'{\' $$3 ClassOptionDeclarationsList \'}\'",
		"ClassOptionDeclarationsList: ClassOptionDeclaration",
		"ClassOptionDeclarationsList: ClassOptionDeclarationsList ClassOptionDeclaration",
		"Number: HEXNUMBER_",
		"Number: INTNUMBER_",
		"ADDRESSList: IPV6ADDR_",
		"ADDRESSList: ADDRESSList \',\' IPV6ADDR_",
		"ADDRESSRangeList: IPV6ADDR_",
		"ADDRESSRangeList: IPV6ADDR_ \'-\' IPV6ADDR_",
		"ADDRESSRangeList: ADDRESSRangeList \',\' IPV6ADDR_",
		"ADDRESSRangeList: ADDRESSRangeList \',\' IPV6ADDR_ \'-\' IPV6ADDR_",
		"ADDRESSDUIDRangeList: IPV6ADDR_",
		"ADDRESSDUIDRangeList: IPV6ADDR_ \'-\' IPV6ADDR_",
		"ADDRESSDUIDRangeList: ADDRESSDUIDRangeList \',\' IPV6ADDR_",
		"ADDRESSDUIDRangeList: ADDRESSDUIDRangeList \',\' IPV6ADDR_ \'-\' IPV6ADDR_",
		"ADDRESSDUIDRangeList: DUID_",
		"ADDRESSDUIDRangeList: DUID_ \'-\' DUID_",
		"ADDRESSDUIDRangeList: ADDRESSDUIDRangeList \',\' DUID_",
		"ADDRESSDUIDRangeList: ADDRESSDUIDRangeList \',\' DUID_ \'-\' DUID_",
		"$$4:",
		"RejectClientsOption: REJECT_CLIENTS_ $$4 ADDRESSDUIDRangeList",
		"$$5:",
		"AcceptOnlyOption: ACCEPT_ONLY_ $$5 ADDRESSDUIDRangeList",
		"$$6:",
		"PoolOption: POOL_ $$6 ADDRESSRangeList",
		"PreferredTimeOption: PREF_TIME_ Number",
		"PreferredTimeOption: PREF_TIME_ Number \'-\' Number",
		"ValidTimeOption: VALID_TIME_ Number",
		"ValidTimeOption: VALID_TIME_ Number \'-\' Number",
		"T1Option: T1_ Number",
		"T1Option: T1_ Number \'-\' Number",
		"T2Option: T2_ Number",
		"T2Option: T2_ Number \'-\' Number",
		"$$7:",
		"DNSServerOption: DNS_SERVER_ $$7 ADDRESSList",
		"$$8:",
		"NTPServerOption: NTP_SERVER_ $$8 ADDRESSList",
		"DomainOption: DOMAIN_ STRING_",
		"TimeZoneOption: TIME_ZONE_ STRING_",
		"ClntMaxLeaseOption: CLNT_MAX_LEASE_ Number",
		"UnicastAddressOption: UNICAST_ IPV6ADDR_",
		"UnicastOption: UNICAST_ Number",
		"RapidCommitOption: RAPID_COMMIT_ Number",
		"PreferenceOption: PREFERENCE_ Number",
		"MaxLeaseOption: MAX_LEASE_ Number",
		"LogLevelOption: LOGLEVEL_ Number",
		"LogNameOption: LOGNAME_ STRING_",
		"WorkDirOption: WORKDIR_ STRING_",
		"GlobalOptionDeclaration: InterfaceOptionDeclaration",
		"GlobalOptionDeclaration: LogLevelOption",
		"GlobalOptionDeclaration: LogNameOption",
		"GlobalOptionDeclaration: WorkDirOption",
		"InterfaceOptionDeclaration: ClassOptionDeclaration",
		"InterfaceOptionDeclaration: UnicastAddressOption",
		"InterfaceOptionDeclaration: DNSServerOption",
		"InterfaceOptionDeclaration: NTPServerOption",
		"InterfaceOptionDeclaration: DomainOption",
		"InterfaceOptionDeclaration: TimeZoneOption",
		"ClassOptionDeclaration: PreferredTimeOption",
		"ClassOptionDeclaration: ValidTimeOption",
		"ClassOptionDeclaration: PoolOption",
		"ClassOptionDeclaration: T1Option",
		"ClassOptionDeclaration: T2Option",
		"ClassOptionDeclaration: RejectClientsOption",
		"ClassOptionDeclaration: AcceptOnlyOption",
		"ClassOptionDeclaration: UnicastOption",
		"ClassOptionDeclaration: RapidCommitOption",
		"ClassOptionDeclaration: PreferenceOption",
		"ClassOptionDeclaration: MaxLeaseOption",
		"ClassOptionDeclaration: ClntMaxLeaseOption"
	};
	yyrule = rule;
#endif

	static const yyreduction_t YYNEARFAR YYBASED_CODE reduction[] = {
		{ 0, 1, -1 },
		{ 1, 1, -1 },
		{ 1, 0, -1 },
		{ 2, 1, -1 },
		{ 2, 1, -1 },
		{ 2, 2, -1 },
		{ 2, 2, -1 },
		{ 4, 0, 0 },
		{ 3, 6, 1 },
		{ 5, 0, 2 },
		{ 3, 6, 3 },
		{ 3, 3, 4 },
		{ 3, 3, 5 },
		{ 6, 1, -1 },
		{ 6, 2, -1 },
		{ 6, 1, -1 },
		{ 6, 2, -1 },
		{ 8, 0, 6 },
		{ 7, 5, 7 },
		{ 9, 1, -1 },
		{ 9, 2, -1 },
		{ 10, 1, 8 },
		{ 10, 1, 9 },
		{ 11, 1, 10 },
		{ 11, 3, 11 },
		{ 12, 1, 12 },
		{ 12, 3, 13 },
		{ 12, 3, 14 },
		{ 12, 5, 15 },
		{ 13, 1, 16 },
		{ 13, 3, 17 },
		{ 13, 3, 18 },
		{ 13, 5, 19 },
		{ 13, 1, 20 },
		{ 13, 3, 21 },
		{ 13, 3, 22 },
		{ 13, 5, 23 },
		{ 15, 0, 24 },
		{ 14, 3, 25 },
		{ 17, 0, 26 },
		{ 16, 3, 27 },
		{ 19, 0, 28 },
		{ 18, 3, 29 },
		{ 20, 2, 30 },
		{ 20, 4, 31 },
		{ 21, 2, 32 },
		{ 21, 4, 33 },
		{ 22, 2, 34 },
		{ 22, 4, 35 },
		{ 23, 2, 36 },
		{ 23, 4, 37 },
		{ 25, 0, 38 },
		{ 24, 3, 39 },
		{ 27, 0, 40 },
		{ 26, 3, 41 },
		{ 28, 2, 42 },
		{ 29, 2, 43 },
		{ 30, 2, 44 },
		{ 31, 2, 45 },
		{ 32, 2, 46 },
		{ 33, 2, 47 },
		{ 34, 2, 48 },
		{ 35, 2, 49 },
		{ 36, 2, 50 },
		{ 37, 2, 51 },
		{ 38, 2, 52 },
		{ 39, 1, -1 },
		{ 39, 1, -1 },
		{ 39, 1, -1 },
		{ 39, 1, -1 },
		{ 40, 1, -1 },
		{ 40, 1, -1 },
		{ 40, 1, -1 },
		{ 40, 1, -1 },
		{ 40, 1, -1 },
		{ 40, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 },
		{ 41, 1, -1 }
	};
	yyreduction = reduction;

	static const yytokenaction_t YYNEARFAR YYBASED_CODE tokenaction[] = {
		{ 127, YYAT_SHIFT, 129 },
		{ 22, YYAT_SHIFT, 1 },
		{ 50, YYAT_SHIFT, 75 },
		{ 47, YYAT_SHIFT, 73 },
		{ 22, YYAT_SHIFT, 2 },
		{ 22, YYAT_SHIFT, 3 },
		{ 22, YYAT_SHIFT, 4 },
		{ 22, YYAT_SHIFT, 5 },
		{ 22, YYAT_SHIFT, 6 },
		{ 22, YYAT_SHIFT, 7 },
		{ 22, YYAT_SHIFT, 8 },
		{ 22, YYAT_SHIFT, 9 },
		{ 22, YYAT_SHIFT, 10 },
		{ 22, YYAT_SHIFT, 11 },
		{ 22, YYAT_SHIFT, 12 },
		{ 22, YYAT_SHIFT, 13 },
		{ 22, YYAT_SHIFT, 14 },
		{ 22, YYAT_SHIFT, 15 },
		{ 22, YYAT_SHIFT, 16 },
		{ 22, YYAT_SHIFT, 17 },
		{ 22, YYAT_SHIFT, 18 },
		{ 22, YYAT_SHIFT, 19 },
		{ 22, YYAT_SHIFT, 20 },
		{ 0, YYAT_SHIFT, 1 },
		{ 106, YYAT_SHIFT, 118 },
		{ 122, YYAT_ERROR, 0 },
		{ 0, YYAT_SHIFT, 2 },
		{ 0, YYAT_SHIFT, 3 },
		{ 0, YYAT_SHIFT, 4 },
		{ 0, YYAT_SHIFT, 5 },
		{ 0, YYAT_SHIFT, 6 },
		{ 0, YYAT_SHIFT, 7 },
		{ 0, YYAT_SHIFT, 8 },
		{ 0, YYAT_SHIFT, 9 },
		{ 0, YYAT_SHIFT, 10 },
		{ 0, YYAT_SHIFT, 11 },
		{ 0, YYAT_SHIFT, 12 },
		{ 0, YYAT_SHIFT, 13 },
		{ 0, YYAT_SHIFT, 14 },
		{ 0, YYAT_SHIFT, 15 },
		{ 0, YYAT_SHIFT, 16 },
		{ 0, YYAT_SHIFT, 17 },
		{ 0, YYAT_SHIFT, 18 },
		{ 0, YYAT_SHIFT, 19 },
		{ 0, YYAT_SHIFT, 20 },
		{ 126, YYAT_SHIFT, 48 },
		{ 126, YYAT_SHIFT, 49 },
		{ 95, YYAT_SHIFT, 110 },
		{ 95, YYAT_SHIFT, 111 },
		{ 59, YYAT_SHIFT, 80 },
		{ 59, YYAT_SHIFT, 81 },
		{ 121, YYAT_SHIFT, 125 },
		{ 120, YYAT_SHIFT, 124 },
		{ 119, YYAT_SHIFT, 123 },
		{ 113, YYAT_SHIFT, 121 },
		{ 111, YYAT_SHIFT, 120 },
		{ 110, YYAT_SHIFT, 119 },
		{ 103, YYAT_SHIFT, 115 },
		{ 102, YYAT_SHIFT, 114 },
		{ 97, YYAT_SHIFT, 113 },
		{ 96, YYAT_SHIFT, 112 },
		{ 94, YYAT_SHIFT, 109 },
		{ 93, YYAT_SHIFT, 108 },
		{ 92, YYAT_SHIFT, 107 },
		{ 91, YYAT_ERROR, 0 },
		{ 85, YYAT_SHIFT, 97 },
		{ 84, YYAT_SHIFT, 96 },
		{ 83, YYAT_SHIFT, 95 },
		{ 82, YYAT_SHIFT, 95 },
		{ 81, YYAT_SHIFT, 94 },
		{ 80, YYAT_SHIFT, 93 },
		{ 79, YYAT_SHIFT, 92 },
		{ 78, YYAT_SHIFT, 92 },
		{ 64, YYAT_SHIFT, 89 },
		{ 63, YYAT_SHIFT, 88 },
		{ 62, YYAT_SHIFT, 87 },
		{ 61, YYAT_SHIFT, 86 },
		{ 60, YYAT_SHIFT, 84 },
		{ 56, YYAT_SHIFT, 77 },
		{ 21, YYAT_ACCEPT, 0 },
		{ 16, YYAT_SHIFT, 65 },
		{ 8, YYAT_SHIFT, 57 },
		{ 6, YYAT_SHIFT, 55 },
		{ 4, YYAT_SHIFT, 53 },
		{ 2, YYAT_SHIFT, 51 },
		{ 1, YYAT_SHIFT, 47 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 50, YYAT_SHIFT, 76 },
		{ 47, YYAT_SHIFT, 74 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 127, YYAT_SHIFT, 9 },
		{ 127, YYAT_SHIFT, 10 },
		{ 127, YYAT_SHIFT, 11 },
		{ 127, YYAT_SHIFT, 12 },
		{ 127, YYAT_SHIFT, 13 },
		{ 127, YYAT_SHIFT, 14 },
		{ 127, YYAT_SHIFT, 15 },
		{ 127, YYAT_SHIFT, 126 },
		{ 127, YYAT_SHIFT, 17 },
		{ 127, YYAT_SHIFT, 18 },
		{ 127, YYAT_SHIFT, 19 },
		{ 127, YYAT_SHIFT, 20 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 106, YYAT_SHIFT, 102 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 106, YYAT_SHIFT, 5 },
		{ 106, YYAT_SHIFT, 6 },
		{ 106, YYAT_SHIFT, 7 },
		{ 106, YYAT_SHIFT, 8 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 106, YYAT_SHIFT, 16 }
	};
	yytokenaction = tokenaction;
	yytokenaction_size = 174;

	static const yystateaction_t YYNEARFAR YYBASED_CODE stateaction[] = {
		{ -234, 1, YYAT_REDUCE, 2 },
		{ -194, 1, YYAT_DEFAULT, 126 },
		{ -195, 1, YYAT_DEFAULT, 8 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ -196, 1, YYAT_DEFAULT, 8 },
		{ 0, 0, YYAT_REDUCE, 53 },
		{ -197, 1, YYAT_DEFAULT, 8 },
		{ 0, 0, YYAT_REDUCE, 51 },
		{ -198, 1, YYAT_ERROR, 0 },
		{ 0, 0, YYAT_REDUCE, 39 },
		{ 0, 0, YYAT_REDUCE, 37 },
		{ 0, 0, YYAT_REDUCE, 41 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ -202, 1, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 79, 1, YYAT_ERROR, 0 },
		{ -256, 1, YYAT_REDUCE, 1 },
		{ 0, 0, YYAT_REDUCE, 3 },
		{ 0, 0, YYAT_REDUCE, 4 },
		{ 0, 0, YYAT_REDUCE, 66 },
		{ 0, 0, YYAT_REDUCE, 70 },
		{ 0, 0, YYAT_REDUCE, 81 },
		{ 0, 0, YYAT_REDUCE, 82 },
		{ 0, 0, YYAT_REDUCE, 78 },
		{ 0, 0, YYAT_REDUCE, 76 },
		{ 0, 0, YYAT_REDUCE, 77 },
		{ 0, 0, YYAT_REDUCE, 79 },
		{ 0, 0, YYAT_REDUCE, 80 },
		{ 0, 0, YYAT_REDUCE, 72 },
		{ 0, 0, YYAT_REDUCE, 73 },
		{ 0, 0, YYAT_REDUCE, 74 },
		{ 0, 0, YYAT_REDUCE, 75 },
		{ 0, 0, YYAT_REDUCE, 87 },
		{ 0, 0, YYAT_REDUCE, 71 },
		{ 0, 0, YYAT_REDUCE, 83 },
		{ 0, 0, YYAT_REDUCE, 84 },
		{ 0, 0, YYAT_REDUCE, 85 },
		{ 0, 0, YYAT_REDUCE, 86 },
		{ 0, 0, YYAT_REDUCE, 67 },
		{ 0, 0, YYAT_REDUCE, 68 },
		{ 0, 0, YYAT_REDUCE, 69 },
		{ -120, 1, YYAT_DEFAULT, 102 },
		{ 0, 0, YYAT_REDUCE, 21 },
		{ 0, 0, YYAT_REDUCE, 22 },
		{ -121, 1, YYAT_DEFAULT, 102 },
		{ 0, 0, YYAT_REDUCE, 64 },
		{ 0, 0, YYAT_REDUCE, 63 },
		{ 0, 0, YYAT_REDUCE, 65 },
		{ 0, 0, YYAT_DEFAULT, 56 },
		{ 0, 0, YYAT_REDUCE, 56 },
		{ -204, 1, YYAT_DEFAULT, 121 },
		{ 0, 0, YYAT_REDUCE, 55 },
		{ 0, 0, YYAT_DEFAULT, 59 },
		{ -233, 1, YYAT_DEFAULT, 121 },
		{ -205, 1, YYAT_DEFAULT, 121 },
		{ 31, 1, YYAT_REDUCE, 47 },
		{ 30, 1, YYAT_REDUCE, 49 },
		{ 29, 1, YYAT_REDUCE, 43 },
		{ 28, 1, YYAT_REDUCE, 45 },
		{ 0, 0, YYAT_REDUCE, 58 },
		{ 0, 0, YYAT_REDUCE, 59 },
		{ 0, 0, YYAT_REDUCE, 61 },
		{ 0, 0, YYAT_REDUCE, 60 },
		{ 0, 0, YYAT_REDUCE, 62 },
		{ 0, 0, YYAT_REDUCE, 57 },
		{ 0, 0, YYAT_REDUCE, 5 },
		{ 0, 0, YYAT_REDUCE, 6 },
		{ 0, 0, YYAT_REDUCE, 7 },
		{ 0, 0, YYAT_REDUCE, 11 },
		{ 0, 0, YYAT_REDUCE, 9 },
		{ 0, 0, YYAT_REDUCE, 12 },
		{ 0, 0, YYAT_REDUCE, 23 },
		{ 28, 1, YYAT_REDUCE, 54 },
		{ 27, 1, YYAT_REDUCE, 52 },
		{ 25, 1, YYAT_REDUCE, 29 },
		{ 24, 1, YYAT_REDUCE, 33 },
		{ 24, 1, YYAT_REDUCE, 40 },
		{ 23, 1, YYAT_REDUCE, 38 },
		{ 21, 1, YYAT_REDUCE, 25 },
		{ 21, 1, YYAT_REDUCE, 42 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 126 },
		{ 0, 0, YYAT_DEFAULT, 91 },
		{ -61, 1, YYAT_DEFAULT, 106 },
		{ -219, 1, YYAT_DEFAULT, 121 },
		{ -220, 1, YYAT_DEFAULT, 121 },
		{ -222, 1, YYAT_DEFAULT, 120 },
		{ -235, 1, YYAT_DEFAULT, 121 },
		{ -222, 1, YYAT_DEFAULT, 121 },
		{ -223, 1, YYAT_DEFAULT, 121 },
		{ 0, 0, YYAT_REDUCE, 48 },
		{ 0, 0, YYAT_REDUCE, 50 },
		{ 0, 0, YYAT_REDUCE, 44 },
		{ 0, 0, YYAT_REDUCE, 46 },
		{ -65, 1, YYAT_ERROR, 0 },
		{ -68, 1, YYAT_DEFAULT, 106 },
		{ 0, 0, YYAT_REDUCE, 13 },
		{ 0, 0, YYAT_REDUCE, 15 },
		{ -101, 1, YYAT_DEFAULT, 127 },
		{ 0, 0, YYAT_REDUCE, 24 },
		{ 0, 0, YYAT_REDUCE, 30 },
		{ 0, 0, YYAT_REDUCE, 34 },
		{ 11, 1, YYAT_REDUCE, 31 },
		{ 10, 1, YYAT_REDUCE, 35 },
		{ 0, 0, YYAT_REDUCE, 26 },
		{ 9, 1, YYAT_REDUCE, 27 },
		{ 0, 0, YYAT_REDUCE, 17 },
		{ 0, 0, YYAT_REDUCE, 8 },
		{ 0, 0, YYAT_REDUCE, 14 },
		{ 0, 0, YYAT_REDUCE, 16 },
		{ 0, 0, YYAT_REDUCE, 10 },
		{ -229, 1, YYAT_DEFAULT, 121 },
		{ -231, 1, YYAT_ERROR, 0 },
		{ -231, 1, YYAT_ERROR, 0 },
		{ -100, 1, YYAT_DEFAULT, 127 },
		{ 0, 0, YYAT_REDUCE, 32 },
		{ 0, 0, YYAT_REDUCE, 36 },
		{ 0, 0, YYAT_REDUCE, 28 },
		{ -235, 1, YYAT_ERROR, 0 },
		{ -125, 1, YYAT_ERROR, 0 },
		{ 0, 0, YYAT_REDUCE, 19 },
		{ 0, 0, YYAT_REDUCE, 18 },
		{ 0, 0, YYAT_REDUCE, 20 }
	};
	yystateaction = stateaction;

	static const yynontermgoto_t YYNEARFAR YYBASED_CODE nontermgoto[] = {
		{ 127, 27 },
		{ 126, 66 },
		{ 127, 28 },
		{ 114, 122 },
		{ 127, 29 },
		{ 122, 127 },
		{ 127, 30 },
		{ 127, 31 },
		{ 127, 32 },
		{ 127, 33 },
		{ 90, 103 },
		{ 106, 117 },
		{ 89, 101 },
		{ 22, 72 },
		{ 88, 100 },
		{ 87, 99 },
		{ 127, 38 },
		{ 22, -1 },
		{ 127, 40 },
		{ 127, 41 },
		{ 127, 42 },
		{ 127, 43 },
		{ 0, 21 },
		{ 0, 22 },
		{ 0, 24 },
		{ 91, 106 },
		{ 91, 105 },
		{ 127, 130 },
		{ 106, 34 },
		{ 86, 98 },
		{ 106, 35 },
		{ 75, 91 },
		{ 106, 36 },
		{ 106, 37 },
		{ 73, 90 },
		{ 106, 39 },
		{ 60, 85 },
		{ 122, 128 },
		{ 59, 83 },
		{ 58, 82 },
		{ 56, 79 },
		{ 54, 78 },
		{ 20, 70 },
		{ 19, 69 },
		{ 106, 116 },
		{ 106, 26 },
		{ 22, 44 },
		{ 22, 45 },
		{ 22, 46 },
		{ 22, 71 },
		{ 22, 25 },
		{ 18, 68 },
		{ 17, 67 },
		{ 15, 64 },
		{ 14, 63 },
		{ 13, 62 },
		{ 12, 61 },
		{ 11, 60 },
		{ 10, 59 },
		{ 91, 104 },
		{ 0, 23 },
		{ 9, 58 },
		{ 7, 56 },
		{ 5, 54 },
		{ 3, 52 },
		{ 1, 50 }
	};
	yynontermgoto = nontermgoto;
	yynontermgoto_size = 66;

	static const yystategoto_t YYNEARFAR YYBASED_CODE stategoto[] = {
		{ 21, 22 },
		{ 55, -1 },
		{ 0, -1 },
		{ 54, -1 },
		{ 0, -1 },
		{ 36, -1 },
		{ 0, -1 },
		{ 37, -1 },
		{ 0, -1 },
		{ 44, -1 },
		{ 43, -1 },
		{ 38, -1 },
		{ 46, -1 },
		{ 45, -1 },
		{ 44, -1 },
		{ 43, -1 },
		{ 0, 126 },
		{ 42, -1 },
		{ 41, -1 },
		{ 33, -1 },
		{ 32, -1 },
		{ 0, -1 },
		{ 10, 106 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 30, -1 },
		{ 0, -1 },
		{ 29, -1 },
		{ 0, -1 },
		{ 26, -1 },
		{ 25, -1 },
		{ 24, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 30, -1 },
		{ 0, -1 },
		{ 26, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 19, -1 },
		{ 5, -1 },
		{ 4, -1 },
		{ 2, -1 },
		{ 4, 91 },
		{ 19, 106 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, 106 },
		{ 0, -1 },
		{ 0, -1 },
		{ 4, 127 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ -5, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ -4, 127 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ -9, -1 },
		{ -14, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 }
	};
	yystategoto = stategoto;

	yydestructorptr = NULL;

	yytokendestptr = NULL;
	yytokendest_size = 0;
	yytokendestbase = 0;
}
