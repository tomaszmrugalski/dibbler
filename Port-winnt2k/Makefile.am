SUBDIRS = .

dist_noinst_DATA = *.cpp *.h *.log *.dev *.ico *.layout *.c Makefile INFO Makefile.win dibbler.iss

# ============================================================
# === C low level stuff ======================================
# ============================================================
lowlevel-winnt2k.o: lowlevel-winnt2k.c
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
