include ../Makefile.inc

all: objs libs

client: objs

server: objs

# tests
test: test1 test2 test3 test4

test1: test1.cpp libs
	cd $(MISC); $(MAKE) libs
	$(CPP) $(OPTS) $(XMLLIBS) -o $@ test1.cpp \
	-L. -lAddrMgr -L$(MISC) -lMisc -L$(IFACEMGR) -lIfaceMgr

test2: test2.cpp AddrAddr.o AddrIA.o
	cd $(MISC); $(MAKE) libs
	$(CPP) $(OPTS) -o $@ test2.cpp AddrAddr.o AddrIA.o -L$(MISC) -lMisc

test3: test3.cpp AddrAddr.o AddrIA.o AddrClient.o
	cd $(MISC); $(MAKE) objs
	$(CPP) $(OPTS) -o $@ test3.cpp AddrAddr.o AddrIA.o AddrClient.o $(MISC)/Logger.o

test4: test4.cpp AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o
	cd $(MISC); $(MAKE) objs
	$(CPP) $(OPTS) -o $@ test4.cpp $(MISC)/Logger.o \
	AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o

# lib
libs: libAddrMgr.a

libAddrMgr.a: AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o 
	ar cr $@ AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o 

# Objects 
objs: AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o 

AddrAddr.o: AddrAddr.cpp AddrAddr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

AddrIA.o: AddrIA.cpp AddrIA.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

AddrClient.o: AddrClient.cpp AddrClient.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

AddrMgr.o: AddrMgr.cpp AddrMgr.h
	$(CPP) $(XMLCFLAGS) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.xml
	rm -f test1 test2 test3 test4