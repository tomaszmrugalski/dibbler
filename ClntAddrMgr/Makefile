include ../Makefile.inc

OBJECTS = ClntAddrMgr.o

objs: $(OBJECTS)

libs: libClntAddrMgr.a

# === tests ===

test: test1

test1: test1.cpp libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp  \
	-L. -lClntAddrMgr
