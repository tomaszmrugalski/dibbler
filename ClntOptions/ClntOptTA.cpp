/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptTA.cpp,v 1.7 2008-08-29 00:07:29 thomson Exp $
 *
 */

#include "AddrIA.h"
#include "ClntOptTA.h"
#include "ClntOptIAAddress.h"
#include "ClntOptStatusCode.h"
#include "ClntOptRapidCommit.h"
#include "Logger.h"
#include "ClntOptIAAddress.h"

/** 
 * Constructor used during 
 * 
 * @param iaid 
 * @param parent 
 */TClntOptTA::TClntOptTA(unsigned int iaid, TMsg* parent)
    :TOptTA(iaid, parent) 
{
    // don't put any suboptions
}

/** 
 * Constructor used during option reception
 * 
 * @param buf 
 * @param bufsize 
 * @param parent 
 */
TClntOptTA::TClntOptTA(char * buf,int bufsize, TMsg* parent)
:TOptTA(buf,bufsize, parent)
{
    int pos=0, length=0;
    while(pos<bufsize) 
    {
	pos+=length;
        int code= buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;
	SmartPtr<TOpt> opt = 0;

	if(!allowOptInOpt(parent->getType(),OPTION_IA_TA,code)) {
	    Log(Warning) << "Option " << code << " is not allowed as suboption of " << OPTION_IA_TA << LogEnd;
	    continue;
	}

	switch (code) {
	case OPTION_IAADDR:
	    opt = new TClntOptIAAddress(buf+pos,length,this->Parent);
	    break;
	case OPTION_STATUS_CODE:
	    opt = new TClntOptStatusCode(buf+pos,length,this->Parent);
	    break;
	default:
	    Log(Warning) <<"Option " << code<< " in message (type=" << parent->getType() 
			 <<") is currently not supported."<<LogEnd;
	    break;
	}
	if( (opt) && (opt->isValid()) )
	    SubOptions.append(opt);
	pos+=length;
    }
}

TClntOptTA::TClntOptTA(SmartPtr<TAddrIA> ta, TMsg* parent)
    :TOptTA(ta->getIAID(), parent) 

{
    ta->firstAddr();
    SmartPtr<TAddrAddr> addr;
    while (addr=ta->getAddr()) {
	SubOptions.append(new TClntOptIAAddress(addr->get(), 0, 0, parent));
    }
}

void TClntOptTA::firstAddr()
{
    SubOptions.first();
}

SmartPtr<TClntOptIAAddress> TClntOptTA::getAddr()
{
    SmartPtr<TClntOptIAAddress> ptr;
    do {
        ptr = (Ptr*) SubOptions.get();
        if (ptr)
            if (ptr->getOptType()==OPTION_IAADDR)
                return ptr;
    } while (ptr);
    return 0;
}

int TClntOptTA::countAddr()
{
    SmartPtr<TOpt> ptr;
    SubOptions.first();
    int count = 0;
    while ( ptr = SubOptions.get() ) {
        if (ptr->getOptType() == OPTION_IAADDR) 
            count++;
    }
    return count;
}

int TClntOptTA::getStatusCode()
{
    SmartPtr<TOpt> option;
    if (option=getOption(OPTION_STATUS_CODE))
    {
        SmartPtr<TClntOptStatusCode> statOpt=(Ptr*) option;
        return statOpt->getCode();
    }
    return STATUSCODE_SUCCESS;
}

TClntOptTA::~TClntOptTA()
{

}

/** 
 * This function sets everything in motion, i.e. sets up temporary addresses
 * 
 * @return 
 */bool TClntOptTA::doDuties()
{
    // find this TA in addrMgr...
    SmartPtr<TAddrIA> ta = AddrMgr->getTA(this->getIAID());

    if (!ta) {
	Log(Debug) << "Creating TA (iaid=" << this->getIAID() << ") in the addrDB." << LogEnd;
	ta = new TAddrIA(this->Iface, 0 /*if unicast, then this->Addr*/, this->DUID, DHCPV6_INFINITY, 
			 DHCPV6_INFINITY, this->getIAID());
	AddrMgr->addTA(ta);
    }

    // IAID found, set up new received options.
    SmartPtr<TAddrAddr> addr;
    SmartPtr<TClntOptIAAddress> optAddr;

    SmartPtr<TIfaceIface> ptrIface;
    ptrIface = IfaceMgr->getIfaceByID(this->Iface);
    if (!ptrIface) 
    {
	Log(Error) << "Interface " << this->Iface << " not found." << LogEnd;
	return true;
    }

    // for each address in IA option...
    this->firstAddr();
    while (optAddr = this->getAddr() ) {
        addr = ta->getAddr( optAddr->getAddr() );
        
        if (!addr) { // - no such address in DB -
            if (!optAddr->getValid()) {
                Log(Warning) << "Server send new addr with valid=0." << LogEnd;
		continue;
	    }
	    // add this address in addrDB...
	    ta->addAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid());
	    ta->setDUID(this->DUID);
	    // ... and in IfaceMgr - 
	    ptrIface->addAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid(), ptrIface->getPrefixLength());
	    Log(Notice) << "Temp. address " << *optAddr->getAddr() << " has been added to "
			<< ptrIface->getName() << "/" << ptrIface->getID() 
			<< " interface." << LogEnd;

        } else { // - we have this addr in DB -

            if ( optAddr->getValid() == 0 ) {
                // valid=0, release this address and delete address from addrDB
                ta->delAddr(optAddr->getAddr());
                // delete address from IfaceMgr
                ptrIface->delAddr(optAddr->getAddr(), ptrIface->getPrefixLength());
                continue; // analyze next option OPTION_IA
            }

            // set up new options in IfaceMgr
	    ptrIface->updateAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid());
            // set up new options in addrDB
            addr->setPref(optAddr->getPref());
            addr->setValid(optAddr->getValid());
            addr->setTimestamp();
        }
    }

    // mark this TA as configured
    SmartPtr<TClntCfgTA> cfgTA;
    SmartPtr<TClntCfgIface> cfgIface;
    if (! (cfgIface = CfgMgr->getIface(this->Iface)) ) {
	Log(Error) << "Unable to find TA class in the CfgMgr, on the " << this->Iface << " interface." << LogEnd;
	return true;
    }
    cfgIface->firstTA();
    cfgTA = cfgIface->getTA();
    cfgTA->setState(STATE_CONFIGURED);

    ta->setState(STATE_CONFIGURED);
    return true;
} 

SmartPtr<TClntOptIAAddress> TClntOptTA::getAddr(SmartPtr<TIPv6Addr> addr)
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

void TClntOptTA::releaseAddr(long IAID, SmartPtr<TIPv6Addr> addr )
{
    SmartPtr<TAddrIA> ptrIA = AddrMgr->getIA(IAID);
    if (ptrIA)
        ptrIA->delAddr(addr);
    else
	Log(Warning) << "Unable to release addr: IA (" 
		     << IAID << ") not present in addrDB." << LogEnd;
}

bool TClntOptTA::isValid()
{
    SmartPtr<TClntOptIAAddress> addr;
    this->firstAddr();
    while (addr = this->getAddr()) {
	if (!addr->getAddr()->linkLocal())
	    continue;
	Log(Warning) << "Address " << addr->getAddr()->getPlain() << " used in IA_TA (IAID=" 
		     << this->IAID << ") is link local. The whole IA_TA option is considered invalid."
		     << LogEnd;
	return false;
    }

    return true;
}

void TClntOptTA::setContext(SmartPtr<TClntAddrMgr> addrMgr, SmartPtr<TClntIfaceMgr> ifaceMgr,
			    SmartPtr<TClntCfgMgr> cfgMgr, int iface,
			    SmartPtr<TIPv6Addr> clntAddr) {
    this->IfaceMgr = ifaceMgr;
    this->AddrMgr  = addrMgr;
    this->CfgMgr   = cfgMgr;
    this->Iface    = iface;
    this->Addr     = clntAddr;
}

void TClntOptTA::setIface(int iface) {
    this->Iface    = iface;
}
