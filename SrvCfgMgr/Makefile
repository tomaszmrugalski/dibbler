include ../Makefile.inc

OBJECTS =  SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
           SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o

objs: $(OBJECTS)

libs: libSrvCfgMgr.a

parser: SrvParser.y SrvLexer.l
	$(BISONPP) -d SrvParser.y -o SrvParser.cpp
	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l

SrvLexer.o: SrvLexer.cpp
	@echo "[CPP    ] $(SUBDIR)/$@"
	$(CPP) -c $(OPTS) -I $(INCDIR) SrvLexer.cpp 
