/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgMgr.h,v 1.5 2004-10-27 22:07:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    
    // --- Iface related ---
    SmartPtr<TClntCfgIA> getIA(long IAID);
    SmartPtr<TClntCfgIface> getIface();
    SmartPtr<TClntCfgIface> getIface(int id);
    void firstIface();
    void addIface(SmartPtr<TClntCfgIface> x);
    int countIfaces();
    void dump();
    
    string getWorkDir();
    bool getReconfigure();
    string getLogName();
    int getLogLevel();
    
    //IA related
    bool setIAState(int iface, int iaid, enum EState state);
    int countAddrForIA(long IAID);
    
    SmartPtr<TClntCfgGroup> getGroupForIA(long IAID);
    SmartPtr<TClntCfgIface> TClntCfgMgr::getIfaceByIAID(int iaid);
    bool isDone();

private:
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    TContainer < SmartPtr<TClntCfgIface> >  ClntCfgIfaceLst;

    bool checkConfigConsistency();
    bool matchParsedSystemInterfaces(clntParser *parser);
};

#endif
