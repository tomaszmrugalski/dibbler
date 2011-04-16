include ../Makefile.inc

OBJECTS = Opt.o               OptIAAddress.o         OptGeneric.o         OptTA.o                \
                              OptEmpty.o             OptInteger.o         OptDUID.o              \
          OptStatusCode.o     OptUserClass.o         OptVendorData.o      OptVendorSpecInfo.o    \
          OptIA_NA.o          OptOptionRequest.o     OptAddr.o            OptAddrLst.o           \
          OptString.o         OptStringLst.o         OptGeneric.o         OptFQDN.o              \
          OptIA_PD.o          OptIAPrefix.o

ifndef MOD_DISABLE_AUTH
OBJECTS += OptAuthentication.o  OptAAAAuthentication.o OptKeyGeneration.o
endif

objs: $(OBJECTS)

libs: libOptions.a
