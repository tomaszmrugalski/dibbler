# === used tools ===
CXX          ?= g++
CXXLD        ?= $(CXX)
CC           ?= gcc
FLEX         = flex
BISONPP      = $(PREFIX)/bison++/bison++ -S $(PREFIX)/bison++/bison.cc -H $(PREFIX)/bison++/bison.h

# === compiler options ===
# C only flags
CFLAGS      += -std=c99 $(CPPFLAGS)
# common flags
CPPFLAGS     = -D$(ARCH) $(PORT_CFLAGS) $(XMLCFLAGS) -I $(INCDIR) $(DEBUGINFO) -Wall -funsigned-char -O2 $(PEDANTIC)
# C++ specific flags
CXXFLAGS     = -ftemplate-depth-40 $(COPTS) $(CPPFLAGS)

# === linker options ===
LDFLAGS      += $(XMLLIBS) $(EFENCE) $(PORT_LDFLAGS)
CLNT_LDFLAGS = $(LDFLAGS)
SRV_LDFLAGS  = $(LDFLAGS)
REL_LDFLAGS  = $(LDFLAGS)

# === debuging parameters ===
PEDANTIC     = -pedantic
#DEBUGINFO    = -g
#EFENCE       = -lefence

# === modular features ===

# See Developer Guide, section named "Modular features" for
# detailed description of those parameters

MOD_CLNT_BIND_REUSE        = y
#MOD_CLNT_EMBEDDED_CFG      = y
#MOD_CLNT_DISABLE_DNSUPDATE = y
#MOD_SRV_DISABLE_DNSUPDATE  = y
#MOD_DISABLE_AUTH      = y
MOD_CLNT_CONFIRM = y
#MOD_REMOTE_AUTOCONF = y
#MOD_LIBXML2 = y
#MOD_DEBUG = y


# === Portability section ===
# Make sure that you have uncommented only one section

# === Port: Linux ===
ARCH         = LINUX
PORT_LDFLAGS ?= -lpthread
PORT_CFLAGS  ?= 
PORT_SUBDIR  = Port-linux

# === Port: Mac OS ===
#ARCH          = MACOS
#PORT_LDFLAGS ?=
#PORT_LDFLAGS ?=
#PORT_SUBDIR   = Port-macos

# === Port: Win2k (mingw) ===
#ARCH         = WIN2K
#PORT_LDFLAGS = -lws2_32
#PORT_CFLAGS  = -DMINGWBUILD
#PORT_SUBDIR  = Port-win2k

# === libxml2 stuff ===
#XMLCFLAGS    = `pkg-config libxml-2.0 --cflags` -DLIBXML2
#XMLLIBS      = `pkg-config libxml-2.0 --libs`

# === do not modify anything below ===
SILENTMAKE    = -s
MAKEFLAGS   += $(SILENTMAKE)
VERSION      = `$(TOPDIR)/test/xtract_version`
INST_WORKDIR = $(DESTDIR)'/var/lib/dibbler'
INST_MANDIR  = $(DESTDIR)'/usr/local/man'
INST_DOCDIR  = $(DESTDIR)'/usr/local/share/doc'
INST_BINDIR  = $(DESTDIR)'/usr/local/sbin'
INST_CONFDIR = $(DESTDIR)'/etc/dibbler'
INSTALL      = 'install'
MKDIR        = mkdir -p
CP           = cp


PREFIX       = $(TOPDIR)
PORTDIR      = $(PREFIX)/$(PORT_SUBDIR)
INCDIR       = $(PREFIX)/include
ADDRMGR      = $(PREFIX)/AddrMgr
CFGMGR       = $(PREFIX)/CfgMgr
IFACEMGR     = $(PREFIX)/IfaceMgr
MESSAGES     = $(PREFIX)/Messages
OPTIONS      = $(PREFIX)/Options
MISC         = $(PREFIX)/Misc

CLNTIFACEMGR= $(PREFIX)/ClntIfaceMgr
CLNTTRANSMGR= $(PREFIX)/ClntTransMgr
CLNTADDRMGR = $(PREFIX)/ClntAddrMgr
CLNTCFGMGR  = $(PREFIX)/ClntCfgMgr
CLNTPARSER  = $(PREFIX)/ClntParser
CLNTMESSAGES= $(PREFIX)/ClntMessages
CLNTOPTIONS = $(PREFIX)/ClntOptions

SRVIFACEMGR = $(PREFIX)/SrvIfaceMgr
SRVTRANSMGR = $(PREFIX)/SrvTransMgr
SRVADDRMGR  = $(PREFIX)/SrvAddrMgr
SRVCFGMGR   = $(PREFIX)/SrvCfgMgr
SRVPARSER   = $(PREFIX)/SrvParser
SRVMESSAGES = $(PREFIX)/SrvMessages
SRVOPTIONS  = $(PREFIX)/SrvOptions

RELIFACEMGR = $(PREFIX)/RelIfaceMgr
RELTRANSMGR = $(PREFIX)/RelTransMgr
RELCFGMGR   = $(PREFIX)/RelCfgMgr
RELMESSAGES = $(PREFIX)/RelMessages
RELOPTIONS  = $(PREFIX)/RelOptions

POSLIB	  = $(PREFIX)/poslib/poslib/.libs

COMMONSUBDIRS= IfaceMgr    AddrMgr      CfgMgr      Messages   Options      include Misc $(PORT_SUBDIR)
CLNTSUBDIRS  = ClntOptions ClntIfaceMgr ClntAddrMgr ClntCfgMgr ClntTransMgr ClntMessages 
SRVSUBDIRS   = SrvOptions  SrvIfaceMgr  SrvAddrMgr  SrvCfgMgr  SrvTransMgr  SrvMessages  
RELSUBDIRS   = RelTransMgr RelMessages RelOptions  RelIfaceMgr RelCfgMgr
REQUESTORDIRS= Requestor   Options Misc IfaceMgr Messages Port-linux
POSLIBDIRS   = poslib	  poslib/poslib
SUBDIRS      = $(COMMONSUBDIRS) $(CLNTSUBDIRS) $(SRVSUBDIRS) $(RELSUBDIRS) $(POSLIBDIRS)

LOWLEVEL     = $(PREFIX)/$(PORTDIR)
CLIENTBIN    = dibbler-client
SERVERBIN    = dibbler-server
RELAYBIN     = dibbler-relay
REQUESTORBIN = dibbler-requestor
CLIENT       = $(PORTDIR)/$(CLIENTBIN).cpp
SERVER       = $(PORTDIR)/$(SERVERBIN).cpp
RELAY        = $(PORTDIR)/$(RELAYBIN).cpp


# === UNIVERSAL TARGETS ===

%.o: %.cpp %.h
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.a: objs
	@echo "[LIB    ] $(SUBDIR)/$@"	
	$(AR) cr $@ $(OBJECTS)

all: libs

objs: $(OBJECTS)

FINDOPTS = -path './poslib/*' -prune -o

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	rm -f *~ *.o *.a $(CLIENTBIN) $(SERVERBIN) $(RELAYBIN) $(REQUESTORBIN) TAGS
	rm -f $(TESTS) $(OBJECTS)
	find . $(FINDOPTS) -type l -exec rm {} \;
	find . $(FINDOPTS) -name *.a -exec rm {} \;
	find . $(FINDOPTS) -name *~ -exec rm {} \;
	find . $(FINDOPTS) -name *.o -exec rm {} \;
	find . $(FINDOPTS) -name test? -exec rm {} \;
	if [ -d bison++/.deps ]; then rm -rf bison++/.deps; fi
	if [ -d doc ]; then make -C doc clean; fi

# === SUBDIR NAMES ===
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))

ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 


# === translate Makefile defines into C/C++ defines ===
ifdef MOD_CLNT_EMBEDDED_CFG
COPTS += -DMOD_CLNT_EMBEDDED_CFG
endif

ifdef MOD_CLNT_DISABLE_DNSUPDATE
COPTS += -DMOD_CLNT_DISABLE_DNSUPDATE
endif

ifdef MOD_CLNT_BIND_REUSE
COPTS += -DMOD_CLNT_BIND_REUSE
endif

ifdef MOD_CLNT_CONFIRM
COPTS += -DMOD_CLNT_CONFIRM
endif

ifdef MOD_SRV_DISABLE_DNSUPDATE
COPTS += -DMOD_SRV_DISABLE_DNSUPDATE
endif

ifdef MOD_DISABLE_AUTH
COPTS += -DMOD_DISABLE_AUTH
endif

ifdef MOD_REMOTE_AUTOCONF
COPTS += -DMOD_REMOTE_AUTOCONF
endif

ifdef MOD_LIBXML2
COPTS += -DMOD_LIBXML2 -I/usr/include/libxml2
CLNT_LDFLAGS += -lxml2
endif

ifdef MOD_DEBUG
COPTS += -DDEBUG
endif
