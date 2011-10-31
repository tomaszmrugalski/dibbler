include ../Makefile.inc

CC = g++

GTKCFLAGS = `pkg-config gtk+-2.0 --cflags`
GTKLIBS = `pkg-config gtk+-2.0 --libs`
CURLCFLAGS = `curl-config --cflags`
CURLLIBS = `curl-config --libs`

LIB = $(GTKCFLAGS) $(GTKLIBS) $(CURLCFLAGS) $(CURLLIBS) -ljson_linux-gcc-4.5.2_libmt

OBJECTS = ReqGeolocInfo.o CURLWrap.o

all: ReqGeolocInfo.o CURLWrap.o CURLWrap.h
	$(CC) -I $(INCDIR) $(LDFLAGS) $(CXXFLAGS) ReqGeolocInfo.o CURLWrap.o CURLWrap.h -o requestor-graphic $(LIB) \
	-L../Misc -lMisc

ReqGeolocInfo.o: ReqGeolocInfo.cpp CURLWrap.h
	$(CC) -c -I $(INCDIR) $(LDFLAGS) $(CXXFLAGS) ReqGeolocInfo.cpp -o ReqGeolocInfo.o $(LIB) \
	-L../Misc -lMisc

CURLWrap.o: CURLWrap.cpp CURLWrap.h
	$(CC) -c CURLWrap.cpp -o CURLWrap.o
	
objs: $(OBJECTS)
	
libs: libRequestorGraphic.a