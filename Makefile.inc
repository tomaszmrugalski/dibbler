# === used tools ===
CPP          = g++
CC           = gcc
FLEX         = flex
BISONPP      = $(PREFIX)/bison++/bison++ -S $(PREFIX)/bison++/bison.cc -H $(PREFIX)/bison++/bison.h

# === compiler options ===
COPTS        = -D$(ARCH) -funsigned-char $(XMLCFLAGS) -I $(INCDIR) $(DEBUGINFO) -Wall
OPTS         = -ftemplate-depth-40 $(COPTS)
CLNT_LDFLAGS = $(LDFLAGS)
SRV_LDFLAGS  = $(LDFLAGS)

# debuging parameters
DEBUGINFO    = -g
#EFENCE       = -lefence

# === libxml2 stuff ===
#XMLCFLAGS    = `pkg-config libxml-2.0 --cflags` -DLIBXML2
#XMLLIBS      = `pkg-config libxml-2.0 --libs`

LDFLAGS       = $(XMLLIBS) $(EFENCE)

# === Portability ===
ARCH         = LINUX
LOWLEVEL     = $(PREFIX)/$(PORTDIR)
CLIENT       = $(PORTDIR)/client-linux.cpp
SERVER       = $(PORTDIR)/server-linux.cpp
CLIENTBIN    = dibbler-client
SERVERBIN    = dibbler-server

# === do not modify anything below ===
MAKEFLAGS   += -s
VERSION      = `$(TOPDIR)/test/xtract_version`
INST_WORKDIR = '/var/lib/dibbler'
INST_MANDIR  = '/usr/local/man'
INST_DOCDIR  = '/usr/local/share/doc'
INST_BINDIR  = '/usr/local/sbin'
INSTALL      = 'install'
MKDIR        = mkdir -p
CP           = cp

PREFIX       = ..
PORTDIR      = $(PREFIX)/port-linux
INCDIR       = $(PREFIX)/include
ADDRMGR      = $(PREFIX)/AddrMgr
CFGMGR       = $(PREFIX)/CfgMgr
IFACEMGR     = $(PREFIX)/IfaceMgr
MESSAGES     = $(PREFIX)/Messages
OPTIONS      = $(PREFIX)/Options
MISC         = $(PREFIX)/misc

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

COMMONSUBDIRS= IfaceMgr    AddrMgr      CfgMgr      Messages   Options      include misc port-linux
CLNTSUBDIRS  = ClntOptions ClntIfaceMgr ClntAddrMgr ClntCfgMgr ClntTransMgr ClntMessages 
SRVSUBDIRS   = SrvOptions  SrvIfaceMgr  SrvAddrMgr  SrvCfgMgr  SrvTransMgr  SrvMessages  
SUBDIRS      = $(COMMONSUBDIRS) $(CLNTSUBDIRS) $(SRVSUBDIRS)

# === UNIVERSAL TARGETS ===

%.o: %.cpp %.h
	@echo "[CPP    ] $(SUBDIR)/$@"
	$(CPP) $(OPTS) -c -o $@ $<

%.a: objs
	@echo "[LIB    ] $(SUBDIR)/$@"	
	ar cr $@ $(OBJECTS)

all: libs

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	rm -f *~ *.o *.a *.xml test? $(CLIENTBIN) $(SERVERBIN) TAGS
	find . -type l -exec rm {} \;
	find . -name *.a -exec rm {} \;
	find . -name *~ -exec rm {} \;
	find . -name *.o -exec rm {} \;
	find . -name test? -exec rm {} \;

# === SUBDIR NAMES ===
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))

ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 
