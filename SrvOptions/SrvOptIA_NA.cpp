#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "SrvOptIA_NA.h"
#include "SrvOptIAAddress.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "Logger.h"
#include "AddrClient.h"
#include "DHCPConst.h"

TSrvOptIA_NA::TSrvOptIA_NA( long IAID, long T1, long T2, TMsg* parent)
:TOptIA_NA(IAID,T1,T2, parent)
{

}

TSrvOptIA_NA::TSrvOptIA_NA( long IAID, long T1, long T2, int Code, string Text, TMsg* parent)
:TOptIA_NA(IAID,T1,T2, parent)
{
    SubOptions.append(new TSrvOptStatusCode(Code, Text, parent));
}

//konstruktor chyba trza zostawic
TSrvOptIA_NA::TSrvOptIA_NA( char * buf, int bufsize, TMsg* parent)
:TOptIA_NA(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize) 
    {

        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(canBeOptInOpt(parent->getType(),OPTION_IA,code))
            {
                SmartPtr<TOpt> opt;
		opt = SmartPtr<TOpt>(); /* NULL */
                switch (code)
                {
                case OPTION_IAADDR:
                    opt =(Ptr*)SmartPtr<TSrvOptIAAddress> (new TSrvOptIAAddress(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    opt =(Ptr*)SmartPtr<TSrvOptStatusCode> (new TSrvOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
                    clog<< logger::logWarning <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() <<") in this version of server."<<logger::endl;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            }
            else
                clog << logger::logWarning << "Illegal option received, opttype=" << code 
                    << " in field options of IA_NA option"<<logger::endl;

        }
        else
        {
                clog << logger::logWarning <<"Unknown option in option IA_NA( optType=" 
                    << code << "). Option ignored." << logger::endl;
        };
        pos+=length;
    }
}

//New constructor for SOLICIT (Rapid also possible) and REQUEST creation used in both
TSrvOptIA_NA::TSrvOptIA_NA(SmartPtr<TSrvAddrMgr> addrMgr,
			   SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntClasses,
			   long *clntFreeAddr,
			   long  &totalFreeAddr,
			   SmartPtr<TContainer<SmartPtr<TIPv6Addr> > > assignedAddresses,
			   SmartPtr<TSrvOptIA_NA> queryOpt,
			   char &preference,
			   SmartPtr<TDUID> DUID,SmartPtr<TIPv6Addr> clntAddr, 
			   int iface, int msgType, TMsg* parent)
    :TOptIA_NA(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    
    long classNr=0;
    SmartPtr<TOpt> ptrQuerySubOpt;
    queryOpt->firstOption();

    int addrCnt=0;
    while (ptrQuerySubOpt=queryOpt->getOption())
    {
        if (ptrQuerySubOpt->getOptType()==OPTION_IAADDR)
            addrCnt++;
    }

    //FIXME:What happened if addrCnt is zero - at least a little bit strange
    if(!addrCnt)
    {
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL,
                                        "No addresses in this IA_NA option.",this->Parent);
        this->T1=0;
        this->T2=0;
        this->SubOptions.append((Ptr*)ptrStatus);

    };
    //Is there enough addresses
    if ((addrCnt<=totalFreeAddr)&&(totalFreeAddr))
    {
        SmartPtr<TOptIAAddress> optAddr;
        queryOpt->firstOption();
        while (ptrQuerySubOpt = queryOpt->getOption())
        {
            switch (ptrQuerySubOpt->getOptType()) 
            {
                case OPTION_IAADDR: 
                {
                    optAddr=(Ptr*) ptrQuerySubOpt;
                    //class to which new address will be belongs to                    
                    SmartPtr<TSrvCfgAddrClass> ptrClass;
                    SmartPtr<TIPv6Addr> addr;
                    //Have we already assigned this address to this client ??
                    if (!isAddrIn(optAddr->getAddr(),assignedAddresses))
                    {   
                        //No - Find class to which address belongs, if any ??
                        clntClasses->first();
                        classNr=0;
                        while((ptrClass=clntClasses->get())
                            &&(!(ptrClass->addrInPool(optAddr->getAddr()))))
                            classNr++;
                        //if found - are there still free addresses in this class ??
                        if ((ptrClass)&&(clntFreeAddr[classNr]>0))
                        {
                            //check if addresses proposed by client is free
                            if(addrMgr->addrIsFree(optAddr->getAddr()))
                            {
                                //yes - it can be assigned
                                addr = optAddr->getAddr(); 
                            }
                            else
                                //no - let assign another address from this 
                                //class - it should be free
                                do {
                                    addr=ptrClass->getRandomAddr();
                                }while((!addrMgr->addrIsFree(addr))
                                    ||(isAddrIn(addr,assignedAddresses)));
                        }
                    }
                    //Have appropriate address been found so far?
                    if(!addr)
                    {  
                        //No - address was already assigned or there is no more free
                        //addresses in this class
                        //Let's look around in other classes
                        long AddrNr=((long)rand()*(long)rand())%totalFreeAddr;
                        classNr=0;
                        while(AddrNr>=0)
                        {
                            AddrNr-=clntFreeAddr[classNr];
                            classNr++;
                        };
                        classNr--;
                        
                        clntClasses->first();
                        int i=0;
                        while((ptrClass=clntClasses->get())&&(i<classNr))
                            i++;
                        do 
                        {
                            addr=ptrClass->getRandomAddr();
                        }while(!addrMgr->addrIsFree(addr)||(isAddrIn(addr,assignedAddresses)));
                    }
                    //So far we ashould have defined address and class to which it
                    //belonge to i.e. variables addr, ptrClass and classNr should
                    //be set approprieatly
                    //Update input variables
                    assignedAddresses->append(addr);
                    clntFreeAddr[classNr]--;
                    totalFreeAddr--;
                    //if we jump among classes for this IA
                    //we sould correct T2 and T2 timeouts
                    if (T1 > ptrClass->getT1(queryOpt->getT1())) 
                        T1 = ptrClass->getT1(queryOpt->getT1());
                    if (T2 > ptrClass->getT2(queryOpt->getT2())) 
                        T2 = ptrClass->getT2(queryOpt->getT2());
                    //Append new IAAddress suboption
                    optAddr = new TSrvOptIAAddress(addr,
                        ptrClass->getPref(optAddr->getPref()),
                        ptrClass->getValid(optAddr->getValid()),this->Parent);
                    SubOptions.append((Ptr*)optAddr);

		    if (ptrClass->getPreference() > preference)
			preference = ptrClass->getPreference();
                    //if yes assigned it to client
                    //if no - are there still free addresses                    
                    break;
                }
                case OPTION_STATUS_CODE: 
                {
                    SmartPtr< TOptStatusCode > ptrStatus = (Ptr*) ptrQuerySubOpt;
                    std::clog << "Receviced STATUS_CODE from client:" 
                        <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
                        << ")" << logger::endl;
                    break;
                }
                default: 
                {
                    std::clog << "Invalid suboption (opttype=" << ptrQuerySubOpt->getOptType() 
                        << ") in OPTION_IA. Option ignored." << logger::endl;
                    break;
                }
            }
        }
        SmartPtr<TSrvOptStatusCode> ptrStatus (new TSrvOptStatusCode(STATUSCODE_SUCCESS,"",this->Parent));

        SubOptions.append((Ptr*)ptrStatus);
        if(msgType==REQUEST_MSG)
        {
            // find this client in addrdb
            SmartPtr <TAddrClient> ptrClient;
            ptrClient = addrMgr->getClient(DUID);
            if (!ptrClient) { // we haven't met this client earlier
                ptrClient = new TAddrClient(DUID);
                addrMgr->addClient(ptrClient);
            }

            //FIXME: when adding IAID to addrMgr id of IAID should be passed
            SmartPtr<TAddrIA> ptrIA;
            ptrIA = ptrClient->getIA(this->IAID);
            if (!ptrIA) {
                ptrIA = new TAddrIA(iface, clntAddr,DUID, this->T1,this->T2,this->IAID);
                ptrClient->addIA(ptrIA);
            }

            SmartPtr<TSrvOptIAAddress> ptr;
            SubOptions.first();
            while ((ptr = (Ptr*)SubOptions.get()) && (ptr->getOptType()==OPTION_IAADDR))
                ptrIA->addAddr( ptr->getAddr(), ptr->getPref(), ptr->getValid());
        }
    }
    else //there is no enough free addresses
    {
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,"No addresses available. Sorry.",this->Parent);
        this->T1=0;
        this->T2=0;
        this->SubOptions.append((Ptr*)ptrStatus);
    }
}


unsigned long TSrvOptIA_NA::countFreeAddrsForClient(
    SmartPtr<TSrvCfgMgr> cfgMgr,
    SmartPtr<TSrvAddrMgr> addrMgr,
    SmartPtr<TDUID> duid,
    SmartPtr<TIPv6Addr> clntAddr,
    int iface)
{
    unsigned long maxAddrs   = cfgMgr->getMaxAddrsPerClient(duid, clntAddr, iface);
    maxAddrs       -= addrMgr->getAddrCount(duid, iface);

    // FIXME: unsigned long long long int (128bit) type would come in handy here
    SmartPtr<TSrvCfgAddrClass> ptrClass;
    SmartPtr<TSrvCfgIface> ptrIface;

    cfgMgr->firstIface();
    while((ptrIface=cfgMgr->getIface())&&(ptrIface->getID()!=iface)) ;

    //tmpcnt will contain maximum number of addresses, which can be assigned
    //to this client
    unsigned long tmpcnt=0;
    ptrIface->firstAddrClass();
    while (ptrClass = ptrIface->getAddrClass() ) {
        if (ptrClass->clntSupported(duid,clntAddr)) {
            tmpcnt += ptrClass->getAddrCount(duid, clntAddr);
        }
    }
    //substract addresses which has been already assigned to this client
    tmpcnt -= addrMgr->getAddrCount(duid, iface);

    float bigNumber = cfgMgr->getFreeAddrs(duid, clntAddr, iface);
    if (maxAddrs > bigNumber ) {
        maxAddrs = (int) bigNumber;
    }
    return maxAddrs;
}

// constructor used only in RENEW, REBIND, DECLINE and RELEASE
TSrvOptIA_NA::TSrvOptIA_NA(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> clntDUID,
                           int iface,  unsigned long &addrCount, 
                           int msgType , TMsg* parent)
                           :TOptIA_NA(queryOpt->getIAID(),0x7fffffff,0x7fffffff, parent)
{
    this->IAID = queryOpt->getIAID();

    switch (msgType) {
    case SOLICIT_MSG:
        //this->solicit(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID,iface, addrCount);
        break;
    case REQUEST_MSG:
        //this->request(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    
    case RENEW_MSG:
        this->renew(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    case REBIND_MSG:
        this->rebind(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    case RELEASE_MSG:
        this->release(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID,iface, addrCount);
        break;
    case CONFIRM_MSG:
        this->confirm(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    case DECLINE_MSG:
        this->decline(cfgMgr, addrMgr, queryOpt, clntAddr, clntDUID, iface, addrCount);
        break;
    default: {
        clog << logger::logWarning << "Unknown message type (" << msgType << "). Cannot generate OPTION_IA."<< logger::endl;
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_UNSPECFAIL,"Unknown message type.",this->Parent));
        break;
             }
    }
}

/*//This constructor is probably not used
void TSrvOptIA_NA::solicit(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> DUID,
                           int iface,  unsigned long &addrCount)
{

    unsigned long maxAddrs = countFreeAddrsForClient(cfgMgr, addrMgr, DUID, clntAddr, iface);
    bool Fatal = false;
    long T1 = 0x7fffffff;
    long T2 = 0x7fffffff;
    long Pref= 0x7fffffff;;
    long Valid= 0x7fffffff;;

    SmartPtr<TOpt> ptrSub;
    SmartPtr<TOptIAAddress> optAddr;
    queryOpt->firstOption();

    while (!Fatal && (ptrSub = queryOpt->getOption()) )
    {
        switch (ptrSub->getOptType()) 
        {
            case OPTION_IAADDR: 
            {
                if (addrCount < maxAddrs ) 
                {
                    addrCount++;
                    optAddr=(Ptr*)ptrSub;
                    SmartPtr<TIPv6Addr> addr = optAddr->getAddr();
                    if ((cfgMgr->addrIsSupported(DUID , clntAddr, addr)) &&
                        (addrMgr->addrIsFree(addr))) 
                    { // requested addr is ok
                        SmartPtr< TSrvCfgAddrClass> ptrClass = cfgMgr->getClassByAddr(iface, addr);
                    } else 
                    { // requested addr sucks, we generate one instead
                        do {
                            //FIXME:And what for are parameters T1, T2, Pref and Valid passed
                            addr =  cfgMgr->getRandomAddr(DUID,clntAddr,T1,T2,Pref,Valid);
                        } while (!addrMgr->addrIsFree(addr));
                    }

                    SmartPtr< TSrvCfgAddrClass> ptrClass = cfgMgr->getClassByAddr(iface, addr);
                    if (T1 > ptrClass->getT1(queryOpt->getT1())) 
                        T1 = ptrClass->getT1(queryOpt->getT1());
                    if (T2 > ptrClass->getT2(queryOpt->getT2())) 
                        T2 = ptrClass->getT2(queryOpt->getT2());
                    optAddr = new TSrvOptIAAddress(addr,
                        ptrClass->getPref(optAddr->getPref()),
                        ptrClass->getValid(optAddr->getValid()),this->Parent);
                    SubOptions.append((Ptr*)optAddr);
                } else { 
                    // out of addresses (or max for this client is reached)
                    addrCount -= SubOptions.count();
                    SubOptions.clear();
                    Fatal = true;
                    continue;
                }
                break;
            }
            case OPTION_STATUS_CODE: 
            {
                SmartPtr< TOptStatusCode > ptrStatus = (Ptr*) ptrSub;
                std::clog << "Receviced STATUS_CODE from client:" 
                    <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
                    << ")" << logger::endl;
                break;
            }
            default: 
            {
                std::clog << "Invalid suboption (opttype=" << ptrSub->getOptType() 
                    << ") in OPTION_IA. Option ommited." << logger::endl;
                break;
            }
        }// end of switch
    } 

    if (!Fatal) 
    {
        SmartPtr<TSrvOptStatusCode> ptrStatus (new TSrvOptStatusCode(STATUSCODE_SUCCESS,"",this->Parent));
        this->T1=T1;
        this->T2=T2;
    } 
    else 
    {
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,"No addresses available. Sorry.",this->Parent);
        this->T1=0;
        this->T2=0;
        SubOptions.append((Ptr*)ptrStatus);
    }

}
//and this also is not used
void TSrvOptIA_NA::request(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> DUID,
                           int iface, unsigned long &addrCount) 
{
    unsigned long maxAddrs = countFreeAddrsForClient(cfgMgr, addrMgr, DUID, clntAddr, iface);
    bool Fatal = false;
    long T1 = 0x7fffffff;
    long T2 = 0x7fffffff;
    long Pref= 0x7fffffff;;
    long Valid= 0x7fffffff;;

    SmartPtr<TOpt> ptrSub;
    SmartPtr<TOptIAAddress> optAddr;
    queryOpt->firstOption();

    while (!Fatal && (ptrSub = queryOpt->getOption()) )
    {
        switch (ptrSub->getOptType()) 
        {
        case OPTION_IAADDR: 
            {
                if (addrCount < maxAddrs ) 
                {
                    addrCount++;
                    optAddr=(Ptr*)ptrSub;
                    SmartPtr<TIPv6Addr> addr = optAddr->getAddr();
                    if ( (cfgMgr->addrIsSupported(DUID, clntAddr, addr)) &&
                        (addrMgr->addrIsFree(addr)) ) { // requested addr is ok
                            SmartPtr< TSrvCfgAddrClass> ptrClass = cfgMgr->getClassByAddr(iface, addr);
                        } else { // requested addr sucks, we generate one instead
                            do {
                                addr = cfgMgr->getRandomAddr(DUID,clntAddr,T1,T2,Pref,Valid);
                            } while (!addrMgr->addrIsFree(addr));
                        }

                        SmartPtr< TSrvCfgAddrClass> ptrClass = cfgMgr->getClassByAddr(iface,addr);
                        if (T1 > ptrClass->getT1(queryOpt->getT1())) 
                                T1 = ptrClass->getT1(queryOpt->getT1());
                        if (T2 > ptrClass->getT2(queryOpt->getT2())) 
                                T2 = ptrClass->getT2(queryOpt->getT2());
                        optAddr = new TSrvOptIAAddress(addr,
                                        ptrClass->getPref(optAddr->getPref()),
                                        ptrClass->getValid(optAddr->getValid()),this->Parent);
                        SubOptions.append((Ptr*)optAddr);
                } 
                else 
                { // out of addresses
                    addrCount -= SubOptions.count();
                    SubOptions.clear();
                    Fatal = true;
                    continue;
                }
                break;
            }
        case OPTION_STATUS_CODE: 
            {
                SmartPtr< TOptStatusCode > ptrStatus = (Ptr*) ptrSub;
                std::clog << "Receviced STATUS_CODE from client:" 
                    <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
                    << ")" << logger::endl;
                break;
            }
        default:
            {
                std::clog << "Invalid suboption (opttype=" << ptrSub->getOptType() 
                    << ") in OPTION_IA. Option ommited." << logger::endl;
                break;
            }
        }
    } // end of switch

    // is everything ok?
    if (!Fatal) {
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Addresses awarded.",this->Parent);
        SubOptions.append((Ptr*)ptrStatus);

        this->T1=T1;
        this->T2=T2;

        // find this client in addrdb
        SmartPtr <TAddrClient> ptrClient;
        ptrClient = addrMgr->getClient(DUID);
        if (!ptrClient) { // we haven't met this client earlier
            ptrClient = new TAddrClient(DUID);
            addrMgr->addClient(ptrClient);
        }

        //FIXME: when adding IAID to addrMgr id of IAID should be passed
        SmartPtr<TAddrIA> ptrIA;
        ptrIA = ptrClient->getIA(this->IAID);
        if (!ptrIA) {
            ptrIA = new TAddrIA(iface, clntAddr,DUID, this->T1,this->T2,this->IAID);
            ptrClient->addIA(ptrIA);
        }

        SmartPtr<TSrvOptIAAddress> ptr;
        SubOptions.first();
        while ((ptr = (Ptr*)SubOptions.get()) && (ptr->getOptType()==OPTION_IAADDR))
            ptrIA->addAddr( ptr->getAddr(), ptr->getPref(), ptr->getValid());

    } 
    else 
    {
        SmartPtr<TSrvOptStatusCode> ptrStatus;
        ptrStatus = new TSrvOptStatusCode(STATUSCODE_NOADDRSAVAIL,"No addresses available. Sorry.",this->Parent);
        this->T1=0;
        this->T2=0;
        SubOptions.append((Ptr*)ptrStatus);
    }
}*/

// generate OPTION_IA based on OPTION_IA received in RENEW message
void TSrvOptIA_NA::renew(SmartPtr<TSrvCfgMgr> cfgMgr,
                         SmartPtr<TSrvAddrMgr> addrMgr,
                         SmartPtr<TSrvOptIA_NA> queryOpt,
                         SmartPtr<TIPv6Addr> clntAddr,
                         SmartPtr<TDUID> DUID,
                         int iface,  unsigned long &addrCount)
{
    // find that client in addrdb
    SmartPtr <TAddrClient> ptrClient;
    ptrClient = addrMgr->getClient(DUID);
    if (!ptrClient) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",this->Parent));
        return;
    }

    // find that IA
    SmartPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(this->IAID);
    if (!ptrIA) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",this->Parent ));
        return;
    }

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    this->T1 = ptrIA->getT1();
    this->T2 = ptrIA->getT2();

    // send addr info to client
    SmartPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SmartPtr<TOptIAAddress> optAddr;
        ptrAddr->setTimestamp();
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),ptrAddr->getValid(),this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Greetings from planet Earth",this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_NA::rebind(SmartPtr<TSrvCfgMgr> cfgMgr,
                          SmartPtr<TSrvAddrMgr> addrMgr,
                          SmartPtr<TSrvOptIA_NA> queryOpt,
                          SmartPtr<TIPv6Addr> clntAddr,
                          SmartPtr<TDUID> DUID,
                          int iface,  unsigned long &addrCount)
{
    // find that client in addrdb
    SmartPtr <TAddrClient> ptrClient;
    ptrClient = addrMgr->getClient(DUID);
    if (!ptrClient) {
        // hmmm, that's not our client
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"Who are you? Do I know you?",this->Parent ));
        return;
    }

    // find that IA
    SmartPtr <TAddrIA> ptrIA;
    ptrIA = ptrClient->getIA(this->IAID);
    if (!ptrClient) {
        SubOptions.append(new TSrvOptStatusCode(STATUSCODE_NOBINDING,"I see this IAID first time.",this->Parent ));
        return;
    }

    // FIXME: 18.2.4 par. 3 (check if addrs are appropriate for this link)

    // everything seems ok, update data in addrdb
    ptrIA->setTimestamp();
    this->T1 = ptrIA->getT1();
    this->T2 = ptrIA->getT2();

    // send addr info to client
    SmartPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        SmartPtr<TOptIAAddress> optAddr;
        optAddr = new TSrvOptIAAddress(ptrAddr->get(), ptrAddr->getPref(),ptrAddr->getValid(),this->Parent);
        SubOptions.append( (Ptr*)optAddr );
    }

    // finally send greetings and happy OK status code
    SmartPtr<TSrvOptStatusCode> ptrStatus;
    ptrStatus = new TSrvOptStatusCode(STATUSCODE_SUCCESS,"Greetings from planet Earth",this->Parent);
    SubOptions.append( (Ptr*)ptrStatus );
}

void TSrvOptIA_NA::release(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> DUID,
                           int iface, unsigned long &addrCount)
{
}

void TSrvOptIA_NA::confirm(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> DUID,
                           int iface, unsigned long &addrCount)
{
}

void TSrvOptIA_NA::decline(SmartPtr<TSrvCfgMgr> cfgMgr,
                           SmartPtr<TSrvAddrMgr> addrMgr,
                           SmartPtr<TSrvOptIA_NA> queryOpt,
                           SmartPtr<TIPv6Addr> clntAddr,
                           SmartPtr<TDUID> DUID,
                           int iface, unsigned long &addrCount)
{
}

bool TSrvOptIA_NA::doDuties()
{
    return true;
}


bool TSrvOptIA_NA::isAddrIn(SmartPtr<TIPv6Addr> addr,
                       SmartPtr<TContainer<SmartPtr<TIPv6Addr> > > addrLst)
{
    SmartPtr<TIPv6Addr> ptrAddr;
    addrLst->first();
    while(ptrAddr=addrLst->get())
        if (*ptrAddr==*addr) return true;
    return false;
}
