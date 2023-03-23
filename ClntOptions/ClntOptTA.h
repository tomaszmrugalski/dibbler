/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TOptTA;
class TMsg;

#ifndef CLNTIA_TA_H
#define CLNTIA_TA_H

#include "IPv6Addr.h"
#include "Msg.h"
#include "OptIAAddress.h"
#include "OptTA.h"

class TClntOptTA : public TOptTA {
public:
  TClntOptTA(unsigned int iaid, TMsg *parent);
  TClntOptTA(char *buf, int bufsize, TMsg *parent);
  TClntOptTA(SPtr<TAddrIA> ta, TMsg *parent);
  ~TClntOptTA();

  bool doDuties();
  int getStatusCode();

  SPtr<TOptIAAddress> getAddr();
  SPtr<TOptIAAddress> getAddr(SPtr<TIPv6Addr> addr);
  void firstAddr();
  int countAddr();
  bool isValid() const;
  void setIface(int iface); // used to override interface (e.g. when msg is
                            // received via loopback)

  void setContext(int iface, SPtr<TIPv6Addr> clntAddr);

private:
  void releaseAddr(long IAID, SPtr<TIPv6Addr> addr);

  SPtr<TIPv6Addr> Addr;
  int Iface;
};

#endif /* CLNTIA_TA_H */
