include ../Makefile.inc

OBJECTS =  RelParser.o RelLexer.o RelCfgIface.o RelCfgMgr.o \
           RelParsGlobalOpt.o RelParsIfaceOpt.o

objs: $(OBJECTS)

libs: libRelCfgMgr.a

parser: RelParser.y RelLexer.l 
	echo "[BISON++] RelParser.y"
#	$(BISONPP) -d RelParser.y -o RelParser.cpp
	$(BISONPP) -v -d RelParser.y -o RelParser.cpp
	echo "[FLEX   ] RelLexer.l"
	$(FLEX) -+ -i -oRelLexer.cpp RelLexer.l
