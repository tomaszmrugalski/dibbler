#include <stdio.h>
#include "Portable.h"

int dns_add(const char * ifname, int ifaceid, char * addrPlain) {
    FILE * f;
    if ( !(f=fopen(RESOLVCONF_FILE,"a+")) ) {
	return -1;
    }
    fprintf(f,"nameserver %s\n",addrPlain);
    fclose(f);
}

int dns_del(const char * ifname, int ifaceid, char *addrPlain) {
    //FIXME
}

int domain_add(const char* ifname, int ifaceid, char* domain) {
    FILE * f;
    if ( !(f=fopen(RESOLVCONF_FILE,"a+")) ) {
	return -1;
    }
    fprintf(f,"search %s\n", domain);
    fclose(f);
}

int domain_del(const char * ifname, int ifaceid, char *domain) {
    //FIXME
}
