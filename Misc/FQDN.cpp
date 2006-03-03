#include <stdlib.h>
#include <string.h>
#include "FQDN.h"
#include "Portable.h"

TFQDN::TFQDN() {
    this->Duid = new TDUID();
    this->Addr = new TIPv6Addr();
    this->Name = "";
    this->used = false;
}

TFQDN::TFQDN(TDUID *duid_, string name_,bool used_) {
    this->Addr = new TIPv6Addr();
    this->Duid = duid_;
    this->Name = name_;
    this->used = used_;
}

TFQDN::TFQDN(TIPv6Addr *addr_, string name_,bool used_) {
    this->Duid = new TDUID();
    this->Addr = addr_;
    this->Name = name_;
    this->used = used_;
}

TFQDN::TFQDN(string name_,bool used_) {
    this->Duid = new TDUID();
    this->Addr = new TIPv6Addr();
    this->Name = name_;
    this->used = used_;
}

TDUID TFQDN::getDuid() {
    return *this->Duid;
}

TIPv6Addr TFQDN::getAddr() {
    return *this->Addr;
}

string TFQDN::getName() {
    return this->Name;
}

bool TFQDN::isUsed() {
    return this->used;
}
