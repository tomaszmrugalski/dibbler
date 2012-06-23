noinst_LIBRARIES = libLowLevel.a

libLowLevel_a_CPPFLAGS = -I$(top_srcdir)/Misc

libLowLevel_a_CFLAGS = -D__EXTENSIONS__ -D_XOPEN_SOURCE=1 -D_XOPEN_SOURCE_EXTENDED=1 $(CFLAGS)

libLowLevel_a_SOURCES = daemon.cpp daemon.h lowlevel-sun.c lowlevel-options-sun.c malloc.h dibbler-client.cpp dibbler-server.cpp dibbler-relay.cpp
