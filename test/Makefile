include ../Makefile.inc

MAKEFLAGS=

all: test14

OBJECTS = test14.o

test14: test14.cpp ../misc/Logger.cpp
	cd ../misc; $(MAKE) Logger.o
	$(CXX) $(OPTS) -o $@ $< ../misc/Logger.o

objs: $(OBJECTS)

libs:
