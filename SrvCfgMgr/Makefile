include ../Makefile.inc

OBJECTS =  SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
           SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o

objs: $(OBJECTS)

libs: libSrvCfgMgr.a

SrvLexer.o: SrvLexer.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -c $(OPTS) -ftemplate-depth-30 SrvLexer.cpp 

parser: SrvParser.y SrvLexer.l 
	echo "[BISON++] SrvParser.y"
	$(BISONPP) -d SrvParser.y -o SrvParser.cpp
#	$(BISONPP) -l -d SrvParser.y -o SrvParser.cpp
	echo "[FLEX   ] SrvLexer.l"
	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l
