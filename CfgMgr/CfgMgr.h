class TCfgMgr;
#ifndef CFGMGR_H_HEADER_INCLUDED_C0FD953F
#define CFGMGR_H_HEADER_INCLUDED_C0FD953F
#include <string>
#include <SmartPtr.h>
#include <DUID.h>

using namespace std;
class TCfgMgr
{
public:
	// method compares both files and if differs
	// returns true if files differs and false in the other case
	bool compareConfigs(const string cfgFile, const string oldCfgFile);
	// replaces copy cfgFile to oldCfgFile
	void copyFile(const string cfgFile, const string oldCfgFile);
	// Tries to read duid from duidFile (if file exists and is not empty
	// else it generates new one)
	void setDUID(const string duidFile,char * mac,int macLen, int macType);

	SmartPtr<TDUID>	getDUID();
	//int	getDUIDlen();

protected:
	SmartPtr<TDUID>	DUID;
	//int	DUIDlen;

private:
	//Generates new DUID on basis of time and mac addresses
	void generateDUID(const string duidFile,char * mac,int macLen, int macType);

};
#endif
