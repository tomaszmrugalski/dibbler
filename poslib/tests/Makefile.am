AM_CPPFLAGS  = -I$(top_srcdir)
AM_CPPFLAGS += -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/poslib
AM_CPPFLAGS += -I$(top_srcdir)/nettle

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

TESTS =
if HAVE_GTEST
TESTS += tsig_tests

tsig_tests_SOURCES = run_tests.cc
tsig_tests_SOURCES += tsig_unittest.cc

tsig_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

tsig_tests_LDADD = $(GTEST_LDADD)
tsig_tests_LDADD += $(top_builddir)/Misc/libMisc.a
tsig_tests_LDADD += $(top_builddir)/poslib/libPoslib.a
tsig_tests_LDADD += $(top_builddir)/nettle/libNettle.a
tsig_tests_LDADD += $(top_builddir)/tests/utils/libTestUtils.a
endif

noinst_PROGRAMS = $(TESTS)
