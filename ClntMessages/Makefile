include ../Makefile.inc

all: objs libs

client: objs

server: objs

libs: libClntMsg.a

libClntMsg.a: 	ClntMsg.o 		ClntMsgAdvertise.o  	ClntMsgDecline.o   \
		ClntMsgRenew.o		ClntMsgSolicit.o 	ClntMsgConfirm.o   \
		ClntMsgRebind.o		ClntMsgReply.o	 	ClntMsgInfRequest.o\
		ClntMsgRelease.o  	ClntMsgRequest.o
	cd $(MESSAGES); $(MAKE) Msg.o
	ar cr $@ $(MESSAGES)/Msg.o  \
		ClntMsgAdvertise.o  	ClntMsgDecline.o  	ClntMsgRenew.o     \
		ClntMsgSolicit.o   	ClntMsgConfirm.o	ClntMsgRebind.o    \
		ClntMsgReply.o	  	ClntMsgInfRequest.o  	ClntMsg.o          \
		ClntMsgRelease.o        ClntMsgRequest.o        

test: 

# Objects 
objs: 		ClntMsg.o 		ClntMsgAdvertise.o  	ClntMsgDecline.o   \
		ClntMsgRenew.o		ClntMsgSolicit.o	ClntMsgConfirm.o   \
		ClntMsgRebind.o		ClntMsgReply.o		ClntMsgInfRequest.o\
		ClntMsgRelease.o	ClntMsgRequest.o        

ClntMsg.o: ClntMsg.cpp ClntMsg.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgAdvertise.o: ClntMsgAdvertise.cpp ClntMsgAdvertise.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgConfirm.o: ClntMsgConfirm.cpp ClntMsgConfirm.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgDecline.o: ClntMsgDecline.cpp ClntMsgDecline.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgRebind.o: ClntMsgRebind.cpp ClntMsgRebind.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgRelease.o: ClntMsgRelease.cpp ClntMsgRelease.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgRenew.o: ClntMsgRenew.cpp ClntMsgRenew.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgReply.o: ClntMsgReply.cpp ClntMsgReply.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgRequest.o: ClntMsgRequest.cpp ClntMsgRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgSolicit.o: ClntMsgSolicit.cpp ClntMsgSolicit.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

ClntMsgInfRequest.o: ClntMsgInfRequest.cpp ClntMsgInfRequest.h
	$(CPP) $(OPTS) -I $(INCDIR) -c -o $@ $<

clean:
	rm -f *.o *.a
	rm -f test? test??