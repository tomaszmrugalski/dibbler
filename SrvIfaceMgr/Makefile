include ../Makefile.inc

OBJECTS = SrvIfaceIface.o SrvIfaceMgr.o

ifndef MOD_SRV_DISABLE_DNSUPDATE
OBJECTS += $(IFACEMGR)/DNSUpdate.o
endif

objs: $(OBJECTS)

libs: libSrvIfaceMgr.a
