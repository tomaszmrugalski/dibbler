noinst_LIBRARIES = libSrvMessages.a

libSrvMessages_a_CPPFLAGS = -I$(top_srcdir)/Misc -I$(top_srcdir)/Messages -I$(top_srcdir)/Options -I$(top_srcdir)/SrvOptions
libSrvMessages_a_CPPFLAGS += -I$(top_srcdir)/SrvIfaceMgr -I$(top_srcdir)/IfaceMgr -I$(top_srcdir)/SrvCfgMgr
libSrvMessages_a_CPPFLAGS += -I$(top_srcdir)/CfgMgr -I$(top_srcdir)/AddrMgr -I$(top_srcdir)/SrvAddrMgr
libSrvMessages_a_CPPFLAGS += -I$(top_srcdir)/SrvTransMgr
libSrvMessages_a_CPPFLAGS += -I$(top_srcdir)/poslib/poslib -I$(top_srcdir)/poslib

libSrvMessages_a_SOURCES  = SrvMsgAdvertise.cpp SrvMsgAdvertise.h SrvMsgConfirm.cpp SrvMsgConfirm.h
libSrvMessages_a_SOURCES += SrvMsg.cpp SrvMsg.h SrvMsgDecline.cpp SrvMsgDecline.h
libSrvMessages_a_SOURCES += SrvMsgInfRequest.cpp SrvMsgInfRequest.h SrvMsgLeaseQuery.cpp SrvMsgLeaseQuery.h
libSrvMessages_a_SOURCES += SrvMsgLeaseQueryReply.cpp SrvMsgLeaseQueryReply.h SrvMsgRebind.cpp SrvMsgRebind.h
libSrvMessages_a_SOURCES += SrvMsgRelease.cpp SrvMsgRelease.h SrvMsgRenew.cpp SrvMsgRenew.h
libSrvMessages_a_SOURCES += SrvMsgReply.cpp SrvMsgReply.h SrvMsgRequest.cpp SrvMsgRequest.h
libSrvMessages_a_SOURCES += SrvMsgSolicit.cpp SrvMsgSolicit.h SrvMsgReconfigure.cpp SrvMsgReconfigure.h
