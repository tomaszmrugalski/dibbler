# === used tools ===
CPP          = g++
CC           = gcc
FLEX         = flex
BISONPP      = $(PREFIX)/bison++/bison++ -S $(PREFIX)/bison++/bison.cc -H $(PREFIX)/bison++/bison.h

# === compiler options ===
COPTS        = -D$(ARCH) -funsigned-char $(XMLCFLAGS) -I $(INCDIR) -g
OPTS         = -ftemplate-depth-40 $(COPTS)

# debug info
#EFENCE      = -lefence

# === Portability ===
ARCH         = LINUX
LOWLEVEL     = $(PREFIX)/port-linux
CLIENT       = port-linux/client-linux.cpp
SERVER       = port-linux/server-linux.cpp
CLIENTBIN    = dibbler-client-linux
SERVERBIN    = dibbler-server-linux

# === libxml2 stuff ===
XMLCFLAGS    = `pkg-config libxml-2.0 --cflags`
XMLLIBS      = `pkg-config libxml-2.0 --libs`

# === do not modify anything below ===
PREFIX       = ..
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

COMMONSUBDIRS= IfaceMgr    AddrMgr      CfgMgr      TransMgr   Messages     Options include misc
CLNTSUBDIRS  = ClntOptions ClntIfaceMgr ClntAddrMgr ClntCfgMgr ClntTransMgr ClntMessages 
SRVSUBDIRS   = SrvOptions  SrvIfaceMgr  SrvAddrMgr  SrvCfgMgr  SrvTransMgr  SrvMessages  
SUBDIRS      = $(COMMONSUBDIRS) $(CLNTSUBDIRS) $(SRVSUBDIRS)
