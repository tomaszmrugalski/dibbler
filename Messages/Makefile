include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libMsg.a

libMsg.a:  Msg.o
	ar cr $@ Msg.o

# tests

test: test1 

test1: test1.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp 

# Objects 
objs: Msg.o

Msg.o: Msg.cpp Msg.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o *.a
	rm -f test? test??