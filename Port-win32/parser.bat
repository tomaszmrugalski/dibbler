@echo "Generating client files..."
flex -+ -L -oClntLexer.cpp ClntLexer.l
bison++ --no-lines -d ClntParser.y -o ClntParser.cpp
@echo "Generating server files..."
flex -+ -L -oSrvLexer.cpp SrvLexer.l
bison++ --no-lines -d SrvParser.y -o SrvParser.cpp
@pause