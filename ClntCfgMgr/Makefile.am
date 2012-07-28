noinst_LIBRARIES = libClntCfgMgr.a

libClntCfgMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/CfgMgr -I$(top_srcdir)/ClntOptions
libClntCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/ClntIfaceMgr
libClntCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/IfaceMgr -I$(top_srcdir)/ClntAddrMgr
libClntCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/ClntTransMgr
libClntCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/ClntMessages -I$(top_srcdir)/Messages
libClntCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/@PORT_SUBDIR@

libClntCfgMgr_a_SOURCES = ClntCfgAddr.cpp ClntCfgAddr.h ClntCfgIA.cpp ClntCfgIA.h ClntCfgIface.cpp ClntCfgIface.h ClntCfgMgr.cpp ClntCfgMgr.h ClntCfgPD.cpp ClntCfgPD.h ClntCfgPrefix.cpp ClntCfgPrefix.h ClntCfgTA.cpp ClntCfgTA.h ClntLexer.cpp ClntParsAddrOpt.cpp ClntParsAddrOpt.h ClntParser.cpp ClntParser.h ClntParsGlobalOpt.cpp ClntParsGlobalOpt.h ClntParsIAOpt.cpp ClntParsIAOpt.h ClntParsIfaceOpt.cpp ClntParsIfaceOpt.h


parser: ClntParser.y ClntLexer.l
	@echo "[BISON++] $(SUBDIR)/ClntParser.y"
	$(top_builddir)/bison++/bison++ --skeleton=$(top_builddir)/bison++/bison.cc --headerskeleton=$(top_builddir)/bison++/bison.h -v --debug --defines -d ClntParser.y -o ClntParser.cpp
	@echo "[FLEX   ] $(SUBDIR)/ClntLexer.l"
	flex -+ -i -L -oClntLexer.cpp ClntLexer.l
	@echo "[SED    ] $(SUBDIR)/ClntLexer.cpp"
	cat ClntLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > ClntLexer.cpp2
	rm -f ClntLexer.cpp
	mv ClntLexer.cpp2 ClntLexer.cpp
