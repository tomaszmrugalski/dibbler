/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 or later licence
 */
#include "AddrIA.h"
//#include "AddrPD.h"
#include "ClntCfgPD.h"
#include "ClntOptIA_PD.h"
#include "ClntOptIAPrefix.h"
#include "ClntOptStatusCode.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"



/** 
 * Used in CONFIRM constructor
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

//I don't know whether this method should be invoked everywhere
//i.e. from Verify\Renew\Rebind
//it's worth to check whether futher reactions in every message will 
//be the same e.g. detection of duplicate address, lack of enough
//addresses
bool TClntOptIA_PD::doDuties()
{
    //TODO this method should be platform independent. 
    SmartPtr<TClntOptIAPrefix> prefix;
    this->firstPrefix();
    while (prefix = this->getPrefix()) {
	AddrMgr->addPrefix(CfgMgr->getDUID(), this->Prefix, this->Iface, this->IAID, this->T1, this->T2,
			   prefix->getPrefix(), prefix->getPref(), prefix->getValid(), prefix->getPrefixLength(), true);
	
    }

    if (!this->createCfgFile()) {
	Log(Warning) << "Unable to generate config. file for router Adv. daemon" << LogEnd;
	return false;
    }

    
    SmartPtr<TClntCfgIface> cfgIface = this->CfgMgr->getIface(this->Iface);
    cfgIface->setPrefixDelegationState(CONFIGURED);

    return true;
} 

//Counts all valid and diffrent addresses in sum of
//addresses received in IA from server and addresses contained
//in its counterpart IA in address manager
int TClntOptIA_PD::countValidPrefixes(SmartPtr<TAddrIA> ptrPD)
{
    int count=0;
   return count;
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
    return SmartPtr<TClntOptIAPrefix>();
}

void TClntOptIA_PD::releasePrefix(long IAID, SmartPtr<TIPv6Addr> prefix )
{
    /*SmartPtr<TAddrIA> ptrPD = AddrMgr->getPD(IAID);
    if (ptrPD)
        ptrPD->delPrefix(prefix);
    else
	Log(Warning) << "Unable to release addr: IA (" 
		     << IAID << ") not present in addrDB." << LogEnd;*/
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

bool TClntOptIA_PD::createCfgFile(){

#ifdef LINUX
	SmartPtr<TClntOptIAPrefix> prefix;
	// FIXME: what if there are several prefixes granted?
	this->firstPrefix();
   	while (prefix = this->getPrefix()) {
		cout<<"Generating config file for prefix "<<*prefix->getPrefix()<< "on interface \n";
		cout<<"Prefix length is " << (int)prefix->getPrefixLength() << "  so we have to truncate it a little bit\n" ; 
    	char truncatedPrefix[128];

		// FIXME: generate subprefixes for each local interface
		truncatePrefixFromConfig((prefix->getPrefix())->getAddr(), truncatedPrefix, prefix->getPrefixLength());

		SmartPtr<TClntIfaceMgr> clntIfaceMgr = new TClntIfaceMgr(CLNTIFACEMGR_FILE);
    	
		ofstream radvdConfigFile;
		radvdConfigFile.open(RADVD_FILE);
		radvdConfigFile<<"# Router Advertisement config file generated by Dibbler \n";
		radvdConfigFile<<"interface eth0 \n";
		radvdConfigFile<<"{ \n";
		radvdConfigFile<<"AdvSendAdvert on; \n";
		radvdConfigFile<<"prefix "<<truncatedPrefix<<"/"<<(int)prefix->getPrefixLength()<<" \n";
		radvdConfigFile<<"	 { \n";
		radvdConfigFile<<"		AdvOnLink on;\n";
		radvdConfigFile<<"              AdvAutonomous on;\n";
		radvdConfigFile<<"	};\n";
		radvdConfigFile<<"};\n";
		radvdConfigFile.close();
	}
#endif

#ifdef WIN32
	// FIXME: Implement prefix-delegation in WIN32
	Log(Error) << "Prefix delegation support in WIN32 is not implemented yet." << LogEnd;
#endif
	return true;
}

