include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libClntCfgMgr.a

libClntCfgMgr.a: objs
	cd $(CFGMGR); $(MAKE) libs
	rm -f $@
	cp $(CFGMGR)/libCfgMgr.a ./$@
	ar r $@ ClntParser.o ClntLexer.o ClntCfgIA.o ClntCfgMgr.o ClntCfgIface.o \
		ClntCfgGroup.o ClntCfgAddr.o \
		ClntParsAddrOpt.o ClntParsGlobalOpt.o ClntParsIAOpt.o ClntParsIfaceOpt.o

test: test1 

test1: test1.cpp libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp  \
	-L. -lClntCfgMgr \
	-L$(CLNTPARSER) -lClntParser \
	-L$(CLNTIFACEMGR) -lClntIfaceMgr \
	-L$(CLNTMESSAGES) -lClntMsg \
	-L$(CLNTOPTS)     -lClntOpts \
        -L$(OPTIONS)      -lOpts \
	-L$(ADDRMGR)      -lAddrMgr \
	$(PREFIX)/Logger.o

test2: test2.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test2.cpp \

# Objects 
objs: ClntParser.o ClntLexer.o ClntCfgIA.o ClntCfgMgr.o ClntCfgIface.o \
      ClntCfgGroup.o ClntCfgAddr.o \
      ClntParsAddrOpt.o ClntParsGlobalOpt.o ClntParsIAOpt.o ClntParsIfaceOpt.o

ClntParser.o: ClntParser.y
	$(BISONPP) -d ClntParser.y -o ClntParser.cpp
	$(CPP) $(OPTS) -c -I $(INCDIR) -ftemplate-depth-30 ClntParser.cpp 

ClntLexer.o: ClntLexer.l
	$(FLEX) -+ -i -oClntLexer.cpp ClntLexer.l
	$(CPP) $(OPTS) -c -I $(INCDIR) ClntLexer.cpp

ClntCfgIA.o: ClntCfgIA.cpp ClntCfgIA.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntCfgAddr.o: ClntCfgAddr.cpp ClntCfgAddr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntCfgGroup.o: ClntCfgGroup.cpp ClntCfgGroup.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntCfgIface.o: ClntCfgIface.cpp ClntCfgIface.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntCfgMgr.o: ClntCfgMgr.cpp ClntCfgMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntParsAddrOpt.o: ClntParsAddrOpt.cpp ClntParsAddrOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntParsGlobalOpt.o: ClntParsGlobalOpt.cpp ClntParsGlobalOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntParsIAOpt.o: ClntParsIAOpt.cpp ClntParsIAOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntParsIfaceOpt.o: ClntParsIfaceOpt.cpp ClntParsIfaceOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4