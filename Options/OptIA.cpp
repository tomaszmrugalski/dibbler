/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "OptIA.h"
#include "DHCPConst.h"
#include "OptStatusCode.h"

TOptIA::TOptIA(uint16_t code, uint32_t iaid, uint32_t t1, uint32_t t2, TMsg *parent)
    : TOpt(code, parent), IAID_(iaid), T1_(t1), T2_(t2) {}

TOptIA::TOptIA(uint16_t code, TMsg *parent) : TOpt(code, parent) {}

void TOptIA::setIAID(uint32_t iaid) { IAID_ = iaid; }

unsigned long TOptIA::getIAID() const { return IAID_; }

unsigned long TOptIA::getT1() const { return T1_; }

void TOptIA::setT1(unsigned long t1) { T1_ = t1; }

unsigned long TOptIA::getT2() const { return T2_; }

void TOptIA::setT2(unsigned long t2) { T2_ = t2; }

int TOptIA::getStatusCode() {
  SPtr<TOpt> ptrOpt;
  SubOptions.first();
  while (ptrOpt = SubOptions.get()) {
    if (ptrOpt->getOptType() != OPTION_STATUS_CODE) {
      continue;
    }

    SPtr<TOptStatusCode> ptrStatus = SPtr_cast<TOptStatusCode>(ptrOpt);
    if (ptrStatus) {
      return ptrStatus->getCode();
    }
  }

  // Missing status code indicates success
  return STATUSCODE_SUCCESS;
}
