/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
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
    TSrvCfgClientClass(std::string);
    TSrvCfgClientClass(std::string , SPtr<Node>&);
    virtual ~TSrvCfgClientClass();
    std::string getClassName();
    SPtr<Node> getCondition();
    bool isStatisfy(SPtr<TSrvMsg> msg);
private:
    std::string classname;
    SPtr<Node> condition;
};

#endif /* SRVCFGCLIENTCLASS_H_ */
