class TMsg;
#ifndef MSG_H
#define MSG_H

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

    //## Odpowiada za reakcj� na otrzymanie wiadomo�ci. W prztpadku zako�czenia
    //## transakcji ustawia pole IsDone na true
    virtual void answer(SmartPtr<TMsg> Rep) = 0;
    
    //## Funkcja odpowiada za transmisj� i retransmisj� danej wiadomo�ci z
    //## uwzglednienirem sta�ych czasowych.
    virtual void doDuties() = 0;

    virtual bool check() = 0;

    //## Zwraca timeout (wykorzystywane po stronie klienta) do okre�lenia
    //## czasu, po kt�rym powinna zosta� wykonana jaka� akcja (retransmisja,
    //## koniec transakcji itp.) wykonywana przez metod� getTimeout/doDuties
    //## Transaction Managera 
    //## po stronie klienta. 
    virtual unsigned long getTimeout();

    virtual void send();

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
    
    void setAttribs(int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID);

    // Is this transaction done?
    bool IsDone;
    char * pkt;
    // it points to server/client interface from/to which message was received/should be sent
    int Iface;

    // it points to server/client address from/to which message was received/should be sent
    SmartPtr<TIPv6Addr> PeerAddr;

};

#endif
