class TClntCfgMgr;
#ifndef CLNTCFGMGR_H_HEADER_INCLUDED_C0FD953F
#define CLNTCFGMGR_H_HEADER_INCLUDED_C0FD953F

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "ClntCfgGroup.h"
#include "CfgMgr.h"


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

	string WorkDir;
	string LogName;
	int LogLevel;

	bool IsDone;
    bool checkConfigConsistency();
};

#endif /* CLNTCFGMGR_H_HEADER_INCLUDED_C0FD953F */
