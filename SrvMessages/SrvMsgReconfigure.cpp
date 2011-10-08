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
#include "SrvOptOptionRequest.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptPreference.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "SrvOptFQDN.h"
#include "SrvOptIA_PD.h"
#include "OptReconfigureMsg.h"
#include "Logger.h"

TSrvMsgReconfigure::TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> clientAddr,
                                       SPtr<TIPv6Addr> ia ,int msgType,
                                       SPtr<TDUID> ptrDUID)
    :TSrvMsg(iface, clientAddr, RECONFIGURE_MSG, 0 )
{
    ClientDUID=ptrDUID;

    // status code is not necessary in Reconfigure
    //appendStatusCode();

    Options.push_back(new TOptReconfigureMsg(OPTION_RECONF_MSG, RENEW_MSG, this) );

    /// @todo Add necessary options here
    /// see RFC3315, section 15.11

    appendAuthenticationOption(ClientDUID);

    pkt = new char[this->getSize()];
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

string TSrvMsgReconfigure::getName() {
    return "RECONFIGURE";
}
