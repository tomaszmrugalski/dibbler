include ../Makefile.inc

OBJECTS = SocketIPv6.o Iface.o IfaceMgr.o 

objs: $(OBJECTS)

libs: libIfaceMgr.a

# === tests ===
test: test1 test2 test3 test4 test5

test1: test1.cpp libIfaceMgr.a
	cd $(MISC); $(MAKE) Logger.o IPv6Addr.o
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp -L. -lIfaceMgr $(MISC)/Logger.o $(MISC)/IPv6Addr.o

test2: test2.cpp IfaceMgr.o Iface.o SocketIPv6.o 
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test2.cpp -L. -lIfaceMgr $(PREFIX)/Logger.o

test3: test3.cpp
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ $< -L. -lIfaceMgr $(PREFIX)/Logger.o

test4: test4.cpp SocketIPv6.o Iface.o IfaceMgr.o 
	echo "[CPP    ]  $@"
	cd $(PREFIX); $(MAKE) Logger.o
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test4.cpp -L. -lIfaceMgr  $(PREFIX)/Logger.o

test5: test5.cpp 
	echo "[CPP    ]  $@"
	$(CPP) $(OPTS) -I $(INCDIR) -o test5 test5.cpp -L. -lIfaceMgr
