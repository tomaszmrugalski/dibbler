include ../Makefile.inc

OBJECTS = lowlevel-linux.o lowlevel-options-linux.o \
          libnetlink.o ll_map.o ll_types.o utils.o daemon.o iproute.o

ifdef MOD_CLNT_CONFIRM
OBJECTS += interface.o lowlevel-linux-link-state.o
endif 

objs: $(OBJECTS)


libs: objs libLowLevel.a 

# ============================================================
# === C low level stuff ======================================
# ============================================================
lowlevel-linux.o: lowlevel-linux.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -I. -c lowlevel-linux.c 

interface.o: interface.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) -std=c99 $(CFLAGS) -I. -c interface.c 

ifplugd.o: ifplugd.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -I. -c ifplugd.c 

lowlevel-linux-link-state.o: lowlevel-linux-link-state.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -I. -c lowlevel-linux-link-state.c

libnetlink.o: libnetlink.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) -std=c99 $(CFLAGS) -c libnetlink.c

ll_map.o: ll_map.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -c ll_map.c

ll_types.o: ll_types.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -c ll_types.c

utils.o: utils.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -c utils.c

lowlevel-options-linux.o: lowlevel-options-linux.c	
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -c $<

iproute.o: iproute.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(CFLAGS) -c iproute.c

# ============================================================
# === tests ==================================================
# ============================================================

test: test1 test2

test1: test1.cpp ../Misc/addrpack.o libLowLevel.a
	$(CXX) $(OPTS) test1.cpp ../Misc/addrpack.o libLowLevel.a -o test1

test2: test2.cpp libLowLevel.a
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CXX) $(OPTS)  test2.cpp -L. -lLowLevel -o test2

test3: test1.cpp SrvParser.cpp SrvLexer.o
	cd $(SRVCFGMGR); $(MAKE) libs
	$(CXX) $(OPTS) -I $(INCDIR) -o $@ test3.cpp SrvParser.cpp SrvLexer.o \
	-L. -lSrvParser -L$(SRVCFGMGR) -lSrvCfgMgr ../IPv6Addr.cpp ../DUID.cpp \
	$(LOWLEVEL)/addrpack.o ../Logger.o
