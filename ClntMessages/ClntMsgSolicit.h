#ifndef CLNTMSGSOLICIT_H
#define CLNTMSGSOLICIT_H
#include "ClntMsg.h"
#include "ClntCfgMgr.h"
#include "ClntCfgIA.h"

#include "SmartPtr.h"

class TClntIfaceMgr;

//## Powinien zosta� wywolany, przy:
//## 1. Restarcie systemu
//## 2. Przy zwolnieniu, kt�regos z IA
class TClntMsgSolicit : public TClntMsg
{
public:
    //## 1. Sprawdza i grupuje IA, kt�re nie sa przydzielone.
    //## 2. Sprawdza, czy nie istnieje transakcja dla brakuj�cych IA.
    //## 3. Odpytuje ConfigMgr dla opcji.
    //## 4. Tworzy wiadomo�� z opcjami, a opcje tworz� podopcje.
    TClntMsgSolicit(SmartPtr<TClntIfaceMgr> IfaceMgr,
		    SmartPtr<TClntTransMgr> TransMgr,
		    SmartPtr<TClntCfgMgr>   CfgMgr,
		    SmartPtr<TClntAddrMgr>  AddrMgr,
		    int iface,
		    SmartPtr<TIPv6Addr> addr,
		    TContainer< SmartPtr<TClntCfgIA> > IAs, bool rapid=false);
    
    //## Odbiera wiadomosci TMsgAdvertise i wstawia jest do listy Answers.
    void answer(SmartPtr<TMsg> msg);

    //## Wysyla wiadomosc
    //## Retransmituje wiadomosc
    //## Po zakonczeniu okresu zbierania SOLICITOW sortuje liste wg preferencji
    //## okreslonych w konfigu (+ odrzucanie niepozadanych serwer�w),
    //## wywoluje metode sendRequest TransMgr(lista serwerow) i ustawia pole
    //## IsDone na true.
    void doDuties();
    bool shallRejectAnswer(SmartPtr<TMsg> msg);

    // sort answers
    void sortAnswers();

    bool check();
    ~TClntMsgSolicit();

 private:
    List(TMsg) AnswersLst;

    // method returns max. preference value of received ADVERTISE messages
    int getMaxPreference();
    /*void setAttribs(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		    SmartPtr<TClntTransMgr> TransMgr, 
		    SmartPtr<TClntCfgMgr> CfgMgr,
		    SmartPtr<TClntAddrMgr> AddrMgr,
		    int iface, SmartPtr<TIPv6Addr> addr, int msgType);*/
};
#endif 
