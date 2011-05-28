TOPDIR=../..

OBJECTS = DnsUpdate01.o

TESTS = DnsUpdate01

tests: $(TESTS)

objs: $(OBJECTS)

libs: testDnsUpdate.a

include $(TOPDIR)/Makefile.inc

DnsUpdate01: DnsUpdate01.cpp
	$(CXX) $(CXXFLAGS) -o DnsUpdate01 $< -L$(TOPDIR)/Misc -lMisc \
		              -L$(TOPDIR)/LowLevel \
			      -L$(TOPDIR)/IfaceMgr -lIfaceMgr

