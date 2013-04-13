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

    // insert authentication option
    Options.push_back(new TOptAuthentication(AUTH_PROTO_RECONFIGURE_KEY, 1,
                                             AUTH_REPLAY_NONE, this));

    SPtr<TAddrClient> cli = SrvAddrMgr().getClient(clientDuid);
    if (cli && cli->ReconfKey_.size() > 0) {
        setAuthKey(cli->ReconfKey_);
        Log(Debug) << "#### Auth: Setting reconfigure-key to " 
                   << hexToText(&cli->ReconfKey_[0], cli->ReconfKey_.size()) << LogEnd;
    }

    SPtr<TOptAuthentication> auth = (Ptr*) getOption(OPTION_AUTH);
    if (auth->getProto() == AUTH_PROTO_RECONFIGURE_KEY) {
        // let's find out what's reconfigure key for this message is
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
