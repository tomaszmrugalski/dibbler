include ../Makefile.inc

all: objs libs

client: objs

server: objs


libs: libSrvAddrMgr.a

libSrvAddrMgr.a: SrvAddrMgr.o
	cd $(ADDRMGR); $(MAKE) libs
	rm -f $@
	cp $(ADDRMGR)/libAddrMgr.a $@
	ar r $@ SrvAddrMgr.o

# tests

test: test1

test1: test1.cpp libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp  \
	-L. -lSrvAddrMgr

# Objects 
objs: SrvAddrMgr.o 

SrvAddrMgr.o: SrvAddrMgr.cpp SrvAddrMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) $(XMLFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4