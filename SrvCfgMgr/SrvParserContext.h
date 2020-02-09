class SrvParserContext;

#ifndef SRV_PARSER_CONTEXT_H
#define SRV_PARSER_CONTEXT_H

#include "SrvParser.h"
#include "location.hh"

#include "SrvParsGlobalOpt.h"
#include "SrvCfgIface.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgTA.h"
#include "SrvCfgPD.h"
#include "Key.h"
#include "SrvCfgMgr.h"
#include "OptAddrLst.h"

// Tell Flex the lexer's prototype ...
#define YY_DECL dibbler::SrvParser::symbol_type parser6_lex (SrvParserContext& driver)

// ... and declare it for the parser's sake.
YY_DECL;

class SrvParserContext
{
 public:

    /// @brief Default constructor
    SrvParserContext();

    /// @brief destructor
    ~SrvParserContext();

    bool parseFile(std::ifstream &f);

    void scanBegin(std::ifstream &f);
    void scanEnd();

    bool trace_scanning_;
    bool trace_parsing_;

    void error(const dibbler::location& loc, const std::string& what);
    void error(const std::string& what);
    static void fatal(const std::string& what);

    /// @brief File name
    std::string file_;

    dibbler::location loc_;

    // %define MEMBERS FlexLexer * lex;
    List(TSrvParsGlobalOpt) ParserOptStack_;    /* list of parsed interfaces/IAs/addrs */
    List(TSrvCfgIface) SrvCfgIfaceLst_;         /* list of SrvCfg interfaces */
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst_; /* list of SrvCfg address classes */
    List(TSrvCfgTA) SrvCfgTALst_;               /* list of SrvCfg TA objects */
    List(TSrvCfgPD) SrvCfgPDLst_;               /* list of SrvCfg PD objects */
    List(TSrvCfgClientClass) SrvCfgClientClassLst_; /* list of SrvCfgClientClass objs */
    List(TIPv6Addr) PresentAddrLst_;            /* address list (used for DNS,NTP,etc.)*/
    List(std::string) PresentStringLst_;             /* string list */
    List(Node) NodeClientClassLst_;             /* Node list */
    List(TFQDN) PresentFQDNLst_;
    SPtr<TIPv6Addr> addr_;
    SPtr<TSIGKey> CurrentKey_;
    DigestTypesLst DigestLst_;
    List(THostRange) PresentRangeLst_;
    List(THostRange) PDLst_;
    List(TSrvCfgOptions) ClientLst_;
    int PDPrefix_;
    bool IfaceDefined(int ifaceNr);
    bool IfaceDefined(std::string ifaceName);
    bool StartIfaceDeclaration(std::string iface);
    bool StartIfaceDeclaration(int ifindex);
    bool EndIfaceDeclaration();
    void StartClassDeclaration();
    bool EndClassDeclaration();
    SPtr<TIPv6Addr> getRangeMin(const char * addrPacked, int prefix);
    SPtr<TIPv6Addr> getRangeMax(const char * addrPacked, int prefix);
    void StartTAClassDeclaration();
    bool EndTAClassDeclaration();
    void StartPDDeclaration();
    bool EndPDDeclaration();
    TSrvCfgMgr * CfgMgr_;
    SPtr<TOpt> nextHop;
};

#endif
