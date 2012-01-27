SUBDIRS = .

dist_noinst_DATA = client.log client-winnt2k.cpp ClntService.cpp ClntService.h config.h dibbler-client.dev dibbler-client.ico dibbler-client.layout dibbler.iss dibbler-relay.dev dibbler-relay.ico dibbler-relay.layout dibbler-requestor.dev dibbler-requestor.layout dibbler-server.dev dibbler-server.ico dibbler-server.layout INFO lowlevel-winnt2k.c Makefile Makefile.win relay.log relay-winnt2k.cpp RelService.cpp RelService.h server.log server-winnt2k.cpp SrvService.cpp SrvService.h tpipv6.h WinService.cpp WinService.h wspiapi.h

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
