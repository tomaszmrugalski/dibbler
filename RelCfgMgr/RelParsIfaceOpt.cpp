/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelParsIfaceOpt.cpp,v 1.3 2008-08-29 00:07:32 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2007-05-01 12:03:13  thomson
 * Support for interface-id location added.
 *
 * Revision 1.1  2005-01-11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

#include "RelParsIfaceOpt.h"

TRelParsIfaceOpt::TRelParsIfaceOpt(void) {
    this->ServerUnicast = 0; // NULL
    this->ClientUnicast = 0;
    this->ServerMulticast = false;
    this->ClientMulticast = false;
    this->InterfaceID = -1;
}

TRelParsIfaceOpt::~TRelParsIfaceOpt(void) {
}

// --- unicast ---
void TRelParsIfaceOpt::setServerUnicast(SPtr<TIPv6Addr> addr) {
    this->ServerUnicast = addr;
}

SPtr<TIPv6Addr> TRelParsIfaceOpt::getServerUnicast() {
    return this->ServerUnicast;
}

void TRelParsIfaceOpt::setClientUnicast(SPtr<TIPv6Addr> addr) {
    this->ClientUnicast = addr;
}

SPtr<TIPv6Addr> TRelParsIfaceOpt::getClientUnicast() {
    return this->ClientUnicast;
}


void TRelParsIfaceOpt::setClientMulticast(bool multi) {
    this->ClientMulticast = multi;
}

bool TRelParsIfaceOpt::getClientMulticast() {
    return this->ClientMulticast;
}

void TRelParsIfaceOpt::setServerMulticast(bool multi) {
    this->ServerMulticast = multi;
}

bool TRelParsIfaceOpt::getServerMulticast() {
    return this->ServerMulticast;
}

void TRelParsIfaceOpt::setInterfaceID(int id) {
    this->InterfaceID= id;
}

int TRelParsIfaceOpt::getInterfaceID() {
    return this->InterfaceID;
}

