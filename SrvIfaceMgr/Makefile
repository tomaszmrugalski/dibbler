TOPDIR = ..

OBJECTS = SrvIfaceIface.o SrvIfaceMgr.o

ifndef MOD_SRV_DISABLE_DNSUPDATE
OBJECTS += $(IFACEMGR)/DNSUpdate.o
endif

include $(TOPDIR)/Makefile.inc

libs: libSrvIfaceMgr.a
