#include "SrvMsgAdvertise.h"
#include "SrvMsg.h"
#include "Logger.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptIA_NA.h"
#include "OptStatusCode.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptPreference.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "AddrClient.h"

#include "Logger.h"

TSrvMsgAdvertise::TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
				   SmartPtr<TSrvTransMgr> TransMgr,
				   SmartPtr<TSrvCfgMgr> CfgMgr,
				   SmartPtr<TSrvAddrMgr> AddrMgr,
				   SmartPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,
	     solicit->getIface(),solicit->getAddr(), ADVERTISE_MSG, solicit->getTransID())
{
    char preference = 0;
    SmartPtr<TOpt> ptrOpt;
    SmartPtr<TSrvOptOptionRequest> reqOpts;
   
    // remember client's DUID
    SmartPtr<TSrvOptClientIdentifier> duidOpt;
    duidOpt=(Ptr*)solicit->getOption(OPTION_CLIENTID);

    //remember requested option in order to add number of "hint" options
    //wich are included in packet but not in Request Option
    //if Request option wasn't included by client - create new one
    //in which number of "hint" options could be stored
    reqOpts=(Ptr*)solicit->getOption(OPTION_ORO);
    //If there is no option Request it will ba added
    if (!reqOpts)
        reqOpts=new TSrvOptOptionRequest(this);
    //Lists: of already assigned addresses to client - 
    SmartPtr<TContainer<SmartPtr<TIPv6Addr > > >
        assAddrLst(new TContainer<SmartPtr<TIPv6Addr> > ());
    //of all classes, from which client can be assigned addresses
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntAllClasses;    
    //of classes from which addresses can be assigned (limits are kept)
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntClasses;
    //all passed to IA_NA option
    long *clntFreeAddr;
    long totalFreeAddresses;
    
    //AddrMgr->dbStore();

    getFreeAddressesForClient(clntAllClasses,clntClasses,clntFreeAddr, 
			      totalFreeAddresses, duidOpt->getDUID(),
			      solicit->getAddr(), solicit->getIface());

    //if client should be ignored - it is not supported
    if(!clntAllClasses->count())
    {
        //There is no reply for this client there is no classes
        //from which addresses/parameters can be assigned to it
        IsDone=true;
        return;
    }

    //So if we can do something for this client at least set configuration
    //parameters - let's do it option by option - try to answer to it
    solicit->firstOption();
    while ( ptrOpt = solicit->getOption())  
    {
        switch (ptrOpt->getOptType()) {
            case OPTION_CLIENTID : {
                this->Options.append(ptrOpt);
                break;
                }
            case OPTION_IA : {
                SmartPtr<TSrvOptIA_NA> optIA_NA;
                optIA_NA=new TSrvOptIA_NA(
                    AddrMgr, clntClasses, clntFreeAddr, 
                    totalFreeAddresses, assAddrLst, (Ptr*)ptrOpt, preference,
                    duidOpt->getDUID(),solicit->getAddr(), solicit->getIface(),
                    SOLICIT_MSG,this);

                this->Options.append((Ptr*)optIA_NA);
                break;
                }
            case OPTION_RAPID_COMMIT :
                //FIXED:No rapid commit option in generating advertise
                //      in solicit it's ok and in reply option
                //      if generating advertise it is just ignores
                clog<<logger::logNotice
                    <<"Option Rapid Commit ignored (Advertise generation)."<<logger::endl;
                break;
            case OPTION_IAADDR :
                std::clog << logger::logWarning << "Invalid option (OPTION_IAADDR) received." << logger::endl;
                break;
            case OPTION_ORO:
                break;
            case OPTION_ELAPSED_TIME :
                break;
            case OPTION_STATUS_CODE : {
                SmartPtr< TOptStatusCode > ptrStatus = (Ptr*) ptrOpt;
                std::clog << logger::logError << "Receviced STATUS_CODE from client:" 
			  <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
			  << ")" << logger::endl;
                break;
                }

            //add options requested by client to option Request Option if
            //client didn't included them
            case OPTION_DNS_RESOLVERS:
                if (!reqOpts->isOption(OPTION_DNS_RESOLVERS))
                    reqOpts->addOption(OPTION_DNS_RESOLVERS);
                break;
            case OPTION_DOMAIN_LIST:
                if (!reqOpts->isOption(OPTION_DOMAIN_LIST))
                    reqOpts->addOption(OPTION_DOMAIN_LIST);
                break;
            case OPTION_NTP_SERVERS:
                if (!reqOpts->isOption(OPTION_NTP_SERVERS))
                    reqOpts->addOption(OPTION_NTP_SERVERS);
                break;
            case OPTION_TIME_ZONE:
                if (!reqOpts->isOption(OPTION_TIME_ZONE))
                    reqOpts->addOption(OPTION_TIME_ZONE);
                break;

            case OPTION_PREFERENCE :
            case OPTION_UNICAST :
            case OPTION_SERVERID :
                std::clog << logger::logWarning 
			  << "Invalid option (OPTION_UNICAST) received." << logger::endl;
                break;
                // options not yet supported 
            case OPTION_IA_TA    :
            case OPTION_RELAY_MSG :
            case OPTION_AUTH_MSG :
            case OPTION_USER_CLASS :
            case OPTION_VENDOR_CLASS :
            case OPTION_VENDOR_OPTS :
            case OPTION_INTERFACE_ID :
            case OPTION_RECONF_MSG :
            case OPTION_RECONF_ACCEPT:
            default:
                std::clog << logger::logDebug << "Option not supported, opttype=" 
			  << ptrOpt->getOptType() << logger::endl;
                break;

            } // end of switch
        } // end of while

    //if client requested parameters and policy doesn't forbid from answering
    appendRequestedOptions(duidOpt->getDUID(),solicit->getAddr(),solicit->getIface(),reqOpts);

    // include our DUID
    SmartPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
    Options.append((Ptr*)ptrSrvID);

    // ... and our preference
    SmartPtr<TSrvOptPreference> ptrPreference;
    
    ptrPreference = new TSrvOptPreference(preference,this);
    Options.append((Ptr*)ptrPreference);

    delete [] clntFreeAddr;
    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 0;
    this->send();
}

bool TSrvMsgAdvertise::check()
{
    // this should never happen
    return true;
}

void TSrvMsgAdvertise::answer(SmartPtr<TMsg> Rep)
{
    // this should never happen
    return;
}

TSrvMsgAdvertise::~TSrvMsgAdvertise()
{
}

unsigned long TSrvMsgAdvertise::getTimeout()
{
    return 0;
}
void TSrvMsgAdvertise::doDuties()
{
    IsDone = true;
}
