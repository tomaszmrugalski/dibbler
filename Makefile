include Makefile.inc
PREFIX = .
export TOPDIR=$(CURDIR)

all: includes bison libs server client tags

includes:
	cd $(INCDIR); $(MAKE) links

bison:
	cd $(PREFIX)/bison++; ./configure; $(MAKE)

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
	echo " Windows XP"        >> VERSION
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
	echo "There are 4 release targets available:"
	echo "release-linux"
	echo "release-win32"
	echo "release-src"
	echo "release-doc"
	echo
	echo "To make release-win32 work, place dibbler-server-win32.exe and"
	echo "dibbler-client-win32.exe in this directory."

release-linux: VERSION-linux
	tar czvf dibbler-$(VERSION)-linux.tar.gz                    \
		$(SERVERBIN) $(CLIENTBIN) client*.conf server*.conf \
		CHANGELOG RELNOTES LICENSE VERSION doc/*.pdf

release-win32: VERSION-win
	tar czvf dibbler-$(VERSION)-win32.tar.gz                  \
		dibbler-server-win32.exe dibbler-client-win32.exe \
                client*.conf server*.conf                         \
		CHANGELOG RELNOTES LICENSE VERSION doc/*.pdf

release-src: VERSION-src
	$(MAKE) clean
	rm -f file-list
	echo "Makefile.inc CHANGELOG RELNOTES LICENSE VERSION " > file-list
	find . -name \*.cpp >> file-list
	find . -name \*.h >> file-list
	find . -name \*.c >> file-list
	find . -name Makefile >> file-list
	find . -name \*.tex >> file-list
	find . -name \*.l >> file-list
	find . -name \*.y >> file-list
	find bison++ -name \* >> file-list
	find port-winxp -name \* >> file-list
#	xargs tar czvf dibbler-$(VERSION)-src.tar.gz < file-list
	tar czvf ../dibbler-$(VERSION)-src.tar.gz .
	mv ../dibbler-$(VERSION)-src.tar.gz .
	echo "File is stored in dibbler-$(VERSION)-src.tar.gz"

release-doc: VERSION-src doc oxygen
	tar czvf dibbler-$(VERSION)-doc.tar.gz VERSION RELNOTES LICENSE CHANGELOG doc/*.pdf doc/html

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

.PHONY: release-winxp release-src release-linux VERSION VERSION-win