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

release: 
	$(CPP) --version > VERSION
	$(CC) --version  >>VERSION
	echo "Version       : $(VERSION)" >> VERSION
	echo -n "Date          : " >> VERSION
	date +%Y-%m-%d >> VERSION
#	if [ "$XMLCFLAGS" != "" ]; then
#	    echo "libxml2       : YES" >> VERSION
#	else
#	    echo "libxml2       : NO" >> VERSION
#	fi
	tar czvf dibbler-$(VERSION)-linux.tar.gz                  \
		$(SERVERBIN) $(CLIENTBIN) client.conf server.conf \
		CHANGELOG RELNOTES LICENSE VERSION doc/*.pdf
fixme:
	rm -rf FIXME
	find . -name \*.cpp -exec grep -H "FIXME" {} \; | tee FIXME

tags:
	rm -f TAGS
	find . -name \*.cpp | xargs etags
	find . -name \*.h | xargs etags

clean-libs:
	find . -name *.a -exec rm {} \;