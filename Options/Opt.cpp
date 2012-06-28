/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "Portable.h"
#include "Opt.h"
#include "OptGeneric.h"
#include "OptRtPrefix.h"
#include "Logger.h"

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

char* TOpt::storeHeader(char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf,getSize() - 4);
    return buf;
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


/// @brief Deletes all specified options of that type
///
/// @param type
///
/// @return
bool TOpt::delOption(uint16_t type) {
    firstOption();
    SPtr<TOpt> opt = 0;
    bool del = false;
    while(opt=getOption()) {
        if (opt->getOptType()==type) {
            SubOptions.del();
            SubOptions.first();
            del = true;
        }
    }
    return del;
}

std::string TOpt::getPlain() {
    return "[generic]";
}


/// Parses options or suboptions, creates appropriate objects and store them in options container
///
/// @param options options container (new options will be added here)
/// @param buf buffer to be parsed
/// @param len length of the buffer
/// @param parent pointer to parent message
/// @param placeId specifies location of the message (option number for option parsing or 0 for message parsing)
/// @param place text representation of the parsed scope
///
/// @return true if parsing was successful, false if anomalies are detected
bool TOpt::parseOptions(TContainer< SPtr<TOpt> >& options,
                        const char* buf,
                        size_t len,
                        TMsg* parent,
                        uint16_t placeId /*= 0*/, // 5 (option 5) or (message 5)
                        std::string place /*= "option"*/ // "option" or "message"
                        ) {

    // parse suboptions
    while (len>0) {
        if (len<4) {
            Log(Warning) << "Truncated suboption in " << place << " " << placeId << LogEnd;
            return false;
        }

        uint16_t optType = readUint16(buf);
        buf += sizeof(uint16_t);
        len -= sizeof(uint16_t);
        uint16_t optLen = readUint16(buf);
        buf += sizeof(uint16_t);
        len -= sizeof(uint16_t);

        if (optLen>len) {
            Log(Warning) << "Truncated suboption " << optType << " in " << place << " " << placeId << LogEnd;
            return false;
        }

        switch (optType) {
        case OPTION_RTPREFIX: {
            SPtr<TOpt> opt = new TOptRtPrefix(buf, len, parent);
            options.append(opt);
            break;
        }
        default: {
            SPtr<TOpt> opt = new TOptGeneric(optType, buf, len, parent);
            options.append(opt);
            break;
        }
        }
        buf += optLen;
        len -= optLen;
    }

    return true;
}
