#ifndef RELOPTDUID_H
#define RELOPTDUID_H
#include <OptDUID.h>
#include <SmartPtr.h>

class TRelOptDUID : public TOptDUID
{
 public:
    TRelOptDUID(int type, char* buf, int bufsize, TMsg* parent);
    bool doDuties();

};

#endif // RELOPTDUID_H
