#include "Portable.h"


int main() {
    dns_add("eth0",2,"dddd::1111");
    dns_del("eth0",2,"dddd::1111");

//     dns_add("eth0",2,"dddd::2222");
//     dns_del("eth0",2,"dddd::2222");
//     dns_del("eth0",2,"dddd::2222");
//     dns_del("eth0",2,"dddd::2222");
//     dns_del_all();

//     domain_add("eth0",2,"chelm.gda.pl");
//     domain_add("eth0",2,"klub.com.pl");


    return 0;
}
