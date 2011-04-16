include ../Makefile.inc

OBJECTS = SrvOptClientIdentifier.o	SrvOptElapsed.o       SrvOptIAAddress.o      \
          SrvOptInterfaceID.o 		SrvOptPreference.o                           \
          SrvOptServerIdentifier.o 	SrvOptServerUnicast.o SrvOptStatusCode.o     \
	  SrvOptUserClass.o 		\
	  SrvOptIA_NA.o 		SrvOptOptionRequest.o SrvOptDNSServers.o     \
	  SrvOptDomainName.o        	SrvOptNTPServers.o    SrvOptTimeZone.o       \
          SrvOptSIPServer.o         	SrvOptSIPDomain.o     SrvOptFQDN.o           \
          SrvOptNISServer.o         	SrvOptNISDomain.o     SrvOptNISPServer.o     \
          SrvOptNISPDomain.o        	SrvOptLifetime.o      SrvOptTA.o             \
	  SrvOptIA_PD.o                 SrvOptIAPrefix.o      SrvOptAddrParams.o     \
	  SrvOptLQ.o                    SrvOptEcho.o

ifndef MOD_DISABLE_AUTH
OBJECTS += SrvOptAuthentication.o SrvOptKeyGeneration.o SrvOptAAAAuthentication.o
endif

objs: $(OBJECTS)


libs: libSrvOptions.a
