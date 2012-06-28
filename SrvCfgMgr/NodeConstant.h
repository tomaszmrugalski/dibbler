/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#ifndef NODECONSTANT_H_
#define NODECONSTANT_H_
#include "Node.h"
#include <iostream>

class NodeConstant : public Node
{
public :
    NodeConstant();
    NodeConstant(std::string v);
    ~NodeConstant();
    std::string getStringValue();
    std::string value;
    virtual std::string exec();
    virtual std::string exec(SPtr<TSrvMsg> msg);
};
#endif /* NODECONSTANT_H_ */
