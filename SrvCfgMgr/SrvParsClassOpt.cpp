/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvParsClassOpt.h"
#include "DHCPDefaults.h"
#include <limits.h>

using namespace std;

TSrvParsClassOpt::TSrvParsClassOpt(void) {
  this->T1Beg = SERVER_DEFAULT_MIN_T1;
  this->T1End = SERVER_DEFAULT_MAX_T1;
  this->T2Beg = SERVER_DEFAULT_MIN_T2;
  this->T2End = SERVER_DEFAULT_MAX_T2;
  this->PrefBeg = SERVER_DEFAULT_MIN_PREF;
  this->PrefEnd = SERVER_DEFAULT_MAX_PREF;
  this->ValidBeg = SERVER_DEFAULT_MIN_VALID;
  this->ValidEnd = SERVER_DEFAULT_MAX_VALID;
  this->Share = SERVER_DEFAULT_CLASS_SHARE;
  this->ClassMaxLease = SERVER_DEFAULT_CLASS_MAX_LEASE;
}

// T1,T2,Valid,Prefered time routines
void TSrvParsClassOpt::setT1Beg(unsigned long t1) { this->T1Beg = t1; }

void TSrvParsClassOpt::setT1End(unsigned long t1) { this->T1End = t1; }

unsigned long TSrvParsClassOpt::getT1Beg() { return T1Beg; }

unsigned long TSrvParsClassOpt::getT1End() { return T1End; }

void TSrvParsClassOpt::setT2Beg(unsigned long t2) { this->T2Beg = t2; }

void TSrvParsClassOpt::setT2End(unsigned long t2) { this->T2End = t2; }

unsigned long TSrvParsClassOpt::getT2Beg() { return this->T2Beg; }

unsigned long TSrvParsClassOpt::getT2End() { return this->T2End; }

void TSrvParsClassOpt::setShare(unsigned long share) { this->Share = share; }

unsigned long TSrvParsClassOpt::getShare() { return this->Share; }

void TSrvParsClassOpt::setPrefBeg(unsigned long pref) { this->PrefBeg = pref; }

void TSrvParsClassOpt::setPrefEnd(unsigned long pref) { this->PrefEnd = pref; }

unsigned long TSrvParsClassOpt::getPrefBeg() { return this->PrefBeg; }

unsigned long TSrvParsClassOpt::getPrefEnd() { return this->PrefEnd; }

void TSrvParsClassOpt::setValidEnd(unsigned long valid) { this->ValidEnd = valid; }

void TSrvParsClassOpt::setValidBeg(unsigned long valid) { this->ValidBeg = valid; }

unsigned long TSrvParsClassOpt::getValidEnd() { return this->ValidEnd; }

unsigned long TSrvParsClassOpt::getValidBeg() { return this->ValidBeg; }

// Rejected clients access routines
void TSrvParsClassOpt::addRejedClnt(SPtr<THostRange> addr) { this->RejedClnt.append(addr); }
void TSrvParsClassOpt::firstRejedClnt() { this->RejedClnt.first(); }
SPtr<THostRange> TSrvParsClassOpt::getRejedClnt() { return this->RejedClnt.get(); }
void TSrvParsClassOpt::setRejedClnt(TContainer<SPtr<THostRange>> *rejedClnt) {
  this->RejedClnt.clear();
  rejedClnt->first();
  SPtr<THostRange> addr;
  while (addr = rejedClnt->get()) this->RejedClnt.append(addr);
}

// Accepted clients access routines
void TSrvParsClassOpt::addAcceptClnt(SPtr<THostRange> addr) { this->AcceptClnt.append(addr); }

void TSrvParsClassOpt::firstAcceptClnt() { this->AcceptClnt.first(); }

SPtr<THostRange> TSrvParsClassOpt::getAcceptClnt() { return this->AcceptClnt.get(); }

void TSrvParsClassOpt::setAcceptClnt(TContainer<SPtr<THostRange>> *acceptClnt) {
  this->AcceptClnt.clear();
  acceptClnt->first();
  SPtr<THostRange> addr;
  while (addr = acceptClnt->get()) this->AcceptClnt.append(addr);
}

// Pool access routines
void TSrvParsClassOpt::addPool(SPtr<THostRange> addr) { this->Pool.append(addr); }
void TSrvParsClassOpt::firstPool() { this->Pool.first(); }
SPtr<THostRange> TSrvParsClassOpt::getPool() { return this->Pool.get(); }

void TSrvParsClassOpt::setPool(TContainer<SPtr<THostRange>> *pool) {
  this->Pool.clear();
  pool->first();
  SPtr<THostRange> addr;
  while (addr = pool->get()) this->Pool.append(addr);
}

void TSrvParsClassOpt::setClassMaxLease(unsigned long classMaxLease) {
  this->ClassMaxLease = classMaxLease;
}

unsigned long TSrvParsClassOpt::getClassMaxLease() { return this->ClassMaxLease; }

TSrvParsClassOpt::~TSrvParsClassOpt(void) {}

long TSrvParsClassOpt::countPool() { return this->Pool.count(); }

void TSrvParsClassOpt::setAddrParams(int prefix, int bitfield) {
  AddrParams = new TSrvOptAddrParams(prefix, bitfield, 0 /* parent */);
}

SPtr<TSrvOptAddrParams> TSrvParsClassOpt::getAddrParams() { return AddrParams; }

void TSrvParsClassOpt::setAllowClientClass(std::string s) {
  allowLst.append(SPtr<string>(new string(s)));
}

void TSrvParsClassOpt::setDenyClientClass(std::string s) {
  denyLst.append(SPtr<string>(new string(s)));
}

List(std::string) TSrvParsClassOpt::getAllowClientClassString() { return allowLst; }

List(std::string) TSrvParsClassOpt::getDenyClientClassString() { return denyLst; }
