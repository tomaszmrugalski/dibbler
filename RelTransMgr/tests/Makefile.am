AM_CPPFLAGS  = -I$(top_srcdir)/RelTransMgr
AM_CPPFLAGS += -I$(top_srcdir)/Options
AM_CPPFLAGS += -I$(top_srcdir)/RelOptions
AM_CPPFLAGS += -I$(top_srcdir)/Messages
AM_CPPFLAGS += -I$(top_srcdir)/RelMessages
AM_CPPFLAGS += -I$(top_srcdir)/RelIfaceMgr
AM_CPPFLAGS += -I$(top_srcdir)/IfaceMgr
AM_CPPFLAGS += -I$(top_srcdir)/RelCfgMgr
AM_CPPFLAGS += -I$(top_srcdir)/CfgMgr
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
TESTS += RelTransMgr_tests

RelTransMgr_tests_SOURCES = run_tests.cpp
RelTransMgr_tests_SOURCES += RelTransMgr_unittest.cc

RelTransMgr_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

RelTransMgr_tests_LDADD = $(GTEST_LDADD)
RelTransMgr_tests_LDADD += $(top_builddir)/RelTransMgr/libRelTransMgr.a
RelTransMgr_tests_LDADD += $(top_builddir)/RelIfaceMgr/libRelIfaceMgr.a
RelTransMgr_tests_LDADD += $(top_builddir)/IfaceMgr/libIfaceMgr.a
RelTransMgr_tests_LDADD += $(top_builddir)/RelCfgMgr/libRelCfgMgr.a
RelTransMgr_tests_LDADD += $(top_builddir)/CfgMgr/libCfgMgr.a
RelTransMgr_tests_LDADD += $(top_builddir)/RelMessages/libRelMessages.a
RelTransMgr_tests_LDADD += $(top_builddir)/Messages/libMessages.a
RelTransMgr_tests_LDADD += $(top_builddir)/Misc/libMisc.a
RelTransMgr_tests_LDADD += $(top_builddir)/Options/libOptions.a
RelTransMgr_tests_LDADD += $(top_builddir)/RelOptions/libRelOptions.a
RelTransMgr_tests_LDADD += $(top_builddir)/@PORT_SUBDIR@/libLowLevel.a

endif

noinst_PROGRAMS = $(TESTS)
