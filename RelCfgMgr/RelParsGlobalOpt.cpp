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

using namespace std;

TRelParsGlobalOpt::TRelParsGlobalOpt(void) 
    : WorkDir_(DEFAULT_WORKDIR), GuessMode_(false),
      InterfaceIDOrder_(REL_IFACE_ID_ORDER_BEFORE) {
}

TRelParsGlobalOpt::~TRelParsGlobalOpt(void) {
}

string TRelParsGlobalOpt::getWorkDir() {
    return WorkDir_;
}

void TRelParsGlobalOpt::setWorkDir(std::string dir) {
    WorkDir_ = dir;
}


void TRelParsGlobalOpt::setGuessMode(bool guess) {
    GuessMode_ = guess;
}

bool TRelParsGlobalOpt::getGuessMode() {
    return GuessMode_;
}

void TRelParsGlobalOpt::setInterfaceIDOrder(ERelIfaceIdOrder order) {
    InterfaceIDOrder_ = order;
}

ERelIfaceIdOrder TRelParsGlobalOpt::getInterfaceIDOrder() {
    return InterfaceIDOrder_;
}

void TRelParsGlobalOpt::setRemoteID(SPtr<TOptVendorData> remoteID)
{
    RemoteID_ = remoteID;
}

SPtr<TOptVendorData> TRelParsGlobalOpt::getRemoteID()
{
    return RemoteID_;
}

void TRelParsGlobalOpt::setEcho(SPtr<TRelOptEcho> echo)
{
    Echo_ = echo;
}

SPtr<TRelOptEcho> TRelParsGlobalOpt::getEcho()
{
    return Echo_;
}
