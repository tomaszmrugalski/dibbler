#include <limits.h>
#include "SrvTransMgr.h"
#include "SmartPtr.h"
#include "SrvCfgIface.h"
#include "Iface.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "AddrClient.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgReply.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRelease.h"
#include "SrvOptIA_NA.h"
#include "SrvOptStatusCode.h"

TSrvTransMgr::TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
                           string newconf, string oldconf)
{
    // FIXME: loadDB
    bool loadDB = false;

    // remember IfaceMgr and create remaining managers
    IfaceMgr = ifaceMgr;
    CfgMgr  = new TSrvCfgMgr(this->IfaceMgr, newconf, oldconf);
    if (!CfgMgr->isDone())
    {
        AddrMgr = new TSrvAddrMgr( CfgMgr->getWorkDir()+"/"+SRVDB_FILE, loadDB);
	AddrMgr->dbStore();

        // for each interface in CfgMgr, create socket (in IfaceMgr)
        SmartPtr<TSrvCfgIface> confIface;
        CfgMgr->firstIface();
        char srvAddr[16];
        inet_pton6(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,srvAddr);
        SmartPtr<TIPv6Addr> ipAddr(new TIPv6Addr(srvAddr));
        while (confIface=CfgMgr->getIface())
        {
            // FIXME: check for NO-CONFIG
            SmartPtr<TIfaceIface> iface=IfaceMgr->getIfaceByID(confIface->getID());
            std::clog << logger::logNotice << "Creating ff02::1:2 socket on " << confIface->getName() 
                << " interface." << logger::endl;
            iface->addSocket( ipAddr, DHCPSERVER_PORT, false);
        }

        // TransMgr is certainly not done yet. We're just getting started
        IsDone = false;
    }
    else
        IsDone=true;
    if (loadDB)
	AddrMgr->doDuties();
}

long TSrvTransMgr::getTimeout()
{
    unsigned long min = 0xffffffff;
    unsigned long addrTimeout = 0xffffffff;
    SmartPtr<TMsg> ptrMsg;
    MsgLst.first();
    while (ptrMsg = MsgLst.get() ) 
    {
        if (ptrMsg->getTimeout() < min) 
            min = ptrMsg->getTimeout();
    }
    addrTimeout = AddrMgr->getTimeout();
//    std::clog << logger::logDebug << "AddrMgr returned " << addrTimeout << " timeout." << logger::endl;
    return min<addrTimeout?min:addrTimeout;
}

void TSrvTransMgr::relayMsg(SmartPtr<TMsg> msg)
{	
    if (!msg->check())
    {
        clog << logger::logWarning << "Invalid message received." << logger::endl;
        return;
    }
    // Do we have ready answer for this?
    SmartPtr<TMsg> answ;
    Log(Debug) << MsgLst.count() << " answers buffered.";

    MsgLst.first();
    while(answ=MsgLst.get()) 
    {
        if (answ->getTransID()==msg->getTransID()) 
        {
            std::clog << "Old reply with transID (" << hex << msg->getTransID() 
		      << dec << ") found. Sending old reply." << logger::endl;
            answ->answer(msg);
            return;
        }
    }
    std::clog << "Old reply for transID=" << hex << msg->getTransID()
	      << " not found. Generating new answer." << dec << logger::endl;

    switch(msg->getType()) 
    {
        case SOLICIT_MSG: 
        {
            //Here should be checked whether message solicit contains Rapid 
            //Commit Option If yes - rapid Reply message shlould be constructed
            //if server is allowed to answer in such way for this 
            //iface/class/client i.e. there is/are appropriate class(es) with
            //Rapid commit option and there is enough addresses to assign to
            //at least one IA use rapid method of answer
            if (msg->getOption(OPTION_RAPID_COMMIT))
            {
                SmartPtr<TSrvMsgSolicit> nmsg = (Ptr*)msg;
                SmartPtr<TSrvMsgReply>
                    answRep=new TSrvMsgReply(IfaceMgr, That, CfgMgr, AddrMgr, nmsg);
                //if at least one IA has in reply message status success
                if (!answRep->isDone())
                {
                    SmartPtr<TOpt> ptrOpt;
                    answRep->firstOption();
                    bool notFound=true;
                    while((ptrOpt=answRep->getOption())&&(notFound))
                    {
                        if (ptrOpt->getOptType()==OPTION_IA)
                        {
                            SmartPtr<TSrvOptIA_NA> ptrIA=(Ptr*) ptrOpt;
                            SmartPtr<TSrvOptStatusCode> ptrStat= (Ptr*)
                                ptrIA->getOption(OPTION_STATUS_CODE);
                            if(ptrStat&&(ptrStat->getCode()==STATUSCODE_SUCCESS))
                                notFound=false;
                        }
                    }
                    if(!notFound)
                    {
                        this->MsgLst.append((Ptr*)answRep);
                        break;
                    }
                    //else we didn't assign any address - this message sucks
                    //it'd better if advertise will be sent - maybe with better
                    //options
                }   
                //else execute instructions after if (no addresses were assigned,
                //nothing has happened)
            }
            //if there was no Rapid Commit option in solicit message or
            //construction of reply with rapid commit wasn't successful
            //Maybe it's possible to construct appropriate advertise message
            //and assign some "not rapid" addresses to this client
            SmartPtr<TSrvMsgAdvertise> x = new TSrvMsgAdvertise(IfaceMgr,That,CfgMgr,AddrMgr,(Ptr*)msg);
            this->MsgLst.append((Ptr*)x);
            break;
        }
        case REQUEST_MSG: 
        {
            SmartPtr<TSrvMsgRequest> nmsg = (Ptr*)msg;
            answ=new TSrvMsgReply(IfaceMgr, That, CfgMgr, AddrMgr, nmsg);
            this->MsgLst.append((Ptr*)answ);
            break;
        }
        case CONFIRM_MSG: 
        {
            SmartPtr<TSrvMsgConfirm> nmsg=(Ptr*)msg;
            answ=new TSrvMsgReply(IfaceMgr,That,CfgMgr,AddrMgr,nmsg);
            this->MsgLst.append((Ptr*)answ);
            break;
        }
        case RENEW_MSG: 
        {
            SmartPtr<TSrvMsgRenew> nmsg=(Ptr*)msg;
            answ=new TSrvMsgReply(IfaceMgr, That,CfgMgr,AddrMgr,nmsg);
            this->MsgLst.append((Ptr*)answ);
            break;
        }
        case REBIND_MSG: 
        {
            SmartPtr<TSrvMsgRebind> nmsg=(Ptr*)msg;
            answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
            MsgLst.append((Ptr*)answ);
            break;
        }
        case DECLINE_MSG: 
        {
            SmartPtr<TSrvMsgDecline> nmsg=(Ptr*)msg;
            answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
            MsgLst.append((Ptr*)answ);
            break;
        }
        case RELEASE_MSG: 
        {
            SmartPtr<TSrvMsgRelease> nmsg=(Ptr*)msg;
            answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
            MsgLst.append((Ptr*)answ);
            break;
        }
        case INFORMATION_REQUEST_MSG : 
        {
             SmartPtr<TSrvMsgInfRequest> nmsg=(Ptr*)msg;
             answ=new TSrvMsgReply( IfaceMgr,  That, CfgMgr, AddrMgr,nmsg);
             MsgLst.append((Ptr*)answ);
            // FIXME: INFORMATION-REQUEST not supported
            break;
        }
        case RECONFIGURE_MSG:
        case ADVERTISE_MSG:
        case REPLY_MSG: 
        {
	    Log(Warning) << "Invalid message type received" << LogEnd;
            break;
        }
        case RELAY_FORW:
        case RELAY_REPL:
        default:
        {
            std::clog << logger::logWarning << "Message type " << msg->getType() 
		      << " not supported." << logger::endl;
            break;
        }
    }
    
    // save DB state regardless of action taken
    AddrMgr->dbStore();
    CfgMgr->dump();
}	

void TSrvTransMgr::doDuties()
{
    // are there any outdated addresses? 
    if (!AddrMgr->getTimeout())
        AddrMgr->doDuties();

    // for each message on list, let it do its duties, if timeout is reached
    SmartPtr<TMsg> msg;
    MsgLst.first();
    while (msg=MsgLst.get()) 
        if ( (!msg->getTimeout()) && (!msg->isDone()) ) 
            msg->doDuties();

    // now delete messages marked as done
    MsgLst.first();
    while (msg = MsgLst.get() ) 
    {
        if (msg->isDone()) 
        {
            MsgLst.del();
        }
    }
}

void TSrvTransMgr::shutdown()
{
    AddrMgr->dbStore();
    IsDone = true;
}

bool TSrvTransMgr::isDone()
{
    return IsDone;
}

void TSrvTransMgr::setThat(SmartPtr<TSrvTransMgr> that)
{
    this->That=that;
    IfaceMgr->setThats(IfaceMgr,That,CfgMgr,AddrMgr);
}

char* TSrvTransMgr::getCtrlAddr() {
	return this->ctrlAddr;
}

int  TSrvTransMgr::getCtrlIface() {
	return this->ctrlIface;
}

