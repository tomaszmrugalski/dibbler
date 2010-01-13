/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: Node.h,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

#ifndef NODE_H_
#define NODE_H_
#include <iostream>
#include "SmartPtr.h"

using namespace std;
class TSrvMsg;
class Node
{
public:
    enum NodeType
    {
	NODE_OPERATOR        = 1,
	NODE_CONST           = 2,
	NODE_CLIENT_SPECIFIC = 3
    };

    Node(NodeType type);
    virtual ~Node();
    virtual string exec(SPtr<TSrvMsg> msg) = 0;

    NodeType Type;
};

#endif /* NODE_H_ */
