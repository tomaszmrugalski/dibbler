/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTOPTIA_PD_H
#define CLNTOPTIA_PD_H

#include "ClntIfaceMgr.h"
#include "IPv6Addr.h"
#include "OptIAPrefix.h"
#include "OptIA_PD.h"

class TOptIA_PD;

class TClntOptIA_PD : public TOptIA_PD {
public:
  TClntOptIA_PD(SPtr<TClntCfgPD> ClntCfgPD, TMsg *parent);
  TClntOptIA_PD(SPtr<TAddrIA> clntAddrPD, TMsg *parent);
  TClntOptIA_PD(char *buf, int bufsize, TMsg *parent);
  ~TClntOptIA_PD();

  bool doDuties();
  int getStatusCode();
  void setContext(SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr,
                  TMsg *originalMsg);
  void setIface(int iface);

  SPtr<TOptIAPrefix> getPrefix();
  void deletePrefix(SPtr<TOptIAPrefix> prefix);
  SPtr<TOptIAPrefix> getPrefix(SPtr<TIPv6Addr> prefix);
  void firstPrefix();
  int countPrefix() const;
  bool isValid() const;

  bool addPrefixes();
  bool updatePrefixes();
  bool delPrefixes();

private:
  bool modifyPrefixes(TClntIfaceMgr::PrefixModifyMode mode);
  void setState(EState state);
  void clearContext();

  SPtr<TIPv6Addr> Prefix;
  bool Unicast;
  SPtr<TDUID> DUID;
  int Iface;

  TMsg *OriginalMsg;
};

#endif /* CLNTOPTIA_PD_H */
