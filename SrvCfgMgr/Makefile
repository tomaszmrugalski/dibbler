TOPDIR = ..


OBJECTS =  SrvParser.o SrvLexer.o SrvCfgAddrClass.o SrvCfgIface.o SrvCfgMgr.o    \
           SrvParsClassOpt.o SrvParsGlobalOpt.o SrvParsIfaceOpt.o SrvCfgTA.o     \
	   SrvCfgPD.o SrvCfgOptions.o SrvCfgClientClass.o Node.o NodeConstant.o  \
	   NodeClientSpecific.o NodeOperator.o

libs: libSrvCfgMgr.a

include ../Makefile.inc

# No longer needed default rule handles it well
#SrvLexer.o: SrvLexer.cpp
#	@echo "[CXX    ] $(SUBDIR)/$@"
#	$(CXX) -c $(CPPFLAGS) -ftemplate-depth-30 SrvLexer.cpp 

# APPROACH 1: ignore .y .l dependency
# After modifying .y and .l files, you have to manually rebuild .cpp files by issuing: make parser
# Pros: Proven and working
# Cons: 
parser: SrvParser.y SrvLexer.l 
	echo "[BISON++] $(SUBDIR)/SrvParser.y"
	$(BISONPP) -v --debug --defines -d SrvParser.y -o SrvParser.cpp
	echo "[FLEX   ] $(SUBDIR)/SrvLexer.l"
	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l
	@echo "[SED    ] $(SUBDIR)/SrvLexer.cpp"
	cat SrvLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > SrvLexer.cpp2
	rm -f SrvLexer.cpp
	mv SrvLexer.cpp2 SrvLexer.cpp

# APPROACH 2: .l .y dependency
# Proper dependency chain .l->.cpp->.o and .y->.cpp->.o
# Pros:
# Cons: - bison++ and flex required during every build

#SrvParser.cpp: SrvParser.y 
#	echo "[BISON++] $(SUBDIR)/SrvParser.y"
#	$(BISONPP) -v --debug --defines -d SrvParser.y -o SrvParser.cpp

#SrvLexer.cpp: SrvLexer.l
#	echo "[FLEX   ] $(SUBDIR)/SrvLexer.l"
#	$(FLEX) -+ -i -oSrvLexer.cpp SrvLexer.l
#	@echo "[SED    ] $(SUBDIR)/SrvLexer.cpp"
#	cat SrvLexer.cpp | sed 's/extern "C" int isatty (int );/\/\/extern "C" int isatty (int ) throw ();/' > SrvLexer.cpp2
#	rm -f SrvLexer.cpp
#	mv SrvLexer.cpp2 SrvLexer.cpp

#parser: SrvParser.cpp SrvLexer.cpp 

