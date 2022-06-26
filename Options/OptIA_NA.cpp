/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "OptIA_NA.h"
#include "DHCPConst.h"
#include "OptIAAddress.h"
#include "OptStatusCode.h"
#include "Portable.h"

TOptIA_NA::TOptIA_NA(long iaid, long t1, long t2, TMsg *parent)
    : TOptIA(OPTION_IA_NA, iaid, t1, t2, parent) {
  Valid_ = true;
}

TOptIA_NA::TOptIA_NA(char *&buf, int &bufsize, TMsg *parent) : TOptIA(OPTION_IA_NA, parent) {
  Valid_ = false;

  if (bufsize < 12) {
    buf += bufsize;
    bufsize = 0;
  } else {
    IAID_ = readUint32(buf);
    buf += sizeof(uint32_t);
    bufsize -= sizeof(uint32_t);

    T1_ = readUint32(buf);
    buf += sizeof(uint32_t);
    bufsize -= sizeof(uint32_t);

    T2_ = readUint32(buf);
    buf += sizeof(uint32_t);
    bufsize -= sizeof(uint32_t);
    Valid_ = true;
  }
}

size_t TOptIA_NA::getSize() {
  int mySize = 16;
  return mySize + getSubOptSize();
}

char *TOptIA_NA::storeSelf(char *buf) {
  buf = writeUint16(buf, OptType);
  buf = writeUint16(buf, getSize() - 4);

  buf = writeUint32(buf, IAID_);
  buf = writeUint32(buf, T1_);
  buf = writeUint32(buf, T2_);

  buf = storeSubOpt(buf);
  return buf;
}

unsigned long TOptIA_NA::getMaxValid() {
  unsigned long maxValid = 0;
  SPtr<TOpt> ptrOpt;
  SubOptions.first();
  while (ptrOpt = SubOptions.get()) {
    if (ptrOpt->getOptType() == OPTION_IAADDR) {
      SPtr<TOptIAAddress> ptrIAAddr = SPtr_cast<TOptIAAddress>(ptrOpt);
      if (ptrIAAddr && (maxValid < ptrIAAddr->getValid())) {
        maxValid = ptrIAAddr->getValid();
      }
    }
  }
  return maxValid;
}

bool TOptIA_NA::isValid() const { return this->Valid; }

/*
 * How many addresses is stored in this IA
 */
int TOptIA_NA::countAddrs() {
  int cnt = 0;
  SPtr<TOpt> opt;
  this->firstOption();
  while (opt = this->getOption()) {
    if (opt->getOptType() == OPTION_IAADDR) cnt++;
  }
  return cnt;
}
