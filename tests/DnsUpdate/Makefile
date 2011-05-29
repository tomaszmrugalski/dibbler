TOPDIR=../..

OBJECTS = DnsUpdate01.o DnsUpdate02.o

TESTS = DnsUpdate01 DnsUpdate02

tests: $(TESTS)

objs: $(OBJECTS)

libs: testDnsUpdate.a

include $(TOPDIR)/Makefile.inc

DnsUpdate01: DnsUpdate01.cpp
	$(CXX) $(CXXFLAGS) -o DnsUpdate01 $< -L$(TOPDIR)/Misc -lMisc \
		              -L$(TOPDIR)/LowLevel \
			      -L$(TOPDIR)/IfaceMgr -lIfaceMgr

DnsUpdate02: DnsUpdate02.cpp
	cd $(TOPDIR); make libposlib
	cd $(MISC); make libs
	cd $(SRVIFACEMGR); make libs
	$(CXX) $(CXXFLAGS) -o DnsUpdate02 $< -L$(TOPDIR)/Misc -lMisc \
		              -L$(TOPDIR)/LowLevel \
			      -L$(TOPDIR)/IfaceMgr -lIfaceMgr \
			      -L$(TOPDIR)/SrvIfaceMgr -lSrvIfaceMgr \
			      -L$(POSLIB) -lposlib \
			      -L$(MISC) -lMisc

