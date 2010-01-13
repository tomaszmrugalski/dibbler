/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptTimeZone.cpp,v 1.8 2008-11-13 22:40:26 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2007-01-07 20:18:46  thomson
 * State enum names changed.
 *
 * Revision 1.6  2004-11-01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.5  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.4  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "ClntOptTimeZone.h"
#include "ClntOptServerIdentifier.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptTimeZone::TClntOptTimeZone(string domain, TMsg* parent)
    :TOptString(OPTION_NEW_TZDB_TIMEZONE, domain, parent) {

}

TClntOptTimeZone::TClntOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_NEW_TZDB_TIMEZONE, buf,bufsize, parent) {
    /// @todo: do some validity check

}

bool TClntOptTimeZone::isValid() {
    /// @todo: check is somehow
    return true;
}

bool TClntOptTimeZone::doDuties() {
    string reason = "trying to set timezone.";
    int ifindex = this->Parent->getIface();
    SPtr<TIPv6Addr> addr = this->Parent->getAddr();
    TClntMsg * msg = (TClntMsg*)(this->Parent);
    SPtr<TClntIfaceMgr> ifaceMgr = msg->getClntIfaceMgr();
    if (!ifaceMgr) {
	Log(Error) << "Unable to access IfaceMgr while " << reason << LogEnd;
        return false;
    }
    SPtr<TClntIfaceIface> iface = (Ptr*)ifaceMgr->getIfaceByID(ifindex);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << ifindex 
		   << " while " << reason << LogEnd;
        return false;
    }

    if (!this->DUID) {
	Log(Error) << "Unable to find proper DUID while " << reason << LogEnd;
	return false;
    }

    SPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();
    SPtr<TClntCfgIface> cfgIface = cfgMgr->getIface(ifindex);
    cfgIface->setTimezoneState(STATE_CONFIGURED);

    return iface->setTimezone(this->DUID, addr,this->Str);
}

void TClntOptTimeZone::setSrvDuid(SPtr<TDUID> duid)

{
    this->SrvDUID=duid;
}

