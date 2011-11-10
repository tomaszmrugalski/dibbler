AM_CPPFLAGS  = -I$(top_srcdir)/Options
AM_CPPFLAGS += -I$(top_srcdir)/Misc

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long

info:
        @echo "GTEST_LDADD=$(GTEST_LDADD)"
        @echo "GTEST_LDFLAGS=$(GTEST_LDFLAGS)"
        @echo "GTEST_INCLUDES=$(GTEST_INCLUDES)"
        @echo "HAVE_GTEST=$(HAVE_GTEST)"

TESTS =
if HAVE_GTEST
TESTS += Opt_tests

Opt_tests_SOURCES = run_tests.cpp
Opt_tests_SOURCES += OptAddr_unittest.cc
Opt_tests_SOURCES += OptIAAddress_unittest.cc
Opt_tests_SOURCES += OptRtPrefix_unittest.cc

Opt_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

Opt_tests_LDADD = $(GTEST_LDADD)
Opt_tests_LDADD += $(top_builddir)/Options/libOptions.a
Opt_tests_LDADD += $(top_builddir)/Misc/libMisc.a
endif

noinst_PROGRAMS = $(TESTS)
