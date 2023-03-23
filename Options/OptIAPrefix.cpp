/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "OptIAPrefix.h"
#include "DHCPConst.h"
#include "Portable.h"
#include <string.h>

TOptIAPrefix::TOptIAPrefix(const char *buf, size_t len, TMsg *parent)
    : TOpt(OPTION_IAPREFIX, parent), Valid_(false) {
  if (len >= 25) {
    PrefLifetime_ = readUint32(buf);
    buf += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    ValidLifetime_ = readUint32(buf);
    buf += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    PrefixLength_ = *buf;
    buf += 1;
    len -= 1;

    Prefix_ = new TIPv6Addr(buf);
    buf += 16;
    len -= 16;

    Valid_ = parseOptions(SubOptions, buf, len, parent, OPTION_IAPREFIX, "IAPrefix option");
  }
}

TOptIAPrefix::TOptIAPrefix(SPtr<TIPv6Addr> prefix, char len, unsigned long pref,
                           unsigned long valid, TMsg *parent)
    : TOpt(OPTION_IAPREFIX, parent),
      Prefix_(prefix),
      PrefLifetime_(pref),
      ValidLifetime_(valid),
      PrefixLength_(len),
      Valid_(true) {
  // we are not checking is prefix is a proper address type,
}

size_t TOptIAPrefix::getSize() { return 29 + getSubOptSize(); }

void TOptIAPrefix::setPref(unsigned long pref) { PrefLifetime_ = pref; }

void TOptIAPrefix::setValid(unsigned long valid) { ValidLifetime_ = valid; }
void TOptIAPrefix::setPrefixLenght(char prefix_length) { PrefixLength_ = prefix_length; }

char *TOptIAPrefix::storeSelf(char *buf) {
  buf = writeUint16(buf, OptType);
  buf = writeUint16(buf, getSize() - 4);

  buf = writeUint32(buf, PrefLifetime_);
  buf = writeUint32(buf, ValidLifetime_);

  *(char *)buf = PrefixLength_;
  buf += 1;
  memcpy(buf, Prefix_->getAddr(), 16);
  buf += 16;

  buf = storeSubOpt(buf);
  return buf;
}

SPtr<TIPv6Addr> TOptIAPrefix::getPrefix() const { return Prefix_; }

unsigned long TOptIAPrefix::getPref() const { return PrefLifetime_; }

unsigned long TOptIAPrefix::getValid() const { return ValidLifetime_; }

uint8_t TOptIAPrefix::getPrefixLength() const { return PrefixLength_; }

bool TOptIAPrefix::isValid() const {
  if (!Valid_ || (getValid() == 0)) {
    return false;
  }

  return this->getValid() >= this->getPref();
}
