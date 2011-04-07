/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "AddrIA.h"
#include "ClntCfgMgr.h"
#include "ClntOptIA_PD.h"
#include "ClntOptIAPrefix.h"
#include "ClntOptStatusCode.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "Portable.h"
#include "DHCPConst.h"

/** 
 * Used in REQUEST, RENEW, REBIND, DECLINE and RELEASE
 * 
 * @param addrPD 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(SPtr<TAddrIA> addrPD, TMsg* parent)
    :TOptIA_PD(addrPD->getIAID(),addrPD->getT1(),addrPD->getT2(), parent)
{
    
    bool zeroTimes = false;
    if ( (parent->getType()==RELEASE_MSG) || (parent->getType()==DECLINE_MSG)) {
        this->T1 = 0;
        this->T2 = 0;
	    zeroTimes = true;
    }

    clearContext();

    SPtr<TAddrPrefix> ptrPrefix;
    addrPD->firstPrefix();
    while ( ptrPrefix = addrPD->getPrefix() )
    {
	SubOptions.append(new TClntOptIAPrefix(ptrPrefix->get(), zeroTimes?0:ptrPrefix->getPref(), 
					       zeroTimes?0:ptrPrefix->getValid(), 
					       ptrPrefix->getLength(), this->Parent) );
    }
}

/** 
 * constructor used in building SOLICIT message
 * 
 * @param ClntCfgPD 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(SPtr<TClntCfgPD> cfgPD, TMsg* parent)
    :TOptIA_PD(cfgPD->getIAID(), cfgPD->getT1(), cfgPD->getT2(), parent)
{
    cfgPD->firstPrefix();
    SPtr<TClntCfgPrefix> cfgPrefix;
    while (cfgPrefix = cfgPD->getPrefix() ) {
        SubOptions.append(new TClntOptIAPrefix(cfgPrefix->get(),
                                               cfgPrefix->getPref(),
                                               cfgPrefix->getValid(),
                                               cfgPrefix->getLength(), 0));
    }
    clearContext();
}

/** 
 * Used to create object from received message
 * 
 * @param buf 
 * @param bufsize 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(char * buf,int bufsize, TMsg* parent)
:TOptIA_PD(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize) 
    {
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
       	pos+=2;
        if ((code>0)&&(code<=26))
        {                
	        if(allowOptInOpt(parent->getType(),OPTION_IA_PD,code))
            {
		        SPtr<TOpt> opt= SPtr<TOpt>();
                switch (code)
                {
                case OPTION_IAPREFIX:
        		        SubOptions.append( new TClntOptIAPrefix(buf+pos,length,this->Parent));
                    break;
                case OPTION_STATUS_CODE:
                    SubOptions.append( new TClntOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
	                Log(Warning) << "Option opttype=" << code<< "not supported "
                                 << " in field of message (type="<< parent->getType() 
                                 << ") in this version of server."<<LogEnd;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            } else
		    Log(Warning) << "Illegal option received, opttype=" << code 
                         << " in field options of IA_PD option" << LogEnd;
        } else {
	        Log(Warning) << "Unknown option in option IA_NA(optType=" 
			     << code << "). Option ignored." << LogEnd;
        };
        pos+=length;
    }

    clearContext();
}

void TClntOptIA_PD::firstPrefix()
{
    SubOptions.first();
}

SPtr<TClntOptIAPrefix> TClntOptIA_PD::getPrefix()
{
    SPtr<TClntOptIAPrefix> ptr;
    do {
        ptr = (Ptr*) SubOptions.get();
        if (ptr)
            if (ptr->getOptType()==OPTION_IAPREFIX)
                return ptr;
    } while (ptr);
    return SPtr<TClntOptIAPrefix>();
}

int TClntOptIA_PD::countPrefix()
{
    SPtr< TOpt> ptr;
    SubOptions.first();
    int count = 0;
    while ( ptr = SubOptions.get() ) {
        if (ptr->getOptType() == OPTION_IAPREFIX) 
            count++;
    }
    return count;
}

int TClntOptIA_PD::getStatusCode()
{
    SPtr<TOpt> option;
    if (option=getOption(OPTION_STATUS_CODE))
    {
        SPtr<TClntOptStatusCode> statOpt=(Ptr*) option;
        return statOpt->getCode();
    }
    return STATUSCODE_SUCCESS;
}

void TClntOptIA_PD::setContext(SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr, 
                               TMsg * originalMsg)
{
    DUID=srvDuid;
    if (srvAddr) {
        Unicast = true;
    } else {
        Unicast = false;
    }
    Prefix = srvAddr;
    OriginalMsg = originalMsg;
    Iface = originalMsg->getIface();
}

TClntOptIA_PD::~TClntOptIA_PD()
{

}

bool TClntOptIA_PD::doDuties()
{
    if (!OriginalMsg) {
	Log(Error) << "Internal error. Unable to set prefixes: setContext() not called." << LogEnd;
	return false;
    }
	
    switch(OriginalMsg->getType()) {
    case REQUEST_MSG:
    case SOLICIT_MSG:
	return addPrefixes();
    case RELEASE_MSG:
	return delPrefixes();
    case RENEW_MSG:
    case REBIND_MSG:
	return updatePrefixes();
    default:
	break;
    }
    
    return true;
} 

SPtr<TClntOptIAPrefix> TClntOptIA_PD::getPrefix(SPtr<TIPv6Addr> prefix)
{
   SPtr<TClntOptIAPrefix> optPrefix;
    this->firstPrefix();
    while(optPrefix=this->getPrefix())
    {   
        //!memcmp(optAddr->getAddr(),addr,16)
        if ((*prefix)==(*optPrefix->getPrefix()))
            return optPrefix;
    }
    return 0;
}

bool TClntOptIA_PD::addPrefixes()
{
    return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_ADD);
}

bool TClntOptIA_PD::delPrefixes()
{
    return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_DEL);
}

bool TClntOptIA_PD::updatePrefixes()
{
    return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_UPDATE);
}

bool TClntOptIA_PD::modifyPrefixes(TClntIfaceMgr::PrefixModifyMode mode)
{
    bool status = false;
    EState state = STATE_NOTCONFIGURED;
    SPtr<TClntOptIAPrefix> prefix;
    string action;
    switch(mode) {
      case TClntIfaceMgr::PREFIX_MODIFY_ADD:
        action = "addition";
        state = STATE_CONFIGURED;
        break;
      case TClntIfaceMgr::PREFIX_MODIFY_UPDATE:
        action = "update";
        state = STATE_CONFIGURED;
        break;
      case TClntIfaceMgr::PREFIX_MODIFY_DEL:
        action = "delete";
        state = STATE_NOTCONFIGURED;
        break;
    }
    
    if ( (mode==TClntIfaceMgr::PREFIX_MODIFY_ADD) || (mode==TClntIfaceMgr::PREFIX_MODIFY_UPDATE) ) {
      if ( (T1==0) && (T2==0) ) {
        firstPrefix();
        if (prefix = getPrefix()) {
          T1 = prefix->getPref()/2;
          T2 = (int)((prefix->getPref())*0.7);
          Log(Notice) << "Server set T1 and T2 to 0. Choosing default (50%, 70% * prefered-lifetime): T1=" << T1 
                      << ", T2=" << T2 << LogEnd;
        }
      }
    }

    this->firstPrefix();
    while (prefix = this->getPrefix() ) {
	switch (mode) {
    case TClntIfaceMgr::PREFIX_MODIFY_ADD:
	    ClntAddrMgr().addPrefix(this->DUID, this->Prefix, this->Iface, this->IAID, this->T1, this->T2,
                           prefix->getPrefix(), prefix->getPref(), prefix->getValid(), prefix->getPrefixLength(), false);
	    status = ClntIfaceMgr().addPrefix(this->Iface, prefix->getPrefix(), prefix->getPrefixLength(),
                                     prefix->getPref(), prefix->getValid());
        Log(Debug) << "RENEW will be sent (T1) after " << T1 << ", REBIND (T2) after " << T2 << " seconds." << LogEnd;
	    action = "addition";
	    break;
	case TClntIfaceMgr::PREFIX_MODIFY_UPDATE:
	    ClntAddrMgr().updatePrefix(this->DUID, this->Prefix, this->Iface, this->IAID, this->T1, this->T2,
				  prefix->getPrefix(), prefix->getPref(), prefix->getValid(), prefix->getPrefixLength(), false);
	    status = ClntIfaceMgr().updatePrefix(this->Iface, prefix->getPrefix(), prefix->getPrefixLength(),
					    prefix->getPref(), prefix->getValid());
        Log(Debug) << "RENEW will be sent (T1) after " << T1 << ", REBIND (T2) after " << T2 << " seconds." << LogEnd;
	    action = "update";
	    break;
	case TClntIfaceMgr::PREFIX_MODIFY_DEL:
	    ClntAddrMgr().delPrefix(ClntCfgMgr().getDUID(), this->IAID, prefix->getPrefix(), false);
	    status = ClntIfaceMgr().delPrefix(this->Iface, prefix->getPrefix(), prefix->getPrefixLength() );
	    action = "delete";
	    break;
	}
	
	if (!status) {
	    string tmp = error_message();
	    Log(Error) << "Prefix error encountered during prefix " << action << " operation: " << tmp << LogEnd;
	    setState(STATE_FAILED);
	    return true;
	}
    }

    setState(state);
    return true;
}

void TClntOptIA_PD::setIface(int iface) {
    this->Iface    = iface;
}


bool TClntOptIA_PD::isValid()
{
    SPtr<TClntOptIAPrefix> prefix;
    this->firstPrefix();
    while (prefix = this->getPrefix()) {
	if (prefix->getPrefix()->linkLocal()) {
	    Log(Warning) << "Address " << prefix->getPrefix()->getPlain() << " used in IA (IAID=" 
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

void TClntOptIA_PD::setState(EState state)
{
    SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(this->Iface);

    SPtr<TClntCfgPD> cfgPD = cfgIface->getPD(getIAID());
    if (!cfgPD) {
	Log(Error) << "Unable to find PD with iaid=" << getIAID() << " on the "
		   << cfgIface->getFullName() << " interface (CfgMgr)." << LogEnd;
	return;
    }
    cfgPD->setState(state);

    SPtr<TAddrIA> addrPD = ClntAddrMgr().getPD(getIAID());
    if (!addrPD) {
	/* Log(Error) << "Unable to find PD with iaid=" << getIAID() << " on the "
	   << cfgIface->getFullName() << " interface (AddrMgr)." << LogEnd; */
	/* Don't complain about it. It is normal that IA is being deleted when there
	   are no more prefixes in it */
	return;
    }
    addrPD->setState(state);
}

void TClntOptIA_PD::clearContext()
{
    DUID = 0;
    OriginalMsg = 0;
}
