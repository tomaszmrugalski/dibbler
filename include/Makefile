include ../Makefile.inc

libs:

objs:

links: 
	find -type l -exec rm {} \;
	echo "[LINKS  ] $(SUBDIR) "
	for dir in $(SUBDIRS); do \
	  if [ "$$dir" != "include" ]; then \
	  ln -s $(PREFIX)/$$dir/*.h . 2>/dev/null;  \
	fi \
	done
	ln -f -s $(SRVCFGMGR)/SrvParser.h SrvParser.h
	ln -f -s $(CLNTCFGMGR)/ClntParser.h ClntParser.h
	ln -f -s $(RELCFGMGR)/RelParser.h RelParser.h
