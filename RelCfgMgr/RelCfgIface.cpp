/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelCfgIface.cpp,v 1.2 2005-02-01 00:57:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

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

TRelCfgIface::~TRelCfgIface() {
}

SmartPtr<TIPv6Addr> TRelCfgIface::getServerUnicast() {
    return this->ServerUnicast;
}

SmartPtr<TIPv6Addr> TRelCfgIface::getClientUnicast() {
    return this->ClientUnicast;
}

bool TRelCfgIface::getClientMulticast() {
    return this->ClientMulticast;
}

bool TRelCfgIface::getServerMulticast() {
    return this->ServerMulticast;
}

void TRelCfgIface::setOptions(SmartPtr<TRelParsGlobalOpt> opt) {
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
    SmartPtr<TIPv6Addr> addr;
    SmartPtr<string> str;

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
