SUBDIRS = .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libOptions.a

libOptions_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/Messages

libOptions_a_SOURCES  = OptAddr.cpp OptAddr.h
libOptions_a_SOURCES += OptAddrLst.cpp OptAddrLst.h
libOptions_a_SOURCES += OptAuthentication.cpp OptAuthentication.h
libOptions_a_SOURCES += Opt.cpp Opt.h
libOptions_a_SOURCES += OptDUID.cpp OptDUID.h
libOptions_a_SOURCES += OptEmpty.cpp OptEmpty.h
libOptions_a_SOURCES += OptFQDN.cpp OptFQDN.h
libOptions_a_SOURCES += OptGeneric.cpp OptGeneric.h
libOptions_a_SOURCES += OptIAAddress.cpp OptIAAddress.h
libOptions_a_SOURCES += OptIA_NA.cpp OptIA_NA.h
libOptions_a_SOURCES += OptIA_PD.cpp OptIA_PD.h
libOptions_a_SOURCES += OptIAPrefix.cpp OptIAPrefix.h
libOptions_a_SOURCES += OptInteger.cpp OptInteger.h
libOptions_a_SOURCES += OptOptionRequest.cpp OptOptionRequest.h
libOptions_a_SOURCES += OptStatusCode.cpp OptStatusCode.h
libOptions_a_SOURCES += OptString.cpp OptString.h
libOptions_a_SOURCES += OptDomainLst.cpp OptDomainLst.h
libOptions_a_SOURCES += OptTA.cpp OptTA.h
libOptions_a_SOURCES += OptUserClass.cpp OptUserClass.h
libOptions_a_SOURCES += OptVendorClass.cpp OptVendorClass.h
libOptions_a_SOURCES += OptVendorSpecInfo.cpp OptVendorSpecInfo.h 
libOptions_a_SOURCES += OptRtPrefix.cpp OptRtPrefix.h
libOptions_a_SOURCES += OptReconfigureMsg.cpp OptReconfigureMsg.h
libOptions_a_SOURCES += OptVendorData.cpp OptVendorData.h
