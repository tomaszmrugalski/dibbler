# === used tools ===
CPP          = g++-3.3
CC           = gcc-3.3
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
CLIENTBIN    = dibbler-client-linux
SERVERBIN    = dibbler-server-linux

# === do not modify anything below ===
MAKEFLAGS   += -s
VERSION      = 0.1.1-CVS

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
	rm -f *.o *.a *.xml test? $(CLIENTBIN) $(SERVERBIN) TAGS
	find . -type l -exec rm {} \;
	find . -name *.a -exec rm {} \;
	find . -name *.o -exec rm {} \;
	find . -name test? -exec rm {} \;

# === SUBDIR NAMES ===
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))

ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 
