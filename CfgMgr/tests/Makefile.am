AM_CPPFLAGS = -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/CfgMgr

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

TESTS = 
if HAVE_GTEST
TESTS += CfgMgr_tests

CfgMgr_tests_SOURCES = run_tests.cc
CfgMgr_tests_SOURCES += HostID_unittest.cc
CfgMgr_tests_SOURCES += HostRange_unittest.cc

CfgMgr_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

CfgMgr_tests_LDADD = $(GTEST_LDADD)
CfgMgr_tests_LDADD += $(top_builddir)/CfgMgr/libCfgMgr.a
CfgMgr_tests_LDADD += $(top_builddir)/Misc/libMisc.a
endif

noinst_PROGRAMS = $(TESTS)
