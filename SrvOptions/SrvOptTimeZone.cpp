#include "SrvOptTimeZone.h"

TSrvOptTimeZone::TSrvOptTimeZone(string domain, TMsg* parent)
    :TOptTimeZone(domain, parent)
{

}

TSrvOptTimeZone::TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptTimeZone(buf,bufsize, parent)
{

}

bool TSrvOptTimeZone::doDuties()
{
    return true;
}
