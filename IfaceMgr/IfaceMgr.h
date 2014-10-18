/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TIfaceMgr;
#ifndef IFACEMGR_H
#define IFACEMGR_H

#include "SmartPtr.h"
#include "Container.h"
#include "ScriptParams.h"

#include "Iface.h"

class TMsg;
class TOpt;

class TIfaceMgr {
  public:
    friend std::ostream & operator <<(std::ostream & strum, TIfaceMgr &x);

    TIfaceMgr(const std::string& xmlFile, bool getIfaces);

    // ---Iface related---
    void firstIface();
    SPtr<TIfaceIface> getIface();
    SPtr<TIfaceIface> getIfaceByName(const std::string& name);
    SPtr<TIfaceIface> getIfaceByID(int id);
    virtual SPtr<TIfaceIface> getIfaceBySocket(int fd);
    int countIface();

    // ---other---
    int select(unsigned long time, char *buf, int &bufsize, SPtr<TIPv6Addr> peer,
               SPtr<TIPv6Addr> myaddr);
    std::string printMac(char * mac, int macLen);
    void dump();
    bool isDone();

    virtual void notifyScripts(const std::string& scriptName, SPtr<TMsg> question,
                               SPtr<TMsg> answer);
    virtual void notifyScripts(const std::string& scriptName, SPtr<TMsg> question,
                               SPtr<TMsg> answer, TNotifyScriptParams& params);
    virtual void notifyScript(const std::string& scriptName, std::string action,
                              TNotifyScriptParams& params);

    virtual void closeSockets();

    virtual ~TIfaceMgr();

 protected:
    virtual void optionToEnv(TNotifyScriptParams& params, SPtr<TOpt> opt, std::string txtPrefix );

    std::string XmlFile;
    List(TIfaceIface) IfaceLst; //Interface list
    bool IsDone;
};

#endif
