/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */
#include <string>
using namespace std;

#include "SrvCfgClientClass.h"
#include "SrvMsg.h"

TSrvCfgClientClass::TSrvCfgClientClass()
    :classname("")
{
}

TSrvCfgClientClass::TSrvCfgClientClass(std::string name)
    :classname(name)
{
}

TSrvCfgClientClass::TSrvCfgClientClass(std::string name , SPtr<Node>& cond)
    :classname(name), condition(cond) {
}

TSrvCfgClientClass::~TSrvCfgClientClass() {
    
}

std::string TSrvCfgClientClass::getClassName() {
    return classname;
}

SPtr<Node> TSrvCfgClientClass::getCondition() {
    return condition;
}

bool TSrvCfgClientClass::isStatisfy(SPtr<TSrvMsg> msg) {
    if (condition->exec(msg) == "true") 
        return true;
    return false;
}
