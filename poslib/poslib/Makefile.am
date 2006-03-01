lib_LTLIBRARIES = libposlib.la
INCLUDES = -I$(top_srcdir)

SUBDIRS = . 

libposlib_la_LDFLAGS = -version-info $(VERINFO) -no-undefined
libposlib_la_includedir=$(includedir)/$(PACKAGE)-$(VERSION)/poslib

libposlib_la_include_HEADERS = \
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
  vsnprintf.h \
  w32poll.h

libposlib_la_SOURCES = \
  dnsmessage.cpp \
  domainfn.cpp \
  exception.cpp \
  lexfn.cpp \
  masterfile.cpp \
  postime.cpp \
  random.cpp \
  resolver.cpp \
  rr.cpp \
  socket.cpp \
  vsnprintf.cpp \
  w32poll.cpp


