include ../Makefile.inc

all: objs libs
all: objs parser lexer libs

objs: layer3.o libnetlink.o ll_map.o ll_types.o utils.o dns.o addrpack.o\
	ClntParser.o ClntLexer.o SrvParser.o SrvLexer.o


ClntParser.o: ClntParser-linux.y
	$(BISONPP) -d ClntParser-linux.y -o ClntParser.cpp
	$(CPP) $(OPTS) -c -I $(INCDIR) -ftemplate-depth-30 ClntParser.cpp 
	cd $(INCDIR); $(MAKE)

ClntLexer.o: ClntLexer-linux.l
	$(FLEX) -+ -oClntLexer.cpp ClntLexer-linux.l
	$(CPP) $(OPTS) -c -I $(INCDIR) ClntLexer.cpp

SrvParser.o: SrvParser-linux.y
	$(BISONPP) -d SrvParser-linux.y -o SrvParser.cpp
	$(CPP) -c $(OPTS) -I $(INCDIR) SrvParser.cpp 

SrvLexer.o: SrvLexer-linux.l
	$(FLEX) -+ -oSrvLexer.cpp SrvLexer-linux.l
	$(CPP) -c $(OPTS) -I $(INCDIR) SrvLexer.cpp 

# ============================================================
# === libs ===================================================
# ============================================================

libs: objs libLowLevel.a libClntParser.a libSrvParser.a

libLowLevel.a: 
	rm -f libLowLevel.a
	ar cr $@ layer3.o libnetlink.o ll_map.o ll_types.o utils.o addrpack.o dns.o

libClntParser.a: ClntParser.o ClntLexer.o 
	ar cr $@ ClntParser.o ClntLexer.o 

libSrvParser.a:  SrvParser.o SrvLexer.o 
	ar cr $@ SrvParser.o SrvLexer.o 

# ============================================================
# === C low level stuff ======================================
# ============================================================
layer3.o: layer3.c 
	$(CC) $(OPTS) -I $(INCDIR) -I. -c layer3.c 

libnetlink.o: libnetlink.c
	$(CC) $(OPTS) -I $(INCDIR) -c libnetlink.c

ll_map.o: ll_map.c 
	$(CC) $(OPTS) -I $(INCDIR) -c ll_map.c

ll_types.o: ll_types.c
	$(CC) $(OPTS) -I $(INCDIR) -c ll_types.c

utils.o: utils.c
	$(CC) $(OPTS) -I $(INCDIR) -c utils.c

dns.o: dns.c	
	$(CC) $(OPTS) -c $<

addrpack.o: addrpack.c
	$(CC) $(OPTS) -c $<

# ============================================================
# === tests ==================================================
# ============================================================

test: test1 test2

test1: test1.cpp SrvParser.cpp SrvLexer.o
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp SrvParser.cpp SrvLexer.o \
	-L. -lSrvParser -L$(SRVCFGMGR) -lSrvCfgMgr ../IPv6Addr.cpp ../DUID.cpp \
	$(LOWLEVEL)/addrpack.o ../Logger.o

test2: test2.cpp SrvParser.o SrvLexer.o
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test2.cpp -L. -lSrvParser -L$(SRVCFGMGR) -lSrvCfgMgr


clean:
	rm -f *.a *.o
	rm -f y.tab.c lex.yy.cc
	rm -f *.pgp *.pgw
	rm -f SrvLexer.cpp SrvParser.cpp SrvParser.h
	rm -f ClntLexer.cpp ClntParser.cpp ClntParser.h
	rm -f test1 test2
