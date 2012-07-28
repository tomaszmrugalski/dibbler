@echo "Generating client files..."
set FOO=..\ClntCfgMgr
flex -+ -L -o%FOO%\ClntLexer.cpp %FOO%\ClntLexer.l
bison++ --no-lines -d %FOO%\ClntParser.y -o %FOO%\ClntParser.cpp
set FOO=..\SrvCfgMgr
@echo "Generating server files..."
flex -+ -L -o%FOO%\SrvLexer.cpp %FOO%\SrvLexer.l
bison++ --no-lines -d %FOO%\SrvParser.y -o %FOO%\SrvParser.cpp
