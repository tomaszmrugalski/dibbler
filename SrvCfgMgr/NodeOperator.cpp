/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#include "NodeOperator.h"
#include "SrvMsg.h"
#include "Logger.h"

using namespace std;

NodeOperator::~NodeOperator()
{
}

NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left, SPtr<Node>& right)
    :Node(NODE_OPERATOR), Type_(t), L_(left), R_(right)
{
    if (left->Type == NODE_CONST && right->Type == NODE_CONST)
        Log(Warning) << "Both tokens (" << left->exec(SPtr<TSrvMsg>())
                     << " and " << right->exec(SPtr<TSrvMsg>())
                     << ") used in expression are constant." << LogEnd;
}

NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left, int in, int len)
    :Node(NODE_OPERATOR), Type_(t), L_(left), Index_(in), Length_(len)
{
}


NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left, std::string s)
    :Node(NODE_OPERATOR), Type_(t), L_(left), ContainString_(s)
{
}

string NodeOperator::exec()
{
    return "";
}

string NodeOperator::exec(SPtr<TSrvMsg> msg)
{
    switch (Type_)
    {
    case OPERATOR_EQUAL :
        if (L_->exec(msg) == R_->exec(msg))
            return "true";
        else
            return "false";
        break;
    case OPERATOR_AND :
        if ( (L_->exec(msg) == "true") && (R_->exec(msg) == "true"))
            return "true";
        else
            return "false";
        break;

    case OPERATOR_OR :
        if ( (L_->exec(msg) == "true") || (R_->exec(msg) == "true") )
            return "true";
        else
            return "false";
        break;

    case OPERATOR_SUBSTRING :
        return (L_->exec(msg)).substr(Index_, Length_);
        break;

    case OPERATOR_CONTAIN :
        if ((L_->exec(msg)).find((R_->exec(msg)))!= string::npos )
            return "true";
        else
            return "false";
        break;
    default :
        return "";
    }
}
