#include <string.h>
#include "long128.h"

ulong128::ulong128()
{
    memset(bytes,0,16);
}

ulong128::ulong128(SmartPtr<TIPv6Addr> addr)
{
    memcpy(this->bytes,addr->getAddr(),16);
}

ulong128 ulong128::operator+(ulong128& other)
{
    ulong128 ret1;
    memcpy(ret1.bytes,this->bytes,16);
    #ifdef WIN32
    __asm{            
        mov     esi,other
        mov     edi,ret1
        mov     ecx,16
    repeat:
        mov     al,[esi]other.bytes
        adc     [edi]ret1.bytes,al
        inc     esi
        inc     edi
        loop repeat
    }
    #endif //WIN32
    #ifdef LINUX
        unsigned int c=0;
        for (int j=0;j<16;j++)
        {
            unsigned int i=c+(unsigned int)other.bytes[j]+
                             (unsigned int)this->bytes[j];
            ret1.bytes[j]=(i&0xFF);
            c=i>>8;
        }
    #endif
    return ret1;
}
