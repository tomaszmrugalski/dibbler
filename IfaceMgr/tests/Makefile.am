AM_CPPFLAGS  = -I$(top_srcdir)
AM_CPPFLAGS += -I$(top_srcdir)/IfaceMgr
AM_CPPFLAGS += -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/poslib
AM_CPPFLAGS += -I$(top_srcdir)/nettle

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

TESTS = 
if HAVE_GTEST
TESTS += DnsUpdate_tests

DnsUpdate_tests_SOURCES = run_tests.cc
DnsUpdate_tests_SOURCES += DnsUpdate_unittest.cc

DnsUpdate_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

DnsUpdate_tests_LDADD = $(GTEST_LDADD)
DnsUpdate_tests_LDADD += $(top_builddir)/IfaceMgr/libIfaceMgr.a
DnsUpdate_tests_LDADD += $(top_builddir)/Misc/libMisc.a
DnsUpdate_tests_LDADD += $(top_builddir)/poslib/libPoslib.a
DnsUpdate_tests_LDADD += $(top_builddir)/nettle/libNettle.a
DnsUpdate_tests_LDADD += $(top_builddir)/tests/utils/libTestUtils.a
endif

noinst_PROGRAMS = $(TESTS)
