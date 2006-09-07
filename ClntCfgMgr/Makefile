include ../Makefile.inc

OBJECTS = ClntParser.o        ClntLexer.o     ClntCfgIA.o        ClntCfgMgr.o      \
	  ClntCfgIface.o                      ClntCfgAddr.o      ClntParsAddrOpt.o \
          ClntParsGlobalOpt.o ClntParsIAOpt.o ClntParsIfaceOpt.o ClntCfgTA.o

objs: $(OBJECTS)

libs: libClntCfgMgr.a

ClntLexer.o: ClntLexer.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) $(OPTS) -c -ftemplate-depth-30 ClntLexer.cpp

parser: ClntParser.y ClntLexer.l
	echo "[BISON++] $(SUBDIR)/ClntParser.y"
	$(BISONPP) -v -d ClntParser.y -o ClntParser.cpp
#	$(BISONPP) -l -d ClntParser.y -o ClntParser.cpp
	echo "[FLEX   ] $(SUBDIR)/ClntLexer.l"
	$(FLEX) -+ -i -L -oClntLexer.cpp ClntLexer.l
	$(FLEX) -+ -i -oClntLexer.cpp ClntLexer.l
	@echo "[SED    ] $(SUBDIR)/ClntLexer.cpp"
	cat ClntLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > ClntLexer.cpp2
	rm -f ClntLexer.cpp
	mv ClntLexer.cpp2 ClntLexer.cpp
