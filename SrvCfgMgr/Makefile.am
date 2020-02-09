SUBDIRS = .

if HAVE_GTEST
  SUBDIRS += tests
endif

noinst_LIBRARIES = libSrvCfgMgr.a

libSrvCfgMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/CfgMgr -I$(top_srcdir)/SrvOptions
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/SrvIfaceMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/IfaceMgr -I$(top_srcdir)/SrvAddrMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/SrvTransMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/SrvMessages -I$(top_srcdir)/Messages
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/poslib -I$(top_srcdir)/poslib/poslib
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/@PORT_SUBDIR@

libSrvCfgMgr_a_SOURCES  = NodeClientSpecific.cpp NodeClientSpecific.h
libSrvCfgMgr_a_SOURCES += NodeConstant.cpp NodeConstant.h
libSrvCfgMgr_a_SOURCES += Node.cpp Node.h
libSrvCfgMgr_a_SOURCES += NodeOperator.cpp NodeOperator.h
libSrvCfgMgr_a_SOURCES += SrvCfgAddrClass.cpp SrvCfgAddrClass.h
libSrvCfgMgr_a_SOURCES += SrvCfgClientClass.cpp SrvCfgClientClass.h
libSrvCfgMgr_a_SOURCES += SrvCfgIface.cpp SrvCfgIface.h
libSrvCfgMgr_a_SOURCES += SrvCfgMgr.cpp SrvCfgMgr.h
libSrvCfgMgr_a_SOURCES += SrvCfgOptions.cpp SrvCfgOptions.h
libSrvCfgMgr_a_SOURCES += SrvCfgPD.cpp SrvCfgPD.h
libSrvCfgMgr_a_SOURCES += SrvCfgTA.cpp SrvCfgTA.h
libSrvCfgMgr_a_SOURCES += SrvLexer.cpp
libSrvCfgMgr_a_SOURCES += SrvParsClassOpt.cpp SrvParsClassOpt.h
libSrvCfgMgr_a_SOURCES += SrvParser.cpp SrvParser.h
libSrvCfgMgr_a_SOURCES += SrvParsGlobalOpt.cpp SrvParsGlobalOpt.h
libSrvCfgMgr_a_SOURCES += SrvParsIfaceOpt.cpp SrvParsIfaceOpt.h
libSrvCfgMgr_a_SOURCES += SrvParserContext.cpp SrvParserContext.h

dist_noinst_DATA = SrvLexer.l SrvParser.y position.hh location.hh stack.hh

parser: SrvParser.y SrvLexer.l
	@echo "[BISON++] $(SUBDIR)/SrvParser.y"
	bison -v --defines=SrvParser.h -d SrvParser.y -o SrvParser.cpp
	@echo "[FLEX   ] $(SUBDIR)/SrvLexer.l"
	flex --prefix dibbler_ -o SrvLexer.cpp SrvLexer.l
