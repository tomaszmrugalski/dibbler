include ../Makefile.inc

all: objs libs

client: objs

server: objs

# tests
test: test1

test1: test1.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp

# lib
libs: libCfgMgr.a

libCfgMgr.a: objs
	ar cr $@ CfgMgr.o TimeZone.o StationID.o StationRange.o

# Objects 
objs: CfgMgr.o TimeZone.o StationID.o StationRange.o

# === Objects ===
CfgMgr.o: CfgMgr.cpp CfgMgr.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

TimeZone.o: TimeZone.cpp TimeZone.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

StationID.o: StationID.cpp StationID.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

StationRange.o: StationRange.cpp StationRange.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<


clean:
	rm -f *.o *.a
	rm -f test? test??
