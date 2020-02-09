
#include "Logger.h"
#include "SrvParserContext.h"
#include <string>

using namespace std;

SrvParserContext::SrvParserContext() {
    ParserOptStack_.append(new TSrvParsGlobalOpt());
}

/////////////////////////////////////////////////////////////////////////////
// programs section

/**
 * method check whether interface with id=ifaceNr has been already declared
 *
 * @param ifaceNr
 *
 * @return true if interface was not declared
 */
bool SrvParserContext::IfaceDefined(int ifaceNr)
{
  SPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst_.first();
  while (ptr = SrvCfgIfaceLst_.get())
    if (ptr->getID() == ifaceNr) {
	Log(Crit) << "Interface with ID=" << ifaceNr << " is already defined." << LogEnd;
	return false;
    }
  return true;
}

/**
 * check whether interface with id=ifaceName has been already declared
 *
 * @param ifaceName
 *
 * @return true, if defined, false otherwise
 */
bool SrvParserContext::IfaceDefined(string ifaceName)
{
  SPtr<TSrvCfgIface> ptr;
  SrvCfgIfaceLst_.first();
  while (ptr=SrvCfgIfaceLst_.get())
  {
    string presName=ptr->getName();
    if (presName==ifaceName) {
	Log(Crit) << "Interface " << ifaceName << " is already defined." << LogEnd;
	return false;
    }
  }
  return true;
}

/**
 * method creates new option for just started interface scope
 * clears all lists except the list of interfaces and adds new group
 *
 */
bool SrvParserContext::StartIfaceDeclaration(string ifaceName)
{
    if (!IfaceDefined(ifaceName))
	return false;

    SrvCfgIfaceLst_.append(new TSrvCfgIface(ifaceName));

    // create new option (representing this interface) on the parser stack
    ParserOptStack_.append(new TSrvParsGlobalOpt(*ParserOptStack_.getLast()));
    SrvCfgAddrClassLst_.clear();
    ClientLst_.clear();

    return true;
}

/**
 * method creates new option for just started interface scope
 * clears all lists except the list of interfaces and adds new group
 *
 */
bool SrvParserContext::StartIfaceDeclaration(int ifindex)
{
    if (!IfaceDefined(ifindex))
	return false;

    SrvCfgIfaceLst_.append(new TSrvCfgIface(ifindex));

    // create new option (representing this interface) on the parser stack
    ParserOptStack_.append(new TSrvParsGlobalOpt(*ParserOptStack_.getLast()));
    SrvCfgAddrClassLst_.clear();
    ClientLst_.clear();

    return true;
}


/**
 * this method is called after inteface declaration has ended. It creates
 * new interface representation used in SrvCfgMgr_. Also removes corresponding
 * element from the parser stack
 *
 * @return true if everything is ok
 */
bool SrvParserContext::EndIfaceDeclaration()
{
    // get this interface object
    SPtr<TSrvCfgIface> iface = SrvCfgIfaceLst_.getLast();

    // set its options
    SrvCfgIfaceLst_.getLast()->setOptions(ParserOptStack_.getLast());

    // copy all IA objects
    SPtr<TSrvCfgAddrClass> ptrAddrClass;
    SrvCfgAddrClassLst_.first();
    while (ptrAddrClass=SrvCfgAddrClassLst_.get())
	iface->addAddrClass(ptrAddrClass);
    SrvCfgAddrClassLst_.clear();

    // copy all TA objects
    SPtr<TSrvCfgTA> ta;
    SrvCfgTALst_.first();
    while (ta=SrvCfgTALst_.get())
	iface->addTA(ta);
    SrvCfgTALst_.clear();

    SPtr<TSrvCfgPD> pd;
    SrvCfgPDLst_.first();
    while (pd=SrvCfgPDLst_.get())
	iface->addPD(pd);
    SrvCfgPDLst_.clear();

    iface->addClientExceptionsLst(ClientLst_);

    // remove last option (representing this interface) from the parser stack
    ParserOptStack_.delLast();

    return true;
}

void SrvParserContext::StartClassDeclaration()
{
    ParserOptStack_.append(new TSrvParsGlobalOpt(*ParserOptStack_.getLast()));
    SrvCfgAddrClassLst_.append(new TSrvCfgAddrClass());
}

/**
 * this method is adds new object representig just parsed IA class.
 *
 * @return true if everything works ok.
 */
bool SrvParserContext::EndClassDeclaration()
{
    if (!ParserOptStack_.getLast()->countPool()) {
	Log(Crit) << "No pools defined for this class." << LogEnd;
	return false;
    }
    //setting interface options on the basis of just read information
    SrvCfgAddrClassLst_.getLast()->setOptions(ParserOptStack_.getLast());
    ParserOptStack_.delLast();

    return true;
}


/**
 * Just add global options
 *
 */
void SrvParserContext::StartTAClassDeclaration()
{
  ParserOptStack_.append(new TSrvParsGlobalOpt(*ParserOptStack_.getLast()));
}

bool SrvParserContext::EndTAClassDeclaration()
{
    if (!ParserOptStack_.getLast()->countPool()) {
	Log(Crit) << "No pools defined for this ta-class." << LogEnd;
	return false;
    }
    // create new object representing just parsed TA and add it to the list
    SPtr<TSrvCfgTA> ptrTA = new TSrvCfgTA();
    ptrTA->setOptions(ParserOptStack_.getLast());
    SrvCfgTALst_.append(ptrTA);

    // remove temporary parser object for this (just finished) scope
    ParserOptStack_.delLast();
    return true;
}

void SrvParserContext::StartPDDeclaration()
{
    ParserOptStack_.append(new TSrvParsGlobalOpt(*ParserOptStack_.getLast()));
    PDLst_.clear();
    PDPrefix_ = 0;
}

bool SrvParserContext::EndPDDeclaration()
{
    if (!PDLst_.count()) {
	Log(Crit) << "No PD pools defined ." << LogEnd;
	return false;
    }
    if (!PDPrefix_) {
	Log(Crit) << "PD prefix length not defined or set to 0." << LogEnd;
	return false;
    }

    int len = 0;
    PDLst_.first();
    while ( SPtr<THostRange> pool = PDLst_.get() ) {
	if (!len)
	    len = pool->getPrefixLength();
	if (len!=pool->getPrefixLength()) {
	    Log(Crit) << "Prefix pools with different lengths are not supported. "
                "Make sure that all 'pd-pool' uses the same prefix length." << LogEnd;
	    return false;
	}
    }
    if (len > PDPrefix_) {
	Log(Crit) << "Clients are supposed to get /" << PDPrefix_ << " prefixes,"
                  << "but pd-pool(s) are only /" << len << " long." << LogEnd;
	return false;
    }
    if (len == PDPrefix_) {
	Log(Warning) << "Prefix pool /" << PDPrefix_ << " defined and clients are "
            "supposed to get /" << len << " prefixes. Only ONE client will get "
            "prefix" << LogEnd;
    }

    SPtr<TSrvCfgPD> ptrPD = new TSrvCfgPD();
    ParserOptStack_.getLast()->setPool(&PDLst_);
    if (!ptrPD->setOptions(ParserOptStack_.getLast(), PDPrefix_))
	return false;
    SrvCfgPDLst_.append(ptrPD);

    // remove temporary parser object for this (just finished) scope
    ParserOptStack_.delLast();
    return true;
}

//namespace std {
//    extern yy_SrvParser_stype yylval;
//}

//int SrvParserContext::yylex()
//{
//    memset(&std::yylval,0, sizeof(std::yylval));
//    memset(&this->yylval,0, sizeof(this->yylval));
//    int x = this->lex->yylex();
//    this->yylval=std::yylval;
//    return x;
//}

//void SrvParserContext::yyerror(char *m)
//{
//    Log(Crit) << "Config parse error: line " << lex->lineno()
//	      << ", unexpected [" << lex->YYText() << "] token." << LogEnd;
//}

SrvParserContext::~SrvParserContext() {
    ParserOptStack_.clear();
    SrvCfgIfaceLst_.clear();
    SrvCfgAddrClassLst_.clear();
    SrvCfgTALst_.clear();
    PresentAddrLst_.clear();
    PresentStringLst_.clear();
    PresentRangeLst_.clear();
}

static char bitMask[]= { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

SPtr<TIPv6Addr> SrvParserContext::getRangeMin(const char * addrPacked, int prefix) {
    char packed[16];
    char mask;
    memcpy(packed, addrPacked, 16);
    if (prefix%8!=0) {
	mask = bitMask[prefix%8];
	packed[prefix/8] = packed[prefix/8] & mask;
	prefix = (prefix/8 + 1)*8;
    }
    for (int i=prefix/8;i<16; i++) {
	packed[i]=0;
    }
    return new TIPv6Addr(packed, false);
}

SPtr<TIPv6Addr> SrvParserContext::getRangeMax(const char * addrPacked, int prefix){
    char packed[16];
    char mask;
    memcpy(packed, addrPacked,16);
    if (prefix%8!=0) {
	mask = bitMask[prefix%8];
	packed[prefix/8] = packed[prefix/8] | ~mask;
	prefix = (prefix/8 + 1)*8;
    }
    for (int i=prefix/8;i<16; i++) {
	packed[i]=0xff;
    }

    return new TIPv6Addr(packed, false);
}
