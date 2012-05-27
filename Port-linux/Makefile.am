noinst_LIBRARIES = libLowLevel.a

libLowLevel_a_CPPFLAGS = -I$(top_srcdir)/Misc

libLowLevel_a_SOURCES = daemon.cpp daemon.h ethtool-kernel.h ethtool-local.h
libLowLevel_a_SOURCES += interface.c interface.h ip_common.h iproute.c
libLowLevel_a_SOURCES += libnetlink.c libnetlink.h
libLowLevel_a_SOURCES += lowlevel-linux.c lowlevel-linux-link-state.c
libLowLevel_a_SOURCES += lowlevel-options-linux.c rtm_map.h rt_names.h utils.c utils.h

# that's ugly hack. Those files should not be built here. They are compiled during link of respective daemons.
# As far as this makefile is concerned, they should be treated as data, so they are included in make dist
dist_noinst_DATA = dibbler-client.cpp dibbler-relay.cpp dibbler-server.cpp
