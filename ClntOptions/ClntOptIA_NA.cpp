/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "AddrIA.h"
#include "ClntCfgIA.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIAAddress.h"
#include "ClntOptStatusCode.h"
#include "ClntOptAddrParams.h"
#include "Logger.h"
#include "Msg.h"
#include "ClntAddrMgr.h"
#include "ClntIfaceMgr.h"

/** 
 * Used in CONFIRM constructor
 * 
 * @param clntAddrIA 
 * @param zeroTimes 
 * @param parent 
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TAddrIA> clntAddrIA, bool zeroTimes, TMsg* parent)
    :TOptIA_NA(clntAddrIA->getIAID(), zeroTimes?0:clntAddrIA->getT1(),
	       zeroTimes?0:clntAddrIA->getT2(), parent) 
{
    SPtr <TAddrAddr> addr;
    clntAddrIA->firstAddr();
    while ( addr = clntAddrIA->getAddr() ) {
	SubOptions.append( new TClntOptIAAddress(addr->get(),
						 zeroTimes?0:addr->getPref(),
						 zeroTimes?0:addr->getValid(),
						 parent) );
    }
    
}

/** 
 * Used in DECLINE, RENEW and RELEASE
 * 
 * @param addrIA 
 * @param parent 
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TAddrIA> addrIA, TMsg* parent)
    :TOptIA_NA(addrIA->getIAID(),addrIA->getT1(),addrIA->getT2(), parent)
{
    // should we include all addrs or tentative ones only?
    bool decline;
    if (parent->getType()==DECLINE_MSG)
	decline = true;
    else
	decline = false;

    bool zeroTimes = false;
    if ( (parent->getType()==RELEASE_MSG) || (parent->getType()==DECLINE_MSG)) {
	this->T1 = 0;
	this->T2 = 0;
	zeroTimes = true;
    }

    SPtr<TAddrAddr> ptrAddr;
    addrIA->firstAddr();
    while ( ptrAddr = addrIA->getAddr() )
    {
	if ( !decline || (ptrAddr->getTentative()==TENTATIVE_YES) )
	    SubOptions.append(new TClntOptIAAddress(ptrAddr->get(), zeroTimes?0:ptrAddr->getPref(), 
						    zeroTimes?0:ptrAddr->getValid(),this->Parent) );
    }
    DUID = SPtr<TDUID>(); // NULL
}

/** 
 * Used in REQUEST constructor
 * 
 * @param ClntCfgIA 
 * @param ClntaddrIA 
 * @param parent 
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, SPtr<TAddrIA> ClntaddrIA, TMsg* parent)
    :TOptIA_NA(ClntaddrIA->getIAID(),ClntaddrIA->getT1(),ClntaddrIA->getT2(), parent)
{
    //checkRequestConstructor
    ClntCfgIA->firstAddr();
    SPtr<TClntCfgAddr> ClntCfgAddr;
    while ((ClntCfgAddr = ClntCfgIA->getAddr())&&
        ((ClntCfgIA->countAddr()-ClntaddrIA->getAddrCount())>this->countAddr() ))
    {
        SPtr<TAddrAddr> ptrAddr=ClntaddrIA->getAddr(ClntCfgAddr->get());
        if(!ptrAddr)
            SubOptions.append(new TClntOptIAAddress(
            ClntCfgAddr->get(),
            ClntCfgAddr->getPref(),
            ClntCfgAddr->getValid(),
            this->Parent));
    }
    DUID = SPtr<TDUID>(); // NULL
}

/** 
 * Used in SOLICIT constructor
 * 
 * @param ClntCfgIA 
 * @param parent 
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, TMsg* parent)
    :TOptIA_NA(ClntCfgIA->getIAID(),ClntCfgIA->getT1(),ClntCfgIA->getT2(), parent)
{
    ClntCfgIA->firstAddr();
    SPtr<TClntCfgAddr> ClntCfgAddr;
    // just copy all addresses defined in the CfgMgr
    while (ClntCfgAddr = ClntCfgIA->getAddr())
        SubOptions.append(new TClntOptIAAddress(ClntCfgAddr->get(),
        ClntCfgAddr->getPref(),
        ClntCfgAddr->getValid(),this->Parent) ); 
    DUID = SPtr<TDUID>(); // NULL
}

/** 
 * Used to create object from received message
 * 
 * @param buf 
 * @param bufsize 
 * @param parent 
 */
TClntOptIA_NA::TClntOptIA_NA(char * buf,int bufsize, TMsg* parent)
:TOptIA_NA(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize) 
    {
	if (pos+4>bufsize) {
	    Log(Warning) << "Truncated IA_NA option received." << LogEnd;
	    return;
	}
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(allowOptInOpt(parent->getType(),OPTION_IA_NA,code))
            {
                switch (code)
                {
                case OPTION_IAADDR:
                    SubOptions.append( new TClntOptIAAddress(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    SubOptions.append( new TClntOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
		    Log(Warning) <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() 
                        <<") in this version of server."<<LogEnd;
                    break;
                }
            }
            else
		Log(Warning) << "Illegal option received, opttype=" << code 
                << " in field options of IA_NA option"<<LogEnd;
        }
        else
        {
	    Log(Warning) <<"Unknown option in option IA_NA( optType=" 
			 << code << "). Option ignored." << LogEnd;
        };
        pos+=length;
    }
}

void TClntOptIA_NA::firstAddr()
{
    SubOptions.first();
}

SPtr<TClntOptIAAddress> TClntOptIA_NA::getAddr()
{
    SPtr<TClntOptIAAddress> ptr;
    do {
        ptr = (Ptr*) SubOptions.get();
        if (ptr)
            if (ptr->getOptType()==OPTION_IAADDR)
                return ptr;
    } while (ptr);
    return SPtr<TClntOptIAAddress>();
}

int TClntOptIA_NA::countAddr()
{
    SPtr< TOpt> ptr;
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
    SPtr<TOpt> option;
    if (option=getOption(OPTION_STATUS_CODE))
    {
        SPtr<TClntOptStatusCode> statOpt=(Ptr*) option;
        return statOpt->getCode();
    }
    return STATUSCODE_SUCCESS;
}

void TClntOptIA_NA::setContext(SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr, int iface)
{
    DUID=srvDuid;
    if (srvAddr) {
        Unicast = true;
    } else {
        Unicast = false;
    }
    Addr = srvAddr;
    Iface = iface;
}

TClntOptIA_NA::~TClntOptIA_NA() {

}


bool TClntOptIA_NA::doDuties() {

    // find this IA in addrMgr...
    SPtr<TAddrIA> ptrIA=ClntAddrMgr().getIA(this->getIAID());
    if (!ptrIA) {
        // unknown IAID, ignore it
        Log(Warning) << "Received message contains unknown IA (IAID="
                     << this->getIAID() << "). We didn't order it. Weird... ignoring it." << LogEnd;
        return true;
    }

    // IAID found, set up new received options.
    SPtr<TAddrAddr> ptrAddrAddr;
    SPtr<TClntOptIAAddress> ptrOptAddr;

#if 0
    //if not we don't like this server, cause
    //WE WANT IA WITH AT LEAST NUMBER OF ADDRESSES
    //and we release the whole IA with addresses just received and those in AddrMgr

    //Was enough number of addresses received by client?
    if (countValidAddrs(ptrIA) < CfgMgr->countAddrForIA(ptrIA->getIAID()))
    {
        //Server provided not enough addresses, so realese this IA in 
        //this particular server (it cheated us in advertise message or
        //or another client stole our addresses in the meantime 
        //or the server doesn't want to prolong lease.

        ptrIA->firstAddr();
        while(SPtr<TAddrAddr> addrToRel=ptrIA->getAddr()) {
	    SPtr<TIPv6Addr> addr2(addrToRel->get());
            ClntIfaceMgr().getIfaceByID(ptrIA->getIface())->delAddr(addr2);
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

        ClntAddrMgr().delIA(ptrIA->getIAID() );
        ClntAddrMgr().addIA(new TAddrIA(ptrIA->getIface(), SPtr<TIPv6Addr>(), SPtr<TDUID>(),
				   0x7fffffff,0x7fffffff,ptrIA->getIAID()));

	List(TAddrIA) pdLst;
	pdLst.clear();
        if (ptrIA->getAddrCount())
            TransMgr->sendRelease(list,0,pdLst);
        return false;
    }
#endif /* if 0 */

    SPtr<TIfaceIface> ptrIface;
    ptrIface = ClntIfaceMgr().getIfaceByID(this->Iface);
    if (!ptrIface) 
    {
	Log(Error) << "Interface with ifindex=" << this->Iface << " not found." << LogEnd;
	return true;
    }

    // for each address in IA option...
    this->firstAddr();
    while (ptrOptAddr = this->getAddr() ) {
        ptrAddrAddr = ptrIA->getAddr( ptrOptAddr->getAddr() );
        
        // no such address in DB ??
        if (!ptrAddrAddr) {
            if (ptrOptAddr->getValid()) {

		int prefixLen = ptrIface->getPrefixLength();
		if (ptrOptAddr->getOption(OPTION_ADDRPARAMS)) {
		    Log(Debug) << "Experimental addr-params found." << LogEnd;
		    SPtr<TClntOptAddrParams> optAddrParams = (Ptr*) ptrOptAddr->getOption(OPTION_ADDRPARAMS);
		    prefixLen = optAddrParams->getPrefix();
		}

                // add this address in addrDB...
                ptrIA->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid(), prefixLen);
                ptrIA->setDUID(this->DUID);

                // ... and in IfaceMgr - 
                ptrIface->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid(), prefixLen);
            } 
            else {
                Log(Warning) << "Server send new addr with valid lifetime 0." << LogEnd;
            }
        } else {
            // we have this addr in DB
            if ( ptrOptAddr->getValid() == 0 ) {
                // valid=0, release this address
                // delete address from addrDB
                ptrIA->delAddr(ptrOptAddr->getAddr());
                // delete address from IfaceMgr
                ptrIface->delAddr(ptrOptAddr->getAddr(), ptrIface->getPrefixLength());
                break; // analyze next option OPTION_IA_NA
            }

            // set up new options in IfaceMgr
            SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(this->Iface);
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
    SPtr<TClntCfgIA> ptrCfgIA;
    ptrCfgIA=ClntCfgMgr().getIA(ptrIA->getIAID());

    if (getT1() && getT2()) {
      ptrIA->setT1( this->getT1() );
      ptrIA->setT2( this->getT2() );
      Log(Debug) << "RENEW will be sent (T1) after " << ptrIA->getT1() << ", REBIND (T2) after " << ptrIA->getT2() << " seconds." << LogEnd;
    } else {
      this->firstAddr();
      ptrOptAddr = this->getAddr();
      if (ptrOptAddr) {
	    ptrIA->setT1( ptrOptAddr->getPref()/2);
	    ptrIA->setT2( (int)((ptrOptAddr->getPref())*0.7) );
	    Log(Notice) << "Server set T1 and T2 to 0. Choosing default (50%, 70% * prefered-lifetime): T1=" << ptrIA->getT1() 
                    << ", T2=" << ptrIA->getT2() << LogEnd;
      }
    }

    ptrIA->setTimestamp();
    ptrIA->setState(STATE_CONFIGURED);
    ptrCfgIA->setState(STATE_CONFIGURED);
    return true;
} 

//Counts all valid and diffrent addresses in sum of
//addresses received in IA from server and addresses contained
//in its counterpart IA in address manager
int TClntOptIA_NA::countValidAddrs(SPtr<TAddrIA> ptrIA)
{
    int count=0;

    //Counts addresses not received in Reply message
    //but which are in addrDB, and are still valid (valid>0)
    SPtr<TAddrAddr> ptrAddr;
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
    /// @todo: A) check if they repeats (possible with maliciious server)
    //       B) and not already assigned in addrDB (in others IAs)
    //       It's easy and worth checking
    //       A) Create list of valid addrs and add new valid addr 
    //          only if it doesn't exist already on this list
    //       B) Address can be assigned only in this IA, not in other
    //          This could be ommited if VerifyIA worked prooperly
    this->firstAddr();
    SPtr<TClntOptIAAddress> ptrOptIAAddr;
    while(ptrOptIAAddr=this->getAddr())
    {
        if (!ptrOptIAAddr->getValid())
            continue;
        //if (!ptrIA->getAddr(ptrOptIAAddr->getAddr()))
               count++;
    }
    return count;
}

SPtr<TClntOptIAAddress> TClntOptIA_NA::getAddr(SPtr<TIPv6Addr> addr)
{
    SPtr<TClntOptIAAddress> optAddr;
    this->firstAddr();
    while(optAddr=this->getAddr())
    {   
        //!memcmp(optAddr->getAddr(),addr,16)
        if ((*addr)==(*optAddr->getAddr()))
            return optAddr;
    };
    return SPtr<TClntOptIAAddress>();
}

void TClntOptIA_NA::releaseAddr(long IAID, SPtr<TIPv6Addr> addr )
{
    SPtr<TAddrIA> ptrIA = ClntAddrMgr().getIA(IAID);
    if (ptrIA)
        ptrIA->delAddr(addr);
    else
	Log(Warning) << "Unable to release addr: IA (" 
		     << IAID << ") not present in addrDB." << LogEnd;
}

void TClntOptIA_NA::setIface(int iface) {
    this->Iface    = iface;
}


bool TClntOptIA_NA::isValid()
{
    SPtr<TClntOptIAAddress> addr;
    this->firstAddr();
    while (addr = this->getAddr()) {
	if (addr->getAddr()->linkLocal()) {
	    Log(Warning) << "Address " << addr->getAddr()->getPlain() << " used in IA (IAID=" 
			 << this->IAID << ") is link local. The whole IA option is considered invalid."
			 << LogEnd;
	    return false;
	}
    }

    if (Valid)
        return this->T1<=this->T2;
    else
        return false;
}
