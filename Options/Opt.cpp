#include "Opt.h"


 int TOpt::getOptType()
{
    return OptType;
}

TOpt::TOpt( int optType, TMsg *parent)
{
	OptType=optType;
    Parent=parent;
}

 int TOpt::getSubOptSize()
{
    int size = 0;
    SubOptions.first();
    SmartPtr<TOpt> ptr;
    while (ptr = SubOptions.get()) 
		size += ptr->getSize();
    return size;
}
 char* TOpt::storeSubOpt( char* buf)
{
	SmartPtr<TOpt> ptr;
	SubOptions.first();
    while ( ptr = SubOptions.get() ) 
	{
		ptr->storeSelf(buf);
		buf += ptr->getSize();
    }
	return buf;
}

void TOpt::firstOption()
{
    SubOptions.first();
}

SmartPtr<TOpt> TOpt::getOption()
{
    return SubOptions.get();
}

SmartPtr<TOpt> TOpt::getOption(int optType)
{
    
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
