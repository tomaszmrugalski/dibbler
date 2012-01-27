noinst_LIBRARIES = libSrvCfgMgr.a

libSrvCfgMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/CfgMgr -I$(top_srcdir)/SrvOptions
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/SrvIfaceMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/IfaceMgr -I$(top_srcdir)/SrvAddrMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/SrvTransMgr
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/SrvMessages -I$(top_srcdir)/Messages
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/poslib -I$(top_srcdir)/poslib/poslib
libSrvCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/@PORT_SUBDIR@

libSrvCfgMgr_a_SOURCES = NodeClientSpecific.cpp NodeClientSpecific.h NodeConstant.cpp NodeConstant.h Node.cpp Node.h NodeOperator.cpp NodeOperator.h SrvCfgAddrClass.cpp SrvCfgAddrClass.h SrvCfgClientClass.cpp SrvCfgClientClass.h SrvCfgIface.cpp SrvCfgIface.h SrvCfgMgr.cpp SrvCfgMgr.h SrvCfgOptions.cpp SrvCfgOptions.h SrvCfgPD.cpp SrvCfgPD.h SrvCfgTA.cpp SrvCfgTA.h SrvLexer.cpp SrvParsClassOpt.cpp SrvParsClassOpt.h SrvParser.cpp SrvParser.h SrvParsGlobalOpt.cpp SrvParsGlobalOpt.h SrvParsIfaceOpt.cpp SrvParsIfaceOpt.h

parser: SrvParser.y SrvLexer.l
	@echo "[BISON++] $(SUBDIR)/SrvParser.y"
	$(top_builddir)/bison++/bison++ --skeleton=$(top_builddir)/bison++/bison.cc --headerskeleton=$(top_builddir)/bison++/bison.h -v --debug --defines -d SrvParser.y -o SrvParser.cpp
	@echo "[FLEX   ] $(SUBDIR)/SrvLexer.l"
	flex -+ -i -oSrvLexer.cpp SrvLexer.l
	@echo "[SED    ] $(SUBDIR)/SrvLexer.cpp"
	cat SrvLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > SrvLexer.cpp2
	rm -f SrvLexer.cpp
	mv SrvLexer.cpp2 SrvLexer.cpp
