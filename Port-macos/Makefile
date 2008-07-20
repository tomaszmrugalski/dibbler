include ../Makefile.inc

OBJECTS = lowlevel-macos.o lowlevel-options-macos.o daemon.o

objs: $(OBJECTS)

libs: objs libLowLevel.a 

# ============================================================
# === C low level functions, kernel specific =================
# ============================================================
lowlevel-macos.o: lowlevel-macos.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -I. -c $<

lowlevel-options-macos.o: lowlevel-options-macos.c	
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c $<

iproute.o: iproute.c
	@echo "[CC     ] $(SUBDIR)/$@ (warnings ignored)"
	$(CC) $(COPTS) -c iproute.c 2> iproute.c-warnings

# ============================================================
# === tests ==================================================
# ============================================================

