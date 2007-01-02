/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#include "AddrIA.h"
#include "ClntCfgPD.h"
#include "ClntOptIA_PD.h"
#include "ClntOptIAPrefix.h"
#include "ClntOptStatusCode.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "Portable.h"
#include "DHCPConst.h"

/** 
 * 
 * 
 * @param clntAddrIA 
 * @param zeroTimes 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(SmartPtr<TAddrIA> clntAddrPD, bool zeroTimes, TMsg* parent)
    :TOptIA_PD(clntAddrPD->getIAID(), zeroTimes?0:clntAddrPD->getT1(),
	       zeroTimes?0:clntAddrPD->getT2(), parent) 
{
    SmartPtr <TIPv6Addr> prefix;
    clntAddrPD->firstPrefix();
    //while ( prefix = clntAddrPD->getPrefix() ) {
	//SubOptions.append( new TClntOptIAPrefix(prefix,3000, 6000,64, parent) );
   // }
    
}

/** 
 * Used in DECLINE, RENEW and RELEASE
 * 
 * @param addrIA 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(SmartPtr<TAddrIA> addrPD, TMsg* parent)
    :TOptIA_PD(addrPD->getIAID(),addrPD->getT1(),addrPD->getT2(), parent)
{
    // should we include all addrs or tentative ones only?
    bool decline;
    if (parent->getType()==DECLINE_MSG)
	decline = true;
    else
	decline = false;

    SmartPtr<TAddrPrefix> ptrPrefix;
    addrPD->firstPrefix();
    while ( ptrPrefix = addrPD->getPrefix() )
    {
	if ( !decline || (ptrPrefix->getTentative()==TENTATIVE_YES) )
	    SubOptions.append(new TClntOptIAPrefix(ptrPrefix->get(), ptrPrefix->getPref(), 
						    ptrPrefix->getValid(),64,this->Parent) );
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
TClntOptIA_PD::TClntOptIA_PD(SmartPtr<TClntCfgPD> ClntCfgPD, SmartPtr<TAddrIA> ClntaddrPD, TMsg* parent)
    :TOptIA_PD(ClntaddrPD->getIAID(),ClntaddrPD->getT1(),ClntaddrPD->getT2(), parent)
{
    
    //checkRequestConstructor
    /*ClntCfgPD->firstPD();
      SmartPtr<TClntCfgPrefix> ClntCfgPrefix;
      while ((ClntCfgPrefix = ClntCfgPD->getPrefix())
      //   &&((ClntCfgPD->countPrefixes()-ClntaddrPD->getPrefixCount())>this->countPrefixes() ))
      {
      SmartPtr<TAddrPrefix> ptrPrefix=ClntaddrPD->getPrefix(ClntCfgPrefix->get());
        if(!ptrPrefix)
	SubOptions.append(new TClntOptIAPrefix(
	ClntCfgPrefix->get(),
	ClntCfgPrefix->getPref(),
            ClntCfgPrefix->getValid(),
	    64,
            this->Parent));
	    }*/
    //DUID = SmartPtr<TDUID>(); // NULL
}

/** 
 * Used in SOLICIT constructor
 * 
 * @param ClntCfgPD 
 * @param parent 
 */
TClntOptIA_PD::TClntOptIA_PD(SmartPtr<TClntCfgPD> ClntCfgPD, TMsg* parent)
    :TOptIA_PD(1, 0, 0, parent) // no hint support implemented yet, will be 
{
    // FIXME: Copy all prefixes defined in CfgMgr (i.e. implement client hints)

    //SmartPtr<TIPv6Addr> prefix ;
    //SmartPtr<TClntCfgPrefix> ClntCfgPrefix();

    // just copy all addresses defined in the CfgMgr
   // while (ClntCfgPrefix = ClntCfgPD->getPrefix())
       // SubOptions.append(new TClntOptIAPrefix(prefix,
       // 64,
       // 4000,5000,this->Parent) ); 
    DUID = SmartPtr<TDUID>(); // NULL
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
		SmartPtr<TOpt> opt= SmartPtr<TOpt>();
                switch (code)
                {
                case OPTION_IAPREFIX:
                    //  SmartPtr<TOptIAAddress> ptr;
		    SubOptions.append( new TClntOptIAPrefix(buf+pos,length,this->Parent));
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
                << " in field options of IA_PD option"<<LogEnd;
        }
        else
        {
	    Log(Warning) <<"Unknown option in option IA_NA( optType=" 
			 << code << "). Option ignored." << LogEnd;
        };
        pos+=length;
    }
}

void TClntOptIA_PD::firstPrefix()
{
    SubOptions.first();
}

SmartPtr<TClntOptIAPrefix> TClntOptIA_PD::getPrefix()
{
    SmartPtr<TClntOptIAPrefix> ptr;
    do {
        ptr = (Ptr*) SubOptions.get();
        if (ptr)
            if (ptr->getOptType()==OPTION_IAPREFIX)
                return ptr;
    } while (ptr);
    return SmartPtr<TClntOptIAPrefix>();
}

int TClntOptIA_PD::countPrefix()
{
    SmartPtr< TOpt> ptr;
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
    SmartPtr<TOpt> option;
    if (option=getOption(OPTION_STATUS_CODE))
    {
        SmartPtr<TClntOptStatusCode> statOpt=(Ptr*) option;
        return statOpt->getCode();
    }
    return STATUSCODE_SUCCESS;
}

void TClntOptIA_PD::setThats(SmartPtr<TClntIfaceMgr> ifaceMgr, 
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
    this->Prefix=srvAddr;
    this->Iface = iface;
}

TClntOptIA_PD::~TClntOptIA_PD()
{

}

bool TClntOptIA_PD::doDuties()
{
    if (Parent->getType()==REQUEST_MSG)
	return addPrefixes();
    if (Parent->getType()==RELEASE_MSG)
	return delPrefixes();
    return true;
} 

SmartPtr<TClntOptIAPrefix> TClntOptIA_PD::getPrefix(SmartPtr<TIPv6Addr> prefix)
{
   SmartPtr<TClntOptIAPrefix> optPrefix;
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
    SmartPtr<TClntOptIAPrefix> prefix;
    this->firstPrefix();
    SmartPtr<TClntCfgIface> cfgIface = this->CfgMgr->getIface(this->Iface);
    
    while (prefix = this->getPrefix()) {
	Log(Info) << "PD: Received prefix: " << prefix->getPrefix()->getPlain() << "/" << this->Prefix << LogEnd;
	AddrMgr->addPrefix(CfgMgr->getDUID(), this->Prefix, this->Iface, this->IAID, this->T1, this->T2,
			   prefix->getPrefix(), prefix->getPref(), prefix->getValid(), prefix->getPrefixLength(), false);
	
	int status = prefix_add(cfgIface->getName().c_str(), this->Iface, prefix->getPrefix()->getPlain(), prefix->getPrefixLength());
	if (status<0) {
	    string tmp = error_message();
	    Log(Error) << "Prefix error encountered during add operation: " << tmp << LogEnd;
	    cfgIface->setPrefixDelegationState(FAILED);
	    return true;
	}
    }
    
    cfgIface->setPrefixDelegationState(CONFIGURED);
    return true;
}

bool TClntOptIA_PD::delPrefixes()
{
    SmartPtr<TClntOptIAPrefix> prefix;
    this->firstPrefix();
    SmartPtr<TClntCfgIface> cfgIface = this->CfgMgr->getIface(this->Iface);

    while (prefix = this->getPrefix()) {
	Log(Info) << "PD: Deleting prefix: " << prefix->getPrefix()->getPlain() << "/" << this->Prefix << LogEnd;
	AddrMgr->delPrefix(CfgMgr->getDUID(), this->IAID, prefix->getPrefix(), false);
	
	int status = prefix_del(cfgIface->getName().c_str(), this->Iface, prefix->getPrefix()->getPlain(), prefix->getPrefixLength());
	if (status<0) {
	    string tmp = error_message();
	    Log(Error) << "Prefix error encountered during delete operation: " << tmp << LogEnd;
	    cfgIface->setPrefixDelegationState(FAILED);
	    return true;
	}
    }
    cfgIface->setPrefixDelegationState(DISABLED);
    return true;
}

void TClntOptIA_PD::setIface(int iface) {
    this->Iface    = iface;
}


bool TClntOptIA_PD::isValid()
{
    SmartPtr<TClntOptIAPrefix> prefix;
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
