include ../Makefile.inc

all: objs libs

client: objs

server: objs

# tests
test: test1 test2 test3 test4 test5

test1: test1.cpp libIfaceMgr.a
	cd $(MISC); $(MAKE) Logger.o IPv6Addr.o
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp -L. -lIfaceMgr $(MISC)/Logger.o $(MISC)/IPv6Addr.o

test2: test2.cpp IfaceMgr.o Iface.o SocketIPv6.o 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test2.cpp -L. -lIfaceMgr $(PREFIX)/Logger.o

test3: test3.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ $< -L. -lIfaceMgr $(PREFIX)/Logger.o

test4: test4.cpp SocketIPv6.o Iface.o IfaceMgr.o 
	cd $(PREFIX); $(MAKE) Logger.o
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test4.cpp -L. -lIfaceMgr  $(PREFIX)/Logger.o

test5: test5.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o test5 test5.cpp -L. -lIfaceMgr

# lib
libs: libIfaceMgr.a

libIfaceMgr.a: objs
	cd $(LOWLEVEL); $(MAKE) libs
	rm -f libIfaceMgr.a
	cp $(LOWLEVEL)/libLowLevel.a $@
	ar r $@ SocketIPv6.o Iface.o IfaceMgr.o

# Objects 
objs: SocketIPv6.o Iface.o IfaceMgr.o 

SocketIPv6.o: SocketIPv6.cpp SocketIPv6.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

Iface.o: Iface.cpp Iface.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

IfaceMgr.o: IfaceMgr.cpp IfaceMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o *.a
	rm -f test? test??
