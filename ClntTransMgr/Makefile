include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libClntTransMgr.a

libClntTransMgr.a: ClntTransMgr.o
	ar cr $@ ClntTransMgr.o

# tests
test: test1 

test1: test1.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp \
	-L. -lClntTransMgr \
	-L$(OPTIONS)    -lOpts \
	-L$(CLNTCFGMGR) -lClntCfgMgr \
	-L$(CLNTADDRMGR) -lClntAddrMgr \
	-L$(CLNTPARSER) -lClntParser \
	-L$(MESSAGES)   -lMsg \
	-L$(OPTIONS)    -lOpts \
	-L$(CLNTIFACEMGR) -lClntIfaceMgr \

# Objects 
objs: ClntTransMgr.o

ClntTransMgr.o: ClntTransMgr.cpp ClntTransMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4
