AUTOMAKE_OPTIONS = foreign

CODE_SUBDIRS = Misc Options AddrMgr CfgMgr ClntAddrMgr ClntCfgMgr ClntIfaceMgr ClntMessages
CODE_SUBDIRS += ClntOptions ClntTransMgr IfaceMgr Messages
CODE_SUBDIRS += RelCfgMgr RelIfaceMgr RelMessages RelOptions RelTransMgr
CODE_SUBDIRS += Requestor SrvAddrMgr SrvCfgMgr SrvIfaceMgr SrvMessages
CODE_SUBDIRS += SrvOptions SrvTransMgr doc
CODE_SUBDIRS += poslib

SUBDIRS = $(CODE_SUBDIRS) @PORT_SUBDIR@

DIST_SUBDIRS = $(CODE_SUBDIRS) Port-bsd Port-linux Port-win32 Port-winnt2k bison++

sbin_PROGRAMS = dibbler-client dibbler-server dibbler-relay dibbler-requestor

client: dibbler-client

dibbler_client_SOURCES = $(top_srcdir)/@PORT_SUBDIR@/dibbler-client.cpp
dibbler_client_SOURCES += $(top_srcdir)/Misc/DHCPClient.cpp $(top_srcdir)/Misc/DHCPClient.h

dibbler_client_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/ClntTransMgr
dibbler_client_CPPFLAGS += -I$(top_srcdir)/ClntCfgMgr -I$(top_srcdir)/CfgMgr
dibbler_client_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/ClntOptions
dibbler_client_CPPFLAGS += -I$(top_srcdir)/ClntIfaceMgr -I$(top_srcdir)/IfaceMgr
dibbler_client_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/ClntAddrMgr
dibbler_client_CPPFLAGS += -I$(top_srcdir)/ClntMessages -I$(top_srcdir)/Messages

dibbler_client_LDADD = -L$(top_builddir)/ClntTransMgr -lClntTransMgr
dibbler_client_LDADD += -L$(top_builddir)/ClntMessages -lClntMessages
dibbler_client_LDADD +=                                -lClntTransMgr
dibbler_client_LDADD += -L$(top_builddir)/ClntOptions -lClntOptions
dibbler_client_LDADD += -L$(top_builddir)/ClntCfgMgr -lClntCfgMgr
dibbler_client_LDADD += -L$(top_builddir)/ClntIfaceMgr -lClntIfaceMgr
dibbler_client_LDADD +=                  -lClntMessages -lClntCfgMgr
dibbler_client_LDADD += -L$(top_builddir)/CfgMgr -lCfgMgr
dibbler_client_LDADD += -L$(top_builddir)/ClntAddrMgr -lClntAddrMgr
dibbler_client_LDADD += -L$(top_builddir)/IfaceMgr -lIfaceMgr
dibbler_client_LDADD += -L$(top_builddir)/AddrMgr -lAddrMgr
dibbler_client_LDADD += -L$(top_builddir)/poslib -lposlib
dibbler_client_LDADD += -L$(top_builddir)/Options -lOptions
dibbler_client_LDADD += -L$(top_builddir)/Messages -lMessages
dibbler_client_LDADD +=                  -lOptions -lMessages
dibbler_client_LDADD += -L$(top_builddir)/@PORT_SUBDIR@ -lLowLevel
dibbler_client_LDADD += -L$(top_builddir)/Misc -lMisc -lClntOptions -lOptions

server: dibbler-server

dibbler_server_SOURCES = $(top_srcdir)/@PORT_SUBDIR@/dibbler-server.cpp
dibbler_server_SOURCES += $(top_srcdir)/Misc/DHCPServer.cpp $(top_srcdir)/Misc/DHCPServer.h

dibbler_server_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/SrvTransMgr
dibbler_server_CPPFLAGS += -I$(top_srcdir)/SrvCfgMgr -I$(top_srcdir)/CfgMgr
dibbler_server_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/SrvOptions
dibbler_server_CPPFLAGS += -I$(top_srcdir)/SrvIfaceMgr -I$(top_srcdir)/IfaceMgr
dibbler_server_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/SrvAddrMgr
dibbler_server_CPPFLAGS += -I$(top_srcdir)/SrvMessages -I$(top_srcdir)/Messages

dibbler_server_LDADD = -L$(top_builddir)/SrvOptions -lSrvOptions
dibbler_server_LDADD += -L$(top_builddir)/SrvMessages -lSrvMessages
dibbler_server_LDADD += -L$(top_builddir)/SrvIfaceMgr -lSrvIfaceMgr
dibbler_server_LDADD += -L$(top_builddir)/SrvCfgMgr -lSrvCfgMgr
dibbler_server_LDADD += -lSrvOptions -lSrvMessages -lSrvIfaceMgr -lSrvCfgMgr
dibbler_server_LDADD += -L$(top_builddir)/SrvTransMgr -lSrvTransMgr
dibbler_server_LDADD += -L$(top_builddir)/SrvAddrMgr -lSrvAddrMgr
dibbler_server_LDADD += -L$(top_builddir)/IfaceMgr -lIfaceMgr
dibbler_server_LDADD += -L$(top_builddir)/AddrMgr -lAddrMgr
dibbler_server_LDADD += -L$(top_builddir)/poslib -lposlib
dibbler_server_LDADD += -L$(top_builddir)/Options -lOptions
dibbler_server_LDADD += -L$(top_builddir)/Messages -lMessages
dibbler_server_LDADD += -L$(top_builddir)/CfgMgr -lCfgMgr
dibbler_server_LDADD += -L$(top_builddir)/@PORT_SUBDIR@ -lLowLevel
dibbler_server_LDADD += -L$(top_builddir)/Misc -lMisc
dibbler_server_LDADD += -lSrvCfgMgr -lSrvMessages -lCfgMgr -lSrvOptions -lOptions
dibbler_server_LDADD += -lIfaceMgr -lposlib -lMisc

relay: dibbler-relay

dibbler_relay_SOURCES = $(top_srcdir)/@PORT_SUBDIR@/dibbler-relay.cpp
dibbler_relay_SOURCES += $(top_srcdir)/Misc/DHCPRelay.cpp $(top_srcdir)/Misc/DHCPRelay.h

dibbler_relay_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/RelTransMgr
dibbler_relay_CPPFLAGS += -I$(top_srcdir)/RelCfgMgr -I$(top_srcdir)/CfgMgr
dibbler_relay_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/RelOptions
dibbler_relay_CPPFLAGS += -I$(top_srcdir)/RelIfaceMgr -I$(top_srcdir)/IfaceMgr
dibbler_relay_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/RelAddrMgr
dibbler_relay_CPPFLAGS += -I$(top_srcdir)/RelMessages -I$(top_srcdir)/Messages

dibbler_relay_LDADD = -L$(top_builddir)/RelTransMgr -lRelTransMgr
dibbler_relay_LDADD += -L$(top_builddir)/RelIfaceMgr -lRelIfaceMgr
dibbler_relay_LDADD += -L$(top_builddir)/RelCfgMgr -lRelCfgMgr
dibbler_relay_LDADD += -lRelIfaceMgr
dibbler_relay_LDADD += -L$(top_builddir)/RelMessages -lRelMessages
dibbler_relay_LDADD += -lRelCfgMgr
dibbler_relay_LDADD += -L$(top_builddir)/CfgMgr -lCfgMgr
dibbler_relay_LDADD += -L$(top_builddir)/RelOptions -lRelOptions
dibbler_relay_LDADD += -L$(top_builddir)/IfaceMgr -lIfaceMgr
dibbler_relay_LDADD += -L$(top_builddir)/poslib -lposlib
dibbler_relay_LDADD += -L$(top_builddir)/Options -lOptions
dibbler_relay_LDADD += -L$(top_builddir)/Messages -lMessages
dibbler_relay_LDADD += -L$(top_builddir)/@PORT_SUBDIR@ -lLowLevel
dibbler_relay_LDADD += -L$(top_builddir)/Misc -lMisc

requestor: dibbler-requestor

dibbler_requestor_SOURCES = $(top_srcdir)/Requestor/Requestor.cpp $(top_srcdir)/Requestor/Requestor.h

dibbler_requestor_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/Options
dibbler_requestor_CPPFLAGS += -I$(top_srcdir)/Messages -I$(top_srcdir)/Requestor
dibbler_requestor_CPPFLAGS += -I$(top_srcdir)/IfaceMgr

dibbler_requestor_LDADD = -L$(top_builddir)/Requestor -lRequestor
dibbler_requestor_LDADD += -L$(top_builddir)/Messages -lMessages
dibbler_requestor_LDADD += -L$(top_builddir)/IfaceMgr -lIfaceMgr
dibbler_requestor_LDADD += -L$(top_builddir)/Misc -lMisc
dibbler_requestor_LDADD += -L$(top_builddir)/Options -lOptions
dibbler_requestor_LDADD += -L$(top_builddir)/@PORT_SUBDIR@ -lLowLevel

nobase_dist_doc_DATA = CHANGELOG LICENSE RELNOTES
nobase_dist_doc_DATA += scripts/notify-scripts/client-notify.sh scripts/notify-scripts/server-notify.sh
nobase_dist_doc_DATA += scripts/INFO scripts/remote-autoconf scripts/xtract_version

dist_noinst_DATA = m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
dist_noinst_DATA += scripts
dist_noinst_DATA += tests

# these are conditional directories. Therefore they are not added to
# dist directory.


bison: bison/bison++

bison/bison++:
		@echo "[CONFIG ] /bison++/"
		cd $(top_srcdir)/bison++; ./configure --host=$(CHOST) --build=$(CBUILD) >configure.log
		@echo "[MAKE   ] /bison++/bison++"
		$(MAKE) -C $(top_srcdir)/bison++

parser:
		$(MAKE) -C SrvCfgMgr parser
		$(MAKE) -C ClntCfgMgr parser
		$(MAKE) -C RelCfgMgr parser
