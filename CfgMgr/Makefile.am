SUBDIRS = .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libCfgMgr.a

libCfgMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/IfaceMgr

libCfgMgr_a_SOURCES = CfgMgr.cpp CfgMgr.h FlexLexer.h
libCfgMgr_a_SOURCES += HostID.cpp HostID.h HostRange.cpp HostRange.h
