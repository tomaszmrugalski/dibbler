#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include <iostream>
#include <string>

#include "SmartPtr.h"
#include "ClntTransMgr.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "Msg.h"
#include "ClntMsgRequest.h"
#include "ClntMsgRenew.h"
#include "ClntMsgRebind.h"
#include "ClntMsgRelease.h"
#include "ClntMsgSolicit.h"
#include "ClntMsgInfRequest.h"
#include "ClntMsgDecline.h"
#include "ClntMsgConfirm.h"
#include "Container.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "ClntMsgDecline.h"

using namespace std;


TClntTransMgr::TClntTransMgr(SmartPtr<TClntIfaceMgr> ifaceMgr, 
                             string config)
{
    //FIXME: oldconf not used
    string oldConf = config+"-old";

    //FIXME: loadDB
    bool loadDB = false;

    this->isStart=true;
    // create managers
    CfgMgr = new TClntCfgMgr(ifaceMgr, config, oldConf);
    IfaceMgr=ifaceMgr;

    AddrMgr=new TClntAddrMgr(CfgMgr, CLNTDB_FILE, loadDB);

    if (!CfgMgr->isDone())
    {
        SmartPtr<TClntCfgIface> iface;
        CfgMgr->firstIface();
        while(iface=CfgMgr->getIface())
        {
            iface->firstGroup();
            SmartPtr<TClntCfgGroup> group;
            while(group=iface->getGroup())
            {
                SmartPtr<TClntCfgIA> ia;
                group->firstIA();
                while(ia=group->getIA())
                    AddrMgr->addIA(new TAddrIA(iface->getID(),SmartPtr<TIPv6Addr>(), 
                    SmartPtr<TDUID>(),0x7fffffff,0x7fffffff,ia->getIAID()));
            }
        }

        //In my opinion on every interface, which is up
        //open socket on port 546 and ANY_ADDRESS, through which all messages 
        //directed to all_dhcp_server_and_relay will be sent and answers will
        //be received
        //unsigned char addrlaptop[16]={0xfe,0x80,0,0,0,0,0,0,0x2,0x0,0x39,0xff,0xfe,0x3a,0x1d,0xb2};
        //unsigned char addrloop[16]={0xfe,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,1}; 

        CfgMgr->firstIface();
        while(iface=CfgMgr->getIface())
        {
            if (!iface->isNoConfig())
            {
                SmartPtr<TIfaceIface> realIface=IfaceMgr->getIfaceByID(iface->getID());
                if (realIface&&realIface->flagUp()&&realIface->flagRunning())
                {
                    realIface->firstLLAddress();
                    char* llAddr;
                    while((llAddr=realIface->getLLAddress())&&
                        (!realIface->addSocket(new TIPv6Addr(llAddr),DHCPCLIENT_PORT,false)));

                    if (llAddr)
                    {
			char buf[48];
                        std::clog<<logger::logInfo<<"Socket created on ";
			inet_ntop6(llAddr,buf);
			std::clog << buf << "/port=" << DHCPCLIENT_PORT << logger::endl;
			this->ctrlIface = realIface->getID();
			memcpy(this->ctrlAddr,buf,48);
                    }
                    else
                    {
                        std::clog << logger::logInfo << "Unable to create any socket on iface:"
				  << iface->getID() 
				  << " (No appropriate link local address available"
				  << logger::endl;
                    }
                }
            }
        }

    }

    // we're just getting started
    if (loadDB) {
	this->checkDB();
	checkConfirm();
    }

    IsDone = CfgMgr->isDone();
    Shutdown = false;
}

void TClntTransMgr::checkDB()
{
    SmartPtr <TAddrIA> ptrIA;
    AddrMgr->firstIA();
    AddrMgr->doDuties();
    while ( ptrIA = AddrMgr->getIA()) {
        SmartPtr<TIfaceIface> ptrIface;
        ptrIface = IfaceMgr->getIfaceByID( ptrIA->getIface() );
        if (!ptrIface) {
            // IA with non-existent iface, purge iface
            SmartPtr<TAddrAddr> ptrAddr;
            ptrIA->firstAddr();
            while (ptrAddr = ptrIA->getAddr())
                ptrIA->delAddr( ptrAddr->get() );
            ptrIA->setState(NOTCONFIGURED);
        }
    }

}


void TClntTransMgr::doDuties()
{
    //Metoda wywo³ywana w momecie, gdy nie przysz³a ¿adna wiadomo¶æ, a
    //powinna byæ wykonana jaka¶ czynno¶æ zwi±zana z gospodark± adresami
    //(np. wygenerowanie RENEW/REBIND itp.) lub z rozpoczêtymi transakcjami
    //(np. retransmisja). Metoda wywo³uje odpowiednio doDuties na wszystkich
    //wiadomo¶ciach/transakcjach oraz wywo³uje:

    // for each message on list, let it do its duties, if timeout is reached
    SmartPtr <TMsg> msg;
    Transactions.first();
    while(msg=Transactions.get())
    {
        if ((!msg->getTimeout())&&(!msg->isDone()))
            msg->doDuties();
    }

    // now delete messages which are marked as done
    Transactions.first();
    while (msg = Transactions.get() ) {
        if (msg->isDone())
            Transactions.del();
    }

    this->removeExpired();

    AddrMgr->dbStore();

    if (!this->Shutdown && !this->IsDone) {
        // are there any tentative addrs?
        checkDecline();

        // are there any IAs to configure?
        checkSolicit();    

        //is there any IA in Address manager, which has not sufficient number 
        //of addresses
        checkRequest();

        // are there any aging IAs?
        checkRenew();

        // did we switched links lately?
	// FIXME: for now, we cannot check link switches, so checkConfirm
	// is run only during startup
        //checkConfirm();        

        //Maybe we require only infromations concernig link
        checkInfRequest();
    } 

    if (this->Shutdown && !Transactions.count())
        this->IsDone = true;
}

// removes expired addrs from addrDB and IfaceMgr
void TClntTransMgr::removeExpired()
{
    if (AddrMgr->getValidTimeout())
        return;

    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TAddrAddr> ptrAddr;
    SmartPtr<TIfaceIface> ptrIface;
    AddrMgr->firstIA();

    while ( ptrIA = AddrMgr->getIA() ) {
        if (ptrIA->getValidTimeout())
            continue;
        ptrIA->firstAddr();
        while ( ptrAddr = ptrIA->getAddr() ) {
            if (ptrAddr->getValidTimeout())
                continue;
            // we fount it, at last!
            // remove it from addrMgr ...
            ptrIA->delAddr(ptrAddr->get());
            // ... and from IfaceMgr
            ptrIface = IfaceMgr->getIfaceByID( ptrIA->getIface() );
        }
    }
}

void TClntTransMgr::shutdown()
{
    this->Shutdown = true;

    std::clog << logger::logNotice << "Shutting down entire client." << logger::endl;

    // delete all transactions
    Transactions.clear();

    List(TAddrIA) releasedIAs;

    SmartPtr<TAddrIA> ptrFirstIA;
    SmartPtr<TAddrIA> ptrNextIA;

    // delete all weird-state and address-free IAs 
    AddrMgr->firstIA();
    while (ptrFirstIA = AddrMgr->getIA()) {
        if ( (ptrFirstIA->getState() != CONFIGURED && ptrFirstIA->getState() != INPROCESS) ||
            !ptrFirstIA->countAddr() )
            AddrMgr->delIA(ptrFirstIA->getIAID()); 
    }

    // normal IAs are to be released
    while (AddrMgr->countIA()) {
        // clear the list
        releasedIAs.clear();

        // get first IA
        AddrMgr->firstIA();
        ptrFirstIA = AddrMgr->getIA();
        releasedIAs.append(ptrFirstIA);
        AddrMgr->delIA( ptrFirstIA->getIAID() );

        // find similar IAs 
        while (ptrNextIA = AddrMgr->getIA()) {
            if ((*(ptrFirstIA->getDUID())==*(ptrNextIA->getDUID())) &&
                (ptrFirstIA->getIface() == ptrNextIA->getIface() ) ) {
                    // IA serviced via this same server, add it do the list
                    releasedIAs.append(ptrNextIA);

                    // delete addressess from IfaceMgr
                    SmartPtr<TAddrAddr> ptrAddr;
                    SmartPtr<TIfaceIface> ptrIface;
                    ptrIface = IfaceMgr->getIfaceByID(ptrNextIA->getIface());
                    if (!ptrIface) {
                        std::clog << logger::logError << "Unable to find " << ptrNextIA->getIface()
                            << " interface while releasing address." << logger::endl;
                        break;
                    }
                    ptrNextIA->firstAddr();
                    while (ptrAddr = ptrNextIA->getAddr() ) {
                        ptrIface->delAddr( ptrAddr->get() );
                    }

                    // delete IA from AddrMgr
                    AddrMgr->delIA( ptrNextIA->getIAID() );
                }
        }
        if (releasedIAs.count()) 
            this->sendRelease(releasedIAs);
    }
}

void TClntTransMgr::relayMsg(SmartPtr<TMsg>  msgAnswer)
{
    // is message valid?
    if (!msgAnswer->check())
        return ;

    // find which message this is answer for
    bool found = false;
    SmartPtr<TMsg> msgQuestion;
    Transactions.first();
    while(msgQuestion=Transactions.get()) {
        if (msgQuestion->getTransID()==msgAnswer->getTransID()) {
            found =true;
            msgQuestion->answer(msgAnswer);
            break;
        }
    }

    if (!found) 
        std::clog << logger::logWarning << "Message with wrong TransID (" << msgAnswer->getTransID()
        << ") received. Ignoring." << logger::endl;

    // save DB state
    AddrMgr->dbStore();
}

unsigned long TClntTransMgr::getTimeout()
{
    unsigned long timeout = DHCPV6_INFINITY;
    unsigned long addrTimeout = DHCPV6_INFINITY;
    unsigned long addrTentativeTimeout = DHCPV6_INFINITY;

    if (this->IsDone) 
        return 0;

    addrTimeout = AddrMgr->getTimeout();
    std::clog << logger::logDebug << "AddrMgr returned " << addrTimeout 
        << " timeout." << logger::endl;

    addrTentativeTimeout = AddrMgr->getTentativeTimeout();
    std::clog << logger::logDebug << "AddrMgr returned " << addrTentativeTimeout 
        << " tentative timeout." << logger::endl;
    if (addrTentativeTimeout < addrTimeout)
        addrTimeout = addrTentativeTimeout;

    SmartPtr<TMsg> ptrMsg;
    Transactions.first();
    while(ptrMsg=Transactions.get())
    {
        if (ptrMsg->getTimeout()<timeout)
            timeout=ptrMsg->getTimeout();
        std::clog << logger::logDebug << 
            "Msg (transID=" << hex << ptrMsg->getTransID() << dec
            << " type:"<<ptrMsg->getType()
            <<") returned timemout " << ptrMsg->getTimeout() << logger::endl;
    }
    if (timeout < addrTimeout)
        return timeout;
    else
        return addrTimeout;
}

void TClntTransMgr::stop()
{
}
// requestOptions list MUST NOT contain server DUID.
void TClntTransMgr::sendRequest( TContainer< SmartPtr<TOpt> > requestOptions, 
                                TContainer< SmartPtr<TMsg> > srvlist,int iface)
{
    SmartPtr<TMsg> ptr = new TClntMsgRequest(IfaceMgr,That,CfgMgr, AddrMgr, requestOptions,srvlist,iface);
    Transactions.append( ptr );
}

// Send RELEASE message
void TClntTransMgr::sendRelease( List(TAddrIA) IALst)
{
    if (!IALst.count()) {
        std::clog << logger::logError << "Unable to send RELEASE with empty IAs list." << logger::endl;
        return;
    }

    SmartPtr<TAddrIA> ptrIA;
    IALst.first();
    ptrIA = IALst.get();
    std::clog << logger::logNotice << "Sending RELEASE for " << IALst.count() << " IAs" << logger::endl;

    SmartPtr<TMsg> ptr = new TClntMsgRelease(IfaceMgr,That,CfgMgr, AddrMgr, ptrIA->getIface(), 
        ptrIA->getSrvAddr(), IALst);
    Transactions.append( ptr );
}

// Send REBIND message
void TClntTransMgr::sendRebind( TContainer<SmartPtr<TOpt> > ptrOpts, int iface)
{
    SmartPtr<TMsg> ptr =  new TClntMsgRebind(IfaceMgr, That, CfgMgr, AddrMgr, ptrOpts, iface);
    Transactions.append( ptr );
}

// send VERIFY pseudo-message
void TClntTransMgr::sendVerify(TContainer< SmartPtr<TOpt> > requestOptions,
                               TContainer< SmartPtr<TMsg> > srvlist,
                               TContainer< SmartPtr<TOpt> > replyOptions,
                               int iface, SmartPtr<TIPv6Addr> peeraddr)
{
}

void TClntTransMgr::sendInfRequest(
                                   TContainer< SmartPtr<TOpt> > requestOptions, 
                                   int iface)
{
    SmartPtr<TMsg> ptr = new TClntMsgInfRequest(
        IfaceMgr,That,CfgMgr,AddrMgr,requestOptions,iface);
    if (!ptr->isDone())
	Transactions.append( ptr );    
}

// should we send SOLICIT ?
void TClntTransMgr::checkSolicit()
{
    //For every iface, every group in iface in ClntCfgMgr 
    //Enumerate IA's from this group
    SmartPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->isNoConfig())
            continue;
        SmartPtr<TClntCfgGroup> group;
        iface->firstGroup();
        while(group=iface->getGroup())
        {
            List(TClntCfgIA) IALstToConfig;
            SmartPtr<TClntCfgIA> ia;
            group->firstIA();
            while(ia=group->getIA())
            {
                //These not assigned in AddrMgr and configurable and not in trasaction
                //group and pass to constructor of Solicit message
                SmartPtr<TAddrIA> iaAddrMgr=AddrMgr->getIA(ia->getIAID());
                if(iaAddrMgr->getState()==NOTCONFIGURED)
                {
                    IALstToConfig.append(ia);
                    iaAddrMgr->setState(INPROCESS);
                }
            };
            if (IALstToConfig.count()) //Are there any IA, which should be configured?
                Transactions.append(
                new TClntMsgSolicit(IfaceMgr,That,CfgMgr,AddrMgr,
				    iface->getID(), SmartPtr<TIPv6Addr>()/*NULL*/, 
				    IALstToConfig, group->getRapidCommit()));
        }//for every group
    }//for every iface
}

void TClntTransMgr::checkConfirm()
{
    //FIXME: When should we send CONFIRM? How to detect switching to new link?
    //Is it a start of address of manager
    if(!isStart)
        return;
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TIfaceIface> ptrIface;
    IfaceMgr->firstIface();
    while(ptrIface=IfaceMgr->getIface())
    {
        TContainer<SmartPtr<TAddrIA> > IALst;
        AddrMgr->firstIA();
        while(ptrIA=AddrMgr->getIA())
        {
            if(ptrIA->getIface()==ptrIface->getID())
            {
                IALst.append(ptrIA);
                ptrIA->setState(INPROCESS);
            }
        }
        if (IALst.count())
            Transactions.append(
            new TClntMsgConfirm(IfaceMgr,That,CfgMgr,AddrMgr,ptrIface->getID(),IALst));
    }
}

void TClntTransMgr::checkInfRequest()
{
    static bool firstTime = true;
    if (!firstTime)
	return;
    firstTime = false;
    SmartPtr<TClntCfgIface> iface;
    CfgMgr->firstIface();
    while( (iface=CfgMgr->getIface()) )
    {
        if (iface->isNoConfig())
            continue;
        if (iface->onlyInformationRequest())
            Transactions.append(new TClntMsgInfRequest(IfaceMgr,That,CfgMgr,AddrMgr,iface));
    }
}

void TClntTransMgr::checkRenew()
{
    // are there any IAs which require RENEW?
    if (AddrMgr->getT1Timeout() > 0 ) return;

    // yes, there are. Find them!
    AddrMgr->firstIA();
    SmartPtr < TAddrIA> ptrIA;
    while (ptrIA = AddrMgr->getIA() ) 
    {
        if ( (ptrIA->getT1Timeout() == 0) && (ptrIA->getState()==CONFIGURED) ) 
        {
            // to avoid race conditions (RENEW vs DECLINE)
            if (ptrIA->getTentative()==DONTKNOWYET)
                continue;

            TContainer<SmartPtr<TAddrIA> > iaLst;
            std::clog << logger::logNotice << "IA (IAID=" << ptrIA->getIAID() 
                << " needs RENEW. Creating one and grouping with other IA:" << logger::endl;
            SmartPtr<TAddrIA> iaPattern=ptrIA;
            iaLst.append(ptrIA);
            ptrIA->setState(INPROCESS);
            while(ptrIA = AddrMgr->getIA())
            {
                //the same diffrence between T1(which has just elapsed
                //for both IAs) and T2
                if (((ptrIA->getT2()-ptrIA->getT1())==
                    (iaPattern->getT2()-iaPattern->getT1()))&&
                    (ptrIA->getIface()==iaPattern->getIface())&&
                    (ptrIA->getDUID()==iaPattern->getDUID()))
                {
                    iaLst.append(ptrIA);
                    ptrIA->setState(INPROCESS);
                }
            }
            SmartPtr <TMsg> ptrRenew = new TClntMsgRenew(IfaceMgr, That, CfgMgr, AddrMgr, iaLst);
            Transactions.append(ptrRenew);
            AddrMgr->firstIA();
        }
    }
}

void TClntTransMgr::checkDecline()
{
    //Find any tentative address and remove them from address manager
    //Group declined addresses and IAs by server and send Decline
    //Find first IA with tentative addresses
    SmartPtr<TAddrIA> firstIA;
    SmartPtr<TAddrIA> ptrIA;
    int result;
    do
    {
        firstIA=SmartPtr<TAddrIA>(); // NULL
        TContainer<SmartPtr<TAddrIA> > declineIALst;

        AddrMgr->firstIA();
        while((ptrIA=AddrMgr->getIA())&&(!firstIA))
        {
            if (ptrIA->getTentative()==YES)
                firstIA=ptrIA;
        }
        if (firstIA)
        {
            declineIALst.append(firstIA);
            while(ptrIA=AddrMgr->getIA())
            {
                if ((ptrIA->getTentative()==YES)&&
                    (*ptrIA->getDUID()==*firstIA->getDUID()))
                {
                    declineIALst.append(ptrIA);
                }

            }
            //Here should be send decline for all tentative addresses in IAs
            SmartPtr<TClntMsgDecline> decline = 
                new TClntMsgDecline(IfaceMgr, That, CfgMgr, AddrMgr,
                firstIA->getIface(), SmartPtr<TIPv6Addr>(),
                declineIALst);
            Transactions.append( (Ptr*) decline);

            // decline sent, now remove those addrs from IfaceMgr
            SmartPtr<TIfaceIface> ptrIface = IfaceMgr->getIfaceByID(firstIA->getIface());

            declineIALst.first();
            while (ptrIA = declineIALst.get() ) {
                SmartPtr<TAddrAddr> ptrAddr;
                ptrIA->firstAddr();
		std::clog << logger::logDebug << "Sending DECLINE for IA(IAID=" << ptrIA->getIAID() << "): ";

                while ( ptrAddr= ptrIA->getAddr() ) {
                    if (ptrAddr->getTentative() == YES) {
                        // remove this address from interface
                        std::clog << "(" << ptrAddr->get()->getPlain();
                        result = ptrIface->delAddr(ptrAddr->get());
                        std::clog << " Iface removal=" << result;

                        // remove this address from addrDB
                        result = ptrIA->delAddr(ptrAddr->get());
                        std::clog << " AddrDB removal=" << result << ")";
                    }
                }
		std::clog << logger::endl;
		
                ptrIA->setTentative();
            }
        }
    } while(firstIA);
}

void TClntTransMgr::checkRequest()
{
    SmartPtr<TAddrIA> firstIA;
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TClntCfgIA> ptrCfgIA;
    SmartPtr<TClntCfgGroup> firstIAGroup;
//    int result;
    do
    {
        firstIA= SmartPtr<TAddrIA>();

        TContainer<SmartPtr<TAddrIA> > requestIALst;

        AddrMgr->firstIA();
        while((ptrIA=AddrMgr->getIA())&&(!firstIA))
        {
            //find first IA which is not in process and all addresses
            //were checked against duplication and dosen't have assigned
            //all addresses
            ptrCfgIA=CfgMgr->getIA(ptrIA->getIAID());
            if ((ptrIA->getTentative()==NO)&&
                (ptrIA->getState()!=INPROCESS)&&
                (ptrIA->countAddr()<ptrCfgIA->countAddr()))
            {
                firstIA=ptrIA;
                firstIAGroup=CfgMgr->getGroupForIA(ptrIA->getIAID());
            }
        }
        if (firstIA)
        {
            requestIALst.append(firstIA);
            firstIA->setState(INPROCESS);
            while(ptrIA=AddrMgr->getIA())
            {
                //find other IA's, which is not in process and all addresses
                //were checked against DAD, and belong to the same group
                //were received from one server
                if(((ptrIA->getTentative()==NO)&&
                    (ptrIA->getState()!=INPROCESS)&&
                    (ptrIA->countAddr()<ptrCfgIA->countAddr()))&&
                    (*ptrIA->getDUID()==*firstIA->getDUID())&&
                    (&(*firstIAGroup)==&(*CfgMgr->getGroupForIA(ptrIA->getIAID())))
                  )
                {
                    ptrIA->setState(INPROCESS);
                    requestIALst.append(ptrIA);
                }

            }
            //  TContainer<SmartPtr<TAddrIA> > IAs,
            //  SmartPtr<TDUID> srvDUID,
            //int iface)

            //Here should be send decline for all tentative addresses in IAs
            SmartPtr<TClntMsgRequest> request = 
                new TClntMsgRequest(IfaceMgr, That, CfgMgr, AddrMgr,
				    requestIALst,firstIA->getDUID(), firstIA->getIface());
            Transactions.append( (Ptr*) request);
        }
    } while(firstIA);
}

bool TClntTransMgr::isDone()
{
    return IsDone;
}

void TClntTransMgr::setThat(SmartPtr<TClntTransMgr> that)
{
    this->That=that;
    IfaceMgr->setThats(IfaceMgr,That,CfgMgr,AddrMgr);
}

char* TClntTransMgr::getCtrlAddr() {
	return this->ctrlAddr;
}
int  TClntTransMgr::getCtrlIface() {
	return this->ctrlIface;
}
