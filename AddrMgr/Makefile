include ../Makefile.inc

OBJECTS = AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o 

objs: $(OBJECTS)

libs: libAddrMgr.a

# === tests ===
test: test1 test2 test3 test4

test1: test1.cpp libs
	cd $(MISC); $(MAKE) libs
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) $(XMLLIBS) -o $@ test1.cpp \
	-L. -lAddrMgr -L$(MISC) -lMisc -L$(IFACEMGR) -lIfaceMgr

test2: test2.cpp AddrAddr.o AddrIA.o
	cd $(MISC); $(MAKE) libs
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -o $@ test2.cpp AddrAddr.o AddrIA.o -L$(MISC) -lMisc

test3: test3.cpp AddrAddr.o AddrIA.o AddrClient.o
	echo $(FOO)
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -o $@ test3.cpp AddrAddr.o AddrIA.o AddrClient.o $(MISC)/Logger.o

test4: test4.cpp AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o
	cd $(MISC); $(MAKE) objs
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -o $@ test4.cpp $(MISC)/Logger.o \
	AddrAddr.o AddrIA.o AddrClient.o AddrMgr.o
