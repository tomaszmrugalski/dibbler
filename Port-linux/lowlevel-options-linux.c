/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: lowlevel-options-linux.c,v 1.18 2008-11-13 21:05:42 thomson Exp $
 *
 */

#define _BSD_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <sys/stat.h>
#include "Portable.h"

#define CR 0x0a
#define LF 0x0d
#define MAX_LINE_LEN 511


extern char * Message;

/* in iproute.c, borrowed from iproute2 */
extern int iproute_modify(int cmd, unsigned flags, int argc, char **argv);

/* Remove value of keyword from opened file in and the result is printed into
 * opened file out. If removed_empty and keyword remains without argument, it
 * will be removed too. Comments (starting with comment char) are respected.
 * All values following keyword are removed on all lines (global remove).
 * 
 * @param in 
 * @param out 
 * @param keyword 
 * @param value 
 * @param comment 
 * @param remove_empty 
 * 
 * @return Returns LOWLEVEL_NO_ERROR by default, LOWLEVEL_ERROR_FILE on I/O error \
 *         (and errno is set up), LOWLEVEL_ERROR_UNSPEC if value or keyword is too long.
 */
int cfg_value_del(FILE *in, FILE *out, const char *keyword, const char *value,
	const char comment, int remove_empty) {
    char buf[MAX_LINE_LEN+1];

    if (strlen(keyword) > MAX_LINE_LEN || strlen(value) > MAX_LINE_LEN)
	return(LOWLEVEL_ERROR_UNSPEC);

    errno = 0;
    while (fgets(buf, MAX_LINE_LEN, in)) {

	char *head;
	/* Skip leading white space */
	for (head=buf; *head!='\0' && isspace((int)(*head)); head++);
	/* Skip comment */
	if (*head!='\0' && *head!=comment) {
	    /* Find keyword */
	    if (strstr(head, keyword)==head) {
		head += strlen(keyword);

		/* Skip alone keyword */
		if (*head!='\0' && *head!=comment && isspace((int)(*head))) {
		    char *keyword_end=head;

		    /* Locate each argument */
		    while (*head!='\0' && *head!=comment) {
		    char *argument_begin=head;
		    /* Skip spaces before argument */
		    for (; *head!='\0' && isspace((int)(*head)); head++);
		    if (*head!='\0' && *head!=comment) {
			/* Compare argument to value */
			if (strstr(head, value)==head) {
			    head += strlen(value);
			    if (*head=='\0' || *head==comment ||
				    isspace((int)(*head))) {
				/* remove this argument */
				/* sprinf() moves data and appends '\0' */
				sprintf(argument_begin, "%s", head);
				head=argument_begin;
			    }
			}
			/* Skip rest of the argument */
			for (; *head!='\0' && !isspace((int)(*head)); head++);
		    }
		}

		/* Remove whole line if keyword remains without any arguments.*/
		if (remove_empty) {
		    for (head=keyword_end; *head!='\0' && isspace((int)(*head));
			    head++);
		    if (*head=='\0' || *head==comment) *buf='\0';
		    }
		}
	    }
	}

	/* print output */
	if (-1 == fprintf(out, "%s", buf)) return(LOWLEVEL_ERROR_FILE);
    }

    return((errno)?LOWLEVEL_ERROR_FILE:LOWLEVEL_NO_ERROR);
}


/* Removes value of keyword from file.
 * It tries to do its best not to corrupt the file.
 * Returns LOWLEVEL ERROR codes */
int cfg_file_del(const char *file, const char *keyword, const char *value) {
    FILE *fold, *ftmp;
    int tmpfd;
    int error=LOWLEVEL_NO_ERROR;
    struct stat st;
    char template[]="/etc/dibbler.XXXXXX";

    /* Create temporary FILE */
    if (-1 == (tmpfd=mkstemp(template)))
	return(LOWLEVEL_ERROR_FILE);

    if (NULL == (ftmp=fdopen(tmpfd, "w"))) {
	unlink(template);
	close(tmpfd);
	return(LOWLEVEL_ERROR_FILE);
    }

    /* Open original file */
    if (!(fold = fopen(file, "r"))) {
	unlink(template);
	fclose(ftmp);
	return(LOWLEVEL_ERROR_FILE);
    }

    /* modify configuration */
    error = cfg_value_del(fold, ftmp, keyword, value, '#', 1);

    /* close the files */
    if (EOF==fclose(fold)) error=LOWLEVEL_ERROR_FILE;
    if (EOF==fclose(ftmp)) error=LOWLEVEL_ERROR_FILE;

    /* move temp file into place of the old one */
    if (error==LOWLEVEL_NO_ERROR) {
	memset(&st,0,sizeof(st));
	if (stat(file, &st) || rename(template, file) ||
		chmod(file, st.st_mode))
	    error=LOWLEVEL_ERROR_FILE;
    }

    return(error);
}


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
    return cfg_file_del(RESOLVCONF_FILE, "nameserver", addrPlain);
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
    return cfg_file_del(RESOLVCONF_FILE, "search", domain);
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
    return cfg_file_del(NTPCONF_FILE, "server", addrPlain);
}

/* 
 * Set new timezone by making symlink (usually from /etc/localtime to
 * /usr/share/zoneinfo/<timezone>).
 * Only symbolic abberviated timezone specification is assumed. POSIX
 * timezones like 'PST8PDT,M4.1.0/02:00,M10.5.0/02:00' are not supported.
 */
int timezone_set(const char* ifname, int ifindex, const char* timezone){
    /* timezone-data README states:
     * file name component must not exceed 14 characters
     * file name components use only ASCII letters, '.', '-' and '_'.
     * Thus TZ_LEN >= 
     * strlen(TIMEZONES_DIR) + '/' + file name component1 + '/' + component2 */
#define TZ_LEN 64
#define TIMEZONE_FILE_TMP TIMEZONE_FILE".dibbler"
    char buf[TZ_LEN];
    struct stat st;
    
    if (!timezone || strlen(timezone)==0) return LOWLEVEL_ERROR_UNSPEC;
    /* Security check: Do not allow evil server to traverse client file system */
    if (strstr(timezone, "..")) return LOWLEVEL_ERROR_UNSPEC;
    if (TZ_LEN <= snprintf(buf, TZ_LEN, "%s/%s", TIMEZONES_DIR, timezone))
	return LOWLEVEL_ERROR_UNSPEC;
    if (stat(buf, &st) || S_ISDIR(st.st_mode)) return LOWLEVEL_ERROR_FILE;

    if (unlink(TIMEZONE_FILE_TMP) && errno!=ENOENT) return LOWLEVEL_ERROR_FILE;
    if (symlink(buf, TIMEZONE_FILE_TMP)) return LOWLEVEL_ERROR_FILE;
    if (rename(TIMEZONE_FILE_TMP, TIMEZONE_FILE)) {
	unlink(TIMEZONE_FILE_TMP);
	return LOWLEVEL_ERROR_FILE;
    }
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
    fprintf(f, "         AdvAutonomous on;\n");
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
