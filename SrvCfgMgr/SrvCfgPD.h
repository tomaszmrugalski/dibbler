/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgPD.h,v 1.6 2008-10-12 20:07:31 thomson Exp $
 *
 */

/*
  Generally prefixes can be divided into 3 parts:
  - constant prefix (a)
  - variable section (b)
  - zeroed tail (c)

       (a)            (b)          (c)
  aaaa:aaaa:aaaa:bbbb:bbbb:bbbb:0000:0000

  When there are several prefix pools defined,
  (a) becomes pool-specific prefix
  (b) becomes common part
  (c) stays zeroed tail
*/

class TSrvCfgPD;
#ifndef SRVCONFPD_H
#define SRVCONFPD_H

#include <iomanip>
#include <iostream>
#include <string>

#include "DHCPConst.h"
#include "DUID.h"
#include "IPv6Addr.h"
#include "Node.h"
#include "SmartPtr.h"
#include "SrvAddrMgr.h"
#include "SrvCfgPD.h"
#include "SrvParsGlobalOpt.h"

class TSrvCfgClientClass;

class TSrvCfgPD {
  friend std::ostream &operator<<(std::ostream &out, TSrvCfgPD &iface);

public:
  TSrvCfgPD();

  // Is client with this DUID and IP address supported?
  bool clntSupported(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);
  bool clntSupported(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr,
                     SPtr<TSrvMsg> msg);
  // Is client with this DUID and IP address prefered? (is in accept-only?)
  bool clntPrefered(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr);

  // checks if the prefix belongs to the pool
  bool prefixInPool(SPtr<TIPv6Addr> prefix);
  unsigned long countPrefixesInPool();
  SPtr<TIPv6Addr> getRandomPrefix();
  List(TIPv6Addr) getRandomList();

  unsigned long getT1(unsigned long hintT1);
  unsigned long getT2(unsigned long hintT2);
  unsigned long getPrefered(unsigned long hintPrefered);
  unsigned long getValid(unsigned long hintValid);

  unsigned long getPD_Length(); // length of prefix
  unsigned long getPD_MaxLease();
  unsigned long getID();

  bool isLinkLocal();

  unsigned long getAssignedCount();
  unsigned long getTotalCount();
  long incrAssigned(int count = 1);
  long decrAssigned(int count = 1);

  bool setOptions(SPtr<TSrvParsGlobalOpt> opt, int PDPrefix);
  virtual ~TSrvCfgPD();
  void mapAllowDenyList(List(TSrvCfgClientClass) clientClassLst);

private:
  unsigned long PD_T1Beg_;
  unsigned long PD_T1End_;
  unsigned long PD_T2Beg_;
  unsigned long PD_T2End_;
  unsigned long PD_Length_; // (shorter) prefix, assigned to the user, e.g. 64
  unsigned long PD_PrefBeg_;
  unsigned long PD_PrefEnd_;
  unsigned long PD_ValidBeg_;
  unsigned long PD_ValidEnd_;

  unsigned long chooseTime(unsigned long beg, unsigned long end,
                           unsigned long clntTime);

  unsigned long ID_;
  static unsigned long StaticID_;

  List(THostRange) PoolLst_;
  SPtr<THostRange> CommonPool_; /* common part of all available prefix pools
                                   (section b in the description above) */
  unsigned long PD_MaxLease_;
  unsigned long PD_Assigned_;
  unsigned long PD_Count_;

  List(std::string) AllowLst_;
  List(std::string) DenyLst_;

  List(TSrvCfgClientClass) AllowClientClassLst_;
  List(TSrvCfgClientClass) DenyClientClassLst_;
};

#endif
