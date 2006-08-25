include ../Makefile.inc

OBJECTS = SocketIPv6.o Iface.o IfaceMgr.o

objs: $(OBJECTS)

libs: libIfaceMgr.a


test1: test1.cpp DNSUpdate.o
	g++ -o test1 -I../include test1.cpp DNSUpdate.o ../Misc/Logger.o -L../poslib/poslib/.libs -lposlib -lpthread
