/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptIA_NA.cpp,v 1.8.2.1 2006-02-05 23:38:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "AddrIA.h"
#include "ClntCfgIA.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIAAddress.h"
#include "ClntOptStatusCode.h"
#include "Logger.h"

/** 
 * Used in CONFIRM constructor
 * 
 * @param clntAddrIA 
 * @param zeroTimes 
 * @param parent 
 */
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

/** 
 * Used in DECLINE, RENEW and RELEASE
 * 
 * @param addrIA 
 * @param parent 
 */
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

/** 
 * Used in REQUEST constructor
 * 
 * @param ClntCfgIA 
 * @param ClntaddrIA 
 * @param parent 
 */
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

/** 
 * Used in SOLICIT constructor
 * 
 * @param ClntCfgIA 
 * @param parent 
 */TClntOptIA_NA::TClntOptIA_NA(SmartPtr<TClntCfgIA> ClntCfgIA, TMsg* parent)
    :TOptIA_NA(ClntCfgIA->getIAID(),ClntCfgIA->getT1(),ClntCfgIA->getT2(), parent)
{
    ClntCfgIA->firstAddr();
    SmartPtr<TClntCfgAddr> ClntCfgAddr;
    // just copy all addresses defined in the CfgMgr
    while (ClntCfgAddr = ClntCfgIA->getAddr())
        SubOptions.append(new TClntOptIAAddress(ClntCfgAddr->get(),
        ClntCfgAddr->getPref(),
        ClntCfgAddr->getValid(),this->Parent) ); 
    DUID = SmartPtr<TDUID>(); // NULL
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
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(allowOptInOpt(parent->getType(),OPTION_IA,code))
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
		    Log(Warning) <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() 
                        <<") in this version of server."<<LogEnd;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
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
                             SmartPtr<TDUID> srvDuid, SmartPtr<TIPv6Addr> srvAddr, int iface)
{
    this->AddrMgr=addrMgr;
    this->IfaceMgr=ifaceMgr;
    this->TransMgr=transMgr;
    this->CfgMgr=cfgMgr;
    this->DUID=srvDuid;
    if (srvAddr) {
        this->Unicast = true;
    } else {
        this->Unicast = false;
    }
    this->Addr=srvAddr;
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
	Log(Warning) << "Received message contains unknown IA (IAID="
            << this->getIAID() << "). We didn't other it. Weird... ignoring it." << LogEnd;
        return true;
    }

    // IAID found, set up new received options.
    SmartPtr<TAddrAddr> ptrAddrAddr;
    SmartPtr<TClntOptIAAddress> ptrOptAddr;

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

        AddrMgr->delIA(ptrIA->getIAID() );
        AddrMgr->addIA(new TAddrIA(ptrIA->getIface(), SmartPtr<TIPv6Addr>(), SmartPtr<TDUID>(),
				   0x7fffffff,0x7fffffff,ptrIA->getIAID()));
        if (ptrIA->getAddrCount())
            TransMgr->sendRelease(list);
        return false;
    }

    //In other case, we have appropriate number of addresses and all is ok
    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = IfaceMgr->getIfaceByID(this->Iface);
    if (!ptrIface) 
    {
	Log(Error) << "Interface " << this->Iface << " not found." << LogEnd;
	return true;
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
		Log(Notice) << "Address " << *ptrOptAddr->getAddr() << " added to "
			    << ptrIface->getName() << "/" << ptrIface->getID() 
			    << " interface." << LogEnd;
            } 
            else {
                Log(Warning) << "Server send new addr with valid=0." << LogEnd;
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
    
    ptrIA->setT1( this->getT1() );
    ptrIA->setT2( this->getT2() );
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
	Log(Warning) << "Unable to release addr: IA (" 
		     << IAID << ") not present in addrDB." << LogEnd;
}

void TClntOptIA_NA::setIface(int iface) {
    this->Iface    = iface;
}


bool TClntOptIA_NA::isValid()
{
    SmartPtr<TClntOptIAAddress> addr;
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

/*
 * Revision 1.8  2004/12/07 20:51:22  thomson
 * Link local safety checks added (bug #39)
 *
 * Revision 1.7  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.6  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.5  2004/09/07 22:02:33  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 *
 * Revision 1.3  2004/06/04 19:03:46  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */
