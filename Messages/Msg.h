/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

class TMsg;
#ifndef MSG_H
#define MSG_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "SmartPtr.h"
#include "Container.h"
#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "Opt.h"
#include "Key.h"
#include "ScriptParams.h"

// Hey! It's grampa of all messages
class TMsg
{
  public:
    // Used to create TMsg object (normal way)
    TMsg(int iface, SPtr<TIPv6Addr> addr, int msgType);
    TMsg(int iface, SPtr<TIPv6Addr> addr, int msgType, long transID);

    // used to create TMsg object based on received char[] data
    TMsg(int iface, SPtr<TIPv6Addr> addr, char* &buf, int &bufSize);

    virtual int getSize();

    // transmit (or retransmit)

    virtual unsigned long getTimeout();

    virtual int storeSelf(char * buffer);

    virtual std::string getName() const = 0;

    // returns requested option (or NULL, there is no such option)
    SPtr<TOpt> getOption(int type);
    void firstOption();
    int countOption();
    void addOption(SPtr<TOpt> opt) { Options.push_back(opt); }

    virtual SPtr<TOpt> getOption();

    long getType();
    long getTransID();
    TOptList & getOptLst();
    int getIface();
    virtual ~TMsg();
    bool isDone();
    bool isDone(bool done);

    // useful auth stuff below
    void calculateDigests(char* buffer,  size_t len);

    /// @todo: remove from here (and move to AUTH option)
    void setAuthDigestPtr(char* ptr, unsigned len);
    bool loadAuthKey();
    void setAuthKey(const TKey& key);
    TKey getAuthKey();

    bool validateAuthInfo(char *buf, int bufSize, AuthProtocols proto,
                          const DigestTypesLst& acceptedDigestTypes);
    uint32_t getSPI();
    void setSPI(uint32_t val);
    DigestTypes DigestType_;

    // notify scripts stuff
    void* getNotifyScriptParams();

    SPtr<TIPv6Addr> getRemoteAddr();

    void setLocalAddr(SPtr<TIPv6Addr> myaddr);
    SPtr<TIPv6Addr> getLocalAddr();

  protected:
    int MsgType;
    long TransID;

    bool delOption(int code);

    TOptList Options;
    TOptList::iterator NextOpt; // to be removed together with firstOption() and getOption();
    void setAttribs(int iface, SPtr<TIPv6Addr> addr,
                    int msgType, long transID);
    virtual bool check(bool clntIDmandatory, bool srvIDmandatory);

    bool IsDone; // Is this transaction done?
    int Iface;   // logical interface (for direct messages it equals PhysicalIface
                 // for relayed messages Iface points to relayX, PhysicalInterface to ethX)


    /// Address of the corresponding node (received from or to be sent to)
    /// @todo: rename to RemoteAddr_
    SPtr<TIPv6Addr> PeerAddr_;

    /// Address the packet was received on
    SPtr<TIPv6Addr> LocalAddr_;

    // Auth stuff
    uint32_t SPI_; // Key identifier
    char* AuthDigestPtr_;    // Digest (pointer to Authentication Information field of OPTION AUTH)
    unsigned AuthDigestLen_; // Length of the digest
    TKey AuthKey_; // Auth Key

    // a pointer to NotifyScriptParams structure (if defined)
    TNotifyScriptParams* NotifyScripts;
};

typedef std::list< SPtr<TMsg> > TMsgLst;

#endif
