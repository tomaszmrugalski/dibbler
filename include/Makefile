include ../Makefile.inc

libs:

objs:

links: poslib-links
#	find -type l -exec rm {} \;
	echo "[LINKS  ] $(SUBDIR) "
	for dir in $(SUBDIRS); do \
	  if [ "$$dir" != "include" ]; then \
	  ln -f -s $(PREFIX)/$$dir/*.h . 2>/dev/null;  \
	  fi;\
	done
	ln -f -s $(SRVCFGMGR)/SrvParser.h SrvParser.h
	ln -f -s $(CLNTCFGMGR)/ClntParser.h ClntParser.h
	ln -f -s $(RELCFGMGR)/RelParser.h RelParser.h

poslib-links:
	echo "[LINKS  ] /poslib"
	ln -f -s ../poslib/poslib/bits.h
	ln -f -s ../poslib/config.h
	ln -f -s ../poslib/poslib/dnsdefs.h
	ln -f -s ../poslib/poslib/dnsmessage.h
	ln -f -s ../poslib/poslib/domainfn.h
	ln -f -s ../poslib/poslib/exception.h
	ln -f -s ../poslib/poslib/lexfn.h
	ln -f -s ../poslib/poslib/masterfile.h
	ln -f -s ../poslib/poslib/poslib.h
	ln -f -s ../poslib/poslib/postime.h
	ln -f -s ../poslib/poslib/random.h
	ln -f -s ../poslib/poslib/resolver.h
	ln -f -s ../poslib/poslib/rr.h
	ln -f -s ../poslib/poslib/socket.h
	ln -f -s ../poslib/poslib/syssocket.h
	ln -f -s ../poslib/poslib/sysstl.h
	ln -f -s ../poslib/poslib/sysstring.h
	ln -f -s ../poslib/poslib/types.h
	ln -f -s ../poslib/poslib/vsnprintf.h
	ln -f -s ../poslib/poslib/w32poll.h
