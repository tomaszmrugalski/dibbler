AM_CPPFLAGS  = -I$(top_srcdir)/AddrMgr
AM_CPPFLAGS += -I$(top_srcdir)/Misc

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long

# Darwin quirks:
# dyld can't find proper lib if make instal was not done for gtest

# Original content of GTEST_ADD (retrned by: gtest-config --libs)
#GTEST_LDADD = /Users/thomson/devel/gtest-1.6.0/lib/libgtest.la

# Library that would do the trick
#GTEST_LDADD = /Users/thomson/devel/gtest-1.6.0/lib/.libs/libgtest.a

# At runtime this could be solved with:
#export DYLD_LIBRARY_PATH=/Users/thomson/devel/gtest-1.6.0/lib/.libs/

info:
	@echo "GTEST_LDADD=$(GTEST_LDADD)"
	@echo "GTEST_LDFLAGS=$(GTEST_LDFLAGS)"
	@echo "GTEST_INCLUDES=$(GTEST_INCLUDES)"
	@echo "HAVE_GTEST=$(HAVE_GTEST)"


TESTS = 
if HAVE_GTEST
TESTS += AddrMgr_tests

AddrMgr_tests_SOURCES = run_tests.cpp
AddrMgr_tests_SOURCES += AddrAddr_unittest.cc

AddrMgr_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS)

AddrMgr_tests_LDADD = $(GTEST_LDADD)
AddrMgr_tests_LDADD += $(top_builddir)/AddrMgr/libAddrMgr.a
AddrMgr_tests_LDADD += $(top_builddir)/Misc/libMisc.a
endif

noinst_PROGRAMS = $(TESTS)
