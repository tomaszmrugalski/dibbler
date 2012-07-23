AM_CPPFLAGS  = -I$(top_srcdir)/CfgMgr
AM_CPPFLAGS += -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/poslib

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

noinst_LIBRARIES = libTestUtils.a

libTestUtils_a_SOURCES = poslib_utils.cc poslib_utils.h
