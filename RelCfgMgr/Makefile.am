noinst_LIBRARIES = libRelCfgMgr.a

libRelCfgMgr_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/CfgMgr
libRelCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/Options -I$(top_srcdir)/RelOptions
libRelCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/IfaceMgr -I$(top_srcdir)/RelIfaceMgr
libRelCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/RelMessages -I$(top_srcdir)/Messages
libRelCfgMgr_a_CPPFLAGS += -I$(top_srcdir)/@PORT_SUBDIR@

libRelCfgMgr_a_SOURCES = RelCfgIface.cpp RelCfgIface.h RelCfgMgr.cpp RelCfgMgr.h RelLexer.cpp RelLexer.h RelParser.cpp RelParser.h RelParsGlobalOpt.cpp RelParsGlobalOpt.h RelParsIfaceOpt.cpp RelParsIfaceOpt.h

parser: RelParser.y RelLexer.l 
	echo "[BISON++] $(SUBDIR)/RelParser.y"
	$(top_builddir)/bison++/bison++ --skeleton=$(top_builddir)/bison++/bison.cc --headerskeleton=$(top_builddir)/bison++/bison.h -v --debug --defines -d RelParser.y -o RelParser.cpp
	echo "[FLEX   ] $(SUBDIR)/RelLexer.l"
	flex -+ -i -oRelLexer.cpp RelLexer.l
	@echo "[SED    ] $(SUBDIR)/RelLexer.cpp"
	cat RelLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/ extern "C" int isatty (int ) throw ();/' > RelLexer.cpp2
	rm -f RelLexer.cpp
	mv RelLexer.cpp2 RelLexer.cpp
