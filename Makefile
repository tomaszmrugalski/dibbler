include Makefile.inc
PREFIX = .
export TOPDIR=$(CURDIR)

all: includes bison libs server client tags

includes:
	cd $(INCDIR); $(MAKE) all 

bison:
	cd $(PREFIX)/bison++; ./configure; $(MAKE)

client: $(CLIENTBIN)

$(CLIENTBIN): includes commonlibs clntlibs DHCPClient.o $(CLIENT)
	$(CPP) -I $(INCDIR) $(OPTS) $(CLNTLINKOPTS) -o $@ DHCPClient.o $(CLIENT) \
	-L$(MISC)	  -lMisc \
	-L$(CLNTADDRMGR)  -lClntAddrMgr \
	-L$(LOWLEVEL)    \
	-L$(CLNTOPTIONS)  -lClntOpts \
	-L$(OPTIONS)      -lOptions \
	-L$(CLNTTRANSMGR) -lClntTransMgr \
	-L$(CLNTCFGMGR)   -lClntCfgMgr \
	-L$(CLNTIFACEMGR) -lClntIfaceMgr \
	-L$(CLNTMESSAGES) -lClntMsg \
	-L$(CLNTADDRMGR)  -lClntAddrMgr \
	-L$(MISC)         -lMisc \
	-lClntOpts -lOptions -lLowLevel $(XMLLIBS) $(EFENCE) 

server: includes commonlibs srvlibs $(SERVERBIN)
$(SERVERBIN): includes commonlibs clntlibs DHCPServer.o $(SERVER)
	$(CPP) $(OPTS) -I $(INCDIR) $(SRVLINKOPTS) -o $@ DHCPServer.o $(SERVER)  \
	-L$(SRVADDRMGR)  -lSrvAddrMgr \
	-L$(LOWLEVEL)    \
	-L$(SRVOPTIONS)  -lSrvOpts \
	-L$(SRVTRANSMGR) -lSrvTransMgr \
	-L$(SRVCFGMGR)   -lSrvCfgMgr \
	-L$(SRVIFACEMGR) -lSrvIfaceMgr \
	-L$(MISC)        -lMisc\
	-lSrvIfaceMgr -lSrvMsg -lSrvCfgMgr \
	-L$(SRVADDRMGR)  -lSrvAddrMgr \
	-L$(SRVOPTIONS)  -lSrvOpts \
	-L$(SRVTRANSMGR) -lSrvTransMgr \
	-L$(SRVCFGMGR)   -lSrvCfgMgr \
	-L$(SRVIFACEMGR) -lSrvIfaceMgr \
	-L$(SRVMESSAGES) -lSrvMsg \
	-L$(MISC)        -lMisc \
	-L$(OPTIONS)     -lOptions $(XMLLIBS) \
	-L$(LOWLEVEL)    -lLowLevel $(EFENCE) 

DHCPClient.o: DHCPClient.cpp DHCPClient.h
	$(CPP) $(OPTS) -c -I $(INCDIR) -o $@ $<

DHCPServer.o: DHCPServer.cpp DHCPServer.h
	$(CPP) $(OPTS) -c -I $(INCDIR) -o $@ $<

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


tags:
	rm -f TAGS
	find . -name \*.cpp | xargs etags
	find . -name \*.h | xargs etags

clean:
	@for dir in $(SUBDIRS); do \
		echo "Cleaning in $$dir"; \
		(cd $$dir; $(MAKE) clean) || exit 1; \
	done
	cd $(LOWLEVEL); $(MAKE) clean
	rm -f *.o $(CLIENTBIN) $(SERVERBIN)
	rm -f TAGS

clean-libs:
	find . -name *.a -exec rm {} \;