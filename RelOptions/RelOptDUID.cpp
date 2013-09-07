
#include "SmartPtr.h"
#include "RelOptDUID.h"



TRelOptDUID::TRelOptDUID(int type, char* buf, int bufsize, TMsg* parent)
    :TOptDUID(type, buf, bufsize, parent) {

}

bool TRelOptDUID::doDuties()
{
}
