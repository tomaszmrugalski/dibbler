/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include <string>
#include "SrvOptStatusCode.h"

TSrvOptStatusCode::TSrvOptStatusCode( char * buf,  int len, TMsg* parent)
	:TOptStatusCode(buf,len, parent) {

}

TSrvOptStatusCode::TSrvOptStatusCode(int status,string message, TMsg* parent)
	:TOptStatusCode(status,message, parent) {
}

bool TSrvOptStatusCode::doDuties() {
    return true;
}
