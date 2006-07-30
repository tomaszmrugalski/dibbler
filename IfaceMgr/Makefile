include ../Makefile.inc

OBJECTS = SocketIPv6.o Iface.o IfaceMgr.o

ifndef MOD_SRV_DISABLE_DNSUPDATE
ifndef MOD_CLNT_DISABLE_DNSUPDATE
OBJECTS += DNSUpdate.o
endif
endif

objs: $(OBJECTS)

libs: libIfaceMgr.a

