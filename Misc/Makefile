include ../Makefile.inc
all: objs libs

objs:	Logger.o DHCPConst.o DUID.o IPv6Addr.o long128.o addrpack.o

# addr conversion (packed[[16] <-> plaintext[48])
addrpack.o: addrpack.c 
	$(CC) $(COPTS) -I $(INCDIR) -I. -c addrpack.c 

Logger.o: Logger.cpp Logger.h
	$(CPP) $(OPTS) -c -o $@ $<

DHCPConst.o: DHCPConst.cpp DHCPConst.h
	$(CPP) $(OPTS) -c -o $@ $<

DUID.o: DUID.cpp DUID.h
	$(CPP) $(OPTS) -c -o $@ $<

IPv6Addr.o: IPv6Addr.cpp IPv6Addr.h
	$(CPP) $(OPTS) -c -o $@ $<

long128.o: long128.cpp long128.h
	$(CPP) $(OPTS) -c -o $@ $<

libs:	libMisc.a

libMisc.a: objs
	ar cr $@ Logger.o DHCPConst.o DUID.o long128.o IPv6Addr.o addrpack.o

clean:
	rm -f *.o *.a
