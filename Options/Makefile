include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libOpts.a

libOpts.a: Opt.o OptClientIdentifier.o OptElapsed.o OptIAAddress.o OptInterfaceID.o \
	OptPreference.o OptRapidCommit.o OptServerIdentifier.o \
	OptServerUnicast.o OptStatusCode.o OptUserClass.o OptVendorClass.o OptVendorSpecInfo.o \
	OptIA_NA.o OptOptionRequest.o OptDNSServers.o OptDomainName.o OptNTPServers.o OptTimeZone.o
	ar cr $@ Opt.o OptClientIdentifier.o OptElapsed.o OptIAAddress.o OptInterfaceID.o \
	OptPreference.o OptRapidCommit.o OptServerIdentifier.o \
	OptServerUnicast.o OptStatusCode.o OptUserClass.o OptVendorClass.o OptVendorSpecInfo.o \
	OptIA_NA.o OptOptionRequest.o \
	OptDNSServers.o OptDomainName.o OptNTPServers.o OptTimeZone.o

# tests

test: test1

test1: test1.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp 

# Objects 
objs: Opt.o OptClientIdentifier.o OptElapsed.o OptIAAddress.o OptInterfaceID.o \
	OptPreference.o OptRapidCommit.o OptServerIdentifier.o \
	OptServerUnicast.o OptStatusCode.o OptUserClass.o OptVendorClass.o OptVendorSpecInfo.o \
	OptIA_NA.o OptOptionRequest.o \
	OptDNSServers.o OptDomainName.o OptNTPServers.o OptTimeZone.o


OptClientIdentifier.o: OptClientIdentifier.cpp OptClientIdentifier.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

Opt.o: Opt.cpp Opt.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptElapsed.o: OptElapsed.cpp OptElapsed.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptIAAddress.o: OptIAAddress.cpp OptIAAddress.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptIA_NA.o: OptIA_NA.cpp OptIA_NA.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptInterfaceID.o: OptInterfaceID.cpp OptInterfaceID.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptOptionRequest.o: OptOptionRequest.cpp OptOptionRequest.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptPreference.o: OptPreference.cpp OptPreference.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptRapidCommit.o: OptRapidCommit.cpp OptRapidCommit.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptServerIdentifier.o: OptServerIdentifier.cpp OptServerIdentifier.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptServerUnicast.o: OptServerUnicast.cpp OptServerUnicast.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptStatusCode.o: OptStatusCode.cpp OptStatusCode.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptUserClass.o: OptUserClass.cpp OptUserClass.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptVendorClass.o: OptVendorClass.cpp OptVendorClass.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptVendorSpecInfo.o: OptVendorSpecInfo.cpp OptVendorSpecInfo.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptDNSServers.o: OptDNSServers.cpp OptDNSServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptDomainName.o: OptDomainName.cpp OptDomainName.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptNTPServers.o: OptNTPServers.cpp OptNTPServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

OptTimeZone.o: OptTimeZone.cpp OptTimeZone.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.[oa]
	rm -f test1 