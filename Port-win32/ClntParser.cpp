#include <cyacc.h>

#line 1 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

/****************************************************************************
parser.y
ParserWizard generated YACC file.

Date: 30 czerwca 2003
****************************************************************************/



//TClntParsServAddr

#line 17 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
// repeated because of possible precompiled header
#include <cyacc.h>

#include "ClntParser.h"

/////////////////////////////////////////////////////////////////////////////
// constructor

YYPARSENAME::YYPARSENAME()
{
	yytables();
#line 58 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    // place any extra initialisation code here
    ParserOptStack.append(new TClntParsGlobalOpt());
    ParserOptStack.getFirst()->setIAIDCnt(1);

#line 35 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 128 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        CheckIsIface(string(yyattribute(2 - 3).strval)); //If no - everything is ok
        StartIfaceDeclaration();
    
#line 85 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 133 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        //Information about new interface has been read
        //Add it to list of read interfaces
        ClntCfgIfaceLst.append(new TClntCfgIface(yyattribute(2 - 6).strval));
        //FIXME:used of char * should be always realeased
        delete yyattribute(2 - 6).strval;
        EndIfaceDeclaration();
    
#line 105 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 146 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        CheckIsIface(yyattribute(2 - 3).ival);   //If no - everything is ok
        StartIfaceDeclaration();
    
#line 121 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 151 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ClntCfgIfaceLst.append(new TClntCfgIface(yyattribute(2 - 6).ival) );
        EndIfaceDeclaration();
    
#line 137 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 4:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 160 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      CheckIsIface(string(yyattribute(2 - 4).strval));
      ClntCfgIfaceLst.append(new TClntCfgIface(yyattribute(2 - 4).strval));
      delete yyattribute(2 - 4).strval;
      EmptyIface();
    
#line 155 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 5:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 171 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      CheckIsIface(yyattribute(2 - 4).ival);
      ClntCfgIfaceLst.append(new TClntCfgIface(yyattribute(2 - 4).ival));
      EmptyIface();
    
#line 172 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 6:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 181 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        CheckIsIface(string(yyattribute(2 - 3).strval));
        ClntCfgIfaceLst.append(new TClntCfgIface(yyattribute(2 - 3).strval));
        ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
        ClntCfgIfaceLst.getLast()->setNoConfig();
        delete yyattribute(2 - 3).strval;
    
#line 191 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 7:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 193 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        CheckIsIface(yyattribute(2 - 3).ival);
        ClntCfgIfaceLst.append(SmartPtr<TClntCfgIface> (new TClntCfgIface(yyattribute(2 - 3).ival)) );
        ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
        ClntCfgIfaceLst.getLast()->setNoConfig();
    
#line 209 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 8:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 213 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
 
    StartIADeclaration(false);
  
#line 224 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 9:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 217 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        EndIADeclaration(1);
    
#line 239 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 10:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 225 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        StartIADeclaration(false);
    
#line 254 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 11:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[7];
			yyinitdebug((void YYFAR**)yya, 7);
#endif
			{
#line 229 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    EndIADeclaration(yyattribute(2 - 6).ival);
  
#line 269 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 12:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 237 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    StartIADeclaration(false);
        EndIADeclaration(yyattribute(2 - 4).ival);
    
#line 285 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 13:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 246 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    StartIADeclaration(false);
        EndIADeclaration(yyattribute(2 - 2).ival);
    
#line 301 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 255 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    StartIADeclaration(true);
        EndIADeclaration(1);
    
#line 317 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 15:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 264 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    StartIADeclaration(true);
        EndIADeclaration(1);
    
#line 333 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 16:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 279 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
    
#line 348 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 17:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 283 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      //ClntCfgAddrLst.append(SmartPtr<TClntCfgAddr> (new TClntCfgAddr()));
      //set proper options specific for this Address
      //ClntCfgAddrLst.getLast()->setOptions(&(*ParserOptStack.getLast()));
      ParserOptStack.delLast();
    
#line 366 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 291 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
      ParserOptStack.getLast()->setAddrHint(false);
    
#line 382 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 19:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[7];
			yyinitdebug((void YYFAR**)yya, 7);
#endif
			{
#line 296 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      for (int i=0;i<yyattribute(2 - 6).ival; i++) EmptyAddr();
      ParserOptStack.delLast();
    
#line 398 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 20:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 302 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      for (int i=0;i<yyattribute(2 - 4).ival; i++) EmptyAddr();
    
#line 413 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 307 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      EmptyAddr();
    
#line 428 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 22:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 312 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      for (int i=0;i<yyattribute(2 - 2).ival; i++) EmptyAddr();
    
#line 443 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 23:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 317 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

      EmptyAddr();
    
#line 458 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 326 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    if (ParserOptStack.getLast()->getAddrHint())
    {
        ClntCfgAddrLst.append(new TClntCfgAddr(new TIPv6Addr(yyattribute(1 - 1).addrval)));
        ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
    }
        else
            //here is agregated version of IA
            yyabort(); 
  
#line 480 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 25:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 337 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        if (ParserOptStack.getLast()->getAddrHint())
        {
                ClntCfgAddrLst.append(new TClntCfgAddr(new TIPv6Addr(yyattribute(2 - 2).addrval)));
                ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
        }
            else
                //here is agregated version of IA
                yyabort();
  
#line 502 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 26:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 384 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        if (yyattribute(2 - 2).ival<5)
            ParserOptStack.getLast()->setLogLevel(yyattribute(2 - 2).ival);
        else
            yyabort();
    
#line 520 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 27:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 400 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

           ParserOptStack.getLast()->setIsIAs(false);
        
#line 535 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 28:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 407 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setWorkDir(yyattribute(2 - 2).strval);
    
#line 550 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 29:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 414 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

            //ParserOptStack.getLast()->clearRejedSrv();
            PresentStationLst.clear();
            if (!(ParserOptStack.getLast()->isNewGroup())) 
                ParserOptStack.getLast()->setNewGroup(true);
    
#line 568 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 30:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 420 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setRejedSrvLst(&PresentStationLst);
    
#line 583 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 31:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 427 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

            PresentStationLst.clear();
            if (!(ParserOptStack.getLast()->isNewGroup())) 
                ParserOptStack.getLast()->setNewGroup(true);
  
#line 600 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 32:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 431 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

            ParserOptStack.getLast()->setPrefSrvLst(&PresentStationLst);
     
#line 615 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 33:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 439 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setPref(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setPrefSendOpt(Send);
    
#line 631 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 34:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 445 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setPref(yyattribute(3 - 3).ival);
        ParserOptStack.getLast()->setPrefSendOpt(yyattribute(2 - 3).SendOpt);
    
#line 647 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 35:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 453 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
 
        if ((!(ParserOptStack.getLast()->isNewGroup()))&&
            (ParserOptStack.getLast()->getRapidCommit()!=(bool)yyattribute(2 - 2).ival))
            ParserOptStack.getLast()->setNewGroup(true);
        ParserOptStack.getLast()->setRapidCommit(yyattribute(2 - 2).ival);
    
#line 665 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 463 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setValid(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setValidSendOpt(Send);
  
#line 681 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 37:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 468 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setValid(yyattribute(3 - 3).ival);
        ParserOptStack.getLast()->setValidSendOpt(yyattribute(2 - 3).SendOpt);
    
#line 697 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 38:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 476 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setT1(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setT1SendOpt(Send);
    
#line 713 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 481 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setT1(yyattribute(3 - 3).ival);
        ParserOptStack.getLast()->setT1SendOpt(yyattribute(2 - 3).SendOpt);
    
#line 729 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 40:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 489 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setT2(yyattribute(2 - 2).ival);
        ParserOptStack.getLast()->setT2SendOpt(Send);
  
#line 745 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 494 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setT2(yyattribute(3 - 3).ival);
        ParserOptStack.getLast()->setT2SendOpt(yyattribute(2 - 3).SendOpt);
  
#line 761 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 42:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 503 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
        ParserOptStack.getLast()->setDNSSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setDNSSrvSendOpt(Send);
        ParserOptStack.getLast()->setDNSSrvReqOpt(Request);
        ParserOptStack.getLast()->setReqDNSSrv(true);
    
#line 780 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 43:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 511 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

            PresentAddrLst.clear();
    
#line 795 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 44:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 514 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

            ParserOptStack.getLast()->setDNSSrvLst(&PresentAddrLst);
            ParserOptStack.getLast()->setDNSSrvSendOpt(Send);
            ParserOptStack.getLast()->setDNSSrvReqOpt(Request);
            ParserOptStack.getLast()->setReqDNSSrv(true);
    
#line 813 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 522 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 828 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 46:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 525 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDNSSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setDNSSrvSendOpt(yyattribute(2 - 4).SendOpt);
        ParserOptStack.getLast()->setDNSSrvReqOpt(Request);
        ParserOptStack.getLast()->setReqDNSSrv(true);
    
#line 846 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 533 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 861 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 48:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 536 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDNSSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setDNSSrvSendOpt(Send);
        ParserOptStack.getLast()->setDNSSrvReqOpt(yyattribute(1 - 4).ReqOpt);
        ParserOptStack.getLast()->setReqDNSSrv(true);
  
#line 879 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 49:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 544 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
  
#line 894 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 50:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 547 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDNSSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setDNSSrvSendOpt(yyattribute(3 - 5).SendOpt);
        ParserOptStack.getLast()->setDNSSrvReqOpt(yyattribute(1 - 5).ReqOpt);
        ParserOptStack.getLast()->setReqDNSSrv(true);
  
#line 912 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
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
#line 555 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
        ParserOptStack.getLast()->setReqDNSSrv(true);
  
#line 928 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 52:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 559 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        if (yyattribute(2 - 4).ival)
            ParserOptStack.getLast()->setAppDNSSrvLst(&PresentAddrLst);
        else
            ParserOptStack.getLast()->setPrepDNSSrvLst(&PresentAddrLst);
  
#line 946 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 53:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 569 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDomainReqOpt(Request); 
        ParserOptStack.getLast()->setDomainSendOpt(Send);
        ParserOptStack.getLast()->setDomain(string(""));
        ParserOptStack.getLast()->setReqDomainName(true);  
  
#line 964 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 54:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 576 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDomainReqOpt(Request); 
        ParserOptStack.getLast()->setDomainSendOpt(Send);
        ParserOptStack.getLast()->setDomain(yyattribute(2 - 2).strval);
        ParserOptStack.getLast()->setReqDomainName(true);
  
#line 982 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 55:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 583 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setDomainReqOpt(Request);
        ParserOptStack.getLast()->setDomainSendOpt(yyattribute(2 - 3).SendOpt);
        ParserOptStack.getLast()->setDomain(yyattribute(3 - 3).strval);
        ParserOptStack.getLast()->setReqDomainName(true);

  
#line 1001 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 56:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 592 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    ParserOptStack.getLast()->setDomainReqOpt(yyattribute(1 - 3).ReqOpt);
    ParserOptStack.getLast()->setDomainSendOpt(Send);
    ParserOptStack.getLast()->setDomain(yyattribute(3 - 3).strval);
    ParserOptStack.getLast()->setReqDomainName(true);
  
#line 1019 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 57:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 599 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    ParserOptStack.getLast()->setDomainReqOpt(yyattribute(1 - 4).ReqOpt);
    ParserOptStack.getLast()->setDomainSendOpt(yyattribute(3 - 4).SendOpt);
    ParserOptStack.getLast()->setDomain(yyattribute(4 - 4).strval);
    ParserOptStack.getLast()->setReqDomainName(true);
  
#line 1037 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 58:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 609 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setTimeZoneReqOpt(Request);   
        ParserOptStack.getLast()->setTimeZoneSendOpt(Send);
        ParserOptStack.getLast()->setTimeZone(string(""));
        ParserOptStack.getLast()->setReqTimeZone(true);
  
#line 1055 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 59:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 616 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setTimeZoneReqOpt(Request);   
        ParserOptStack.getLast()->setTimeZoneSendOpt(Send);
        ParserOptStack.getLast()->setTimeZone(yyattribute(2 - 2).strval);
        ParserOptStack.getLast()->setReqTimeZone(true);
  
#line 1073 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 60:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 624 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setTimeZoneReqOpt(Request);
        ParserOptStack.getLast()->setTimeZoneSendOpt(yyattribute(2 - 3).SendOpt);
        ParserOptStack.getLast()->setTimeZone(yyattribute(3 - 3).strval);
        ParserOptStack.getLast()->setReqTimeZone(true);
    
#line 1091 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 61:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 632 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    ParserOptStack.getLast()->setTimeZoneReqOpt(yyattribute(1 - 3).ReqOpt);
    ParserOptStack.getLast()->setTimeZoneSendOpt(Send);
    ParserOptStack.getLast()->setTimeZone(yyattribute(3 - 3).strval);
    ParserOptStack.getLast()->setReqTimeZone(true);
  
#line 1109 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 62:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 640 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    ParserOptStack.getLast()->setTimeZoneReqOpt(yyattribute(1 - 4).ReqOpt);
    ParserOptStack.getLast()->setTimeZoneSendOpt(yyattribute(3 - 4).SendOpt);
    ParserOptStack.getLast()->setTimeZone(yyattribute(4 - 4).strval);
    ParserOptStack.getLast()->setReqTimeZone(true);
  
#line 1127 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 63:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 650 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
        ParserOptStack.getLast()->setNTPSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setNTPSrvSendOpt(Send);
        ParserOptStack.getLast()->setNTPSrvReqOpt(Request);
        ParserOptStack.getLast()->setReqNTPSrv(true);        
    
#line 1146 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 64:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 657 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 1161 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 65:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 660 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setNTPSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setNTPSrvSendOpt(Send);
        ParserOptStack.getLast()->setNTPSrvReqOpt(Request);
        ParserOptStack.getLast()->setReqNTPSrv(true);
    
#line 1179 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 66:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 666 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 1194 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 67:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 669 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setNTPSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setNTPSrvSendOpt(yyattribute(2 - 4).SendOpt);
        ParserOptStack.getLast()->setNTPSrvReqOpt(Request);
        ParserOptStack.getLast()->setReqNTPSrv(true);
    
#line 1212 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 68:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 675 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 1227 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 69:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 678 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setNTPSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setNTPSrvSendOpt(Send);
        ParserOptStack.getLast()->setNTPSrvReqOpt(yyattribute(1 - 4).ReqOpt);
        ParserOptStack.getLast()->setReqNTPSrv(true);
    
#line 1245 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 70:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 684 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        PresentAddrLst.clear();
    
#line 1260 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 71:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[6];
			yyinitdebug((void YYFAR**)yya, 6);
#endif
			{
#line 687 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        ParserOptStack.getLast()->setNTPSrvLst(&PresentAddrLst);
        ParserOptStack.getLast()->setNTPSrvSendOpt(yyattribute(3 - 5).SendOpt);
        ParserOptStack.getLast()->setNTPSrvReqOpt(yyattribute(1 - 5).ReqOpt);
        ParserOptStack.getLast()->setReqNTPSrv(true);
    
#line 1278 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 72:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[3];
			yyinitdebug((void YYFAR**)yya, 3);
#endif
			{
#line 694 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
      
        PresentAddrLst.clear(); 
        ParserOptStack.getLast()->setReqNTPSrv(true);
    
#line 1294 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 73:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[5];
			yyinitdebug((void YYFAR**)yya, 5);
#endif
			{
#line 698 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

        if (yyattribute(2 - 4).ival)
            ParserOptStack.getLast()->setAppNTPSrvLst(&PresentAddrLst);
        else
            ParserOptStack.getLast()->setPrepNTPSrvLst(&PresentAddrLst);
    
#line 1312 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 74:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 742 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TIPv6Addr(yyattribute(1 - 1).addrval))));
  
#line 1327 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 75:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 746 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TDUID(yyattribute(1 - 1).duidval.duid,yyattribute(1 - 1).duidval.length))));
  
#line 1342 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 76:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 750 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    PresentStationLst.append(SmartPtr<TStationID> (new TStationID(new TIPv6Addr(yyattribute(3 - 3).addrval))));
  
#line 1357 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 77:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 754 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"

    PresentStationLst.append(SmartPtr<TStationID> (new TStationID( new TDUID(yyattribute(3 - 3).duidval.duid,yyattribute(3 - 3).duidval.length))));
  
#line 1372 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 78:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 760 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr(yyattribute(1 - 1).addrval)));
#line 1385 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 79:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[4];
			yyinitdebug((void YYFAR**)yya, 4);
#endif
			{
#line 761 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
PresentAddrLst.append(SmartPtr<TIPv6Addr> (new TIPv6Addr(yyattribute(3 - 3).addrval)));
#line 1398 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 80:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 765 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).SendOpt=Send;
#line 1411 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 81:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 766 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).SendOpt=Default;
#line 1424 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 82:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 767 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).SendOpt=Supersede;
#line 1437 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 83:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 771 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ReqOpt=Request;
#line 1450 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 84:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 772 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ReqOpt=Require;
#line 1463 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 85:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 776 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=1;
#line 1476 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 86:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 777 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=0;
#line 1489 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 87:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 780 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=yyattribute(1 - 1).ival;
#line 1502 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	case 88:
		{
#ifdef YYDEBUG
			YYSTYPE YYFAR* yya[2];
			yyinitdebug((void YYFAR**)yya, 2);
#endif
			{
#line 781 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"
(*(YYSTYPE YYFAR*)yyvalptr).ival=yyattribute(1 - 1).ival;
#line 1515 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
			}
		}
		break;
	default:
		yyassert(0);
		break;
	}
}
#line 784 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.y"


    /////////////////////////////////////////////////////////////////////////////
    // programs section
    
    //method check whether interface with id=ifaceNr has been 
    //already declared
void clntParser::CheckIsIface(int ifaceNr)
{
  SmartPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
    if ((ptr->getID())==ifaceNr) yyabort();
};
    
    //method check whether interface with id=ifaceName has been
    //already declared 
void clntParser::CheckIsIface(string ifaceName)
{
  SmartPtr<TClntCfgIface> ptr;
  ClntCfgIfaceLst.first();
  while (ptr=ClntCfgIfaceLst.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) yyabort();
  };
};

    //method creates new scope appropriately for interface options and declarations
    //clears all lists except the list of interfaces and adds new group
void clntParser::StartIfaceDeclaration()
{
  //Interface scope, so parameters associated with global scope are pushed on stack
  ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
  ParserOptStack.getLast()->setNewGroup(false);
  ClntCfgGroupLst.clear();
  ClntCfgIALst.clear();
  ClntCfgAddrLst.clear();
  //creation of default, new group
  ClntCfgGroupLst.append(new TClntCfgGroup());
  ClntCfgGroupLst.getLast()->setOptions(ParserOptStack.getLast());

}

void clntParser::EndIfaceDeclaration()
{
  //add all identity associations to last group
  SmartPtr<TClntCfgIA> ptrIA;
  ClntCfgIALst.first();
  while (ptrIA=ClntCfgIALst.get())
      ClntCfgGroupLst.getLast()->addIA(ptrIA);
   
  //set interface options on the basis of just read information
  ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
  //add list of groups to this iface
  ClntCfgGroupLst.first();
  SmartPtr<TClntCfgGroup> ptr;
  int groupsCnt=0;
  while (ptr=ClntCfgGroupLst.get())
    if (ptr->countIA()>0)
    {
      ClntCfgIfaceLst.getLast()->addGroup(ptr);
      groupsCnt++;
    };
    
  if ((ClntCfgIfaceLst.getLast())->onlyInformationRequest())
  {
    if (groupsCnt) yyabort();
  }
  else
  {
    //if only options has changed for this iface
    if (groupsCnt==0)
        EmptyIface();  //add one IA with one address to this iface
  }
  //restore global options
  ParserOptStack.delLast();
    
}   

void clntParser::EmptyIface()
{
    //set iface options on the basis of recent information
  ClntCfgIfaceLst.getLast()->setOptions(ParserOptStack.getLast());
  //add one IA with one address to this iface
  ClntCfgIfaceLst.getLast()->addGroup(new TClntCfgGroup());
  EmptyIA();
  ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());
  ClntCfgIfaceLst.getLast()->getLastGroup()->
    addIA(ClntCfgIALst.getLast());
    
}

//method creates new scope appropriately for interface options and declarations
//clears list of addresses
//bool aggregation - whether it is agregated IA option
void clntParser::StartIADeclaration(bool aggregation)
{
  ParserOptStack.append(new TClntParsGlobalOpt(*ParserOptStack.getLast()));
  ParserOptStack.getLast()->setNewGroup(false);
  ParserOptStack.getLast()->setAddrHint(!aggregation);
  ClntCfgAddrLst.clear();
}

void clntParser::EndIADeclaration(long iaCnt)
{
  if(ClntCfgAddrLst.count()==0)
    EmptyIA();
  else
  {
    ClntCfgIALst.append(new TClntCfgIA(ParserOptStack.getFirst()->getIncedIAIDCnt()));
    SmartPtr<TClntCfgAddr> ptr;
    ClntCfgAddrLst.first();
    while(ptr=ClntCfgAddrLst.get())
          ClntCfgIALst.getLast()->addAddr(ptr);
  }
  //set proper options specific for this IA
  ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());

  //Options change - new group should be created
  if (ParserOptStack.getLast()->isNewGroup())
  {

    //this IA will have its own group, bcse it does't match with previous ones
    ClntCfgGroupLst.prepend(new TClntCfgGroup());
    ClntCfgGroupLst.getFirst()->setOptions(ParserOptStack.getLast());
    ClntCfgGroupLst.first();
    ClntCfgGroupLst.getFirst()->addIA(ClntCfgIALst.getLast());
    for (int i=1;i<iaCnt;i++)
    {
      //przy tworzeniu wskaŸnika utworzenie kopi opisu opcji znajduj¹cej siê na koñcu listy
      SmartPtr<TClntCfgIA> ptr 
        (new TClntCfgIA(ClntCfgIALst.getLast(),ParserOptStack.getFirst()->getIncedIAIDCnt()));
      ClntCfgGroupLst.getFirst()->addIA(ptr);
    }
    ClntCfgIALst.delLast();
    ParserOptStack.delLast();
  }
  else
  {
    //FIXME: increase IAID when copy
     for (int i=1;i<iaCnt;i++)
     {
        SmartPtr<TClntCfgIA> ia(new TClntCfgIA(*ClntCfgIALst.getLast()));
        ia->setIAID(ParserOptStack.getFirst()->getIncedIAIDCnt());
        ClntCfgIALst.append(ia);
     }
     //this IA matches with previous ones and can be grouped with them
     //so it's should be left on the list and be appended with them to present list
     ParserOptStack.delLast();
       //new IA was found, so should new group be created ??

     if (ParserOptStack.getLast()->isNewGroup())
     {
          //ParserOptStack.delLast();
            //new IA was found, so should new group be created ??
        //FIXED: in the case list of IAs has more than iaCnt elements insert them into
        //the group except the last iaCnt IA, which belongs to the new group
        while(ClntCfgIALst.count()>iaCnt)
        {
             ClntCfgGroupLst.getLast()->addIA(ClntCfgIALst.getFirst());
             ClntCfgIALst.delFirst();
        }
        ClntCfgGroupLst.append(new TClntCfgGroup());
        //and now we will be waiting for matchin IAs
        ParserOptStack.getLast()->setNewGroup(false);
        ClntCfgGroupLst.getLast()->setOptions(ParserOptStack.getLast());
     };
  };
}

//metoda dodaje 1 IA do kolejki ClntCfgIAs, a nastêpnie adres do 
//ostatniego IA w kolejce ClntCfgIALst o w³aœciwoœciach opcji 
//znajduj¹cych siê na stosie ParsOptStack
void clntParser::EmptyIA()
{
      EmptyAddr();
    ClntCfgIALst.append(new TClntCfgIA(ParserOptStack.getFirst()->getIncedIAIDCnt()));
    ClntCfgIALst.getLast()->setOptions(ParserOptStack.getLast());
    ClntCfgIALst.getLast()->addAddr(ClntCfgAddrLst.getLast());
}   

//metoda dodaje 1 adres do kolejki ClntCfgAddrLst o w³aœciwoœciach opcji 
//znajduj¹cych siê na stosie ParsOptStack
void clntParser::EmptyAddr()
{
    ClntCfgAddrLst.append(new TClntCfgAddr());
    ClntCfgAddrLst.getLast()->setOptions(ParserOptStack.getLast());
}   

#line 1715 "C:\\Dyplom\\sources\\ClntParser\\ClntParser.cpp"
void YYPARSENAME::yytables()
{
	yyattribute_size = sizeof(YYSTYPE);
	yysstack_size = YYSTACK_SIZE;

#ifdef YYDEBUG
	static const yysymbol_t YYNEARFAR YYBASED_CODE symbol[] = {
		{ "$end", 0 },
		{ "\',\'", 44 },
		{ "\'{\'", 123 },
		{ "\'}\'", 125 },
		{ "error", 256 },
		{ "T1_", 257 },
		{ "T2_", 258 },
		{ "PREF_TIME_", 259 },
		{ "DNS_SERVER_", 260 },
		{ "VALID_TIME_", 261 },
		{ "NTP_SERVER_", 262 },
		{ "DOMAIN_", 263 },
		{ "TIME_ZONE_", 264 },
		{ "IFACE_", 265 },
		{ "NO_CONFIG_", 266 },
		{ "REJECT_SERVERS_", 267 },
		{ "PREFERRED_SERVERS_", 268 },
		{ "REQUIRE_", 269 },
		{ "REQUEST_", 270 },
		{ "SEND_", 271 },
		{ "DEFAULT_", 272 },
		{ "SUPERSEDE_", 273 },
		{ "APPEND_", 274 },
		{ "PREPEND_", 275 },
		{ "IA_", 276 },
		{ "ADDRES_", 277 },
		{ "IPV6ADDR_", 278 },
		{ "LOGLEVEL_", 279 },
		{ "WORKDIR_", 280 },
		{ "RAPID_COMMIT_", 281 },
		{ "NOIA_", 282 },
		{ "STRING_", 283 },
		{ "HEXNUMBER_", 284 },
		{ "INTNUMBER_", 285 },
		{ "DUID_", 286 },
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
		"InterfaceDeclaration: IFACE_ STRING_ \'{\' \'}\'",
		"InterfaceDeclaration: IFACE_ Number \'{\' \'}\'",
		"InterfaceDeclaration: IFACE_ STRING_ NO_CONFIG_",
		"InterfaceDeclaration: IFACE_ Number NO_CONFIG_",
		"InterfaceDeclarationsList: InterfaceOptionDeclaration",
		"InterfaceDeclarationsList: InterfaceDeclarationsList InterfaceOptionDeclaration",
		"InterfaceDeclarationsList: IADeclaration",
		"InterfaceDeclarationsList: InterfaceDeclarationsList IADeclaration",
		"$$3:",
		"IADeclaration: IA_ \'{\' $$3 IADeclarationList \'}\'",
		"$$4:",
		"IADeclaration: IA_ Number \'{\' $$4 IADeclarationList \'}\'",
		"IADeclaration: IA_ Number \'{\' \'}\'",
		"IADeclaration: IA_ Number",
		"IADeclaration: IA_ \'{\' \'}\'",
		"IADeclaration: IA_",
		"IADeclarationList: IAOptionDeclaration",
		"IADeclarationList: IADeclarationList IAOptionDeclaration",
		"IADeclarationList: ADDRESDeclaration",
		"IADeclarationList: IADeclarationList ADDRESDeclaration",
		"$$5:",
		"ADDRESDeclaration: ADDRES_ \'{\' $$5 ADDRESDeclarationList \'}\'",
		"$$6:",
		"ADDRESDeclaration: ADDRES_ Number \'{\' $$6 ADDRESDeclarationList \'}\'",
		"ADDRESDeclaration: ADDRES_ Number \'{\' \'}\'",
		"ADDRESDeclaration: ADDRES_ \'{\' \'}\'",
		"ADDRESDeclaration: ADDRES_ Number",
		"ADDRESDeclaration: ADDRES_",
		"ADDRESDeclarationList: ADDRESOptionDeclaration",
		"ADDRESDeclarationList: ADDRESDeclarationList ADDRESOptionDeclaration",
		"ADDRESDeclarationList: IPV6ADDR_",
		"ADDRESDeclarationList: ADDRESDeclarationList IPV6ADDR_",
		"GlobalOptionDeclaration: InterfaceOptionDeclaration",
		"GlobalOptionDeclaration: LogLevelOption",
		"GlobalOptionDeclaration: WorkDirOption",
		"InterfaceOptionDeclaration: IAOptionDeclaration",
		"InterfaceOptionDeclaration: DNSServerOption",
		"InterfaceOptionDeclaration: NTPServerOption",
		"InterfaceOptionDeclaration: NoIAsOptions",
		"InterfaceOptionDeclaration: DomainOption",
		"InterfaceOptionDeclaration: TimeZoneOption",
		"IAOptionDeclaration: T1Option",
		"IAOptionDeclaration: T2Option",
		"IAOptionDeclaration: RejectServersOption",
		"IAOptionDeclaration: PreferServersOption",
		"IAOptionDeclaration: RapidCommitOption",
		"IAOptionDeclaration: ADDRESOptionDeclaration",
		"ADDRESOptionDeclaration: PreferredTimeOption",
		"ADDRESOptionDeclaration: ValidTimeOption",
		"LogLevelOption: LOGLEVEL_ Number",
		"NoIAsOptions: NOIA_",
		"WorkDirOption: WORKDIR_ STRING_",
		"$$7:",
		"RejectServersOption: REJECT_SERVERS_ $$7 ADDRESDUIDList",
		"$$8:",
		"PreferServersOption: PREFERRED_SERVERS_ $$8 ADDRESDUIDList",
		"PreferredTimeOption: PREF_TIME_ Number",
		"PreferredTimeOption: PREF_TIME_ SendDefaultSupersedeOpt Number",
		"RapidCommitOption: RAPID_COMMIT_ Number",
		"ValidTimeOption: VALID_TIME_ Number",
		"ValidTimeOption: VALID_TIME_ SendDefaultSupersedeOpt Number",
		"T1Option: T1_ Number",
		"T1Option: T1_ SendDefaultSupersedeOpt Number",
		"T2Option: T2_ Number",
		"T2Option: T2_ SendDefaultSupersedeOpt Number",
		"DNSServerOption: DNS_SERVER_",
		"$$9:",
		"DNSServerOption: DNS_SERVER_ $$9 ADDRESSList",
		"$$10:",
		"DNSServerOption: DNS_SERVER_ SendDefaultSupersedeOpt $$10 ADDRESSList",
		"$$11:",
		"DNSServerOption: RequestRequirePrefix DNS_SERVER_ $$11 ADDRESSList",
		"$$12:",
		"DNSServerOption: RequestRequirePrefix DNS_SERVER_ SendDefaultSupersedeOpt $$12 ADDRESSList",
		"$$13:",
		"DNSServerOption: DNS_SERVER_ SuperAppPrepOpt $$13 ADDRESSList",
		"DomainOption: DOMAIN_",
		"DomainOption: DOMAIN_ STRING_",
		"DomainOption: DOMAIN_ SendDefaultSupersedeOpt STRING_",
		"DomainOption: RequestRequirePrefix DOMAIN_ STRING_",
		"DomainOption: RequestRequirePrefix DOMAIN_ SendDefaultSupersedeOpt STRING_",
		"TimeZoneOption: TIME_ZONE_",
		"TimeZoneOption: TIME_ZONE_ STRING_",
		"TimeZoneOption: TIME_ZONE_ SendDefaultSupersedeOpt STRING_",
		"TimeZoneOption: RequestRequirePrefix TIME_ZONE_ STRING_",
		"TimeZoneOption: RequestRequirePrefix TIME_ZONE_ SendDefaultSupersedeOpt STRING_",
		"NTPServerOption: NTP_SERVER_",
		"$$14:",
		"NTPServerOption: NTP_SERVER_ $$14 ADDRESSList",
		"$$15:",
		"NTPServerOption: NTP_SERVER_ SendDefaultSupersedeOpt $$15 ADDRESSList",
		"$$16:",
		"NTPServerOption: RequestRequirePrefix NTP_SERVER_ $$16 ADDRESSList",
		"$$17:",
		"NTPServerOption: RequestRequirePrefix NTP_SERVER_ SendDefaultSupersedeOpt $$17 ADDRESSList",
		"$$18:",
		"NTPServerOption: NTP_SERVER_ SuperAppPrepOpt $$18 ADDRESSList",
		"ADDRESDUIDList: IPV6ADDR_",
		"ADDRESDUIDList: DUID_",
		"ADDRESDUIDList: ADDRESDUIDList \',\' IPV6ADDR_",
		"ADDRESDUIDList: ADDRESDUIDList \',\' DUID_",
		"ADDRESSList: IPV6ADDR_",
		"ADDRESSList: ADDRESSList \',\' IPV6ADDR_",
		"SendDefaultSupersedeOpt: SEND_",
		"SendDefaultSupersedeOpt: DEFAULT_",
		"SendDefaultSupersedeOpt: SUPERSEDE_",
		"RequestRequirePrefix: REQUEST_",
		"RequestRequirePrefix: REQUIRE_",
		"SuperAppPrepOpt: APPEND_",
		"SuperAppPrepOpt: PREPEND_",
		"Number: HEXNUMBER_",
		"Number: INTNUMBER_"
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
		{ 3, 4, 4 },
		{ 3, 4, 5 },
		{ 3, 3, 6 },
		{ 3, 3, 7 },
		{ 6, 1, -1 },
		{ 6, 2, -1 },
		{ 6, 1, -1 },
		{ 6, 2, -1 },
		{ 8, 0, 8 },
		{ 7, 5, 9 },
		{ 9, 0, 10 },
		{ 7, 6, 11 },
		{ 7, 4, 12 },
		{ 7, 2, 13 },
		{ 7, 3, 14 },
		{ 7, 1, 15 },
		{ 10, 1, -1 },
		{ 10, 2, -1 },
		{ 10, 1, -1 },
		{ 10, 2, -1 },
		{ 12, 0, 16 },
		{ 11, 5, 17 },
		{ 13, 0, 18 },
		{ 11, 6, 19 },
		{ 11, 4, 20 },
		{ 11, 3, 21 },
		{ 11, 2, 22 },
		{ 11, 1, 23 },
		{ 14, 1, -1 },
		{ 14, 2, -1 },
		{ 14, 1, 24 },
		{ 14, 2, 25 },
		{ 15, 1, -1 },
		{ 15, 1, -1 },
		{ 15, 1, -1 },
		{ 16, 1, -1 },
		{ 16, 1, -1 },
		{ 16, 1, -1 },
		{ 16, 1, -1 },
		{ 16, 1, -1 },
		{ 16, 1, -1 },
		{ 17, 1, -1 },
		{ 17, 1, -1 },
		{ 17, 1, -1 },
		{ 17, 1, -1 },
		{ 17, 1, -1 },
		{ 17, 1, -1 },
		{ 18, 1, -1 },
		{ 18, 1, -1 },
		{ 19, 2, 26 },
		{ 20, 1, 27 },
		{ 21, 2, 28 },
		{ 23, 0, 29 },
		{ 22, 3, 30 },
		{ 25, 0, 31 },
		{ 24, 3, 32 },
		{ 26, 2, 33 },
		{ 26, 3, 34 },
		{ 27, 2, 35 },
		{ 28, 2, 36 },
		{ 28, 3, 37 },
		{ 29, 2, 38 },
		{ 29, 3, 39 },
		{ 30, 2, 40 },
		{ 30, 3, 41 },
		{ 31, 1, 42 },
		{ 32, 0, 43 },
		{ 31, 3, 44 },
		{ 33, 0, 45 },
		{ 31, 4, 46 },
		{ 34, 0, 47 },
		{ 31, 4, 48 },
		{ 35, 0, 49 },
		{ 31, 5, 50 },
		{ 36, 0, 51 },
		{ 31, 4, 52 },
		{ 37, 1, 53 },
		{ 37, 2, 54 },
		{ 37, 3, 55 },
		{ 37, 3, 56 },
		{ 37, 4, 57 },
		{ 38, 1, 58 },
		{ 38, 2, 59 },
		{ 38, 3, 60 },
		{ 38, 3, 61 },
		{ 38, 4, 62 },
		{ 39, 1, 63 },
		{ 40, 0, 64 },
		{ 39, 3, 65 },
		{ 41, 0, 66 },
		{ 39, 4, 67 },
		{ 42, 0, 68 },
		{ 39, 4, 69 },
		{ 43, 0, 70 },
		{ 39, 5, 71 },
		{ 44, 0, 72 },
		{ 39, 4, 73 },
		{ 45, 1, 74 },
		{ 45, 1, 75 },
		{ 45, 3, 76 },
		{ 45, 3, 77 },
		{ 46, 1, 78 },
		{ 46, 3, 79 },
		{ 47, 1, 80 },
		{ 47, 1, 81 },
		{ 47, 1, 82 },
		{ 48, 1, 83 },
		{ 48, 1, 84 },
		{ 49, 1, 85 },
		{ 49, 1, 86 },
		{ 50, 1, 87 },
		{ 50, 1, 88 }
	};
	yyreduction = reduction;

	static const yytokenaction_t YYNEARFAR YYBASED_CODE tokenaction[] = {
		{ 128, YYAT_SHIFT, 138 },
		{ 20, YYAT_SHIFT, 1 },
		{ 20, YYAT_SHIFT, 2 },
		{ 20, YYAT_SHIFT, 3 },
		{ 20, YYAT_SHIFT, 4 },
		{ 20, YYAT_SHIFT, 5 },
		{ 20, YYAT_SHIFT, 6 },
		{ 20, YYAT_SHIFT, 7 },
		{ 20, YYAT_SHIFT, 8 },
		{ 20, YYAT_SHIFT, 9 },
		{ 162, YYAT_ERROR, 0 },
		{ 20, YYAT_SHIFT, 10 },
		{ 20, YYAT_SHIFT, 11 },
		{ 20, YYAT_SHIFT, 12 },
		{ 20, YYAT_SHIFT, 13 },
		{ 142, YYAT_SHIFT, 148 },
		{ 153, YYAT_SHIFT, 157 },
		{ 124, YYAT_SHIFT, 133 },
		{ 66, YYAT_SHIFT, 93 },
		{ 65, YYAT_SHIFT, 91 },
		{ 166, YYAT_SHIFT, 167 },
		{ 57, YYAT_SHIFT, 43 },
		{ 57, YYAT_SHIFT, 44 },
		{ 20, YYAT_SHIFT, 14 },
		{ 20, YYAT_SHIFT, 15 },
		{ 20, YYAT_SHIFT, 16 },
		{ 20, YYAT_SHIFT, 17 },
		{ 0, YYAT_SHIFT, 1 },
		{ 0, YYAT_SHIFT, 2 },
		{ 0, YYAT_SHIFT, 3 },
		{ 0, YYAT_SHIFT, 4 },
		{ 0, YYAT_SHIFT, 5 },
		{ 0, YYAT_SHIFT, 6 },
		{ 0, YYAT_SHIFT, 7 },
		{ 0, YYAT_SHIFT, 8 },
		{ 0, YYAT_SHIFT, 9 },
		{ 159, YYAT_SHIFT, 163 },
		{ 0, YYAT_SHIFT, 10 },
		{ 0, YYAT_SHIFT, 11 },
		{ 0, YYAT_SHIFT, 12 },
		{ 0, YYAT_SHIFT, 13 },
		{ 6, YYAT_SHIFT, 40 },
		{ 6, YYAT_SHIFT, 41 },
		{ 6, YYAT_SHIFT, 42 },
		{ 6, YYAT_SHIFT, 51 },
		{ 6, YYAT_SHIFT, 52 },
		{ 156, YYAT_SHIFT, 161 },
		{ 149, YYAT_SHIFT, 156 },
		{ 6, YYAT_REDUCE, 98 },
		{ 0, YYAT_SHIFT, 14 },
		{ 0, YYAT_SHIFT, 15 },
		{ 0, YYAT_SHIFT, 16 },
		{ 0, YYAT_SHIFT, 17 },
		{ 4, YYAT_SHIFT, 40 },
		{ 4, YYAT_SHIFT, 41 },
		{ 4, YYAT_SHIFT, 42 },
		{ 4, YYAT_SHIFT, 51 },
		{ 4, YYAT_SHIFT, 52 },
		{ 148, YYAT_SHIFT, 154 },
		{ 147, YYAT_ERROR, 0 },
		{ 4, YYAT_REDUCE, 77 },
		{ 75, YYAT_SHIFT, 40 },
		{ 75, YYAT_SHIFT, 41 },
		{ 75, YYAT_SHIFT, 42 },
		{ 8, YYAT_SHIFT, 40 },
		{ 8, YYAT_SHIFT, 41 },
		{ 8, YYAT_SHIFT, 42 },
		{ 7, YYAT_SHIFT, 40 },
		{ 7, YYAT_SHIFT, 41 },
		{ 7, YYAT_SHIFT, 42 },
		{ 73, YYAT_SHIFT, 40 },
		{ 73, YYAT_SHIFT, 41 },
		{ 73, YYAT_SHIFT, 42 },
		{ 75, YYAT_SHIFT, 105 },
		{ 143, YYAT_SHIFT, 150 },
		{ 116, YYAT_SHIFT, 129 },
		{ 8, YYAT_SHIFT, 63 },
		{ 68, YYAT_SHIFT, 95 },
		{ 18, YYAT_SHIFT, 72 },
		{ 7, YYAT_SHIFT, 61 },
		{ 18, YYAT_SHIFT, 73 },
		{ 18, YYAT_SHIFT, 74 },
		{ 18, YYAT_SHIFT, 75 },
		{ 116, YYAT_SHIFT, 130 },
		{ 141, YYAT_SHIFT, 146 },
		{ 68, YYAT_SHIFT, 96 },
		{ 72, YYAT_SHIFT, 40 },
		{ 72, YYAT_SHIFT, 41 },
		{ 72, YYAT_SHIFT, 42 },
		{ 5, YYAT_ERROR, 0 },
		{ 5, YYAT_SHIFT, 43 },
		{ 5, YYAT_SHIFT, 44 },
		{ 134, YYAT_SHIFT, 141 },
		{ 133, YYAT_SHIFT, 139 },
		{ 132, YYAT_SHIFT, 109 },
		{ 131, YYAT_SHIFT, 109 },
		{ 125, YYAT_SHIFT, 135 },
		{ 120, YYAT_SHIFT, 109 },
		{ 119, YYAT_SHIFT, 83 },
		{ 118, YYAT_SHIFT, 109 },
		{ 115, YYAT_ERROR, 0 },
		{ 111, YYAT_SHIFT, 109 },
		{ 110, YYAT_SHIFT, 109 },
		{ 109, YYAT_SHIFT, 123 },
		{ 108, YYAT_SHIFT, 109 },
		{ 107, YYAT_SHIFT, 109 },
		{ 106, YYAT_SHIFT, 122 },
		{ 104, YYAT_SHIFT, 121 },
		{ 98, YYAT_SHIFT, 116 },
		{ 97, YYAT_SHIFT, 116 },
		{ 93, YYAT_SHIFT, 114 },
		{ 91, YYAT_SHIFT, 112 },
		{ 88, YYAT_SHIFT, 109 },
		{ 84, YYAT_SHIFT, 109 },
		{ 74, YYAT_SHIFT, 103 },
		{ 64, YYAT_SHIFT, 90 },
		{ 62, YYAT_SHIFT, 89 },
		{ 19, YYAT_ACCEPT, 0 },
		{ 15, YYAT_SHIFT, 70 },
		{ 9, YYAT_SHIFT, 65 },
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
		{ 128, YYAT_SHIFT, 4 },
		{ -1, YYAT_ERROR, 0 },
		{ 128, YYAT_SHIFT, 6 },
		{ 128, YYAT_SHIFT, 7 },
		{ 128, YYAT_SHIFT, 8 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 128, YYAT_SHIFT, 12 },
		{ 128, YYAT_SHIFT, 13 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 153, YYAT_SHIFT, 1 },
		{ 153, YYAT_SHIFT, 2 },
		{ -1, YYAT_ERROR, 0 },
		{ 128, YYAT_SHIFT, 124 },
		{ 128, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 166, YYAT_SHIFT, 3 },
		{ -1, YYAT_ERROR, 0 },
		{ 166, YYAT_SHIFT, 5 },
		{ 128, YYAT_SHIFT, 17 },
		{ 153, YYAT_SHIFT, 10 },
		{ 153, YYAT_SHIFT, 11 },
		{ -1, YYAT_ERROR, 0 },
		{ 66, YYAT_SHIFT, 94 },
		{ 65, YYAT_SHIFT, 92 },
		{ 162, YYAT_SHIFT, 158 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 153, YYAT_SHIFT, 142 },
		{ 153, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 153, YYAT_SHIFT, 16 },
		{ 166, YYAT_SHIFT, 164 },
		{ -1, YYAT_ERROR, 0 },
		{ -1, YYAT_ERROR, 0 },
		{ 142, YYAT_SHIFT, 43 },
		{ 142, YYAT_SHIFT, 44 },
		{ 124, YYAT_SHIFT, 43 },
		{ 124, YYAT_SHIFT, 44 }
	};
	yytokenaction = tokenaction;
	yytokenaction_size = 180;

	static const yystateaction_t YYNEARFAR YYBASED_CODE stateaction[] = {
		{ -230, 1, YYAT_REDUCE, 2 },
		{ 0, 0, YYAT_DEFAULT, 5 },
		{ 0, 0, YYAT_DEFAULT, 5 },
		{ 0, 0, YYAT_DEFAULT, 5 },
		{ -218, 1, YYAT_REDUCE, 76 },
		{ -194, 1, YYAT_DEFAULT, 75 },
		{ -230, 1, YYAT_REDUCE, 97 },
		{ -204, 1, YYAT_REDUCE, 87 },
		{ -207, 1, YYAT_REDUCE, 92 },
		{ -164, 1, YYAT_DEFAULT, 57 },
		{ 0, 0, YYAT_REDUCE, 63 },
		{ 0, 0, YYAT_REDUCE, 65 },
		{ 0, 0, YYAT_REDUCE, 118 },
		{ 0, 0, YYAT_REDUCE, 117 },
		{ 0, 0, YYAT_DEFAULT, 57 },
		{ -165, 1, YYAT_DEFAULT, 106 },
		{ 0, 0, YYAT_DEFAULT, 57 },
		{ 0, 0, YYAT_REDUCE, 61 },
		{ -182, 1, YYAT_ERROR, 0 },
		{ 117, 1, YYAT_ERROR, 0 },
		{ -256, 1, YYAT_REDUCE, 1 },
		{ 0, 0, YYAT_REDUCE, 3 },
		{ 0, 0, YYAT_REDUCE, 4 },
		{ 0, 0, YYAT_REDUCE, 43 },
		{ 0, 0, YYAT_REDUCE, 46 },
		{ 0, 0, YYAT_REDUCE, 57 },
		{ 0, 0, YYAT_REDUCE, 44 },
		{ 0, 0, YYAT_REDUCE, 45 },
		{ 0, 0, YYAT_REDUCE, 47 },
		{ 0, 0, YYAT_REDUCE, 48 },
		{ 0, 0, YYAT_REDUCE, 49 },
		{ 0, 0, YYAT_REDUCE, 50 },
		{ 0, 0, YYAT_REDUCE, 51 },
		{ 0, 0, YYAT_REDUCE, 52 },
		{ 0, 0, YYAT_REDUCE, 53 },
		{ 0, 0, YYAT_REDUCE, 54 },
		{ 0, 0, YYAT_REDUCE, 55 },
		{ 0, 0, YYAT_REDUCE, 56 },
		{ 0, 0, YYAT_REDUCE, 58 },
		{ 0, 0, YYAT_REDUCE, 59 },
		{ 0, 0, YYAT_REDUCE, 114 },
		{ 0, 0, YYAT_REDUCE, 115 },
		{ 0, 0, YYAT_REDUCE, 116 },
		{ 0, 0, YYAT_REDUCE, 121 },
		{ 0, 0, YYAT_REDUCE, 122 },
		{ 0, 0, YYAT_REDUCE, 72 },
		{ 0, 0, YYAT_DEFAULT, 57 },
		{ 0, 0, YYAT_REDUCE, 74 },
		{ 0, 0, YYAT_DEFAULT, 57 },
		{ 0, 0, YYAT_REDUCE, 67 },
		{ 0, 0, YYAT_DEFAULT, 57 },
		{ 0, 0, YYAT_REDUCE, 119 },
		{ 0, 0, YYAT_REDUCE, 120 },
		{ 0, 0, YYAT_REDUCE, 79 },
		{ 0, 0, YYAT_REDUCE, 85 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 70 },
		{ -263, 1, YYAT_ERROR, 0 },
		{ 0, 0, YYAT_REDUCE, 100 },
		{ 0, 0, YYAT_REDUCE, 106 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 88 },
		{ -167, 1, YYAT_DEFAULT, 106 },
		{ 0, 0, YYAT_REDUCE, 93 },
		{ -168, 1, YYAT_DEFAULT, 106 },
		{ -104, 1, YYAT_DEFAULT, 66 },
		{ -105, 1, YYAT_ERROR, 0 },
		{ 0, 0, YYAT_DEFAULT, 68 },
		{ -201, 1, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 60 },
		{ 0, 0, YYAT_REDUCE, 62 },
		{ 0, 0, YYAT_REDUCE, 69 },
		{ -185, 1, YYAT_REDUCE, 81 },
		{ -201, 1, YYAT_REDUCE, 102 },
		{ -169, 1, YYAT_DEFAULT, 75 },
		{ -210, 1, YYAT_DEFAULT, 106 },
		{ 0, 0, YYAT_REDUCE, 5 },
		{ 0, 0, YYAT_REDUCE, 6 },
		{ 0, 0, YYAT_REDUCE, 73 },
		{ 0, 0, YYAT_REDUCE, 75 },
		{ 0, 0, YYAT_REDUCE, 68 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 112 },
		{ 69, 1, YYAT_REDUCE, 78 },
		{ 0, 0, YYAT_REDUCE, 71 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 68, 1, YYAT_REDUCE, 99 },
		{ 0, 0, YYAT_REDUCE, 89 },
		{ 0, 0, YYAT_REDUCE, 94 },
		{ -14, 1, YYAT_REDUCE, 7 },
		{ 0, 0, YYAT_REDUCE, 13 },
		{ -15, 1, YYAT_REDUCE, 9 },
		{ 0, 0, YYAT_REDUCE, 14 },
		{ 0, 0, YYAT_REDUCE, 108 },
		{ 0, 0, YYAT_REDUCE, 109 },
		{ 65, 1, YYAT_REDUCE, 64 },
		{ 64, 1, YYAT_REDUCE, 66 },
		{ 0, 0, YYAT_REDUCE, 83 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 104 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 90 },
		{ -176, 1, YYAT_DEFAULT, 106 },
		{ 0, 0, YYAT_REDUCE, 95 },
		{ -177, 1, YYAT_ERROR, 0 },
		{ 61, 1, YYAT_REDUCE, 80 },
		{ 60, 1, YYAT_REDUCE, 86 },
		{ -175, 1, YYAT_DEFAULT, 119 },
		{ 58, 1, YYAT_REDUCE, 101 },
		{ 57, 1, YYAT_REDUCE, 107 },
		{ 0, 0, YYAT_REDUCE, 11 },
		{ 0, 0, YYAT_DEFAULT, 115 },
		{ 0, 0, YYAT_REDUCE, 12 },
		{ -25, 1, YYAT_DEFAULT, 128 },
		{ -203, 1, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_DEFAULT, 119 },
		{ 55, 1, YYAT_REDUCE, 82 },
		{ -180, 1, YYAT_ERROR, 0 },
		{ 53, 1, YYAT_REDUCE, 103 },
		{ 0, 0, YYAT_REDUCE, 91 },
		{ 0, 0, YYAT_REDUCE, 96 },
		{ 0, 0, YYAT_REDUCE, 113 },
		{ -106, 1, YYAT_REDUCE, 26 },
		{ -29, 1, YYAT_DEFAULT, 128 },
		{ 0, 0, YYAT_REDUCE, 15 },
		{ 0, 0, YYAT_REDUCE, 17 },
		{ -125, 1, YYAT_DEFAULT, 153 },
		{ 0, 0, YYAT_REDUCE, 110 },
		{ 0, 0, YYAT_REDUCE, 111 },
		{ 51, 1, YYAT_REDUCE, 84 },
		{ 50, 1, YYAT_REDUCE, 105 },
		{ -32, 1, YYAT_REDUCE, 19 },
		{ -31, 1, YYAT_REDUCE, 24 },
		{ 0, 0, YYAT_REDUCE, 8 },
		{ 0, 0, YYAT_REDUCE, 16 },
		{ 0, 0, YYAT_REDUCE, 18 },
		{ 0, 0, YYAT_REDUCE, 10 },
		{ 0, 0, YYAT_REDUCE, 25 },
		{ 0, 0, YYAT_DEFAULT, 147 },
		{ -41, 1, YYAT_REDUCE, 21 },
		{ -108, 1, YYAT_REDUCE, 38 },
		{ -51, 1, YYAT_DEFAULT, 153 },
		{ 0, 0, YYAT_REDUCE, 27 },
		{ 0, 0, YYAT_REDUCE, 29 },
		{ 0, 0, YYAT_REDUCE, 23 },
		{ -66, 1, YYAT_DEFAULT, 153 },
		{ -67, 1, YYAT_REDUCE, 31 },
		{ -76, 1, YYAT_REDUCE, 37 },
		{ 0, 0, YYAT_REDUCE, 20 },
		{ 0, 0, YYAT_REDUCE, 28 },
		{ 0, 0, YYAT_REDUCE, 30 },
		{ -109, 1, YYAT_DEFAULT, 166 },
		{ 0, 0, YYAT_REDUCE, 36 },
		{ 0, 0, YYAT_DEFAULT, 162 },
		{ -79, 1, YYAT_REDUCE, 33 },
		{ 0, 0, YYAT_REDUCE, 22 },
		{ 0, 0, YYAT_REDUCE, 41 },
		{ -89, 1, YYAT_DEFAULT, 166 },
		{ 0, 0, YYAT_REDUCE, 39 },
		{ 0, 0, YYAT_REDUCE, 35 },
		{ -115, 1, YYAT_DEFAULT, 166 },
		{ 0, 0, YYAT_REDUCE, 32 },
		{ 0, 0, YYAT_REDUCE, 42 },
		{ 0, 0, YYAT_REDUCE, 40 },
		{ -105, 1, YYAT_DEFAULT, 119 },
		{ 0, 0, YYAT_REDUCE, 34 }
	};
	yystateaction = stateaction;

	static const yynontermgoto_t YYNEARFAR YYBASED_CODE nontermgoto[] = {
		{ 128, 137 },
		{ 153, 152 },
		{ 72, 100 },
		{ 156, 162 },
		{ 128, -1 },
		{ 155, 159 },
		{ 73, 102 },
		{ 153, 151 },
		{ 153, 25 },
		{ 128, 136 },
		{ 128, 24 },
		{ 73, 101 },
		{ 153, 35 },
		{ 128, 30 },
		{ 153, 36 },
		{ 72, 99 },
		{ 148, 155 },
		{ 153, 37 },
		{ 162, 166 },
		{ 153, 33 },
		{ 153, 34 },
		{ 20, 77 },
		{ 162, 160 },
		{ 142, 149 },
		{ 128, 28 },
		{ 20, -1 },
		{ 0, 19 },
		{ 0, 20 },
		{ 0, 22 },
		{ 4, 55 },
		{ 128, 31 },
		{ 128, 32 },
		{ 128, 29 },
		{ 20, 76 },
		{ 20, 23 },
		{ 166, 165 },
		{ 141, 147 },
		{ 20, 26 },
		{ 140, 143 },
		{ 20, 27 },
		{ 0, 21 },
		{ 128, 18 },
		{ 6, 60 },
		{ 166, 38 },
		{ 4, 53 },
		{ 166, 39 },
		{ 4, 54 },
		{ 115, 128 },
		{ 115, 127 },
		{ 6, 58 },
		{ 133, 140 },
		{ 6, 59 },
		{ 147, 153 },
		{ 147, 145 },
		{ 124, 134 },
		{ 5, 57 },
		{ 119, 132 },
		{ 115, 126 },
		{ 5, 56 },
		{ 147, 144 },
		{ 3, 50 },
		{ 2, 48 },
		{ 1, 46 },
		{ 3, 49 },
		{ 2, 47 },
		{ 1, 45 },
		{ 117, 131 },
		{ 113, 125 },
		{ 102, 120 },
		{ 101, 119 },
		{ 100, 118 },
		{ 99, 117 },
		{ 93, 115 },
		{ 91, 113 },
		{ 87, 111 },
		{ 86, 110 },
		{ 82, 108 },
		{ 81, 107 },
		{ 75, 106 },
		{ 74, 104 },
		{ 68, 98 },
		{ 67, 97 },
		{ 60, 88 },
		{ 59, 87 },
		{ 58, 86 },
		{ 57, 85 },
		{ 55, 84 },
		{ 54, 82 },
		{ 53, 81 },
		{ 50, 80 },
		{ 48, 79 },
		{ 46, 78 },
		{ 16, 71 },
		{ 14, 69 },
		{ 11, 68 },
		{ 10, 67 },
		{ 9, 66 },
		{ 8, 64 },
		{ 7, 62 }
	};
	yynontermgoto = nontermgoto;
	yynontermgoto_size = 99;

	static const yystategoto_t YYNEARFAR YYBASED_CODE stategoto[] = {
		{ 25, 20 },
		{ 15, -1 },
		{ 14, -1 },
		{ 13, -1 },
		{ -3, -1 },
		{ 8, -1 },
		{ 2, -1 },
		{ 51, -1 },
		{ 50, -1 },
		{ 46, -1 },
		{ 72, -1 },
		{ 69, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 43, -1 },
		{ 0, -1 },
		{ 42, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 18, 128 },
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
		{ 41, -1 },
		{ 0, -1 },
		{ 40, -1 },
		{ 0, -1 },
		{ 39, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 55, -1 },
		{ 51, -1 },
		{ 40, -1 },
		{ 0, -1 },
		{ 35, -1 },
		{ 43, -1 },
		{ 39, -1 },
		{ 36, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 36, -1 },
		{ 35, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ -32, -1 },
		{ -36, -1 },
		{ 32, -1 },
		{ 31, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 31, -1 },
		{ 30, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 29, -1 },
		{ 28, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 69, -1 },
		{ 0, -1 },
		{ 67, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 36, -1 },
		{ 24, -1 },
		{ 26, -1 },
		{ 22, -1 },
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
		{ 61, 115 },
		{ 0, -1 },
		{ 41, 128 },
		{ 0, -1 },
		{ 20, -1 },
		{ 0, -1 },
		{ 10, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 4, -1 },
		{ 0, 128 },
		{ 0, -1 },
		{ 0, -1 },
		{ -7, 153 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 42, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 28, 147 },
		{ 27, -1 },
		{ -27, -1 },
		{ 0, 153 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 42, 153 },
		{ 4, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ -10, 166 },
		{ 0, -1 },
		{ -9, 162 },
		{ -10, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, 166 },
		{ 0, -1 },
		{ 0, -1 },
		{ 4, 166 },
		{ 0, -1 },
		{ 0, -1 },
		{ 0, -1 },
		{ 17, -1 },
		{ 0, -1 }
	};
	yystategoto = stategoto;

	yydestructorptr = NULL;

	yytokendestptr = NULL;
	yytokendest_size = 0;
	yytokendestbase = 0;
}
