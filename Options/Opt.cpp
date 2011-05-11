/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "Opt.h"

int TOpt::getOptType() {
    return OptType;
}

TOpt::~TOpt() {

}

TOpt::TOpt( int optType, TMsg *parent)
    :Valid(true) {
    OptType=optType;
    Parent=parent;
}

int TOpt::getSubOptSize() {
    int size = 0;
    SubOptions.first();
    SPtr<TOpt> ptr;
    while (ptr = SubOptions.get())
	size += ptr->getSize();
    return size;
}

char* TOpt::storeSubOpt( char* buf){
    SPtr<TOpt> ptr;
    SubOptions.first();
    while ( ptr = SubOptions.get() ) {
	ptr->storeSelf(buf);
	buf += ptr->getSize();
    }
    return buf;
}

void TOpt::firstOption() {
    SubOptions.first();
}

SPtr<TOpt> TOpt::getOption() {
    return SubOptions.get();
}

SPtr<TOpt> TOpt::getOption(int optType) {
    firstOption();
    SPtr<TOpt> opt = 0;
    while(opt=getOption()) {
	if (opt->getOptType()==optType)
	    return opt;
    }
    return 0;
}

void TOpt::addOption(SPtr<TOpt> opt) {
    SubOptions.append(opt);
}

int TOpt::countOption() {
    return SubOptions.count();
}

void TOpt::setParent(TMsg* Parent) {
    this->Parent=Parent;
}

void TOpt::delAllOptions() {
    SubOptions.clear();
}

bool TOpt::isValid() {
    return Valid;
}

SPtr<TDUID> TOpt::getDUID() {
    return this->DUID;
}
void TOpt::setDUID(SPtr<TDUID> duid) {
    this->DUID = duid;
}

void TOpt::delOption() {
  SubOptions.del();
}

std::string TOpt::getPlain() {
    return "[generic]";
}
