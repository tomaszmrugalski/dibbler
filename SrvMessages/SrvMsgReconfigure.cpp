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
#include "OptAuthentication.h"
#include "Logger.h"
#include "hex.h"
#include "Key.h"

TSrvMsgReconfigure::TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> clientAddr,
                                       int msgType, SPtr<TDUID> clientDuid)
    :TSrvMsg(iface, clientAddr, RECONFIGURE_MSG, 0/*trans-id*/)
{

    // include our DUID (server-id)
    Options.push_back(new TOptDUID(OPTION_SERVERID, SrvCfgMgr().getDUID(), this));

    // include his DUID (client-id)
    Options.push_back(new TOptDUID(OPTION_CLIENTID, clientDuid, this));

    // include Reconfigure Message
    Options.push_back(new TOptReconfigureMsg(msgType, this) );


    SPtr<TAddrClient> cli = SrvAddrMgr().getClient(clientDuid);
    if (cli && cli->ReconfKey_.size() > 0) {
        setAuthKey(cli->ReconfKey_);
        Log(Debug) << "Auth: Setting reconfigure-key to "
                   << hexToText(&cli->ReconfKey_[0], cli->ReconfKey_.size()) << LogEnd;

        // insert authentication option
        SPtr<TOptAuthentication> optAuth = new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY, 1,
                                                                  AUTH_REPLAY_NONE, this);
        TKey tmp(17,0);
        tmp[0] = 2; // see RFC3315, section 21.5.1
        optAuth->setPayload(tmp);

        Options.push_back((Ptr*)optAuth);

    } else {
        Log(Warning) << "Auth: No reconfigure-key specified for client. Sending"
                     << " without key, client will likely to ignore this update." << LogEnd;
    }

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
