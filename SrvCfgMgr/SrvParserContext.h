class SrvParserContext;

#ifndef SRV_PARSER_CONTEXT_H
#define SRV_PARSER_CONTEXT_H

#include "SrvParser.h"
#include "location.hh"


// Tell Flex the lexer's prototype ...
#define YY_DECL SrvParser::symbol_type parser6_lex (SrvParserContext& driver)

// ... and declare it for the parser's sake.
//YY_DECL;

class SrvParserContext
{
 public:

    /// @brief Default constructor
    SrvParserContext();

    bool parseFile(std::ifstream &f);

    void scanBegin(std::ifstream &f);
    void scanEnd();

    bool trace_scanning_;
    bool trace_parsing_;

    //void error(const dibbler::location& loc, const std::string& what);
    void error(const std::string& what);
    static void fatal(const std::string& what);

    /// @brief File name
    std::string file_;

    dibbler::location loc_;

    // %define MEMBERS FlexLexer * lex;
    List(TSrvParsGlobalOpt) ParserOptStack;    /* list of parsed interfaces/IAs/addrs */
    List(TSrvCfgIface) SrvCfgIfaceLst;         /* list of SrvCfg interfaces */
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst; /* list of SrvCfg address classes */
    List(TSrvCfgTA) SrvCfgTALst;               /* list of SrvCfg TA objects */
    List(TSrvCfgPD) SrvCfgPDLst;               /* list of SrvCfg PD objects */
    List(TSrvCfgClientClass) SrvCfgClientClassLst; /* list of SrvCfgClientClass objs */
    List(TIPv6Addr) PresentAddrLst;            /* address list (used for DNS,NTP,etc.)*/
    List(std::string) PresentStringLst;             /* string list */
    List(Node) NodeClientClassLst;             /* Node list */
    List(TFQDN) PresentFQDNLst;
    SPtr<TIPv6Addr> addr;
    SPtr<TSIGKey> CurrentKey;
    DigestTypesLst DigestLst;
    List(THostRange) PresentRangeLst;
    List(THostRange) PDLst;
    List(TSrvCfgOptions) ClientLst;
    int PDPrefix;
    bool IfaceDefined(int ifaceNr);
    bool IfaceDefined(std::string ifaceName);
    bool StartIfaceDeclaration(std::string iface);
    bool StartIfaceDeclaration(int ifindex);
    bool EndIfaceDeclaration();
    void StartClassDeclaration();
    bool EndClassDeclaration();
    SPtr<TIPv6Addr> getRangeMin(char * addrPacked, int prefix);
    SPtr<TIPv6Addr> getRangeMax(char * addrPacked, int prefix);
    void StartTAClassDeclaration();
    bool EndTAClassDeclaration();
    void StartPDDeclaration();
    bool EndPDDeclaration();
    TSrvCfgMgr * CfgMgr_;
    SPtr<TOpt> nextHop;
};

#endif
