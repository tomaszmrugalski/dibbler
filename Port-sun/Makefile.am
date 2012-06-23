noinst_LIBRARIES = libLowLevel.a

libLowLevel_a_CPPFLAGS = -I$(top_srcdir)/Misc

# Don't use -std=c99
libLowLevel_a_CFLAGS = $(PORT_CFLAGS)

libLowLevel_a_SOURCES = daemon.cpp daemon.h lowlevel-sun.c lowlevel-options-sun.c malloc.h dibbler-client.cpp dibbler-server.cpp dibbler-relay.cpp
