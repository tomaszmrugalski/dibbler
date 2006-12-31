include ../Makefile.inc

OBJECTS =  SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
           SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o SrvCfgTA.o \
	   SrvCfgPD.o SrvCfgOptions.o

objs: $(OBJECTS)

libs: libSrvCfgMgr.a

SrvLexer.o: SrvLexer.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -c $(OPTS) -ftemplate-depth-30 SrvLexer.cpp 

parser: SrvParser.y SrvLexer.l 
	echo "[BISON++] $(SUBDIR)/SrvParser.y"
	$(BISONPP) -v --debug --defines -d SrvParser.y -o SrvParser.cpp
#	$(BISONPP) -l -d SrvParser.y -o SrvParser.cpp
	echo "[FLEX   ] $(SUBDIR)/SrvLexer.l"
	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l
	@echo "[SED    ] $(SUBDIR)/SrvLexer.cpp"
	cat SrvLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > SrvLexer.cpp2
	rm -f SrvLexer.cpp
	mv SrvLexer.cpp2 SrvLexer.cpp
