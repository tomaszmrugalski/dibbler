/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: NodeOperator.cpp,v 1.2 2008-10-13 22:41:18 thomson Exp $
 *
 */

#include "NodeOperator.h"
#include "SrvMsg.h"
#include "Logger.h"

NodeOperator::NodeOperator()
    :Node(NODE_OPERATOR)
{
}

NodeOperator::~NodeOperator() 
{
}

NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left, SPtr<Node>& right)
    :Node(NODE_OPERATOR)
{
    if (left->Type == NODE_CONST && right->Type == NODE_CONST)
	Log(Warning) << "Both tokens (" << left->exec(0) << " and " << right->exec(0) 
		     << ") used in expression are constant." << LogEnd;

    type = t;
    l = left;
    r = right;
}

NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left,  int in,  int len)
    :Node(NODE_OPERATOR)
{
    type = t;
    l = left;
    index  = in;
    length = len;
}


NodeOperator::NodeOperator(OperatorType t, SPtr<Node>& left,  string s )
    :Node(NODE_OPERATOR)
{
    type = t;
    l = left;
    cotainString = s ;
}

string NodeOperator::exec()
{
    return "";
}

string NodeOperator::exec(SPtr<TSrvMsg> msg)
{
	switch (type)
	{
	case OPERATOR_EQUAL :
		if (l->exec(msg) == r->exec(msg)) return "true";
		return "false";
		break;
	case OPERATOR_AND :
		if ( (l->exec(msg) == "true") && (r->exec(msg) == "true")) return "true";
		return "false";
		break;
	case OPERATOR_OR :
		if ( (l->exec(msg) == "true") || (r->exec(msg) == "true") ) return "true";
		return "false";
		break;

	case OPERATOR_SUBSTRING :
		return (l->exec(msg)).substr(index,length);
		break;

	case OPERATOR_CONTAIN :
		// cout <<" OPERATOR_CONTAIN    OPERATOR_CONTAIN    OPERATOR_CONTAIN : "<< l->exec(msg) <<endl;
   		// cout <<" Left : "<< l->exec(msg) <<endl;
		// cout <<" Right : "<< r->exec(msg) << endl;

		if ((l->exec(msg)).find((r->exec(msg)))!= string::npos )
		{

			return "true";
		}

		else return "false";
			break;
	default :
		return "";

	}
}

