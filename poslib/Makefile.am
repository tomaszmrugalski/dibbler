SUBDIRS = .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libPoslib.a
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/nettle -I$(top_srcdir)/Misc

libPoslib_a_SOURCES = \
  dnsmessage.cpp \
  domainfn.cpp \
  dnssec-sign.cpp \
  dnssec-sign.h \
  exception.cpp \
  lexfn.cpp \
  masterfile.cpp \
  postime.cpp \
  random.cpp \
  resolver.cpp \
  rr.cpp \
  socket.cpp \
  vsnprintf.cpp \
  bits.h \
  dnsmessage.h \
  dnsdefs.h \
  domainfn.h \
  exception.h \
  lexfn.h \
  masterfile.h \
  poslib.h \
  postime.h \
  random.h \
  resolver.h \
  rr.h \
  socket.h \
  syssocket.h \
  sysstl.h \
  sysstring.h \
  syssocket.h \
  types.h \
  vsnprintf.h


dist_noinst_DATA = w32poll.cpp w32poll.h ChangeLog-poslib
