include ../Makefile.inc

OBJECTS =  WinService.o ClntService.o SrvService.o RelService.o addrpack.o lowlevel-winnt2k.o

objs: $(OBJECTS)

libs: objs libLowLevel.a 

# ============================================================
# === C low level stuff ======================================
# ============================================================
lowlevel-winnt2k.o: lowlevel-winnt2k.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c $<

addrpack.o: addrpack.c
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c $<

ClntService.o: ClntService.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -I../include $(OPTS) -c -o $@ $<

SrvService.o: SrvService.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -I../include $(OPTS) -c -o $@ $<

RelService.o: RelService.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -I../include $(OPTS) -c -o $@ $<

WinService.o: WinService.cpp
	@echo "[CXX    ] $(SUBDIR)/$@"
	$(CXX) -I../include $(OPTS) -c -o $@ $<
