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

TSrvCfgClientClass::TSrvCfgClientClass()
    :classname("")
{
}

TSrvCfgClientClass::TSrvCfgClientClass(string name)
    :classname(name)
{
}

TSrvCfgClientClass::TSrvCfgClientClass(string name , SPtr<Node>& cond)
{
    classname = name;
    condition = cond;
}

TSrvCfgClientClass::~TSrvCfgClientClass() 
{
    
}


string TSrvCfgClientClass::getClassName()
{
    return classname;
}

SPtr<Node> TSrvCfgClientClass::getCondition()
{
    return condition;
}

bool TSrvCfgClientClass::isStatisfy(SPtr<TSrvMsg> msg)
{
    if (condition->exec(msg) == "true") 
        return true ;
    return false;
}
