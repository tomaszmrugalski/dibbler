/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgClientClass.h,v 1.2 2008-10-13 22:41:18 thomson Exp $
 *
 */

#ifndef SRVCFGCLIENTCLASS_H_
#define SRVCFGCLIENTCLASS_H_

#include <string>
#include "SmartPtr.h"
#include "Node.h"

class TSrvCfgClientClass {
public:
    TSrvCfgClientClass();
    TSrvCfgClientClass(string);
    TSrvCfgClientClass(string , SmartPtr<Node>&);
    virtual ~TSrvCfgClientClass();
    std::string getClassName();
    SmartPtr<Node> getCondition();
    bool isStatisfy(SmartPtr<TSrvMsg> msg);
private:
  std::string classname;
    SmartPtr<Node> condition;
};

#endif /* SRVCFGCLIENTCLASS_H_ */
