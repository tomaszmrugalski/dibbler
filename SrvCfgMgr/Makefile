include ../Makefile.inc

all: objs libs

libs: libSrvCfgMgr.a

libSrvCfgMgr.a: objs
	cd $(CFGMGR); $(MAKE) libs
	rm -f $@
	cp $(CFGMGR)/libCfgMgr.a ./$@
	ar r $@ $(CFGMGR)/CfgMgr.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
	SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o SrvParser.o SrvLexer.o

# Objects 
objs: SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o \
      SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o

SrvParser.o: SrvParser.y
	$(BISONPP) -d SrvParser.y -o SrvParser.cpp
	$(CPP) -c $(OPTS) -I $(INCDIR) SrvParser.cpp 

SrvLexer.o: SrvLexer.l
	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l
	$(CPP) -c $(OPTS) -I $(INCDIR) SrvLexer.cpp 

SrvCfgAddrClass.o: SrvCfgAddrClass.cpp SrvCfgAddrClass.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvCfgIface.o: SrvCfgIface.cpp SrvCfgIface.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvCfgMgr.o: SrvCfgMgr.cpp SrvCfgMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvParsClassOpt.o: SrvParsClassOpt.cpp SrvParsClassOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvParsGlobalOpt.o: SrvParsGlobalOpt.cpp SrvParsGlobalOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvParsIfaceOpt.o: SrvParsIfaceOpt.cpp SrvParsIfaceOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

# tests

test: test1 

test1: test1.cpp libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp  \
	-L. -lSrvCfgMgr \
	-L$(ADDRMGR) -lAddrMgr \
	-L$(IFACEMGR) -lIfaceMgr \
	-L$(SRVPARSER) -lSrvParser

test2: test2.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test2.cpp

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4