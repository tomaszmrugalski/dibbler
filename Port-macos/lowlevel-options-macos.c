/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: lowlevel-options-macos.c,v 1.1 2008-07-20 15:54:37 thomson Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "sys/stat.h"
#include "Portable.h"

#define CR 0x0a
#define LF 0x0d

extern char * Message;

/*
 * results 0 - ok
          -1 - unable to open temp. file
          -2 - unable to open resolv.conf file
 */
int dns_add(const char * ifname, int ifaceid, const char * addrPlain) {
    FILE * f;
    unsigned char c;
    if ( !(f=fopen(RESOLVCONF_FILE,"a+"))) {
	return LOWLEVEL_ERROR_FILE;
    }

    fseek(f, -1, SEEK_END);
    c = fgetc(f);
    fseek(f,0, SEEK_END);
    if ( (c!=CR) && (c!=LF) ) {
	fprintf(f,"\n");
    }

    fprintf(f,"nameserver %s\n",addrPlain);
    fclose(f);
    return LOWLEVEL_NO_ERROR;
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
    return LOWLEVEL_NO_ERROR;
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
	return LOWLEVEL_ERROR_FILE;
    if ( !(f2= fopen(RESOLVCONF_FILE,"w+")))
	return LOWLEVEL_ERROR_FILE;
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, "search")) ) {
	    if (strlen(buf))
		buf[strlen(buf)-1]=0;
	    fprintf(f2, "%s %s\n", buf, domain);
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

    return LOWLEVEL_NO_ERROR;
}

int domain_del(const char * ifname, int ifaceid, const char *domain) {
    FILE * f, *f2;
    char buf[512], searchbuf[512], *ptr;
    int found=0;
    struct stat st;
    memset(&st,0,sizeof(st));
    stat(RESOLVCONF_FILE, &st);

    if (strlen(domain) >= sizeof(searchbuf)-1 )
	return LOWLEVEL_ERROR_UNSPEC;
    searchbuf[0] = ' ';
    strcpy(&(searchbuf[1]), domain);
    unlink(RESOLVCONF_FILE".old");
    rename(RESOLVCONF_FILE,RESOLVCONF_FILE".old");
    if ( !(f = fopen(RESOLVCONF_FILE".old","r")) )
	return LOWLEVEL_ERROR_FILE;
    if ( !(f2= fopen(RESOLVCONF_FILE,"w+")))
	return LOWLEVEL_ERROR_FILE;
    while (fgets(buf,511,f)) {
	if ( (!found) && (ptr=strstr(buf, searchbuf)) ) {
	    found = 1;
	    strcpy(ptr, ptr+strlen(searchbuf));
	    if (strlen(buf)<11) /* 11=minimum length (one letter domain in 2letter top domain, e.g. "search x.pl") */
		continue;
	}
	fprintf(f2,"%s",buf);
    }
    fclose(f);
    fclose(f2);

    chmod(RESOLVCONF_FILE,st.st_mode);
    return LOWLEVEL_NO_ERROR;
}

int ntp_add(const char* ifname, const int ifindex, const char* addrPlain){
    FILE * f;
    unsigned char c;
    if ( !(f=fopen(NTPCONF_FILE,"a+"))) {
	return LOWLEVEL_ERROR_FILE;
    }

    fseek(f, -1, SEEK_END);
    c = fgetc(f);
    fseek(f,0, SEEK_END);
    if ( (c!=CR) && (c!=LF) ) {
	fprintf(f,"\n");
    }

    fprintf(f,"server %s\n",addrPlain);
    fclose(f);
    return LOWLEVEL_NO_ERROR;
}

int ntp_del(const char* ifname, const int ifindex, const char* addrPlain){
    FILE * f, *f2;
    char buf[512];
    int found=0;
    struct stat st;
    memset(&st,0,sizeof(st));
    stat(NTPCONF_FILE, &st);

    unlink(NTPCONF_FILE".old");
    rename(NTPCONF_FILE, NTPCONF_FILE".old");
    f = fopen(NTPCONF_FILE".old","r");
    f2 = fopen(NTPCONF_FILE,"w"); 
    while (fgets(buf,511,f)) {
	if ( (!found) && (strstr(buf, addrPlain)) ) {
	    found = 1;
	    continue;
	}
	fprintf(f2,"%s",buf);
    }
    fclose(f);
    fclose(f2);

    chmod(NTPCONF_FILE, st.st_mode);
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
    /** @todo: implement this */
  return 0;
}

/** 
 * adds prefix - if this node has IPv6 forwarding disabled, it will configure that prefix on the
 * interface, which prefix has been received on. If the forwarding is enabled, it will be assigned
 * to all other up, running and multicast capable interfaces.
 * In both cases, radvd.conf file will be created.
 * 
 * @param ifname interface name
 * @param ifindex interface index
 * @param prefixPlain prefix (specified in human readable format)
 * @param prefixLength prefix length
 * @param prefered preferred lifetime
 * @param valid valid lifetime
 * 
 * @return negative error code or 0 if successful
 */
int prefix_add(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
	       unsigned long prefered, unsigned long valid)
{
    /** @todo: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
		  unsigned long prefered, unsigned long valid)
{
    /** @todo: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}


int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength) {
    /** @todo: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}
