AM_CPPFLAGS  = -I$(top_srcdir)/CfgMgr
AM_CPPFLAGS += -I$(top_srcdir)/Misc
AM_CPPFLAGS += -I$(top_srcdir)/poslib

# This is to workaround long long in gtest.h
AM_CPPFLAGS += $(GTEST_INCLUDES) -Wno-long-long -Wno-variadic-macros

noinst_LIBRARIES = libTestUtils.a
if HAVE_GTEST_SOURCE
noinst_LIBRARIES += libgtest.a
endif

libTestUtils_a_SOURCES = poslib_utils.cc poslib_utils.h

libgtest_a_CXXFLAGS = $(GTEST_INCLUDES) $(AM_CXXFLAGS)
nodist_libgtest_a_SOURCES = gtest-all.cc

gtest-all.cc: $(GTEST_SOURCE)/src/gtest-all.cc
	cp -p $(GTEST_SOURCE)/src/gtest-all.cc $@
