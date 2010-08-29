include ../Makefile.inc

OBJECTS =                           ClntOptElapsed.o 	       ClntOptIAAddress.o      \
	  ClntOptPreference.o                                  ClntOptAddrParams.o     \
	                                                       ClntOptStatusCode.o     \
	  ClntOptUserClass.o 	    ClntOptVendorClass.o       ClntOptVendorSpec.o     \
	  ClntOptIA_PD.o	    ClntOptOptionRequest.o     ClntOptDNSServers.o     \
	  ClntOptDomainName.o	    ClntOptNTPServers.o        ClntOptTimeZone.o       \
          ClntOptSIPServer.o        ClntOptSIPDomain.o         ClntOptFQDN.o           \
          ClntOptNISServer.o        ClntOptNISDomain.o         ClntOptNISPServer.o     \
          ClntOptNISPDomain.o       ClntOptLifetime.o          ClntOptTA.o             \
	  ClntOptIAPrefix.o         ClntOptIA_NA.o

ifndef MOD_DISABLE_AUTH
OBJECTS += ClntOptAuthentication.o ClntOptKeyGeneration.o    ClntOptAAAAuthentication.o
endif

objs: $(OBJECTS)

libs: libClntOptions.a

