include Makefile.inc
PREFIX = .

all: includes libs server client

includes:
	cd $(INCDIR); $(MAKE) all 

client: $(CLIENTBIN)

$(CLIENTBIN): includes DHCPClient.o $(CLIENT)
	cd $(LOWLEVEL);     $(MAKE) libs
	cd $(CLNTTRANSMGR); $(MAKE) libs
	cd $(CLNTIFACEMGR); $(MAKE) libs
	cd $(CLNTADDRMGR);  $(MAKE) libs
	cd $(CLNTCFGMGR);   $(MAKE) libs
	cd $(CLNTMESSAGES); $(MAKE) libs
	cd $(CLNTOPTIONS);  $(MAKE) libs
	cd $(OPTIONS);      $(MAKE) libs
	cd $(MISC);	    $(MAKE) libs
	$(CPP) -I $(INCDIR) $(OPTS) -o $@ DHCPClient.o $(CLIENT) \
	-L$(MISC)	  -lMisc \
	-L$(CLNTADDRMGR)  -lClntAddrMgr \
	-L$(LOWLEVEL)    \
	-L$(CLNTOPTIONS)  -lClntOpts \
	-L$(OPTIONS)      -lOpts \
	-L$(CLNTTRANSMGR) -lClntTransMgr \
	-L$(CLNTCFGMGR)   -lClntCfgMgr \
	-L$(CLNTIFACEMGR) -lClntIfaceMgr \
	-L$(CLNTMESSAGES) -lClntMsg \
	-L$(MISC)         -lMisc \
	-lClntOpts -lOpts $(XMLLIBS) $(EFENCE)

server: $(SERVERBIN)
$(SERVERBIN): includes DHCPServer.o $(SERVER)
	cd $(LOWLEVEL);     $(MAKE) libs
	cd $(SRVTRANSMGR);  $(MAKE) libs
	cd $(SRVIFACEMGR);  $(MAKE) libs
	cd $(SRVADDRMGR);   $(MAKE) libs
	cd $(SRVCFGMGR);    $(MAKE) libs
	cd $(SRVMESSAGES);  $(MAKE) libs
	cd $(SRVOPTIONS);   $(MAKE) libs
	cd $(OPTIONS);      $(MAKE) libs
	cd $(MISC);	    $(MAKE) libs
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ DHCPServer.o $(SERVER)  \
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
	-L$(OPTIONS)     -lOpts $(XMLLIBS) \
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



clean:
	@for dir in $(SUBDIRS); do \
		echo "Cleaning in $$dir"; \
		(cd $$dir; $(MAKE) clean) || exit 1; \
	done
	cd $(LOWLEVEL); $(MAKE) clean
	rm -f *.o $(CLIENTBIN) $(SERVERBIN)
