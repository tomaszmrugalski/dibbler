/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Grzgorz Pluto
 * changes: Tomasz Mrugalski
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvMsgReconfigure.h"
#include "Logger.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "OptStatusCode.h"
#include "OptReconfigureMsg.h"
#include "OptDUID.h"
#include "Opt.h"
#include "SrvOptIA_PD.h"
#include "OptReconfigureMsg.h"
#include "Logger.h"

TSrvMsgReconfigure::TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> clientAddr,
                                       SPtr<TIPv6Addr> ia ,int msgType,
                                       SPtr<TDUID> client_duid)
    :TSrvMsg(iface, clientAddr, RECONFIGURE_MSG, 0 )
{
    ClientDUID=client_duid;

    //appendAuthenticationOption(ClientDUID);

    // include our DUID (Server ID)
    Options.push_back(new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(),this));

    // include his DUID (Client ID)
    Options.push_back(new TOptDUID(OPTION_CLIENTID, client_duid, this));

    // include Reconfigure Message Options
    Options.push_back(new TOptReconfigureMsg(RENEW_MSG, this) );

    send();
    return;
}

bool TSrvMsgReconfigure::check() {
    // this should never happen
    return true;
}

TSrvMsgReconfigure::~TSrvMsgReconfigure() {
}

unsigned long TSrvMsgReconfigure::getTimeout() {
    return 0;
}
void TSrvMsgReconfigure::doDuties() {
    IsDone = true;
}

std::string TSrvMsgReconfigure::getName() const {
    return "RECONFIGURE";
}
