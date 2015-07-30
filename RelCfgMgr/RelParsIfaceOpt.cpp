/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "DHCPDefaults.h"
#include "RelParsIfaceOpt.h"

TRelParsIfaceOpt::TRelParsIfaceOpt(void)
    :ClientUnicast_(), ServerUnicast_(),
     ClientMulticast_(false), ServerMulticast_(false),
     InterfaceID_(-1) {
}

TRelParsIfaceOpt::~TRelParsIfaceOpt(void) {
}

// --- unicast ---
void TRelParsIfaceOpt::setServerUnicast(SPtr<TIPv6Addr> addr) {
    ServerUnicast_ = addr;
}

SPtr<TIPv6Addr> TRelParsIfaceOpt::getServerUnicast() {
    return ServerUnicast_;
}

void TRelParsIfaceOpt::setClientUnicast(SPtr<TIPv6Addr> addr) {
    ClientUnicast_ = addr;
}

SPtr<TIPv6Addr> TRelParsIfaceOpt::getClientUnicast() {
    return ClientUnicast_;
}


void TRelParsIfaceOpt::setClientMulticast(bool multi) {
    ClientMulticast_ = multi;
}

bool TRelParsIfaceOpt::getClientMulticast() {
    return ClientMulticast_;
}

void TRelParsIfaceOpt::setServerMulticast(bool multi) {
    ServerMulticast_ = multi;
}

bool TRelParsIfaceOpt::getServerMulticast() {
    return ServerMulticast_;
}

void TRelParsIfaceOpt::setInterfaceID(int id) {
    InterfaceID_ = id;
}

int TRelParsIfaceOpt::getInterfaceID() {
    return InterfaceID_;
}
