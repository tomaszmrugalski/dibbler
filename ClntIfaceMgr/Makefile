include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libClntIfaceMgr.a

libClntIfaceMgr.a: ClntIfaceMgr.o
	cd $(IFACEMGR); $(MAKE) libs
	rm -f $@
	cp $(IFACEMGR)/libIfaceMgr.a $@
	ar r $@ ClntIfaceMgr.o

# tests
test: test1 

test1: test1.cpp libs 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp ClntIfaceMgr.o \
	-L. -lClntIfaceMgr \
	-L$(CLNTMSG) -lClntMsg \
	-L$(CLNTOPTS) -lClntOpts \
	$(PREFIX)/Logger.o

# Objects 
objs: ClntIfaceMgr.o

ClntIfaceMgr.o: ClntIfaceMgr.cpp ClntIfaceMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c $(XMLCFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f *.a
	rm -f test1 test2 test3 test4
