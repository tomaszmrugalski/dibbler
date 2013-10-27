/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Grzegorz Pluto
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#include <sstream>
#include "ClntMsgReconfigure.h"
#include "OptInteger.h"
#include "OptDUID.h"
#include "OptReconfigureMsg.h"
#include "DHCPConst.h"
#include "Logger.h"

/*
 * creates buffer based on buffer
 */
TClntMsgReconfigure::TClntMsgReconfigure(int iface, SPtr<TIPv6Addr> addr,
                                     char* buf, int buflen)
    :TClntMsg(iface,addr,buf,buflen) {
}

bool TClntMsgReconfigure::check() {
    /// @todo: check if reconfigure-message option is received, if not, drop the message
    SPtr<TOptReconfigureMsg> reconfMsg = (Ptr*) getOption(OPTION_RECONF_MSG);
    if (!reconfMsg) {
        Log(Warning) << "Received Reconfigure message without mandatory reconfigure-message option. Dropped." << LogEnd;
        return false;
    }
    if (TransID != 0) {
        Log(Warning) << "Received Reconfigure message with non-zero transaction-id (" << TransID << "). Dropped" << LogEnd;
        return false;
    }

    return TClntMsg::check(true /* clientID mandatory */, true /* serverID mandatory */ );
}

void TClntMsgReconfigure::doDuties() {
}

TClntMsgReconfigure::~TClntMsgReconfigure() {
}
