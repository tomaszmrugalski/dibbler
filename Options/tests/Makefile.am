AM_CPPFLAGS  = -I$(top_srcdir)/Options
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
TESTS += Opt_tests

Opt_tests_SOURCES = run_tests.cpp
Opt_tests_SOURCES += Opt_unittest.cc
Opt_tests_SOURCES += OptAddr_unittest.cc
Opt_tests_SOURCES += OptAuthentication_unittest.cc
Opt_tests_SOURCES += OptIAAddress_unittest.cc
Opt_tests_SOURCES += OptFQDN_unittest.cc
Opt_tests_SOURCES += OptRtPrefix_unittest.cc
Opt_tests_SOURCES += OptDomainLst_unittest.cc
Opt_tests_SOURCES += OptUserClass_unittest.cc
Opt_tests_SOURCES += OptVendorClass_unittest.cc
Opt_tests_SOURCES += OptVendorSpecInfo_unittest.cc
Opt_tests_SOURCES += OptVendorData_unittest.cc

Opt_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

Opt_tests_LDADD = $(GTEST_LDADD)
Opt_tests_LDADD += $(top_builddir)/Options/libOptions.a
Opt_tests_LDADD += $(top_builddir)/Messages/libMessages.a
Opt_tests_LDADD += $(top_builddir)/Misc/libMisc.a
Opt_tests_LDADD += $(top_builddir)/@PORT_SUBDIR@/libLowLevel.a

endif

noinst_PROGRAMS = $(TESTS)
