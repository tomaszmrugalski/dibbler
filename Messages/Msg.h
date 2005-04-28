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
    
    virtual int getSize();
    
    // trasnmit (or retransmit)

    virtual unsigned long getTimeout();

    virtual int storeSelf(char * buffer);

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
    
    bool IsDone; // Is this transaction done?
    char * pkt;  // buffer where this packet will be build
    int Iface;   // interface from/to which message was received/should be sent
    SmartPtr<TIPv6Addr> PeerAddr; // server/client address from/to which message was received/should be sent
};

#endif
