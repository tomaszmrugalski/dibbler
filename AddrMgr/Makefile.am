SUBDIRS= .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libAddrMgr.a

libAddrMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc

libAddrMgr_a_SOURCES = AddrAddr.cpp AddrAddr.h AddrClient.cpp AddrClient.h AddrIA.cpp AddrIA.h AddrMgr.cpp AddrMgr.h AddrPrefix.cpp AddrPrefix.h
