include Makefile.inc
PREFIX = .
export TOPDIR=$(CURDIR)

all: includes bison libs server client tags

includes:
	cd $(INCDIR); $(MAKE) links

bison:
	@echo "[CONFIG ] /bison++/"
	cd $(PREFIX)/bison++; ./configure &>configure.log
	@echo "[MAKE   ] /bison++/bison++"
	cd $(PREFIX)/bison++; $(MAKE)

client: $(CLIENTBIN)

$(CLIENTBIN): includes commonlibs clntlibs DHCPClient.o $(CLIENT)
	@echo "[LINK   ] $(SUBDIR)/$@"
	$(CPP) $(OPTS) $(CLNTLINKOPTS) -o $@ DHCPClient.o $(CLIENT) \
	-L$(MISC)	  -lMisc \
	-L$(ADDRMGR)      -lAddrMgr \
	-L$(CLNTADDRMGR)  -lClntAddrMgr \
	-L$(LOWLEVEL)    \
	-L$(CLNTOPTIONS)  -lClntOptions \
	-L$(OPTIONS)      -lOptions \
	-L$(CLNTTRANSMGR) -lClntTransMgr \
	-L$(CLNTCFGMGR)   -lClntCfgMgr \
	-L$(CFGMGR)       -lCfgMgr \
	-L$(CLNTIFACEMGR) -lClntIfaceMgr \
	-L$(IFACEMGR)     -lIfaceMgr \
	-L$(CLNTMESSAGES) -lClntMsg \
	                  -lClntAddrMgr \
	                  -lAddrMgr \
	-L$(MISC)         -lMisc \
	-L$(MESSAGES)     -lMsg \
	-lClntOptions -lOptions -lLowLevel $(XMLLIBS) $(EFENCE) 

server: $(SERVERBIN)

$(SERVERBIN): includes commonlibs srvlibs DHCPServer.o $(SERVER)
	@echo "[LINK   ] $(SUBDIR)/$@"
	$(CPP) $(OPTS) -I $(INCDIR) $(SRVLINKOPTS) -o $@ DHCPServer.o $(SERVER)  \
	-L$(SRVADDRMGR)   -lSrvAddrMgr \
	-L$(ADDRMGR)      -lAddrMgr \
	-L$(LOWLEVEL)    \
	-L$(SRVOPTIONS)  -lSrvOptions \
	-L$(SRVTRANSMGR) -lSrvTransMgr \
	-L$(SRVCFGMGR)   -lSrvCfgMgr \
	-L$(CFGMGR)      -lCfgMgr\
	-L$(SRVIFACEMGR) -lSrvIfaceMgr \
	-L$(IFACEMGR)     -lIfaceMgr \
	-L$(MISC)        -lMisc\
	-lSrvIfaceMgr -lSrvMsg -lSrvCfgMgr \
	-L$(SRVADDRMGR)  -lSrvAddrMgr \
	                 -lAddrMgr \
	-L$(SRVOPTIONS)  -lSrvOptions \
	-L$(SRVTRANSMGR) -lSrvTransMgr \
	-L$(SRVCFGMGR)   -lSrvCfgMgr \
	-L$(SRVIFACEMGR) -lSrvIfaceMgr \
	-L$(SRVMESSAGES) -lSrvMsg \
	-L$(MESSAGES)    -lMsg \
	-L$(MISC)        -lMisc \
	-L$(OPTIONS)     -lOptions $(XMLLIBS) \
	-L$(LOWLEVEL)    -lLowLevel $(EFENCE) 

objs:	includes
	@for dir in $(COMMONSUBDIRS); do \
		( cd $$dir; $(MAKE) objs ) || exit 1; \
	done
	@for dir in $(CLNTSUBDIRS); do \
		( cd $$dir; $(MAKE) objs ) || exit 1; \
	done
	@for dir in $(SRVSUBDIRS); do \
		( cd $$dir; $(MAKE) objs ) || exit 1; \
	done


# === libs ===

libs:	commonlibs clntlibs srvlibs

commonlibs:	includes
	@for dir in $(COMMONSUBDIRS); do \
		( cd $$dir; $(MAKE) libs ) || exit 1; \
	done

clntlibs:	includes
	@for dir in $(CLNTSUBDIRS); do \
		( cd $$dir; $(MAKE) libs ) || exit 1; \
	done

srvlibs:	includes
	@for dir in $(SRVSUBDIRS); do \
		( cd $$dir; $(MAKE) libs ) || exit 1; \
	done

doc: 
	cd doc; $(MAKE)

oxygen:
	@echo "[DOXYGEN]"
	doxygen oxygen.cfg >oxygen.log 2>oxygen.err

VERSION-linux:
	echo " Operating system " >  VERSION
	echo "------------------" >> VERSION
	uname -o >> VERSION
	echo >> VERSION

	echo " Version " >> VERSION
	echo "---------" >> VERSION
	echo "$(VERSION)" >> VERSION
	echo >> VERSION

	echo " C++ compiler used " >> VERSION
	echo "-------------------" >> VERSION
	$(CPP) --version >> VERSION
	echo >> VERSION

	echo " C compiler used " >> VERSION
	echo "-----------------" >> VERSION
	$(CC) --version  >>VERSION
	echo >> VERSION

	echo " Date " >> VERSION
	echo "------" >> VERSION
	date +%Y-%m-%d >> VERSION
	echo >> VERSION
#	if [ "$XMLCFLAGS" != "" ]; then
#	    echo "libxml2       : YES" >> VERSION
#	else
#	    echo "libxml2       : NO" >> VERSION
#	fi

VERSION-win:
	echo " Operating system " >  VERSION
	echo "------------------" >> VERSION
	echo " Windows XP/2003"   >> VERSION
	echo >> VERSION

	echo " Version " >> VERSION
	echo "---------" >> VERSION
	echo "$(VERSION)" >> VERSION
	echo >> VERSION

	echo " C++ compiler used " >> VERSION
	echo "-------------------" >> VERSION
	echo "MS Visual C++ 2003 edition" >> VERSION
	echo >> VERSION

	echo " C compiler used " >> VERSION
	echo "-----------------" >> VERSION
	echo "MS Visual C++ 2003 edition" >> VERSION
	echo >> VERSION

	echo " Date " >> VERSION
	echo "------" >> VERSION
	date +%Y-%m-%d >> VERSION
	echo >> VERSION

VERSION-src:
	echo " Version " > VERSION
	echo "---------" >> VERSION
	echo "$(VERSION)" >> VERSION
	echo >> VERSION

	echo " Date " >> VERSION
	echo "------" >> VERSION
	date +%Y-%m-%d >> VERSION
	echo >> VERSION

release:
	echo "Following release targets are available:"
	echo "release-linux"
	echo "release-win32"
	echo "release-src"
	echo "release-doc"
	echo "release-deb"
	echo "release-rpm"
	echo
	echo "To make release-win32 work, place dibbler-server.exe and"
	echo "dibbler-client.exe in this directory."

release-linux: VERSION-linux
	tar czvf dibbler-$(VERSION)-linux.tar.gz                    \
		$(SERVERBIN) $(CLIENTBIN) client*.conf server*.conf \
		CHANGELOG RELNOTES LICENSE VERSION doc/dibbler-user.pdf

release-win32: VERSION-win
	tar czvf dibbler-$(VERSION)-win32.tar.gz                  \
		dibbler-server.exe dibbler-client.exe             \
                client*.conf server*.conf                         \
		CHANGELOG RELNOTES LICENSE VERSION doc/dibbler-user.pdf

release-src: VERSION-src 
	@echo "[RM     ] dibbler-$(VERSION)-src.tar.gz"
	rm -f dibbler-$(VERSION)-src.tar.gz
	cd doc; $(MAKE) clean
	@echo "[CLEAN  ] /bison++"
	cd bison++; $(MAKE) clean
	@echo "[TAR/GZ ] ../dibbler-tmp.tar.gz"
	cd ..; tar czvf dibbler-tmp.tar.gz --exclude CVS --exclude '*.exe' --exclude '*.o' \
        --exclude '*.a' --exclude '*.deb' --exclude '*.tar.gz' dibbler > filelist
	@echo "[RENAME ] dibbler-$(VERSION)-src.tar.gz"
	mv ../dibbler-tmp.tar.gz dibbler-$(VERSION)-src.tar.gz

release-deb: VERSION-linux
	@echo "[RM     ] dibbler_$(VERSION)_i386.deb"
	rm -f dibbler_$(VERSION)_i386.deb
	@echo "[RM     ] port-linux/debian/root"
	rm -rf port-linux/debian/root
	@echo "[MKDIR  ] port-linux/debian/root"
	$(MKDIR) port-linux/debian/root/usr/sbin
	$(MKDIR) port-linux/debian/root/usr/share/doc/dibbler
	$(MKDIR) port-linux/debian/root/usr/share/man/man8
	$(MKDIR) port-linux/debian/root/var/lib/dibbler
	$(MKDIR) port-linux/debian/root/DEBIAN
	@echo "[CP     ] port-linux/debian/root"
	$(CP) port-linux/debian/dibbler-$(VERSION).control port-linux/debian/root/DEBIAN/control
	$(CP) $(SERVERBIN) port-linux/debian/root/usr/sbin
	$(CP) $(CLIENTBIN) port-linux/debian/root/usr/sbin
	$(CP) CHANGELOG port-linux/debian/root/usr/share/doc/dibbler/changelog
	$(CP) RELNOTES port-linux/debian/root/usr/share/doc/dibbler
	$(CP) VERSION port-linux/debian/root/usr/share/doc/dibbler
	$(CP) port-linux/debian//copyright port-linux/debian/root/usr/share/doc/dibbler
	$(CP) doc/dibbler-user.pdf port-linux/debian/root/usr/share/doc/dibbler
	$(CP) doc/man/dibbler-server.8 port-linux/debian/root/usr/share/man/man8
	$(CP) doc/man/dibbler-client.8 port-linux/debian/root/usr/share/man/man8
	$(CP) *.conf port-linux/debian/root/var/lib/dibbler
	@echo "[GZIP   ] port-linux/debian/root"
	gzip -9 port-linux/debian/root/usr/share/doc/dibbler/changelog
	gzip -9 port-linux/debian/root/usr/share/man/man8/dibbler-server.8
	gzip -9 port-linux/debian/root/usr/share/man/man8/dibbler-client.8
	@echo "[STRIP  ] port-linux/debian/root"
	strip --remove-section=.comment port-linux/debian/root/usr/sbin/dibbler-server
	strip --remove-section=.comment port-linux/debian/root/usr/sbin/dibbler-client
	@echo "[CHOWN  ] port-linux/debian/root"
	chown -R root:root port-linux/debian/root/usr
	chown -R root:root port-linux/debian/root/var
	@echo "[CHMOD  ] port-linux/debian/root"
	find port-linux/debian/root/ -type d -exec chmod 755 {} \;
	find port-linux/debian/root/ -type f -exec chmod 644 {} \;
	@echo "[DPKG   ] dibbler_$(VERSION)_i386.deb"
	cd port-linux/debian; dpkg-deb --build root 1>dpkg-deb.log
	mv port-linux/debian/root.deb dibbler_$(VERSION)_i386.deb
	@echo "[LINTIAN] dibbler_$(VERSION)_i386.deb"
	lintian -i dibbler_$(VERSION)_i386.deb &> port-linux/debian/dibbler_$(VERSION)_i386.log

#release-rpm: VERSION-linux release-src
release-rpm: VERSION-linux
	$(MKDIR) port-linux/redhat/BUILD
	$(MKDIR) port-linux/redhat/RPMS/athlon
	$(MKDIR) port-linux/redhat/RPMS/i386
	$(MKDIR) port-linux/redhat/RPMS/i486
	$(MKDIR) port-linux/redhat/RPMS/i586
	$(MKDIR) port-linux/redhat/RPMS/i686
	$(MKDIR) port-linux/redhat/RPMS/noarch
	$(MKDIR) port-linux/redhat/SOURCES
	$(MKDIR) port-linux/redhat/SPECS
	$(MKDIR) port-linux/redhat/SRPMS
	@echo "[CP     ] port-linux/redhat/SOURCES/dibbler-$(VERSION)-src.tar.gz"
	$(CP) dibbler-$(VERSION)-src.tar.gz port-linux/redhat/SOURCES
	@echo "[RPM    ] port-linux/redhat/SPEC/dibbler-$(VERSION).spec"
	rpmbuild --define \'_topdir `pwd`/port-linux/redhat\' -bb port-linux/redhat/SPECS/dibbler-$(VERSION).spec
	cd port-linux/redhat/RPMS/i386; for file in *; do \
	echo "[CP     ] $$file"; \
	$(CP) $$file ../../../.. ; \
	done

release-doc: VERSION-src doc oxygen
	tar czvf dibbler-$(VERSION)-doc.tar.gz VERSION RELNOTES LICENSE CHANGELOG \
                 doc/*.pdf doc/html doc/rfc doc/rfc-drafts 

install: server client doc
	$(MKDIR) $(INST_WORKDIR)
	@echo "[INSTALL] $(SERVERBIN)"
	$(INSTALL) -m 755 $(SERVERBIN) $(INST_WORKDIR)/
	@echo "[INSTALL] $(CLIENTBIN)"
	$(INSTALL) -m 755 $(CLIENTBIN) $(INST_WORKDIR)/
	@for dir in *.conf; do \
		(echo "[INSTALL] $$dir" && $(INSTALL) -m 644 $$dir $(INST_WORKDIR)) \
	done
	$(MKDIR) $(INST_MANDIR)/man8
	@for dir in doc/man/*.8; do \
		(echo "[INSTALL] $$dir" && $(INSTALL) -m 644 $$dir $(INST_MANDIR)/man8) \
	done
	$(MKDIR) $(INST_DOCDIR)/dibbler
	@echo "[INSTALL] /doc/dibbler-user.pdf"
	$(INSTALL) -m 755 doc/dibbler-user.pdf $(INST_DOCDIR)/dibbler/dibbler-user.pdf
	@echo "[INSTALL] /doc/dibbler-devel.pdf"
	$(INSTALL) -m 755 doc/dibbler-devel.pdf $(INST_DOCDIR)/dibbler/dibbler-devel.pdf
	echo "[LINKS  ] $(SERVERBIN) "
	ln -sf $(INST_WORKDIR)/$(SERVERBIN) $(INST_BINDIR)
	echo "[LINKS  ] $(CLIENTBIN) "
	ln -sf $(INST_WORKDIR)/$(CLIENTBIN) $(INST_BINDIR)


fixme:
	rm -rf FIXME
	find . -name \*.cpp -exec grep -H "FIXME" {} \; | tee FIXME

tags:
	@echo "[TAGS   ]"
	rm -f TAGS
	find . -name '*.cpp' -or -name '*.h' | xargs etags

clean-libs:
	find . -name *.a -exec rm {} \;

links: includes
clobber: clean

.PHONY: release-winxp release-src release-linux release-deb relase-rpm VERSION VERSION-win doc
