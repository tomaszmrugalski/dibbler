include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libSrvIfaceMgr.a

libSrvIfaceMgr.a: SrvIfaceMgr.o
	cd $(IFACEMGR); $(MAKE) libs
	rm -f $@
	cp $(IFACEMGR)/libIfaceMgr.a $@
	ar r $@ SrvIfaceMgr.o

# tests
test: test1 

test1: test1.cpp libs 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp SrvIfaceMgr.o \
	-L. -lSrvIfaceMgr \
	$(PREFIX)/Logger.o

# Objects 
objs: SrvIfaceMgr.o

SrvIfaceMgr.o: SrvIfaceMgr.cpp SrvIfaceMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c $(XMLCFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4
