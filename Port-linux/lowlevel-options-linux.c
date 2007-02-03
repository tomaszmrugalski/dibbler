/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: lowlevel-options-linux.c,v 1.10 2007-02-03 19:07:01 thomson Exp $
 *
 */

#include <stdio.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include "sys/stat.h"
#include "Portable.h"

#define CR 0x0a
#define LF 0x0d

extern char * Message;

/* in iproute.c, borrowed from iproute2 */
extern int iproute_modify(int cmd, unsigned flags, int argc, char **argv);

/*
 * results 0 - ok
          -1 - unable to open temp. file
          -2 - unable to open resolv.conf file
 */
int dns_add(const char * ifname, int ifaceid, const char * addrPlain) {
    FILE * f;
    unsigned char c;
    if ( !(f=fopen(RESOLVCONF_FILE,"a+"))) {
	return -1;
    }

    fseek(f, -1, SEEK_END);
    c = fgetc(f);
    fseek(f,0, SEEK_END);
    if ( (c!=CR) && (c!=LF) ) {
	fprintf(f,"\n");
    }

    fprintf(f,"nameserver %s\n",addrPlain);
    fclose(f);
    return 0;
}

int dns_del(const char * ifname, int ifaceid, const char *addrPlain) {
    FILE * f, *f2;
    char buf[512];
    int found=0;
    struct stat st;
    memset(&st,0,sizeof(st));
    stat(RESOLVCONF_FILE, &st);

    unlink(RESOLVCONF_FILE".old");
    rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
    f = fopen(RESOLVCONF_FILE".old","r");
    f2 = fopen(RESOLVCONF_FILE,"w"); 
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, addrPlain)) ) {
	    found = 1;
	    continue;
	}
	fprintf(f2,"%s",buf);
    }
    fclose(f);
    fclose(f2);

    chmod(RESOLVCONF_FILE, st.st_mode);
    return 0;
}

int domain_add(const char* ifname, int ifaceid, const char* domain) {
    FILE * f, *f2;
    char buf[512];
    int found = 0;
    unsigned char c;
    struct stat st;

    memset(&st,0,sizeof(st));
    stat(RESOLVCONF_FILE, &st);

    unlink(RESOLVCONF_FILE".old");
    rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
    if ( !(f = fopen(RESOLVCONF_FILE".old","r")) )
	return -1;
    if ( !(f2= fopen(RESOLVCONF_FILE,"w+")))
	return -1;
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, "search")) ) {
	    if (strlen(buf))
		buf[strlen(buf)-1]=0;
	    fprintf(f2, "%s %s", buf, domain);
	    found = 1;
	    continue;
	}
	fprintf(f2,"%s",buf);
    }

    fseek(f2, -1, SEEK_END);
    c = fgetc(f2);
    fseek(f2,0, SEEK_END);
    if ( (c!=CR) && (c!=LF) ) {
	fprintf(f2,"\n");
    }

    if (!found)
	fprintf(f2,"search %s\n",domain);

    fclose(f);
    fclose(f2);
    chmod(RESOLVCONF_FILE,st.st_mode);

    return 0;
}

int domain_del(const char * ifname, int ifaceid, const char *domain) {
    FILE * f, *f2;
    char buf[512];
    int found=0;
    int x;
    struct stat st;
    memset(&st,0,sizeof(st));
    stat(RESOLVCONF_FILE, &st);

    unlink(RESOLVCONF_FILE".old");
    rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
    if ( !(f = fopen(RESOLVCONF_FILE".old","r")) )
	return -1;
    if ( !(f2= fopen(RESOLVCONF_FILE,"w+")))
	return -1;
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, domain)) ) {
	    found = 1;
	    x = strstr(buf, domain) - buf;
	    memmove (strstr(buf, domain)-1, strstr(buf, domain)+strlen(domain), 511-x);
	    if (strlen(buf)<11) /* 11=minimum length (one letter domain in 2letter top domain, e.g. "search x.pl") */
		continue;
	    fprintf(f2, "%s", buf);
	    continue;
	}
	fprintf(f2,"%s",buf);
    }
    fclose(f);
    fclose(f2);

    chmod(RESOLVCONF_FILE,st.st_mode);
    return 0;
}

int ntp_add(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int ntp_del(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int timezone_set(const char* ifname, int ifindex, const char* timezone){
    return LOWLEVEL_NO_ERROR;
}

int timezone_del(const char* ifname, int ifindex, const char* timezone){
    return LOWLEVEL_NO_ERROR;
}

int sipserver_add(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int sipserver_del(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int sipdomain_add(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int sipdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisserver_add(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisserver_del(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisdomain_set(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusserver_add(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusserver_del(const char* ifname, const int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusdomain_set(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

/** 
 * returns if forwarding is enabled on this node (i.e. is it a router or not)
 * 
 * 
 * @return 
 */
int prefix_forwarding_enabled()
{
    char returnCode;
    int router = 0; /* is packet forwarding enabled on this machine? */
    
    FILE *f = fopen("/proc/sys/net/ipv6/conf/all/forwarding", "r");
    if (f) {
	returnCode = fgetc(f); /* read exactly one byte */
	if (returnCode=='1')
	    router = 1;
    }
    fclose(f);
    return router;
}

/** 
 * adds prefix - if this node has IPv6 forwarding disabled, it will configure that prefix on the
 * interface, which prefix has been received on. If the forwarding is enabled, it will be assigned
 * to all other up, running and multicast capable interfaces.
 * In both cases, radvd.conf file will be created.
 * 
 * @param ifname 
 * @param ifindex 
 * @param prefixPlain 
 * @param prefixLength 
 * 
 * @return 
 */
int prefix_add(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
	       unsigned long prefered, unsigned long valid)
{
    char *argv[3];
    int result;
    char buf[128];
    char * errorMsg = error_message();

    FILE * f;
    f = fopen(RADVD_FILE, "r+");
    if (!f) {
	/* unable to open, so this file is missing, let's create it */
	f = fopen(RADVD_FILE, "w");
	fprintf(f, "#\n");
	fprintf(f, "# Router Advertisement config file generated by Dibbler %s\n", DIBBLER_VERSION);
	fprintf(f, "#\n");
	fprintf(f, "\n");
    }
    if (!f) {
	sprintf(errorMsg, "Unable to open %s file.", RADVD_FILE);
	return LOWLEVEL_ERROR_FILE;
    }
    fseek(f, 0, SEEK_END);
    
    fprintf(f, "\n### %s start ###\n", ifname);
    fprintf(f, "interface %s \n", ifname);
    fprintf(f, "{ \n");
    fprintf(f, "     AdvSendAdvert on; \n");
    fprintf(f, "     prefix %s/%d", prefixPlain, prefixLength);
    fprintf(f, "     { \n");
    fprintf(f, "         AdvOnLink on;\n");
    fprintf(f, "         AdvAutonomous false;\n");
    fprintf(f, "     };\n");
    fprintf(f, "};\n");
    fprintf(f, "### %s end ###\n", ifname);
    fprintf(f, "\n");

    fclose(f);

    snprintf(buf, 127, "%s/%d", prefixPlain, prefixLength);
    argv[0] = buf;
    argv[1] = "dev";
    argv[2] = (char*)ifname;
    result = iproute_modify(RTM_NEWROUTE, NLM_F_CREATE|NLM_F_EXCL, 3, argv);

    /* FIXME: Parse result */
    return LOWLEVEL_NO_ERROR;
}

int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
		  unsigned long prefered, unsigned long valid)
{
    /* update is not supported in Linux */
    return LOWLEVEL_NO_ERROR;
}


int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength) {

    int result;
    char *argv[3];
    char buf[512];
    char buf2[512];
    FILE *f, *f2;
    struct stat st;
    int found = 0;

    memset(&st,0,sizeof(st));
    stat(RADVD_FILE, &st);
    unlink(RADVD_FILE".old");
    rename(RADVD_FILE,RADVD_FILE".old");

    f = fopen(RADVD_FILE".old","r");
    f2 = fopen(RADVD_FILE,"w"); 

    snprintf(buf2, 511, "### %s start ###\n", ifname);
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, buf2)) ) {
	    found = 1;
	    snprintf(buf2, 511, "### %s end ###\n", ifname);
	    continue;
	}
	if ( (found) && (strstr(buf, buf2)) ) {
	    found = 0;
	    continue;
	}

	if (!found)
	    fprintf(f2,"%s",buf);
    }
    fclose(f);
    fclose(f2);

    snprintf(buf, 127, "%s/%d", prefixPlain, prefixLength);
    argv[0] = buf;
    argv[1] = "dev";
    argv[2] = (char*)ifname;
    
    result = iproute_modify(RTM_DELROUTE, 0, 3, argv);
    /* FIXME: Parse result */

    return LOWLEVEL_NO_ERROR;
}
