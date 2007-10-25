include ../Makefile.inc

OBJECTS =  RelParser.o RelLexer.o RelCfgIface.o RelCfgMgr.o \
           RelParsGlobalOpt.o RelParsIfaceOpt.o

objs: $(OBJECTS)

libs: libRelCfgMgr.a

RelParser.cpp: RelParser.y
	echo "[BISON++] $(SUBDIR)/RelParser.y"
	$(BISONPP) -v -d RelParser.y -o RelParser.cpp

RelLexer.cpp: RelLexer.l
	echo "[FLEX   ] $(SUBDIR)/RelLexer.l"
	$(FLEX) -+ -i -oRelLexer.cpp RelLexer.l
	@echo "[SED    ] $(SUBDIR)/RelLexer.cpp"
	cat RelLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/ extern "C" int isatty (int ) throw ();/' > RelLexer.cpp2
	rm -f RelLexer.cpp
	mv RelLexer.cpp2 RelLexer.cpp

parser: RelParser.cpp RelLexer.cpp 
