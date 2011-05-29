TOPDIR = ..

include $(TOPDIR)/Makefile.inc

OBJECTS = ClntParser.o        ClntLexer.o     ClntCfgIA.o        ClntCfgMgr.o      \
	  ClntCfgIface.o      		      ClntCfgAddr.o      ClntParsAddrOpt.o \
          ClntParsGlobalOpt.o ClntParsIAOpt.o ClntParsIfaceOpt.o ClntCfgTA.o       \
	  ClntCfgPD.o        ClntCfgPrefix.o

objs: $(OBJECTS)

libs: libClntCfgMgr.a

# APPROACH 1: ignore .y .l dependency
# After modifying .y and .l files, you have to manually rebuild .cpp files by issuing: make parser
# Pros: Proven and working
# Cons: 

parser: ClntParser.y ClntLexer.l
	echo "[BISON++] $(SUBDIR)/ClntParser.y"
	$(BISONPP) -v -d ClntParser.y -o ClntParser.cpp
#	$(BISONPP) -l -d ClntParser.y -o ClntParser.cpp
	echo "[FLEX   ] $(SUBDIR)/ClntLexer.l"
	$(FLEX) -+ -i -L -oClntLexer.cpp ClntLexer.l
#	$(FLEX) -+ -i -oClntLexer.cpp ClntLexer.l
	@echo "[SED    ] $(SUBDIR)/ClntLexer.cpp"
	cat ClntLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > ClntLexer.cpp2
	rm -f ClntLexer.cpp
	mv ClntLexer.cpp2 ClntLexer.cpp

# APPROACH 2: .l .y dependency
# Proper dependency chain .l->.cpp->.o and .y->.cpp->.o
# Pros:
# Cons: - bison++ and flex required during every build

#parser: ClntParser.cpp ClntLexer.cpp

#ClntParser.cpp: ClntParser.y
#	echo "[BISON++] $(SUBDIR)/ClntParser.y"
#	$(BISONPP) -v -d ClntParser.y -o ClntParser.cpp

#ClntLexer.cpp: ClntLexer.l
#	echo "[FLEX   ] $(SUBDIR)/ClntLexer.l"
#	$(FLEX) -+ -i -L -oClntLexer.cpp ClntLexer.l
#	$(FLEX) -+ -i -oClntLexer.cpp ClntLexer.l
#	@echo "[SED    ] $(SUBDIR)/ClntLexer.cpp"
#	cat ClntLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > ClntLexer.cpp2
#	rm -f ClntLexer.cpp
#	mv ClntLexer.cpp2 ClntLexer.cpp
