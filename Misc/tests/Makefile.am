AM_CPPFLAGS = -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/CfgMgr

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

TESTS = 
if HAVE_GTEST
TESTS += Misc_tests

Misc_tests_SOURCES = run_tests.cc
Misc_tests_SOURCES += IPv6Addr_unittest.cc
Misc_tests_SOURCES += DUID_unittest.cc
Misc_tests_SOURCES += SPtr_unittest.cc
Misc_tests_SOURCES += Container_unittest.cc

Misc_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

Misc_tests_LDADD = $(GTEST_LDADD)
Misc_tests_LDADD += $(top_builddir)/Misc/libMisc.a
endif

noinst_PROGRAMS = $(TESTS)
