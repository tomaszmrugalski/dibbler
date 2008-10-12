/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgClientClass.cpp,v 1.1 2008-10-12 20:54:15 thomson Exp $
 *
 */

#include "SrvCfgClientClass.h"
#include "SrvMsg.h"

TSrvCfgClientClass::TSrvCfgClientClass() {
    // TODO Auto-generated constructor stub
    classname = "";
}

TSrvCfgClientClass::TSrvCfgClientClass(string name) {
    // TODO Auto-generated constructor stub
    classname = name;
}

TSrvCfgClientClass::TSrvCfgClientClass(string name , SmartPtr<Node>& cond){
    classname = name;
    condition = cond;
}

TSrvCfgClientClass::~TSrvCfgClientClass() {
    // TODO Auto-generated destructor stub
}


string TSrvCfgClientClass::getClassName()
{
    return classname;
}

SmartPtr<Node> TSrvCfgClientClass::getCondition()
{
    return condition;
}

bool TSrvCfgClientClass::isStatisfy(SmartPtr<TSrvMsg> msg)
{
    if (condition->exec(msg) == "true") return true ;
    return false;
}
