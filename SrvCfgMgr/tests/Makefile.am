AM_CPPFLAGS  = -I$(top_srcdir)/SrvCfgMgr
AM_CPPFLAGS += -I$(top_srcdir)/CfgMgr
AM_CPPFLAGS += -I$(top_srcdir)/SrvIfaceMgr
AM_CPPFLAGS += -I$(top_srcdir)/IfaceMgr
AM_CPPFLAGS += -I$(top_srcdir)/AddrMgr
AM_CPPFLAGS += -I$(top_srcdir)/SrvAddrMgr
AM_CPPFLAGS += -I$(top_srcdir)/Options
AM_CPPFLAGS += -I$(top_srcdir)/SrvOptions
AM_CPPFLAGS += -I$(top_srcdir)/Messages
AM_CPPFLAGS += -I$(top_srcdir)/SrvMessages
AM_CPPFLAGS += -I$(top_srcdir)/Misc

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

info:
	@echo "GTEST_LDADD=$(GTEST_LDADD)"
	@echo "GTEST_LDFLAGS=$(GTEST_LDFLAGS)"
	@echo "GTEST_INCLUDES=$(GTEST_INCLUDES)"
	@echo "HAVE_GTEST=$(HAVE_GTEST)"

TESTS =
if HAVE_GTEST
TESTS += SrvCfgMgr_tests

SrvCfgMgr_tests_SOURCES = run_tests.cpp
SrvCfgMgr_tests_SOURCES += SrvCfgMgr_unittest.cc
SrvCfgMgr_tests_SOURCES += expressions_unittest.cc

SrvCfgMgr_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

SrvCfgMgr_tests_LDADD = $(GTEST_LDADD)
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvTransMgr/libSrvTransMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvCfgMgr/libSrvCfgMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/CfgMgr/libCfgMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvIfaceMgr/libSrvIfaceMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/IfaceMgr/libIfaceMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvAddrMgr/libSrvAddrMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/AddrMgr/libAddrMgr.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvMessages/libSrvMessages.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/Messages/libMessages.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/SrvOptions/libSrvOptions.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/Options/libOptions.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/Misc/libMisc.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/poslib/libPoslib.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/nettle/libNettle.a
SrvCfgMgr_tests_LDADD += $(top_builddir)/@PORT_SUBDIR@/libLowLevel.a

dist_noinst_DATA = testdata/info.txt testdata/keys-mapping
endif

noinst_PROGRAMS = $(TESTS)
