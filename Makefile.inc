# === used tools ===
CPP          = g++-3.3
CC           = gcc-3.3
FLEX         = flex
BISONPP      = bison++

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
