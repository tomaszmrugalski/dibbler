#include "Portable.h"


int main() {
    unlink(WORKDIR "/resolv.conf-tmp");

    dns_add("eth0",2,"dddd::1111");
    dns_add("eth0",2,"dddd::2222");

//    dns_del("eth0",2,"dddd::2222");
//    dns_del("eth0",2,"dddd::2222");
//    dns_del("eth0",2,"dddd::2222");
    dns_del_all();

    return 0;
}
