class TMsg;
#ifndef MSG_H
#define MSG_H

#include <iostream>
#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "DHCPConst.h"
#include "IPv6Addr.h"
#include "Opt.h"

// Hey! It's grampa of all messages
class TMsg
{
  public:
    // Used to create TMsg object (normal way)
    TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType);
    TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID);

    // used to create TMsg object based on received char[] data
    TMsg(int iface, SmartPtr<TIPv6Addr> addr, char* &buf, int &bufSize);
    
    int getSize();

    //answer for a specific message
    virtual void answer(SmartPtr<TMsg> Rep) = 0;
    
    // trasnmit (or retransmit)
    virtual void doDuties() = 0;

    virtual bool check() = 0;

    virtual unsigned long getTimeout();

    virtual void send();

    virtual string getName() = 0;

    // returns requested option (or NULL, there is no such option)
    SmartPtr<TOpt> getOption(int type);
    void firstOption();
    int countOption();

    virtual SmartPtr<TOpt> getOption();
    
    long getType();
    long getTransID();
    TContainer< SmartPtr<TOpt> > getOptLst();
    SmartPtr<TIPv6Addr> getAddr();
    int getIface();
    virtual ~TMsg();
    bool isDone();

  protected:
    int MsgType;

    long TransID;

    List(TOpt) Options;
    
    void setAttribs(int iface, SmartPtr<TIPv6Addr> addr, 
		    int msgType, long transID);

    // Is this transaction done?
    bool IsDone;
    char * pkt;
    // interface from/to which message was received/should be sent
    int Iface;
    // server/client address from/to which message was received/should be sent
    SmartPtr<TIPv6Addr> PeerAddr;

};

#endif
