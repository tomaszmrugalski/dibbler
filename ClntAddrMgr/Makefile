include ../Makefile.inc

all: objs libs test

client: objs

server: objs


libs: libClntAddrMgr.a

libClntAddrMgr.a: ClntAddrMgr.o
	cd $(ADDRMGR); $(MAKE) libs
	rm -f $@
	cp $(ADDRMGR)/libAddrMgr.a $@
	ar r $@ ClntAddrMgr.o

# tests

test: test1

test1: test1.cpp libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp  \
	-L. -lClntAddrMgr

# Objects 
objs: ClntAddrMgr.o 

ClntAddrMgr.o: ClntAddrMgr.cpp ClntAddrMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4