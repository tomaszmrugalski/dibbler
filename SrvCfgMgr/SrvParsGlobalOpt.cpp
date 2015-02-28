/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvParsGlobalOpt.h"
#include "DHCPDefaults.h"
#include "Portable.h"
#include "Logger.h"

using namespace std;

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void) 
    :Experimental_(false), WorkDir_(DEFAULT_WORKDIR), Stateless_(false),
     InactiveMode_(false), GuessMode_(false), CacheSize_(SERVER_DEFAULT_CACHE_SIZE),
     InterfaceIDOrder_(SRV_IFACE_ID_ORDER_BEFORE)
{
}

TSrvParsGlobalOpt::~TSrvParsGlobalOpt(void) {
}

std::string TSrvParsGlobalOpt::getWorkDir() const {
    return WorkDir_;
}

void TSrvParsGlobalOpt::setWorkDir(const std::string& dir) {
    WorkDir_ = dir;
}

void TSrvParsGlobalOpt::setStateless(bool stateless) {
    Stateless_ = stateless;
}

bool TSrvParsGlobalOpt::getStateless() const {
    return Stateless_;
}

void TSrvParsGlobalOpt::setExperimental(bool exper) {
    Experimental_ = exper;
}

bool TSrvParsGlobalOpt::getExperimental() const {
    return Experimental_;
}

void TSrvParsGlobalOpt::setCacheSize(int bytes) {
    CacheSize_ = bytes;
}

int  TSrvParsGlobalOpt::getCacheSize() const {
    return CacheSize_;
}

void TSrvParsGlobalOpt::setInterfaceIDOrder(ESrvIfaceIdOrder order) {
    InterfaceIDOrder_ = order;
}

ESrvIfaceIdOrder TSrvParsGlobalOpt::getInterfaceIDOrder() const {
    return InterfaceIDOrder_;
}

void TSrvParsGlobalOpt::setInactiveMode(bool flex) {
    InactiveMode_ = flex;
}

bool TSrvParsGlobalOpt::getInactiveMode() const {
    return InactiveMode_;
}

void TSrvParsGlobalOpt::setGuessMode(bool guess) {
    GuessMode_ = guess;
}
bool TSrvParsGlobalOpt::getGuessMode() const {
    return GuessMode_;
}
