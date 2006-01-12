/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgMgr.h,v 1.9 2006-01-12 00:23:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/02/01 00:57:36  thomson
 * no message
 *
 * Revision 1.7  2004/12/07 20:51:35  thomson
 * Link local safety checks added (bug #39)
 *
 * Revision 1.6  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.5  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.4  2004/05/23 20:41:03  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

class TClntCfgMgr;
#ifndef CLNTCFGMGR_H
#define CLNTCFGMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgGroup.h"
#include "CfgMgr.h"
#include "FlexLexer.h"
#include "ClntParser.h"

class TClntCfgMgr : public TCfgMgr
{
    friend ostream & operator<<(ostream &strum, TClntCfgMgr &x);
 public:
    TClntCfgMgr(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		const string cfgFile,const string oldCfgFile);
    ~TClntCfgMgr();
    
    // --- Iface related ---
    SmartPtr<TClntCfgIA> getIA(long IAID);
    SmartPtr<TClntCfgIface> getIface();
    SmartPtr<TClntCfgIface> getIface(int id);
    void firstIface();
    void addIface(SmartPtr<TClntCfgIface> x);
    int countIfaces();
    void dump();
    
    bool getReconfigure();
    
    //IA related
    bool setIAState(int iface, int iaid, enum EState state);
    int countAddrForIA(long IAID);
    
    SmartPtr<TClntCfgGroup> getGroupForIA(long IAID);
    SmartPtr<TClntCfgIface> getIfaceByIAID(int iaid);
    bool isDone();

private:
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    List(TClntCfgIface) ClntCfgIfaceLst;

    bool validateConfig();
    bool validateIface(SmartPtr<TClntCfgIface> iface);
    bool validateIA(SmartPtr<TClntCfgIface> ptrIface, SmartPtr<TClntCfgIA> ptrIA);
    bool validateAddr(SmartPtr<TClntCfgIface> ptrIface, 
		      SmartPtr<TClntCfgIA> ptrIA,
		      SmartPtr<TClntCfgAddr> ptrAddr);

    bool matchParsedSystemInterfaces(clntParser *parser);
};

#endif
