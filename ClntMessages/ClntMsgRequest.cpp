#include "ClntMsgRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptIA_NA.h"
#include "ClntOptElapsed.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptOptionRequest.h"
#include <cmath>
#include "Logger.h"

//opts - options list WITHOUT serverDUID
//FIXED: remover server identifier from opt if it exists there
TClntMsgRequest::TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr, 
				 SmartPtr<TClntAddrMgr> AddrMgr, 
				 TContainer< SmartPtr<TOpt> > opts, 
				 TContainer< SmartPtr<TMsg> > advs,
				 int iface)
     :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface, SmartPtr<TIPv6Addr>() /*NULL*/, REQUEST_MSG) {
    IRT = REQ_TIMEOUT;
    MRT = REQ_MAX_RT;
    MRC = REQ_MAX_RC;
    MRD = 0;   
    RT=0;

    Iface=iface;
    IsDone=false;

    std::clog << logger::logDebug << "Create REQUEST. BackupSrvLst.count()=" 
        << advs.count() << logger::endl;
    if (!advs.count()) 
    {
        // FIXME: set State of unconfigured IAs
        this->IsDone = true;
        return;
    }

    //C++ RULES: to kopiuje jedna liste na druga za pomoca konstruktora 
    BackupSrvLst = advs;

    // looking for first server's DUID on server list	
    BackupSrvLst.first();
    SmartPtr<TClntMsgAdvertise> srv = (Ptr*) BackupSrvLst.get();

    SmartPtr<TOpt> srvDUID;
    
    //Delete all server DUID options if any (can appear after verify)
    opts.first();
    while(srvDUID=opts.get())
    {
        if (srvDUID->getOptType()==OPTION_SERVERID)
            opts.del();
    }
    srvDUID = srv->getOption(OPTION_SERVERID) ;

    // copy whole list from SOLICIT ...
    Options = opts;
    
    // ... and append server's DUID from ADVERTISE
    Options.append( srvDUID );
    
    pkt = new char[getSize()];

    // remove just used server
    BackupSrvLst.delFirst();    
}

TClntMsgRequest::TClntMsgRequest(SmartPtr<TClntIfaceMgr> IfaceMgr, 
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr>   CfgMgr, 
				 SmartPtr<TClntAddrMgr> AddrMgr, 
				 TContainer<SmartPtr<TAddrIA> > IAs,
				 SmartPtr<TDUID> srvDUID,
				 int iface)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,SmartPtr<TIPv6Addr>()/*NULL*/,REQUEST_MSG) {
    IRT = REQ_TIMEOUT;
    MRT = REQ_MAX_RT;
    MRC = REQ_MAX_RC;
    MRD = 0;   
    RT=0;

    Iface=iface;
    IsDone=false;
    SmartPtr<TOpt> ptr;
    ptr = new TClntOptClientIdentifier( CfgMgr->getDUID(), this );
    Options.append( ptr );
    
    ptr = (Ptr*) SmartPtr<TClntOptServerIdentifier> (new TClntOptServerIdentifier(srvDUID,this));
    // all IAs provided by checkSolicit
    SmartPtr<TAddrIA> ClntAddrIA;
    Options.append( ptr );
	
    IAs.first();
    while (ClntAddrIA = IAs.get()) 
    {
        SmartPtr<TClntCfgIA> ClntCfgIA = 
            ClntCfgMgr->getIA(ClntAddrIA->getIAID());
	SmartPtr<TClntOptIA_NA> IA_NA =
	    new TClntOptIA_NA(ClntCfgIA, ClntAddrIA, this);
	Options.append((Ptr*)IA_NA);
    }
    Options.append(new TClntOptElapsed(this));
    pkt = new char[getSize()];
}

void TClntMsgRequest::answer(SmartPtr<TMsg> msg)
{
    //to:    requestedOptions, backSrvLst,   receivedOptions
    //to:    RequestOptions,   BackupSrvLst, Options
    //na to: this->Options   , BackupSrvLst, msg->getOptLst()


    // FIXME: address checking
    // check if addressess are not used
    // remove valid IAs from BackupSrvList's Options
    // if any IA has used addressess, send DECLINE,
    // send next request (with reduced IAs list)
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) this->getOption(OPTION_SERVERID);
    
    // analyse all options received
    SmartPtr<TOpt> option;

    // find ORO in received options
    msg->firstOption();
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt = (Ptr*) msg->getOption(OPTION_ORO);

    msg->firstOption();
    while (option = msg->getOption() ) 
    {
        switch (option->getOptType()) 
        {
            case OPTION_IA:
            {
                SmartPtr<TClntOptIA_NA> clntOpt = (Ptr*)option;
                //FIXME: replace this NULL with something meaningfull
                //       (required for Unicast to work correctly)
                clntOpt->setThats(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
				  ptrDUID->getDUID(), SmartPtr<TIPv6Addr>()/*NULL*/, this->Iface);

                clntOpt->doDuties();

                if (clntOpt->getStatusCode()==STATUSCODE_SUCCESS)
                {
                    //if we have received enough addresses,
                    //remove assigned IA's by server from request message
                    SmartPtr<TOpt> requestOpt;
                    this->Options.first();
                    while (requestOpt = this->Options.get())
                    {
                        if (requestOpt->getOptType()==OPTION_IA)
                        {
                            SmartPtr<TClntOptIA_NA> ptrIA = (Ptr*) requestOpt;
                            if ((ptrIA->getIAID() == clntOpt->getIAID() ) &&
                                (ClntCfgMgr->countAddrForIA(ptrIA->getIAID()) == ptrIA->countAddr()) )	
                            {
                                //found this IA, it has enough addresses and everything is ok.
                                //Shortly, we have this IA configured.
                                this->Options.del();
                                break;
                            }
                        } //if
                    } //while
                }
                break;
            }
            case OPTION_IAADDR:
                std::clog << logger::logWarning << "Option OPTION_IAADDR misplaced." << logger::endl;
                break;
            default:
            {
                option->setParent(this);
                if (option->doDuties()) 
                {
                    SmartPtr<TOpt> requestOpt;
                    this->Options.first();
                    while ( requestOpt = this->Options.get()) {
                        if ( requestOpt->getOptType() == option->getOptType() ) 
                        {
                            if (ptrOptionReqOpt&&(ptrOptionReqOpt->isOption(option->getOptType())))
                                ptrOptionReqOpt->delOption(option->getOptType());
                            this->Options.del();
                        }//if
                    }//while
                }
            }
        }
    }
    //Options and IAs serviced by server are removed from requestOptions list

    SmartPtr<TOpt> requestOpt;
    this->Options.first();
    bool isIA = false;
    while ( requestOpt = this->Options.get()) {
        if (requestOpt->getOptType() == OPTION_IA) {
            isIA = true;
            break;
        }
    }
    if (isIA) {
        // send new Request to another server
        clog<<logger::logNotice<<"There are still some IA to configure" << logger::endl;
        ClntTransMgr->sendRequest(this->Options, BackupSrvLst, this->Iface);
    } else {
        if (ptrOptionReqOpt&&(ptrOptionReqOpt->getOptCnt()))
        {
            clog<<logger::logNotice<<"All Identity associations were supplied, but not all requested options.";
            clog<<"Sending Information Request" << logger::endl;
            ClntTransMgr->sendInfRequest(this->Options, this->Iface);
            //Here should be information request message send
            //FIXME: only options, send Information-Request to another server
        }
    }
    IsDone = true;
    return;


}

void TClntMsgRequest::doDuties()
{
    // mamy timeout i nadal nie ma odpowiedzi, retransmisja
    //uplynal ostateczny timeout dla wiadomosci
    if (RC>MRC) 
    {
	// przekroczony MRC, brak odpowiedzi i nie ma ju� nowych serwer�w
	if (BackupSrvLst.count() == 0) 
	{ 
	    SmartPtr<TOpt> option;
	    Options.first();
	    while (option = Options.get()) 
	    {
		if (option->getOptType() == OPTION_IA)
		{
		    ClntCfgMgr->setIAState(Iface,( (SmartPtr<TOptIA_NA>)option )->getIAID(),FAILED);
		}
	    }
	    IsDone = true;
	    return;
	}
	
//    void sendRequest(TContainer< SmartPtr<TOpt> > requestOptions, 
//		     TContainer< SmartPtr<TMsg> > srvlist,int iface);
	ClntTransMgr->sendRequest(Options, BackupSrvLst, Iface);

	IsDone = true;
	return;
    }
    send();
    return;
}

bool TClntMsgRequest::check()
{
    return false;
}

TClntMsgRequest::~TClntMsgRequest()
{
    delete pkt;
    pkt=NULL;
}
