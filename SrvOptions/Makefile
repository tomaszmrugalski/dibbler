include ../Makefile.inc

all: objs libs test

client: objs

server: objs

libs: libSrvOpts.a

libSrvOpts.a: 	SrvOptClientIdentifier.o	SrvOptElapsed.o 	SrvOptIAAddress.o  \
		SrvOptInterfaceID.o 		SrvOptPreference.o 	SrvOptRapidCommit.o \
		SrvOptServerIdentifier.o 	SrvOptServerUnicast.o 	SrvOptStatusCode.o  \
		SrvOptUserClass.o 		SrvOptVendorClass.o 	SrvOptVendorSpecInfo.o \
		SrvOptIA_NA.o 			SrvOptOptionRequest.o   SrvOptDNSServers.o \
		SrvOptDomainName.o              SrvOptNTPServers.o SrvOptTimeZone.o
	ar cr $@ SrvOptClientIdentifier.o SrvOptElapsed.o SrvOptIAAddress.o SrvOptInterfaceID.o \
	SrvOptPreference.o SrvOptRapidCommit.o SrvOptServerIdentifier.o \
	SrvOptServerUnicast.o SrvOptStatusCode.o SrvOptUserClass.o SrvOptVendorClass.o \
	SrvOptVendorSpecInfo.o SrvOptIA_NA.o SrvOptOptionRequest.o SrvOptDNSServers.o \
	SrvOptDomainName.o              SrvOptNTPServers.o SrvOptTimeZone.o


# tests

test: test1

test1: test1.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp 

# Objects 
objs: SrvOptClientIdentifier.o SrvOptElapsed.o SrvOptIAAddress.o SrvOptInterfaceID.o \
	SrvOptPreference.o SrvOptRapidCommit.o SrvOptServerIdentifier.o \
	SrvOptServerUnicast.o SrvOptStatusCode.o SrvOptUserClass.o SrvOptVendorClass.o \
	SrvOptVendorSpecInfo.o SrvOptIA_NA.o SrvOptOptionRequest.o SrvOptDNSServers.o \
	SrvOptDomainName.o              SrvOptNTPServers.o

SrvOptClientIdentifier.o: SrvOptClientIdentifier.cpp SrvOptClientIdentifier.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOpt.o: SrvOpt.cpp SrvOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptElapsed.o: SrvOptElapsed.cpp SrvOptElapsed.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptIAAddress.o: SrvOptIAAddress.cpp SrvOptIAAddress.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptIA_NA.o: SrvOptIA_NA.cpp SrvOptIA_NA.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptInterfaceID.o: SrvOptInterfaceID.cpp SrvOptInterfaceID.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptOptionRequest.o: SrvOptOptionRequest.cpp SrvOptOptionRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptPreference.o: SrvOptPreference.cpp SrvOptPreference.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptRapidCommit.o: SrvOptRapidCommit.cpp SrvOptRapidCommit.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptServerIdentifier.o: SrvOptServerIdentifier.cpp SrvOptServerIdentifier.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptServerUnicast.o: SrvOptServerUnicast.cpp SrvOptServerUnicast.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptStatusCode.o: SrvOptStatusCode.cpp SrvOptStatusCode.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptUserClass.o: SrvOptUserClass.cpp SrvOptUserClass.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptVendorClass.o: SrvOptVendorClass.cpp SrvOptVendorClass.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptVendorSpecInfo.o: SrvOptVendorSpecInfo.cpp SrvOptVendorSpecInfo.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptDNSServers.o: SrvOptDNSServers.cpp SrvOptDNSServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptDomainName.o: SrvOptDomainName.cpp SrvOptDomainName.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptNTPServers.o: SrvOptNTPServers.cpp SrvOptNTPServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvOptTimeZone.o: SrvOptTimeZone.cpp SrvOptTimeZone.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.[oa]
	rm -f test1 