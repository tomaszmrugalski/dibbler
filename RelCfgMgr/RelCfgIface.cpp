/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <sstream>
#include "DHCPDefaults.h"
#include "RelCfgIface.h"
#include "Logger.h"

using namespace std;

TRelCfgIface::TRelCfgIface(int ifindex)
    :Name_("[unknown]"), ID_(ifindex), InterfaceID_(-1),
     ClientUnicast_(), ServerUnicast_(),
     ClientMulticast_(false), ServerMulticast_(false) {
}

TRelCfgIface::TRelCfgIface(const std::string& ifaceName)
    :Name_(ifaceName), ID_(-1), InterfaceID_(-1),
     ClientUnicast_(), ServerUnicast_(),
     ClientMulticast_(false), ServerMulticast_(false) {
}

int TRelCfgIface::getID() {
    return ID_;
}

string TRelCfgIface::getName() {
    return Name_;
}

string TRelCfgIface::getFullName() {
    ostringstream oss;
    oss << ID_;
    return Name_ + "/" + oss.str();
}

TRelCfgIface::~TRelCfgIface() {
}

SPtr<TIPv6Addr> TRelCfgIface::getServerUnicast() {
    return ServerUnicast_;
}

SPtr<TIPv6Addr> TRelCfgIface::getClientUnicast() {
    return ClientUnicast_;
}

bool TRelCfgIface::getClientMulticast() {
    return ClientMulticast_;
}

bool TRelCfgIface::getServerMulticast() {
    return ServerMulticast_;
}

void TRelCfgIface::setOptions(SPtr<TRelParsGlobalOpt> opt) {
    ClientUnicast_ = opt->getClientUnicast();
    ServerUnicast_ = opt->getServerUnicast();
    ClientMulticast_ = opt->getClientMulticast();
    ServerMulticast_ = opt->getServerMulticast();
    InterfaceID_ = opt->getInterfaceID();
}

void TRelCfgIface::setName(std::string ifaceName) {
    Name_ = ifaceName;
}

void TRelCfgIface::setID(int ifaceID) {
    ID_ = ifaceID;
}

int TRelCfgIface::getInterfaceID() {
    return InterfaceID_;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out, TRelCfgIface& iface) {
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    out << dec;
    out << "  <RelCfgIface name=\"" << iface.Name_ << "\" ifindex=\"" << iface.ID_
        << "\" interfaceID=\"" << iface.InterfaceID_ << "\">" << std::endl;

    if (iface.ClientUnicast_) {
        out << "    <ClientUnicast>" << iface.ClientUnicast_->getPlain()
            << "</ClientUnicast>" << endl;
    } else {
        out << "    <!-- <ClientUnicast/> -->" << endl;
    }
    if (iface.ClientMulticast_) {
        out << "    <ClientMulticast/>" << endl;
    } else {
        out << "    <!-- <ClientMulticast/> -->" << endl;
    }

    if (iface.ServerUnicast_) {
        out << "    <ServerUnicast>" << iface.ServerUnicast_->getPlain()
            << "</ServerUnicast>" << endl;
    } else {
        out << "    <!-- <ServerUnicast/> -->" << endl;
    }
    if (iface.ServerMulticast_) {
        out << "    <ServerMulticast/>" << endl;
    } else {
        out << "    <!-- <ServerMulticast/> -->" << endl;
    }

    out << "  </RelCfgIface>" << endl;
    return out;
}
