include ../Makefile.inc

all: objs libs test

client: objs

server: objs

libs: libSrvTransMgr.a

libSrvTransMgr.a: SrvTransMgr.o
	ar cr $@ SrvTransMgr.o

# tests
test: 

test1: test1.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp \
	-L. -lSrvTransMgr \
	-L$(OPTIONS)    -lOpts \
	-L$(SRVCFGMGR) -lSrvCfgMgr \
	-L$(SRVADDRMGR) -lSrvAddrMgr \
	-L$(SRVPARSER) -lSrvParser \
	-L$(MESSAGES)   -lMsg \
	-L$(OPTIONS)    -lOpts \
	-L$(SRVIFACEMGR) -lSrvIfaceMgr \

# Objects 
objs: SrvTransMgr.o

SrvTransMgr.o: SrvTransMgr.cpp SrvTransMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4
