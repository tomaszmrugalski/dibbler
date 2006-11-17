include ../Makefile.inc
OBJECTS = Logger.o DHCPConst.o DUID.o IPv6Addr.o addrpack.o FQDN.o sha1.o

objs: $(OBJECTS)

libs:	libMisc.a

# addr conversion (packed[[16] <-> plaintext[48])
addrpack.o: addrpack.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c addrpack.c 

# SHA-1 routines
sha1.o: sha1.c sha1.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c sha1.c 
