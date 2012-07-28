/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#ifndef NODECONSTANT_CPP_
#define NODECONSTANT_CPP_

#include "NodeConstant.h"
#include "SrvMsg.h"
#include "SrvMsg.h"

using namespace std;

NodeConstant::NodeConstant()
    :Node(NODE_CONST) {
}

NodeConstant::~NodeConstant() {
}

NodeConstant::NodeConstant(std::string v)
    :Node(NODE_CONST), value(v) {
}


std::string NodeConstant::getStringValue() {
    return value;
}

std::string NodeConstant::exec() {
    return value;
}

string NodeConstant::exec(SPtr<TSrvMsg> msg) {
    return value;
}
#endif /* NODECONSTANT_CPP_ */
