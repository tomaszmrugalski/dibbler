/*
 * Dibbler - a portable DHCPv6
 *
 * author : Vinh Nghiem Nguyen
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: Node.cpp,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

/*
 * Node.cpp
 *
 *  Created on: 9 sept. 2008
 */
#ifndef NODE_CPP_
#define NODE_CPP_

#include "Node.h"
#include "SrvMsg.h"

Node::Node(NodeType type)
{
    Type = type;
}

Node::~Node()
{
}

#endif /* NODE_H_ */

