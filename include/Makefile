include ../Makefile.inc

PREFIX=..
COMMONSUBDIRS= IfaceMgr    AddrMgr      CfgMgr      Messages     Options misc 

all: links

libs:

objs:

links: clean
	@for dir in $(SUBDIRS); do \
	  echo $(PREFIX)/$$dir; \
          ln -f -s $(PREFIX)/$$dir/*.h .;  \
	done
	ln -f -s $(PREFIX)/*.h .
	ln -f -s $(SRVCFGMGR)/SrvParser.h SrvParser.h
	ln -f -s $(CLNTCFGMGR)/ClntParser.h ClntParser.h

clean:
	find . -type l -exec rm {} \;
