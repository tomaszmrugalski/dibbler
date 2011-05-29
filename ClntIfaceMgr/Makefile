TOPDIR = ..

include $(TOPDIR)/Makefile.inc

OBJECTS = ClntIfaceMgr.o ClntIfaceIface.o

ifndef MOD_CLNT_DISABLE_DNSUPDATE
OBJECTS += $(IFACEMGR)/DNSUpdate.o
endif

objs: $(OBJECTS)

libs: libClntIfaceMgr.a
