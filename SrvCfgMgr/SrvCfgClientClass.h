/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgClientClass.h,v 1.1 2008-10-12 20:54:15 thomson Exp $
 *
 */

#ifndef SRVCFGCLIENTCLASS_H_
#define SRVCFGCLIENTCLASS_H_
using namespace std;

#include <string>
#include "SmartPtr.h"
#include "Node.h"

class TSrvCfgClientClass {
public:
    TSrvCfgClientClass();
    TSrvCfgClientClass(string);
    TSrvCfgClientClass(string , SmartPtr<Node>&);
    virtual ~TSrvCfgClientClass();
    string getClassName();
    SmartPtr<Node> getCondition();
    bool isStatisfy(SmartPtr<TSrvMsg> msg);
private:
    string classname;
    SmartPtr<Node> condition;
};

#endif /* SRVCFGCLIENTCLASS_H_ */
