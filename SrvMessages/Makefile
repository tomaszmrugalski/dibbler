include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libSrvMsg.a

libSrvMsg.a: 	SrvMsgAdvertise.o	SrvMsgDecline.o		SrvMsgRenew.o	  SrvMsgSolicit.o   \
		SrvMsgConfirm.o	  	SrvMsgRebind.o   	SrvMsgReply.o	  \
		SrvMsg.o		SrvMsgRelease.o  	SrvMsgRequest.o   SrvMsgInfRequest.o
	cd $(MESSAGES); $(MAKE) libs
	ar cr $@ $(MESSAGES)/Msg.o \
		SrvMsgAdvertise.o  	SrvMsgDecline.o  	SrvMsgRenew.o	  SrvMsgSolicit.o   \
		SrvMsgConfirm.o	 	SrvMsgRebind.o   	SrvMsgReply.o	  \
	 	SrvMsg.o		SrvMsgRelease.o  	SrvMsgRequest.o   SrvMsgInfRequest.o

# tests

test: test1 

test1: test1.cpp 
	$(CPP) $(OPTS) -I $(INCDIR) -o $@ test1.cpp 

# Objects 
objs: SrvMsgAdvertise.o  SrvMsgDecline.o  SrvMsgRenew.o	  SrvMsgSolicit.o   \
SrvMsgConfirm.o	  SrvMsgRebind.o   SrvMsgReply.o	  \
SrvMsg.o		  SrvMsgRelease.o  SrvMsgRequest.o SrvMsgInfRequest.o


SrvMsg.o: SrvMsg.cpp SrvMsg.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgAdvertise.o: SrvMsgAdvertise.cpp SrvMsgAdvertise.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgConfirm.o: SrvMsgConfirm.cpp SrvMsgConfirm.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgDecline.o: SrvMsgDecline.cpp SrvMsgDecline.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgRebind.o: SrvMsgRebind.cpp SrvMsgRebind.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgRelease.o: SrvMsgRelease.cpp SrvMsgRelease.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgRenew.o: SrvMsgRenew.cpp SrvMsgRenew.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgReply.o: SrvMsgReply.cpp SrvMsgReply.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgRequest.o: SrvMsgRequest.cpp SrvMsgRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgSolicit.o: SrvMsgSolicit.cpp SrvMsgSolicit.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

SrvMsgInfRequest.o: SrvMsgInfRequest.cpp SrvMsgInfRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o *.a
	rm -f test? test??