/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef TSRVPARSGLOBALOPT_H_
#define TSRVPARSGLOBALOPT_H_
#include "SrvParsIfaceOpt.h"

class TSrvParsGlobalOpt : public TSrvParsIfaceOpt
{
public:
	TSrvParsGlobalOpt(void);
	~TSrvParsGlobalOpt(void);

    int	getLogLevel();
	string getLogName();
	string getWorkDir();

    void setLogLevel(int logLevel);
	void setLogName(string logName);
	void setWorkDir(string dir);

private:
	int				LogLevel;
	string			LogName;
	string			WorkDir;
};
#endif
