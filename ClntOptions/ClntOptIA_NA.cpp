#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "AddrIA.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIAAddress.h"
#include "ClntOptStatusCode.h"
#include "ClntOptRapidCommit.h"
#include "Logger.h"
#include "ClntOptIAAddress.h"

TClntOptIA_NA::TClntOptIA_NA(SmartPtr<TAddrIA> clntAddrIA, bool zeroTimes, TMsg* parent)
    :TOptIA_NA(clntAddrIA->getIAID(), zeroTimes?0:clntAddrIA->getT1(),
	       zeroTimes?0:clntAddrIA->getT2(), parent) 
{
    SmartPtr <TAddrAddr> addr;
    clntAddrIA->firstAddr();
    while ( addr = clntAddrIA->getAddr() ) {
	SubOptions.append( new TClntOptIAAddress(addr->get(),
						 zeroTimes?0:addr->getPref(),
						 zeroTimes?0:addr->getValid(),
						 parent) );
    }

}

TClntOptIA_NA::TClntOptIA_NA(SmartPtr<TAddrIA> addrIA, TMsg* parent)
    :TOptIA_NA(addrIA->getIAID(),addrIA->getT1(),addrIA->getT2(), parent)
{
    // should we include all addrs or tentative ones only?
    bool decline;
    if (parent->getType()==DECLINE_MSG)
	decline = true;
    else
	decline = false;

    SmartPtr<TAddrAddr> ptrAddr;
    addrIA->firstAddr();
    while ( ptrAddr = addrIA->getAddr() )
    {
	if ( !decline || (ptrAddr->getTentative()==YES) )
	    SubOptions.append(new TClntOptIAAddress(ptrAddr->get(), ptrAddr->getPref(), 
						    ptrAddr->getValid(),this->Parent) );
    }
    DUID = SmartPtr<TDUID>(); // NULL
}

TClntOptIA_NA::TClntOptIA_NA(SmartPtr<TClntCfgIA> ClntCfgIA, SmartPtr<TAddrIA> ClntaddrIA, TMsg* parent)
    :TOptIA_NA(ClntaddrIA->getIAID(),ClntaddrIA->getT1(),ClntaddrIA->getT2(), parent)
{
    //checkRequestConstructor
    ClntCfgIA->firstAddr();
    SmartPtr<TClntCfgAddr> ClntCfgAddr;
    while ((ClntCfgAddr = ClntCfgIA->getAddr())&&
        ((ClntCfgIA->countAddr()-ClntaddrIA->getAddrCount())>this->countAddr() ))
    {
        SmartPtr<TAddrAddr> ptrAddr=ClntaddrIA->getAddr(ClntCfgAddr->get());
        if(!ptrAddr)
            SubOptions.append(new TClntOptIAAddress(
            ClntCfgAddr->get(),
            ClntCfgAddr->getPref(),
            ClntCfgAddr->getValid(),
            this->Parent));
    }
    DUID = SmartPtr<TDUID>(); // NULL
}

TClntOptIA_NA::TClntOptIA_NA(SmartPtr<TClntCfgIA> ClntCfgIA, TMsg* parent)
    :TOptIA_NA(ClntCfgIA->getIAID(),ClntCfgIA->getT1(),ClntCfgIA->getT2(), parent)
{
    ClntCfgIA->firstAddr();
    SmartPtr<TClntCfgAddr> ClntCfgAddr;
    while (ClntCfgAddr = ClntCfgIA->getAddr())
        SubOptions.append(new TClntOptIAAddress(ClntCfgAddr->get(),
        ClntCfgAddr->getPref(),
        ClntCfgAddr->getValid(),this->Parent) ); 
    DUID = SmartPtr<TDUID>(); // NULL
}

//konstruktor chyba trza zostawic
TClntOptIA_NA::TClntOptIA_NA(char * buf,int bufsize, TMsg* parent)
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
                SmartPtr<TOpt> opt= SmartPtr<TOpt>();
                switch (code)
                {
                case OPTION_IAADDR:
                    //  SmartPtr<TOptIAAddress> ptr;
                    SubOptions.append( new TClntOptIAAddress(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    SubOptions.append( new TClntOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
                    clog<< logger::logWarning <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() 
                        <<") in this version of server."<<logger::endl;
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

void TClntOptIA_NA::firstAddr()
{
    SubOptions.first();
}

SmartPtr<TClntOptIAAddress> TClntOptIA_NA::getAddr()
{
    SmartPtr<TClntOptIAAddress> ptr;
    do {
        ptr = (Ptr*) SubOptions.get();
        if (ptr)
            if (ptr->getOptType()==OPTION_IAADDR)
                return ptr;
    } while (ptr);
    return SmartPtr<TClntOptIAAddress>();
}

int TClntOptIA_NA::countAddr()
{
    SmartPtr< TOpt> ptr;
    SubOptions.first();
    int count = 0;
    while ( ptr = SubOptions.get() ) {
        if (ptr->getOptType() == OPTION_IAADDR) 
            count++;
    }
    return count;
}

int TClntOptIA_NA::getStatusCode()
{
    SmartPtr<TOpt> option;
    if (option=getOption(OPTION_STATUS_CODE))
    {
        SmartPtr<TClntOptStatusCode> statOpt=(Ptr*) option;
        return statOpt->getCode();
    }
    return STATUSCODE_SUCCESS;
}

void TClntOptIA_NA::setThats(SmartPtr<TClntIfaceMgr> ifaceMgr, 
                             SmartPtr<TClntTransMgr> transMgr, 
                             SmartPtr<TClntCfgMgr> cfgMgr, 
                             SmartPtr<TClntAddrMgr> addrMgr,
                             SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, int iface)
{
    this->AddrMgr=addrMgr;
    this->IfaceMgr=ifaceMgr;
    this->TransMgr=transMgr;
    this->CfgMgr=cfgMgr;
    /*if (DUIDlen>0) {
    this->DUID = new char [DUIDlen];
    memcpy(this->DUID,DUID, DUIDlen);
    }
    this->DUIDlen = DUIDlen;*/
    this->DUID=duid;
    if (addr) 
    {
        this->Unicast = true;
        //memcpy(this->Addr,addr,16);
    } else {
        //memset(this->Addr,0,16);
        this->Unicast = false;
    }
    this->Addr=addr;
    this->Iface = iface;
}

TClntOptIA_NA::~TClntOptIA_NA()
{

}
//I don't know whether this method should be invoked everywhere
//i.e. from Verify\Renew\Rebind
//it's worth to check whether futher reactions in every message will 
//be the same e.g. detection of duplicate address, lack of enough
//addresses
bool TClntOptIA_NA::doDuties()
{
    // find this IA in addrMgr...
    SmartPtr<TAddrIA> ptrIA=AddrMgr->getIA(this->getIAID());
    if (!ptrIA) {
        // unknown IAID, ignore it
        clog << logger::logWarning << "Received message contains unknown IA (IAID="
            << this->getIAID() << ").Weird... ignoring it." << logger::endl;
        return true;
    }

    // IAID found, set up new received options.
    SmartPtr<TAddrAddr> ptrAddrAddr;
    SmartPtr<TClntOptIAAddress> ptrOptAddr;

    // are all addrs configured?
    //some yes some no
    //we should have sufficient address number in IA
    //i.e. in this IA and in addrMgr which are
    //valid and can be used

    //if not we don't like this server, cause
    //WE WANT IA WITH AT LEAST NUMBER OF ADDRESSES
    //and we release the whole IA with addresses just received and those in AddrMgr

    //Was enough number of addresses received by client?
    if (countValidAddrs(ptrIA) < CfgMgr->countAddrForIA(ptrIA->getIAID()))
    {
        //No there is no enough addresses, so realese this IA received from
        //this particular server (it deceit us in advertise message or
        //or another client stole our addresses in the meantime 
        //or the server doesn't want to prolong lease - everything possible)


        //FIXME:New release transaction should be generated for this IA 
        //      It should contained addresses contained both in just received
        //      IA and in address manager - probably fixed
        
        //Why number of addresses in received option must be lower than in 
        //IA in address manager - I don't know
        //if (this->countAddr() < ptrIA->countAddr() ) 
        //{
        //I change it to:
        //  there is something to release in AddrMgr 
        //if (!((ptrIA)&&(ptrIA->countAddr())))
        //    ptrIA=new TAddrIA
        //{
        //yes - fine
        
        //FIXED:Before some of addresse will be released
        //      shouldn't they also be removed from iface manager        
        //release all addresses from iface manager
        ptrIA->firstAddr();
        while(SmartPtr<TAddrAddr> addrToRel=ptrIA->getAddr()) {
	    SmartPtr<TIPv6Addr> addr2(addrToRel->get());
            IfaceMgr->getIfaceByID(ptrIA->getIface())->delAddr(addr2);
	}

        this->firstAddr();
        while ( ptrOptAddr = this->getAddr() )
        {
            //add valid addresses from received IA not contained in IA 
            //in address manager to IA in address manager
            if(!ptrIA->getAddr(ptrOptAddr->getAddr())
                && (ptrOptAddr->getValid()) )
            {
                ptrIA->addAddr(ptrOptAddr->getAddr(), 
                    ptrOptAddr->getPref(), 
                    ptrOptAddr->getValid()
                    );
            }
            //and remove from IA in address manager those addresses, which 
            //has expired
            if (ptrIA->getAddr(ptrOptAddr->getAddr()) 
                && !(ptrOptAddr->getValid()) )
            {
                ptrIA->delAddr(ptrOptAddr->getAddr());
            }
        }

        //Now all addrs (new, old, expired) are in ptrIA now - those which 
        //should be freed
        List(TAddrIA) list;
        list.append(ptrIA);

        //FIXED: Before  they can be realese they should be removed from AddrMgr
        AddrMgr->delIA(ptrIA->getIAID() );
        AddrMgr->addIA(new TAddrIA(ptrIA->getIface(), SmartPtr<TIPv6Addr>(), SmartPtr<TDUID>(),
				   0x7fffffff,0x7fffffff,ptrIA->getIAID()));

        if (ptrIA->getAddrCount())
            TransMgr->sendRelease(list);
        //but what about removing request option from options
        //}
        return false;
    }
    //In other case, we have appropriate number of addresses and all is ok
    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = IfaceMgr->getIfaceByID(this->Iface);
    if (!ptrIface) 
    {
        clog << logger::logCrit << "Interface " << this->Iface << " not found." << logger::endl;
    }

    // for each address in IA option...
    this->firstAddr();
    while (ptrOptAddr = this->getAddr() ) {
        ptrAddrAddr = ptrIA->getAddr( ptrOptAddr->getAddr() );
        
        // no such address in DB ??
        if (!ptrAddrAddr) {
            if (ptrOptAddr->getValid()) {
                // add this address in addrDB...
                ptrIA->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid());
                ptrIA->setDUID(this->DUID);
                // ... and in IfaceMgr - 
                ptrIface->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid());
            } 
            else {
                std::clog << logger::logWarning << "Server send new addr with valid=0." << logger::endl;
            }
        } 
        else {
            // we have this addr in DB
            if ( ptrOptAddr->getValid() == 0 ) {
                // valid=0, release this address
                // delete address from addrDB
                ptrIA->delAddr(ptrOptAddr->getAddr());
                // delete address from IfaceMgr
                ptrIface->delAddr(ptrOptAddr->getAddr());
                break; // analyze next option OPTION_IA
            }

            // set up new options in IfaceMgr
            SmartPtr<TIfaceIface> ptrIface = IfaceMgr->getIfaceByID(this->Iface);
            if (ptrIface)
                ptrIface->updateAddr(ptrOptAddr->getAddr(), 
                        ptrOptAddr->getPref(), 
                        ptrOptAddr->getValid());
            // set up new options in addrDB
            ptrAddrAddr->setPref(ptrOptAddr->getPref());
            ptrAddrAddr->setValid(ptrOptAddr->getValid());
            ptrAddrAddr->setTimestamp();
        }
    }
    SmartPtr<TClntCfgIA> ptrCfgIA;
    ptrCfgIA=CfgMgr->getIA(ptrIA->getIAID());
    
    enum ESendOpt opt=ptrCfgIA->getT1SendOpt();
    //if really want to set our, but shorter value
    if ((opt==Supersede)&&(ptrCfgIA->getT1()<this->getT1()))
            ptrIA->setT1( ptrCfgIA->getT1() );       
    else
        //or server allows to set it at our convenience
        if( (opt==Default)&&(!this->getT1()) )
            ptrIA->setT1( ptrCfgIA->getT1() );
        else
            //or we accept just value returne untill it zero
            if(this->getT1() )
                ptrIA->setT1( this->getT1() );
            else
                ptrIA->setT1(ptrCfgIA->getT1());

    //The same for T2
    opt=ptrCfgIA->getT2SendOpt();
    if ((opt==Supersede)&&(ptrCfgIA->getT2()<this->getT2()))
            ptrIA->setT2( ptrCfgIA->getT2() );       
    else
        if( (opt==Default)&&(!this->getT2()) )
            ptrIA->setT2( ptrCfgIA->getT2() );
        else
            if(this->getT2() )
                ptrIA->setT2( this->getT2() );
            else
                ptrIA->setT2(ptrCfgIA->getT2());

    ptrIA->setTimestamp();
    ptrIA->setState(CONFIGURED);
    return true;
} 

//Counts all valid and diffrent addresses in sum of
//addresses received in IA from server and addresses contained
//in its counterpart IA in address manager
int TClntOptIA_NA::countValidAddrs(SmartPtr<TAddrIA> ptrIA)
{
    int count=0;

    //Counts addresses not received in Reply message
    //but which are in addrDB, and are still valid (valid>0)
    SmartPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    //For each addr in DB for this IA
    while(ptrAddr = ptrIA->getAddr())
    {
        //If addr is still valid
        if (!ptrAddr->getValid())
            continue;
        //and is not included in received option
        if (!this->getAddr(ptrAddr->get()))
            //we can increase counter
            count++;
    }
    
    //count all valid (valid>0) addresses received from server
    //FIXME: A) check if they repeats (possible with maliciious server)
    //       B) and not already assigned in addrDB (in others IAs)
    //       It's easy and worth checking
    //       A) Create list of valid addrs and add new valid addr 
    //          only if it doesn't exist already on this list
    //       B) Address can be assigned only in this IA, not in other
    //          This could be ommited if VerifyIA worked prooperly
    this->firstAddr();
    SmartPtr<TClntOptIAAddress> ptrOptIAAddr;
    while(ptrOptIAAddr=this->getAddr())
    {
        if (!ptrOptIAAddr->getValid())
            continue;
        //if (!ptrIA->getAddr(ptrOptIAAddr->getAddr()))
               count++;
    }
    //and what abou addresses in message but not in DB
    return count;
}

SmartPtr<TClntOptIAAddress> TClntOptIA_NA::getAddr(SmartPtr<TIPv6Addr> addr)
{
    SmartPtr<TClntOptIAAddress> optAddr;
    this->firstAddr();
    while(optAddr=this->getAddr())
    {   
        //!memcmp(optAddr->getAddr(),addr,16)
        if ((*addr)==(*optAddr->getAddr()))
            return optAddr;
    };
    return SmartPtr<TClntOptIAAddress>();
}

void TClntOptIA_NA::releaseAddr(long IAID, SmartPtr<TIPv6Addr> addr )
{
    SmartPtr<TAddrIA> ptrIA = AddrMgr->getIA(IAID);
    if (ptrIA)
        ptrIA->delAddr(addr);
    else
        std::clog << logger::logWarning << "Unable to release addr: IA (" 
		  << IAID << ") not in addrDB." << logger::endl;
}

bool TClntOptIA_NA::isValid()
{
    if (Valid)
        return this->T1<=this->T2;
    else
        return false;
}
