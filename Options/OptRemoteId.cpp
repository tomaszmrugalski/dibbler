#include <string.h>
#include "Portable.h"
#include "OptRemoteId.h"
#include "DHCPConst.h"
#include "Logger.h"


TOptRemoteID::TOptRemoteID(int type, char * buf,  int n, TMsg* parent)
    :TOpt(type, parent) {
}

TOptRemoteID::TOptRemoteID(int type, int enterprise, char *data, int dataLen, TMsg* parent)
    :TOpt(type, parent) {
}

size_t TOptRemoteID::getSize() {
    return 4 + 4 + RemoteId_.size();
}

char * TOptRemoteID::storeSelf( char* buf) {

    // option-code OPTION_VENDOR_OPTS (2 bytes long)
    buf = writeUint16(buf,OptType);

    // option-len size of total option-data
    buf = writeUint16(buf, getSize() - 4);

    // enterprise-number (4 bytes long)
    buf = writeUint32(buf, EnterpriseId_);

    if (!RemoteId_.empty()) {
        buf = writeData(buf, (char*)&RemoteId_[0], RemoteId_.size());
    }

    return buf;
}

std::vector<uint8_t> TOptRemoteID::getRemoteId() {
    return RemoteId_;
}

std::string TOptRemoteID::getPlain() {
    return Plain_;
}

TOptRemoteID::~TOptRemoteID() {
}

bool TOptRemoteID::doDuties() {
    return true;
}
