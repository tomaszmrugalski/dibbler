include ../Makefile.inc

libs:

objs:

links: clean
	for dir in $(SUBDIRS); do \
	  echo $(PREFIX)/$$dir; \
	  if [ "$$dir" != "include" ]; then \
	  ln -s $(PREFIX)/$$dir/*.h .;  \
	fi \
	done
	ln -f -s $(PREFIX)/*.h .
	ln -f -s $(SRVCFGMGR)/SrvParser.h SrvParser.h
	ln -f -s $(CLNTCFGMGR)/ClntParser.h ClntParser.h
