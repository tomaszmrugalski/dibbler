include ../Makefile.inc

all: links

libs:

objs:

links: clean
	find .. -name '*.h' -maxdepth 2 -type f -exec ln -s {} \; 2>/dev/null
	ln -f -s $(LOWLEVEL)/SrvParser.h SrvParser.h
	ln -f -s $(LOWLEVEL)/ClntParser.h ClntParser.h

clean:
	find . -type l -exec rm {} \;
