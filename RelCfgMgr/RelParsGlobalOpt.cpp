/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "RelParsGlobalOpt.h"
#include "Portable.h"

TRelParsGlobalOpt::TRelParsGlobalOpt(void) {
    WorkDir   = WORKDIR;
    GuessMode = false;
    InterfaceIDOrder = REL_IFACE_ID_ORDER_BEFORE;
}

TRelParsGlobalOpt::~TRelParsGlobalOpt(void) {
}

string TRelParsGlobalOpt::getWorkDir() {
    return this->WorkDir;
}

void TRelParsGlobalOpt::setWorkDir(string dir) {
    this->WorkDir=dir;
}


void TRelParsGlobalOpt::setGuessMode(bool guess) {
    GuessMode = guess;
}

bool TRelParsGlobalOpt::getGuessMode() {
    return GuessMode;
}

void TRelParsGlobalOpt::setInterfaceIDOrder(ERelIfaceIdOrder order) {
    InterfaceIDOrder = order;
}

ERelIfaceIdOrder TRelParsGlobalOpt::getInterfaceIDOrder() {
    return InterfaceIDOrder;
}

void TRelParsGlobalOpt::setRemoteID(SPtr<TOptVendorData> remoteID)
{
    RemoteID = remoteID;
}

SPtr<TOptVendorData> TRelParsGlobalOpt::getRemoteID()
{
    return RemoteID;
}

void TRelParsGlobalOpt::setEcho(SPtr<TRelOptEcho> echo)
{
    Echo = echo;
}

SPtr<TRelOptEcho> TRelParsGlobalOpt::getEcho()
{
    return Echo;
}
