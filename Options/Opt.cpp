/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: Opt.cpp,v 1.5 2004-05-23 16:40:12 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/05/23 16:38:18  thomson
 * CR-LF issues.
 *
 * Revision 1.3  2004/03/29 19:11:56  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */

#include "Opt.h"
int TOpt::getOptType()
{
    return OptType;
}

TOpt::~TOpt() {

}

TOpt::TOpt( int optType, TMsg *parent){	
    OptType=optType;    
    Parent=parent;
}
int TOpt::getSubOptSize() {
    int size = 0;    
    SubOptions.first();    
    SmartPtr<TOpt> ptr;    
    while (ptr = SubOptions.get()) 		
	size += ptr->getSize();    
    return size;
}
char* TOpt::storeSubOpt( char* buf){
    SmartPtr<TOpt> ptr;	
    SubOptions.first();    
    while ( ptr = SubOptions.get() ) {
	ptr->storeSelf(buf);
	buf += ptr->getSize();    
    }	
    return buf;
}

void TOpt::firstOption() {
    SubOptions.first();
}

SmartPtr<TOpt> TOpt::getOption() {
    return SubOptions.get();
}

SmartPtr<TOpt> TOpt::getOption(int optType) {
    firstOption();
    SmartPtr<TOpt> opt;
    while(opt=getOption())
	if (opt->getOptType()==optType)
	    return opt;
    return SmartPtr<TOpt>();
}

void TOpt::addOption(SmartPtr<TOpt> opt)
{
    SubOptions.append(opt);
}

 int TOpt::countOption()
{
    return SubOptions.count();
}

void TOpt::setParent(TMsg* Parent)
{
    this->Parent=Parent;
}

bool TOpt::isValid()
{   
    //for most of options it's enough
    return true;
}
