#include <stdio.h>
#include "sys/stat.h"
#include "Portable.h"

/*
 * results 0 - ok
          -1 - unable to open temp. file
          -2 - unable to open resolv.conf file
 */
int dns_add(const char * ifname, int ifaceid, char * addrPlain) {
    FILE * f;
    if ( !(f=fopen(RESOLVCONF_FILE,"a+"))) {
	return -1;
    }

    fprintf(f,"nameserver %s\n",addrPlain);
    fclose(f);
    if (!(f=fopen(WORKDIR"/tmp-dns","a+"))) {
	return -2;
    }
    fprintf(f,"%s\n",addrPlain);
    fclose(f);
    return 0;
}

int dns_del(const char * ifname, int ifaceid, char *addrPlain) {
    FILE * f, *f2;
    char buf[512];
    int found=0;
    rename(WORKDIR"/tmp-dns",WORKDIR"/tmp-dns.old");

    if ( !(f=fopen(WORKDIR"/tmp-dns.old","r"))) {
	// Unable to open file for reading
	return -1;
    }

    if ( !(f2=fopen(WORKDIR"/tmp-dns","w+"))) {
	// Unable to open file for writing
	return -1;
    }

    while (!feof(f)) {
	fgets(buf,511,f);
	if (!strncmp(addrPlain,buf,strlen(addrPlain))) {
	    found=1;
	} else {
	    fprintf(f2,"%s",buf);
	}
    }
    // now we've got remaining info in tmp-dns

    fclose(f);
    fclose(f2);

    if (found) {
	rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
	f = fopen(RESOLVCONF_FILE".old","r");
	f2 = fopen(RESOLVCONF_FILE,"w+"); 
	while (!feof(f)) {
	    fgets(buf,511,f);
	    if (!strstr(buf, addrPlain)) {
		fprintf(f2,"%s",buf);
	    }
	}
	fclose(f);
	fclose(f2);
    }
    chmod(RESOLVCONF_FILE,12);
    return 0;
}

int domain_add(const char* ifname, int ifaceid, char* domain) {
    FILE * f;
    char buf[512];
    memset(buf,0,512);
    if ( !(f=fopen(RESOLVCONF_FILE,"a+"))) {
	return -1;
    }
    fprintf(f,"search %s\n",domain);
    fclose(f);
    if (!(f=fopen(WORKDIR"/tmp-domain","a+"))) {
	return -2;
    }
    fprintf(f,"%s\n",domain);
    fclose(f);
    return 0;
}

int domain_del(const char * ifname, int ifaceid, char *domain) {
    FILE * f, *f2;
    char buf[512];
    int found=0;
    rename(WORKDIR"/tmp-domain",WORKDIR"/tmp-domain.old");

    if ( !(f=fopen(WORKDIR"/tmp-domain.old","r"))) {
//	printf("Unable to open %s file for reading.\n", RESOLVCONF_FILE);
	return -1;
    }

    if ( !(f2=fopen(WORKDIR"/tmp-domain","w+"))) {
//	printf("Unable to open %s file for writing.\n", RESOLVCONF_FILE);
	return -1;
    }

    while (!feof(f)) {
	fgets(buf,511,f);
	if (!strncmp(domain,buf,strlen(domain))) {
	    found=1;
	} else {
	    fprintf(f2,"%s",buf);
	}
    }
    // now we've got reamaining info in tmp-domain

    fclose(f);
    fclose(f2);

    if (found) {
	rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
	if ( !(f = fopen(RESOLVCONF_FILE".old","r")) )
	    return -1;
	if ( !(f2= fopen(RESOLVCONF_FILE,"w+")))
	    return -1;
	while (!feof(f)) {
	    fgets(buf,511,f);
	    if (!strstr(buf, domain)) {
		fprintf(f2,"%s",buf);
	    }
	}
	fclose(f);
	fclose(f2);
    }
    chmod(RESOLVCONF_FILE,12);
    return 0;
}
