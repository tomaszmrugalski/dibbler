include ../Makefile.inc

OBJECTS = ClntParser.o        ClntLexer.o     ClntCfgIA.o        ClntCfgMgr.o      \
	  ClntCfgIface.o      ClntCfgGroup.o  ClntCfgAddr.o      ClntParsAddrOpt.o \
          ClntParsGlobalOpt.o ClntParsIAOpt.o ClntParsIfaceOpt.o

objs: $(OBJECTS)

libs: libClntCfgMgr.a

ClntLexer.o: ClntLexer.cpp
	@echo "[CPP    ] $(SUBDIR)/$@"
	$(CPP) $(OPTS) -c ClntLexer.cpp

parser: ClntParser.y ClntLexer.l
	echo "[BISON  ] ClntParser.y"
	$(BISONPP) -d ClntParser.y -o ClntParser.cpp
	echo "[FLEX   ] ClntLexer.l"
	$(FLEX) -+ -i -oClntLexer.cpp ClntLexer.l
	echo "[CPP    ]  ClntParser.cpp"
	$(CPP) $(OPTS) -c -ftemplate-depth-30 ClntParser.cpp 

