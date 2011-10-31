include ../Makefile.inc

CC = g++

GTKCFLAGS = `pkg-config gtk+-2.0 --cflags`
GTKLIBS = `pkg-config gtk+-2.0 --libs`
CURLCFLAGS = `curl-config --cflags`
CURLLIBS = `curl-config --libs`

LIB = $(GTKCFLAGS) $(GTKLIBS) $(CURLCFLAGS) $(CURLLIBS) -ljson_linux-gcc-4.5.2_libmt

OBJECTS = ClntGeolocInfo.o CURLWrap.o

all: ClntGeolocInfo.o CURLWrap.o CURLWrap.h
	$(CC) -I $(INCDIR) $(LDFLAGS) $(CXXFLAGS) ClntGeolocInfo.o CURLWrap.o CURLWrap.h -o dibbler-geolocation $(LIB) \
	-L../Misc -lMisc

ClntGeolocInfo.o: ClntGeolocInfo.cpp CURLWrap.h
	$(CC) -c -I $(INCDIR) $(LDFLAGS) $(CXXFLAGS) ClntGeolocInfo.cpp -o ClntGeolocInfo.o $(LIB) \
	-L../Misc -lMisc

CURLWrap.o: CURLWrap.cpp CURLWrap.h
	$(CC) -c CURLWrap.cpp -o CURLWrap.o
	
objs: $(OBJECTS)
	
libs: libClntGraphic.a