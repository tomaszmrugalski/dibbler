SUBDIRS = .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libOptions.a

libOptions_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/Messages

libOptions_a_SOURCES = OptAAAAuthentication.cpp OptAAAAuthentication.h OptAddr.cpp OptAddr.h OptAddrLst.cpp OptAddrLst.h OptAuthentication.cpp OptAuthentication.h Opt.cpp OptDUID.cpp OptDUID.h OptEmpty.cpp OptEmpty.h OptFQDN.cpp OptFQDN.h OptGeneric.cpp OptGeneric.h Opt.h OptIAAddress.cpp OptIAAddress.h OptIA_NA.cpp OptIA_NA.h OptIA_PD.cpp OptIA_PD.h OptIAPrefix.cpp OptIAPrefix.h OptInteger.cpp OptInteger.h OptKeyGeneration.cpp OptKeyGeneration.h OptOptionRequest.cpp OptOptionRequest.h OptStatusCode.cpp OptStatusCode.h OptString.cpp OptString.h OptStringLst.cpp OptStringLst.h OptTA.cpp OptTA.h OptUserClass.cpp OptUserClass.h OptVendorData.cpp OptVendorData.h OptVendorSpecInfo.cpp OptVendorSpecInfo.h OptRtPrefix.cpp OptRtPrefix.h
