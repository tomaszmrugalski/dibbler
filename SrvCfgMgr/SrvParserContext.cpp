#include "SrvParserContext.h"

SrvParserContext::SrvParserContext() {
    ParserOptStack.append(new TSrvParsGlobalOpt());
}
