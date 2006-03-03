include ../Makefile.inc
OBJECTS = Logger.o DHCPConst.o DUID.o IPv6Addr.o addrpack.o FQDN.o

objs: $(OBJECTS)

libs:	libMisc.a

# addr conversion (packed[[16] <-> plaintext[48])
addrpack.o: addrpack.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c addrpack.c 
