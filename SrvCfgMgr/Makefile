include ../Makefile.inc

OBJECTS =  SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
           SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o

objs: $(OBJECTS)

libs: libSrvCfgMgr.a

parser: SrvParser.y SrvLexer.l
	echo "[BISON++] SrvParser.y"
	$(BISONPP) -l -d SrvParser.y -o SrvParser.cpp
	echo "[FLEX   ] SrvLexer.l"
	$(FLEX) -+ -i -L -oSrvLexer.cpp SrvLexer.l
	@echo "[CPP    ] $(SUBDIR)/$@"
	$(CPP) -c $(OPTS) SrvLexer.cpp 

SrvLexer.o: SrvLexer.cpp
	@echo "[CPP    ] $(SUBDIR)/$@"
	$(CPP) -c $(OPTS) SrvLexer.cpp 
