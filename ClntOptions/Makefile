include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libClntOpts.a

libClntOpts.a: 	ClntOptClientIdentifier.o 	ClntOptElapsed.o 	ClntOptIAAddress.o      \
		ClntOptInterfaceID.o 		ClntOptPreference.o 	ClntOptRapidCommit.o    \
		ClntOptServerIdentifier.o 	ClntOptServerUnicast.o	ClntOptStatusCode.o     \
		ClntOptUserClass.o		ClntOptVendorClass.o 	ClntOptVendorSpecInfo.o \
		ClntOptIA_NA.o			ClntOptOptionRequest.o  ClntOptDNSServers.o     \
		ClntOptDomainName.o		ClntOptNTPServers.o     ClntOptTimeZone.o
	ar cr $@ \
		ClntOptClientIdentifier.o 	ClntOptElapsed.o 	ClntOptIAAddress.o      \
		ClntOptInterfaceID.o 		ClntOptPreference.o 	ClntOptRapidCommit.o    \
		ClntOptServerIdentifier.o 	ClntOptServerUnicast.o 	ClntOptStatusCode.o 	\
		ClntOptUserClass.o 		ClntOptVendorClass.o 	ClntOptVendorSpecInfo.o \
		ClntOptIA_NA.o 			ClntOptOptionRequest.o  ClntOptDNSServers.o     \
		ClntOptDomainName.o		ClntOptNTPServers.o     ClntOptTimeZone.o

test: test1

test1: test1.cpp
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp 

# Objects 
objs:  		ClntOptClientIdentifier.o 	ClntOptElapsed.o 	ClntOptIAAddress.o      \
		ClntOptInterfaceID.o 		ClntOptPreference.o 	ClntOptRapidCommit.o    \
		ClntOptServerIdentifier.o 	ClntOptServerUnicast.o	ClntOptStatusCode.o     \
		ClntOptUserClass.o		ClntOptVendorClass.o 	ClntOptVendorSpecInfo.o \
		ClntOptIA_NA.o			ClntOptOptionRequest.o  ClntOptDNSServers.o     \
		ClntOptDomainName.o		ClntOptNTPServers.o     ClntOptTimeZone.o
	cd $(OPTIONS); $(MAKE) objs

ClntOptClientIdentifier.o: ClntOptClientIdentifier.cpp ClntOptClientIdentifier.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOpt.o: ClntOpt.cpp ClntOpt.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptElapsed.o: ClntOptElapsed.cpp ClntOptElapsed.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptIAAddress.o: ClntOptIAAddress.cpp ClntOptIAAddress.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptIA_NA.o: ClntOptIA_NA.cpp ClntOptIA_NA.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptInterfaceID.o: ClntOptInterfaceID.cpp ClntOptInterfaceID.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptOptionRequest.o: ClntOptOptionRequest.cpp ClntOptOptionRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptPreference.o: ClntOptPreference.cpp ClntOptPreference.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptRapidCommit.o: ClntOptRapidCommit.cpp ClntOptRapidCommit.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptServerIdentifier.o: ClntOptServerIdentifier.cpp ClntOptServerIdentifier.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptServerUnicast.o: ClntOptServerUnicast.cpp ClntOptServerUnicast.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptStatusCode.o: ClntOptStatusCode.cpp ClntOptStatusCode.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptUserClass.o: ClntOptUserClass.cpp ClntOptUserClass.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptVendorClass.o: ClntOptVendorClass.cpp ClntOptVendorClass.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptVendorSpecInfo.o: ClntOptVendorSpecInfo.cpp ClntOptVendorSpecInfo.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptDNSServers.o: ClntOptDNSServers.cpp ClntOptDNSServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptDomainName.o: ClntOptDomainName.cpp ClntOptDomainName.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptNTPServers.o: ClntOptNTPServers.cpp ClntOptNTPServers.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntOptTimeZone.o:   ClntOptTimeZone.cpp   ClntOptTimeZone.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.[oa]
	rm -f test1 