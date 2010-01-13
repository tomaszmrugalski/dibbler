/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: NodeConstant.cpp,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

#ifndef NODECONSTANT_CPP_
#define NODECONSTANT_CPP_

#include "NodeConstant.h"
#include "SrvMsg.h"
#include "SrvMsg.h"

NodeConstant::NodeConstant()
    :Node(NODE_CONST)
{
}

NodeConstant::~NodeConstant()
{
}

NodeConstant::NodeConstant(string v)
    :Node(NODE_CONST)
{
    value = v;
}


string NodeConstant::getStringValue()
{
    return value;
}

string NodeConstant::exec()
{
    return value;
}

string NodeConstant::exec(SPtr<TSrvMsg> msg)
{
    return value;
}
#endif /* NODECONSTANT_CPP_ */
