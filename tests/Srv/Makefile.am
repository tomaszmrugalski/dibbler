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
AM_CPPFLAGS += -I$(top_srcdir)/SrvTransMgr
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
TESTS += Srv_tests

Srv_tests_SOURCES = run_tests.cpp
Srv_tests_SOURCES += assign_utils.cc assign_utils.h
Srv_tests_SOURCES += assign_addr_unittest.cc assign_prefix_unittest.cc
Srv_tests_SOURCES += options_unittest.cc
Srv_tests_SOURCES += relay_unittest.cc
Srv_tests_SOURCES += wireshark.cc

Srv_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

Srv_tests_LDADD = $(GTEST_LDADD)
Srv_tests_LDADD += $(top_builddir)/SrvTransMgr/libSrvTransMgr.a
Srv_tests_LDADD += $(top_builddir)/SrvCfgMgr/libSrvCfgMgr.a
Srv_tests_LDADD += $(top_builddir)/CfgMgr/libCfgMgr.a
Srv_tests_LDADD += $(top_builddir)/SrvIfaceMgr/libSrvIfaceMgr.a
Srv_tests_LDADD += $(top_builddir)/IfaceMgr/libIfaceMgr.a
Srv_tests_LDADD += $(top_builddir)/SrvAddrMgr/libSrvAddrMgr.a
Srv_tests_LDADD += $(top_builddir)/AddrMgr/libAddrMgr.a
Srv_tests_LDADD += $(top_builddir)/SrvMessages/libSrvMessages.a
Srv_tests_LDADD += $(top_builddir)/Messages/libMessages.a
Srv_tests_LDADD += $(top_builddir)/SrvOptions/libSrvOptions.a
Srv_tests_LDADD += $(top_builddir)/Options/libOptions.a
Srv_tests_LDADD += $(top_builddir)/Misc/libMisc.a
Srv_tests_LDADD += $(top_builddir)/poslib/libPoslib.a
Srv_tests_LDADD += $(top_builddir)/nettle/libNettle.a
Srv_tests_LDADD += $(top_builddir)/@PORT_SUBDIR@/libLowLevel.a
endif

noinst_PROGRAMS = $(TESTS)
