/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelCfgIface.cpp,v 1.4 2008-08-29 00:07:31 thomson Exp $
 *
 */

#include <sstream>
#include "RelCfgIface.h"
#include "Logger.h"

using namespace std;

/*
 * default contructor
 */

TRelCfgIface::TRelCfgIface(int ifaceNr) {
    this->setDefaults();
    this->ID=ifaceNr;
}

TRelCfgIface::TRelCfgIface(string ifaceName) {
    this->setDefaults();
    this->Name=ifaceName;
}

void TRelCfgIface::setDefaults() {
    this->Name = "[unknown]";
    this->ID = -1;
    this->InterfaceID = -1;
}

int TRelCfgIface::getID() {
	return this->ID;
}

string TRelCfgIface::getName() {
	return this->Name;
}

string TRelCfgIface::getFullName() {
    ostringstream oss;
    oss << this->ID;
    return string(this->Name)
	+"/"
	+oss.str();
}

TRelCfgIface::~TRelCfgIface() {
}

SPtr<TIPv6Addr> TRelCfgIface::getServerUnicast() {
    return this->ServerUnicast;
}

SPtr<TIPv6Addr> TRelCfgIface::getClientUnicast() {
    return this->ClientUnicast;
}

bool TRelCfgIface::getClientMulticast() {
    return this->ClientMulticast;
}

bool TRelCfgIface::getServerMulticast() {
    return this->ServerMulticast;
}

void TRelCfgIface::setOptions(SPtr<TRelParsGlobalOpt> opt) {
    this->ClientUnicast = opt->getClientUnicast();
    this->ServerUnicast = opt->getServerUnicast();
    this->ClientMulticast = opt->getClientMulticast();
    this->ServerMulticast = opt->getServerMulticast();
    this->InterfaceID = opt->getInterfaceID();
}

void TRelCfgIface::setName(string ifaceName) {
	this->Name=ifaceName;
}

void TRelCfgIface::setID(int ifaceID) {
	this->ID=ifaceID;
}

int TRelCfgIface::getInterfaceID() {
    return this->InterfaceID;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream& operator<<(ostream& out,TRelCfgIface& iface) {
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    out << dec;
    out << "  <RelCfgIface name=\""<<iface.Name << "\" ifindex=\""<<iface.ID 
	<< "\" interfaceID=\"" << iface.InterfaceID << "\">" << std::endl;

    if (iface.ClientUnicast) {
        out << "    <ClientUnicast>" << iface.ClientUnicast->getPlain() << "</ClientUnicast>" << endl;
    } else {
        out << "    <!-- <ClientUnicast/> -->" << endl;
    }
    if (iface.ClientMulticast) {
        out << "    <ClientMulticast/>" << endl;
    } else {
        out << "    <!-- <ClientMulticast/> -->" << endl;
    }

    if (iface.ServerUnicast) {
        out << "    <ServerUnicast>" << iface.ServerUnicast->getPlain() << "</ServerUnicast>" << endl;
    } else {
        out << "    <!-- <ServerUnicast/> -->" << endl;
    }
    if (iface.ServerMulticast) {
        out << "    <ServerMulticast/>" << endl;
    } else {
        out << "    <!-- <ServerMulticast/> -->" << endl;
    }
    
    out << "  </RelCfgIface>" << endl;
    return out;
}
