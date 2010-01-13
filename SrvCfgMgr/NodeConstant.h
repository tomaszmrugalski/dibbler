/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: NodeConstant.h,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

#ifndef NODECONSTANT_H_
#define NODECONSTANT_H_
#include "Node.h"
#include <iostream>
using namespace std;

class NodeConstant : public Node
{
public :
    NodeConstant();
    NodeConstant(string v);
    ~NodeConstant();
    string getStringValue();
    string value;
    virtual string exec();
    virtual string exec(SPtr<TSrvMsg> msg);
};
#endif /* NODECONSTANT_H_ */
