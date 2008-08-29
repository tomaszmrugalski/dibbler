/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgRebind.cpp,v 1.17 2008-08-29 00:07:28 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntMsgRebind.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptServerUnicast.h"
#include "AddrIA.h"
#include "Logger.h"
#include "ClntOptOptionRequest.h"
#include <cmath>
#include <iostream>

TClntMsgRebind::TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                               SmartPtr<TClntTransMgr> TransMgr, 
                               SmartPtr<TClntCfgMgr> CfgMgr, 
                               SmartPtr<TClntAddrMgr> AddrMgr,
                               List(TOpt) ptrOpts, int iface)
  :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr, iface, 0, REBIND_MSG)
{
    Options=ptrOpts;
    IRT=REB_TIMEOUT;
    MRT=REB_MAX_RT;
    MRC=0;
    RT=0;

    // there are options copied from RENEW. Get rid of some of them
    SmartPtr<TOpt> opt;
    firstOption();
    while(opt=getOption())
    {
        if (opt->getOptType()==OPTION_ELAPSED_TIME)
            Options.del();
        if (opt->getOptType()==OPTION_SERVERID)
            Options.del();
    };

    // calculate timeout (how long should be the REBIND message transmitted)
    unsigned long maxMRD=0;    
    firstOption();
    while(opt=getOption())
    {
      switch (opt->getOptType()) {
        case OPTION_IA_NA:
          {
            SmartPtr<TClntOptIA_NA> ptrIA=(Ptr*) opt;
            SmartPtr<TAddrIA> ptrAddrIA= ClntAddrMgr->getIA(ptrIA->getIAID());
            if (ptrAddrIA && maxMRD<ptrAddrIA->getMaxValidTimeout())
              maxMRD=ptrAddrIA->getMaxValidTimeout();
            break;
          }
        case OPTION_IA_PD:
          {
            SPtr<TClntOptIA_PD> pd = (Ptr*) opt;
            SPtr<TAddrIA> addrPd = ClntAddrMgr->getPD(pd->getIAID());
            if (addrPd && maxMRD<addrPd->getMaxValidTimeout())
              maxMRD=addrPd->getMaxValidTimeout();
            break;
          }
      }
    }
    MRD= maxMRD;

    appendElapsedOption();
    appendAuthenticationOption(AddrMgr);

    pkt = new char[getSize()];
    this->IsDone = false;    
}

void TClntMsgRebind::answer(SmartPtr<TClntMsg> Reply)
{
  TClntMsg::answer(Reply);
  return;

#if 0
  // FIXME: Fix REPLY support for REBIND
  
SmartPtr<TOpt> opt;
    
    // get DUID
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) Reply->getOption(OPTION_SERVERID);
    
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt=(Ptr*)getOption(OPTION_ORO);

    Reply->firstOption();
    // for each option in message... (there should be only one IA option, as we send 
    // separate RENEW for each IA, but we check all options anyway)
    while ( opt = Reply->getOption() ) 
    {
        switch (opt->getOptType()) 
        {
            case OPTION_IA_NA:
            {
                SmartPtr<TClntOptIA_NA> ptrOptIA = (Ptr*)opt;
                ptrOptIA->setContext(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                                     ptrDUID->getDUID(), SmartPtr<TIPv6Addr>() /*NULL*/,Reply->getIface());
                //FIXME: replace this NULL with something meaningfull
                //       (required for Unicast to work correctly)
                ptrOptIA->doDuties();
                if (ptrOptIA->getStatusCode()==STATUSCODE_SUCCESS)
                {
                    //if we have received enough addresses,
                    //remove assigned IA's by server from request message
                    SmartPtr<TOpt> requestOpt;
                    Options.first();
                    while (requestOpt = Options.get())
                    {
                        if (requestOpt->getOptType()==OPTION_IA_NA)
                        {
                            SmartPtr<TClntOptIA_NA> ptrIA = (Ptr*) requestOpt;
                            if ((ptrIA->getIAID() == ptrOptIA->getIAID() ) &&
                                (ClntCfgMgr->countAddrForIA(ptrIA->getIAID()) == ptrIA->countAddr()) )
                            {
                                //found this IA, it has enough addresses and everything is ok.
                                //Shortly, we have this IA configured.
                                Options.del();
                                break;
                            }
                        } //if
                    } //while
                }
                break;
            }
            case OPTION_IA_PD:
            {
              break;
            }
            case OPTION_ORO:
            case OPTION_RELAY_MSG:
            case OPTION_INTERFACE_ID:
            case OPTION_IAADDR:
            case OPTION_RECONF_MSG:
            {
		Log(Warning) << "Illegal option (" << opt->getOptType() 
                    << ") in received REPLY message." << LogEnd;
                break;
            }
            default:
            {             
                // what to do with unknown/other options? execute them
                opt->setParent(this);
                if (opt->doDuties()) 
                {
                    SmartPtr<TOpt> requestOpt;
                    Options.first();
                    while ( requestOpt = Options.get()) {
                        if (requestOpt->getOptType()==opt->getOptType()) 
                        {
                            if (ptrOptionReqOpt&&(ptrOptionReqOpt->isOption(opt->getOptType())))
                                ptrOptionReqOpt->delOption(opt->getOptType());
                            Options.del();
                        }//if
                    }//while
                }
            }
        }
    }
    IsDone = true;
#endif
}

void TClntMsgRebind::updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA,
			      SmartPtr<TClntOptServerIdentifier> optSrvDUID, 
			      SmartPtr<TClntOptServerUnicast> optUnicast) {

    SmartPtr< TAddrIA> ptrAddrIA;
    bool found = false;
    
    // ..find IA in addrMgr...
    ClntAddrMgr->firstIA();
    while  (ptrAddrIA = ClntAddrMgr->getIA() ) {
	if (ptrOptIA->getIAID() == ptrAddrIA->getIAID()) {
	    found = true;
	    break;
	}
    }

    if (found) {

	// set new server's DUID to handle this IA
	ptrAddrIA->setDUID(optSrvDUID->getDUID());

	//
	if (optUnicast) {
	    ptrAddrIA->setUnicast(optUnicast->getAddr());
	} else {
	    ptrAddrIA->setMulticast();
	}

	// IAID found, set up new received options.
	SmartPtr<TAddrAddr> ptrAddrAddr;
	SmartPtr<TClntOptIAAddress> ptrOptAddr;
	
	// are all addrs configured?
	if (ptrOptIA->countAddr() != ptrAddrIA->countAddr() ) {
	    this->releaseIA(ptrOptIA->getIAID());
	}
	
	// for each address in IA option...
	ptrOptIA->firstAddr();
	while (ptrOptAddr = ptrOptIA->getAddr() ) {
	    ptrAddrAddr = ptrAddrIA->getAddr( ptrOptAddr->getAddr() );
	    if (!ptrAddrAddr) {
		// there is no such addr in db
		// FIXME: what to do with new addrs?
		// (Thomson:I think we should RELEASE all addrs, and issue new REQUEST for this IA)
	    } else {
		if ( (ptrOptAddr->getPref() == 0) || (ptrOptAddr->getValid() == 0) ) {
		    releaseIA( ptrOptIA->getIAID() );
		    break; // analyze next option OPTION_IA_NA
		}
		// set up new options
		if (ptrOptAddr->getPref() != ptrAddrAddr->getPref() )
		{
		    // received diffrent prefered-lifetime
		    // FIXME:
		}
		if (ptrOptAddr->getValid() != ptrAddrAddr->getValid() )
		{
		    // received diffrent prefered-lifetime
		    // FIXME:
		}
		ptrAddrAddr->setTimestamp();
	    }
	}
	ptrAddrIA->setT1( ptrOptIA->getT1() );
	ptrAddrIA->setT2( ptrOptIA->getT2() );
	ptrAddrIA->setTimestamp();
	ptrAddrIA->setState(STATE_CONFIGURED);
    } else {
	// unknown IAID, ignore it
	Log(Warning) << "Received message contains unknown IA (IAID="
	     << ptrOptIA->getIAID() << "). Ignoring it." << LogEnd;
    }
}

void TClntMsgRebind::doDuties()
{
    SmartPtr<TIfaceIface> iface = ClntIfaceMgr->getIfaceByID(this->Iface);

    if (!MRD)
    {
	stringstream iaLst;
	stringstream pdLst;
        SmartPtr<TOpt> ptrOpt;
        firstOption();
        while(ptrOpt=getOption())
        {
	    switch( ptrOpt->getOptType()) {
	    case OPTION_IA_NA:
	    {
		SmartPtr<TClntOptIA_NA> ptrIA=(Ptr*)ptrOpt;
		iaLst << ptrIA->getIAID() << " ";
		releaseIA(ptrIA->getIAID());
		break;
	    }
	    case OPTION_IA_PD:
	    {
		SPtr<TClntOptIA_PD> ptrPD = (Ptr*)ptrOpt;
		ptrPD->setContext(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr, 0, 0, this);
		ptrPD->delPrefixes();
		break;
	    }
	    };
        }
	Log(Warning) << "REBIND for the IA(s):" << iaLst.str()
		     << ", PD(s):" << pdLst.str()
		     << " failed on the " << iface->getFullName() 
		     << " interface." << LogEnd;
	Log(Warning) << "Restarting server discovery process." << LogEnd;
        IsDone=true;
    }
    else
      this->send();
}

void TClntMsgRebind::releaseIA(int IAID)
{
    SmartPtr<TAddrIA> ptrAddrIA=this->ClntAddrMgr->getIA(IAID);
    if (!ptrAddrIA)
    {
        Log(Error) << "IA has not been found in Address Manager."<< LogEnd;
        return;
    }

    SmartPtr<TAddrAddr> ptrAddr;
    ptrAddrIA->firstAddr();
    while(ptrAddr=ptrAddrIA->getAddr())
    {
        //remove outdated address from interface
	SPtr<TIfaceIface> ptrIface = ClntIfaceMgr->getIfaceByID(ptrAddrIA->getIface());
	if (!ptrIface) {
	    Log(Error) << "Unable to find interface with ifindex " << ptrAddrIA->getIface() << LogEnd;
	    continue;
	}
	ptrIface->delAddr(ptrAddr->get(), ptrIface->getPrefixLength());
        //and from db
        ptrAddrIA->delAddr(ptrAddr->get());
    }
    ptrAddrIA->setState(STATE_NOTCONFIGURED);

    SPtr<TClntCfgIA> cfgIA = ClntCfgMgr->getIA(IAID);
    if (!cfgIA)
	return;
    cfgIA->setState(STATE_NOTCONFIGURED);
}

bool TClntMsgRebind::check() {
	return 0;
}

string TClntMsgRebind::getName() {
    return "REBIND";
}

TClntMsgRebind::~TClntMsgRebind() {
}
