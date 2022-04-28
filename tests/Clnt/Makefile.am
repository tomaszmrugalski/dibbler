AUTOMAKE_OPTIONS = subdir-objects

Clnt_tests_CPPFLAGS  = -I$(top_srcdir)
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/IfaceMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/Misc
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/poslib
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/nettle
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntIfaceMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntCfgMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/CfgMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/Options
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntOptions
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/AddrMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntAddrMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntTransMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntMessages
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/Messages

# This is to workaround long long in gtest.h
Clnt_tests_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros
Clnt_tests_LDFLAGS = $(AM_LDFLAGS) $(GTEST_LDFLAGS) -pthread
info:
	@echo "GTEST_LDADD=$(GTEST_LDADD)"
	@echo "GTEST_LDFLAGS=$(GTEST_LDFLAGS)"
	@echo "GTEST_INCLUDES=$(GTEST_INCLUDES)"
	@echo "HAVE_GTEST=$(HAVE_GTEST)"

TESTS =
if HAVE_GTEST
TESTS += Clnt_tests

Clnt_tests_SOURCES = run_tests.cc
Clnt_tests_SOURCES += ClntIfaceMgr_unittest.cc

# ///Clnt_tests_SOURCES = $(top_srcdir)/@PORT_SUBDIR@/dibbler-client.cpp
Clnt_tests_SOURCES += $(top_srcdir)/Misc/DHCPClient.cpp $(top_srcdir)/Misc/DHCPClient.h

Clnt_tests_CPPFLAGS += -I$(top_srcdir)/Misc -I$(top_srcdir)/ClntTransMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntCfgMgr -I$(top_srcdir)/CfgMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/ClntOptions
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntIfaceMgr -I$(top_srcdir)/IfaceMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/ClntAddrMgr
Clnt_tests_CPPFLAGS += -I$(top_srcdir)/ClntMessages -I$(top_srcdir)/Messages

Clnt_tests_LDADD = $(GTEST_LDADD)
Clnt_tests_LDADD += -L$(top_builddir)/ClntTransMgr -lClntTransMgr
Clnt_tests_LDADD += -L$(top_builddir)/ClntMessages -lClntMessages
Clnt_tests_LDADD +=                                -lClntTransMgr
Clnt_tests_LDADD += -L$(top_builddir)/ClntOptions -lClntOptions
Clnt_tests_LDADD += -L$(top_builddir)/ClntCfgMgr -lClntCfgMgr

Clnt_tests_LDADD += -L$(top_builddir)/ClntIfaceMgr -lClntIfaceMgr
Clnt_tests_LDADD +=                  -lClntMessages -lClntCfgMgr
Clnt_tests_LDADD += -L$(top_builddir)/CfgMgr -lCfgMgr
Clnt_tests_LDADD += -L$(top_builddir)/ClntAddrMgr -lClntAddrMgr
Clnt_tests_LDADD += -L$(top_builddir)/IfaceMgr -lIfaceMgr

Clnt_tests_LDADD += -L$(top_builddir)/AddrMgr -lAddrMgr
Clnt_tests_LDADD += -L$(top_builddir)/poslib -lPoslib
Clnt_tests_LDADD += -L$(top_builddir)/nettle -lNettle
Clnt_tests_LDADD += -L$(top_builddir)/Options -lOptions
Clnt_tests_LDADD += -L$(top_builddir)/Messages -lMessages

Clnt_tests_LDADD +=                  -lOptions -lMessages
Clnt_tests_LDADD += -L$(top_builddir)/@PORT_SUBDIR@ -lLowLevel
Clnt_tests_LDADD += -L$(top_builddir)/Misc -lMisc -lClntOptions -lOptions

endif

noinst_PROGRAMS = $(TESTS)
