include ../Makefile.inc

OBJECTS = layer3.o libnetlink.o ll_map.o ll_types.o utils.o dns.o addrpack.o

objs: $(OBJECTS)


libs: objs libLowLevel.a 

# ============================================================
# === C low level stuff ======================================
# ============================================================
layer3.o: layer3.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -I. -c layer3.c 

libnetlink.o: libnetlink.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c libnetlink.c

ll_map.o: ll_map.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c ll_map.c

ll_types.o: ll_types.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c ll_types.c

utils.o: utils.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c utils.c

dns.o: dns.c	
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c $<

addrpack.o: addrpack.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c $<

# ============================================================
# === tests ==================================================
# ============================================================

test: test1 test2

test1: test1.cpp dns.c
	$(CPP) $(OPTS) test1.cpp dns.c -o test1

test2: test2.cpp libLowLevel.a
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CPP) $(OPTS)  test2.cpp -L. -lLowLevel -o test2

test3: test1.cpp SrvParser.cpp SrvLexer.o
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test3.cpp SrvParser.cpp SrvLexer.o \
	-L. -lSrvParser -L$(SRVCFGMGR) -lSrvCfgMgr ../IPv6Addr.cpp ../DUID.cpp \
	$(LOWLEVEL)/addrpack.o ../Logger.o
