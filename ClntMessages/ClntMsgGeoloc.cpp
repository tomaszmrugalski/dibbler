/* 
 * File:   ClntMsgGeoloc.cpp
 * Author: Michal Golon
 * 
 */

#include "ClntMsgGeoloc.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "OptDUID.h"
#include "OptAddr.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "ClntOptIA_NA.h"
#include "ClntOptTA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptElapsed.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptStatusCode.h"
#include "ClntTransMgr.h"
#include <cmath>
#include "Logger.h"
#include "OptStringLst.h"

// create message with geolocation information
TClntMsgGeoloc::TClntMsgGeoloc(int iface, SPtr<TIPv6Addr> addr, List(string)coordinates) 
    :TClntMsg(iface, addr, GEOLOC_MSG) {
    
    IRT = REQ_TIMEOUT;
    MRT = REQ_MAX_RT;
    MRC = REQ_MAX_RC;
    MRD = 0;   
    RT=0;

    Iface = iface;
    PeerAddr = addr;

    // adding client's DUID
    Options.push_back(new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this ) );
    
    // adding coordinates
    SPtr<TOpt> ptr = new TOptStringLst(OPTION_GEOLOC, coordinates, this);
    Options.push_back(ptr);

    appendRequestedOptions();
    appendElapsedOption();
    appendAuthenticationOption();
        
    IsDone=false;
    
    send();
}


void TClntMsgGeoloc::answer(SPtr<TClntMsg> msg) {
    // there is no answer to this message
}

void TClntMsgGeoloc::doDuties() {
    Log(Info) << "Geolocation information has been sent!" << LogEnd;
    IsDone = true;
    
    return;
}

bool TClntMsgGeoloc::check() {
    return false;
}

string TClntMsgGeoloc::getName() {
    return "GEOLOC";
}


TClntMsgGeoloc::~TClntMsgGeoloc() {
}