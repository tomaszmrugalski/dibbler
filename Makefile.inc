# === used tools ===
CPP          = g++-2.95
CC           = gcc-2.95
FLEX         = flex
BISONPP      = bison++

# === compiler options ===
OPTS         = -D$(ARCH) -funsigned-char -ftemplate-depth-40 $(XMLCFLAGS) -I $(INCDIR)
EFENCE       =
#OPTS         = -D$(ARCH) -funsigned-char -ftemplate-depth-40 $(XMLCFLAGS) -I $(INCDIR) -g
#EFENCE       = -lefence

# === Portability ===
ARCH         = LINUX
LOWLEVEL     = $(PREFIX)/port-linux
CLIENT       = port-linux/client-linux.cpp
SERVER       = port-linux/server-linux.cpp
CLIENTBIN    = client-linux-efence
SERVERBIN    = server-linux-efence

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
