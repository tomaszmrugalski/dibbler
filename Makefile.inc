# === used tools ===
CXX          = g++
CC           = gcc
FLEX         = flex
BISONPP      = $(PREFIX)/bison++/bison++ -S $(PREFIX)/bison++/bison.cc -H $(PREFIX)/bison++/bison.h

# === compiler options ===
COPTS        = -D$(ARCH) $(PORT_CFLAGS) $(XMLCFLAGS) -I $(INCDIR) $(DEBUGINFO) -Wall -funsigned-char -O0 $(PEDANTIC)
OPTS         = -ftemplate-depth-40 $(COPTS) $(CFLAGS) $(CXXFLAGS)
CLNT_LDFLAGS = $(LDFLAGS)
SRV_LDFLAGS  = $(LDFLAGS)
REL_LDFLAGS  = $(LDFLAGS)

# debuging parameters
#PEDANTIC     = -padantic
DEBUGINFO    = -g
#EFENCE       = -lefence

# === libxml2 stuff ===
#XMLCFLAGS    = `pkg-config libxml-2.0 --cflags` -DLIBXML2
#XMLLIBS      = `pkg-config libxml-2.0 --libs`

LDFLAGS       = $(XMLLIBS) $(EFENCE) $(PORT_LDFLAGS)

# === Portability section ===
# Make sure that you have uncommented only one section

# === Port: Linux ===
ARCH         = LINUX
LOWLEVEL     = $(PREFIX)/$(PORTDIR)
CLIENTBIN    = dibbler-client
SERVERBIN    = dibbler-server
RELAYBIN     = dibbler-relay
CLIENT       = $(PORTDIR)/$(CLIENTBIN).cpp
SERVER       = $(PORTDIR)/$(SERVERBIN).cpp
RELAY        = $(PORTDIR)/$(RELAYBIN).cpp
PORT_LDFLAGS = 
PORT_CFLAGS  = 
PORT_SUBDIR  = Port-linux

# === Port: Win2k (mingw) ===
#ARCH         = WIN2K
#LOWLEVEL     = $(PREFIX)/$(PORTDIR)
#CLIENTBIN    = dibbler-client-win2k
#SERVERBIN    = dibbler-server-win2k
#RELAYBIN     = dibbler-relay-win2k
#CLIENT       = $(PORTDIR)/$(CLIENTBIN).cpp
#SERVER       = $(PORTDIR)/$(SERVERBIN).cpp
#RELAY        = $(PORTDIR)/$(RELAYBIN).cpp
#PORT_LDFLAGS = -lws2_32
#PORT_CFLAGS  = -DMINGWBUILD
#PORT_SUBDIR  = Port-win2k

# === do not modify anything below ===
MAKEFLAGS   += -s
VERSION      = `$(TOPDIR)/test/xtract_version`
INST_WORKDIR = $(DESTDIR)'/var/lib/dibbler'
INST_MANDIR  = $(DESTDIR)'/usr/local/share/man'
INST_DOCDIR  = $(DESTDIR)'/usr/local/share/doc'
INST_BINDIR  = $(DESTDIR)'/usr/local/sbin'
INST_CONFDIR = $(DESTDIR)'/etc/dibbler'
INSTALL      = 'install'
MKDIR        = mkdir -p
CP           = cp

PREFIX       = ..
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

COMMONSUBDIRS= IfaceMgr    AddrMgr      CfgMgr      Messages   Options      include Misc $(PORT_SUBDIR)
CLNTSUBDIRS  = ClntOptions ClntIfaceMgr ClntAddrMgr ClntCfgMgr ClntTransMgr ClntMessages 
SRVSUBDIRS   = SrvOptions  SrvIfaceMgr  SrvAddrMgr  SrvCfgMgr  SrvTransMgr  SrvMessages  
RELSUBDIRS   = RelTransMgr RelMessages RelOptions  RelIfaceMgr RelCfgMgr
SUBDIRS      = $(COMMONSUBDIRS) $(CLNTSUBDIRS) $(SRVSUBDIRS) $(RELSUBDIRS)

# === UNIVERSAL TARGETS ===

%.o: %.cpp %.h
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) $(OPTS) -c -o $@ $<

%.a: objs
	@echo "[LIB    ] $(SUBDIR)/$@"	
	ar cr $@ $(OBJECTS)

all: libs

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	rm -f *~ *.o *.a *.xml test? $(CLIENTBIN) $(SERVERBIN) $(RELAYBIN) TAGS
	find . -type l -exec rm {} \;
	find . -name *.a -exec rm {} \;
	find . -name *~ -exec rm {} \;
	find . -name *.o -exec rm {} \;
	find . -name test? -exec rm {} \;
	if [ -d bison++/.deps ]; then rm -rf bison++/.deps; fi
	if [ -d doc ]; then make -C doc clean; fi

# === SUBDIR NAMES ===
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))

ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 
