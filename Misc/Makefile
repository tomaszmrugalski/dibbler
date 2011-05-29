TOPDIR = ..

OBJECTS = Logger.o DHCPConst.o DUID.o IPv6Addr.o addrpack.o FQDN.o KeyList.o

ifndef MOD_DISABLE_AUTH
OBJECTS += sha1.o sha256.o sha512.o md5.o hmac-sha-md5.o
endif

objs: $(OBJECTS)

libs:	libMisc.a

include $(TOPDIR)/Makefile.inc

# addr conversion (packed[[16] <-> plaintext[48])
addrpack.o: addrpack.c 
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c addrpack.c 

# SHA-1 routines
sha1.o: sha1.c sha1.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c sha1.c 

# SHA-224 and SHA-256 routines
sha256.o: sha256.c sha256.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c sha256.c 

# SHA-384 and SHA-512 routines
sha512.o: sha512.c sha512.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -std=c99 -c sha512.c 

# MD5 routined
md5.o: md5.c md5.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c md5.c 

# HMAC-SHA and HMAC-MD5
hmac-sha-md5.o: hmac-sha-md5.c hmac.h
	@echo "[CC     ] $(SUBDIR)/$@"
	$(CC) $(COPTS) -c hmac-sha-md5.c 
