/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#define _BSD_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "Portable.h"
#include "dibbler-config.h"

#define CR 0x0a
#define LF 0x0d
#define MAX_LINE_LEN 511


extern char * Message;


#ifdef MOD_RESOLVCONF
/** @brief check whether the resolconf executable exists
 *
 * Tries to spawn resolvconf process and returns a pipe to it.
 * Parameters are passed to resolvconf as command line 
 * arguments (e.g. -a|-d, "IFNAME")
 *
 * the pipe needs to be closed by the caller
 *
 * @param arg1 first command line argument passed to resolvconf
 * @param arg2 second command line argument passed to resolvconf
 *
 * @return file handler (pipe to resolvconf process) or NULL
 */
FILE *resolvconf_open(const char *arg1, const char *arg2)
{
    pid_t child;
    int pipefd[2];

    if (access(RESOLVCONF, X_OK) != 0)
        return NULL;
    if (pipe(pipefd) != 0)
        return NULL;
    switch(child = fork()) {
      case 0: /* child */
          close(pipefd[1]);
	  close(0);
	  dup(pipefd[0]);
	  close(pipefd[0]);
	  /* double fork so init reaps the child */
	  if (!fork()) { /* child */
              execl(RESOLVCONF, RESOLVCONF, arg1, arg2, (char *)NULL);
	  } /* All other cases are meaningless here */
	  exit(EXIT_FAILURE);
	  break;
    case EXIT_FAILURE: /* error */
          return NULL;
	  break;
    }
    /* parent */
    close(pipefd[0]);
    waitpid(child, NULL, 0);
    return fdopen(pipefd[1], "w");
}
#endif

/* in iproute.c, borrowed from iproute2 */
extern int iproute_modify(int cmd, unsigned flags, int argc, char **argv);

/**
 * @brief Removes an entry from a file.
 * 
 * Remove value of keyword from opened file in and the result is printed into
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


/**
 * @brief removes value of keyword from file.
 *
 * It tries to do its best not to corrupt the file.
 * Returns LOWLEVEL ERROR codes
 *
 * @param file
 * @param keyword
 * @param value
 *
 * @return status code (one of LOWLEVEL_* defines)
 */
int cfg_file_del(const char *file, const char *keyword, const char *value) {
    FILE *fold, *ftmp;
    int tmpfd;
    int error = LOWLEVEL_NO_ERROR;
    struct stat st;
    char template[]="/etc/dibbler.XXXXXX";

    /* Create temporary FILE */
    if ((tmpfd = mkstemp(template)) == -1)
	return (LOWLEVEL_ERROR_FILE);

    if (!(ftmp = fdopen(tmpfd, "w"))) {
        /* close the file first, then... */
	close(tmpfd);

        /* then delete it */
	unlink(template);
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
    if (fclose(fold) == EOF) {
        error=LOWLEVEL_ERROR_FILE;
    }

    if (fclose(ftmp) == EOF) {
        error=LOWLEVEL_ERROR_FILE;
    }

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
    FILE * f = NULL;
    unsigned char c;

#ifdef MOD_RESOLVCONF
    /* try to use resolvconf */
    f=resolvconf_open("-a", ifname);
#endif

    /* if resolvconf is not available, fallback to normal file append */
    if (!f && !(f=fopen(RESOLVCONF_FILE, "a+")) ) {
            return LOWLEVEL_ERROR_FILE;
    }
    
    fseek(f, -1, SEEK_END);
    c = fgetc(f); /* read the last character */

    fseek(f,0, SEEK_END);
    /* if the file does not end with new-line, add it */
    if ( (c != CR) && (c != LF) ) {
        fprintf(f,"\n");
    }
    fprintf(f,"nameserver %s\n",addrPlain);
    fclose(f);
    return LOWLEVEL_NO_ERROR;
}

int dns_del(const char * ifname, int ifaceid, const char *addrPlain) {
    
#ifdef MOD_RESOLVCONF
    FILE *f = NULL;
    /* try to use resolvconf to remove config */
    if ((f=resolvconf_open("-d", ifname))) {
        fclose(f);
        return LOWLEVEL_NO_ERROR;
    }
#endif
    
    return cfg_file_del(RESOLVCONF_FILE, "nameserver", addrPlain);
}

int domain_add(const char* ifname, int ifaceid, const char* domain) {
    FILE * f, *f2;
    char buf[512];
    int found = 0;
    unsigned char c;
    struct stat st;

#ifdef MOD_RESOLVCONF
    /* try to use resolvconf it is available */
    if ( (f=resolvconf_open("-a", ifname))) {
        fprintf(f, "search %s\n", domain);
        fclose(f);
        return LOWLEVEL_NO_ERROR;
    }
#endif
    
    /* otherwise do the edit on your own */

    memset(&st,0,sizeof(st));
    stat(RESOLVCONF_FILE, &st);

    unlink(RESOLVCONF_FILE ".old");
    rename(RESOLVCONF_FILE, RESOLVCONF_FILE ".old");
    if ( !(f = fopen(RESOLVCONF_FILE ".old", "r")) )
	return LOWLEVEL_ERROR_FILE;
    if ( !(f2 = fopen(RESOLVCONF_FILE, "w+")) ) {
        fclose(f);
	return LOWLEVEL_ERROR_FILE;
    }
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

#ifdef MOD_RESOLVCONF
    FILE * f;
    /* try to use resolvconf if it is available */
    if ((f = resolvconf_open("-d", ifname))) {
        fclose(f);
        return LOWLEVEL_NO_ERROR;
    }
#endif

    /* otherwise fallback to normal file manipulation */
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

void add_radvd_conf(const char* ifname, const char* prefixPlain, int prefixLength,
                    uint32_t preferred, uint32_t valid) {
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
    }
    fseek(f, 0, SEEK_END);
    
    fprintf(f, "\n### %s start ###\n", ifname);
    fprintf(f, "interface %s \n", ifname);
    fprintf(f, "{ \n");
    fprintf(f, "     AdvSendAdvert on; \n");
    fprintf(f, "     prefix %s/%d\n", prefixPlain, prefixLength);
    fprintf(f, "     { \n");
    fprintf(f, "         AdvOnLink on;\n");
    fprintf(f, "         AdvPreferredLifetime %lu;\n", preferred);
    fprintf(f, "         AdvValidLifetime %lu;\n", valid);
    fprintf(f, "         AdvAutonomous on;\n");
    fprintf(f, "     };\n");
    fprintf(f, "};\n");
    fprintf(f, "### %s end ###\n", ifname);
    fprintf(f, "\n");

    fclose(f);
}

void delete_radvd_conf(const char* ifname, const char* prefixPlain, int prefixLen) {

    FILE *f, *f2;
    struct stat st;
    int found = 0;
    char buf[512];
    char buf2[512];

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
	       unsigned long preferred, unsigned long valid)
{
    char *argv[5];
    int result;
    char buf[128];
    int numargs = 0;
    /* char buf2[128]; */

    add_radvd_conf(ifname, prefixPlain, prefixLength, preferred, valid);

    snprintf(buf, 127, "%s/%d", prefixPlain, prefixLength);
    argv[0] = buf;
    argv[1] = "dev";
    argv[2] = (char*)ifname;
    numargs = 3;

    /* this is not supported in the kernel:
    snprintf(buf2, 127, "%d", valid);
    argv[3] = "lifetime";
    argv[4] = buf2;
    numargs = 5; */

    result = iproute_modify(RTM_NEWROUTE, NLM_F_CREATE|NLM_F_EXCL, numargs, argv);

    if (result == 0)
        return LOWLEVEL_NO_ERROR;
    else
        return LOWLEVEL_ERROR_UNSPEC;
}

int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
		  unsigned long prefered, unsigned long valid)
{
    char *argv[3];
    int result;
    char buf[128];

    delete_radvd_conf(ifname, prefixPlain, prefixLength);
    add_radvd_conf(ifname, prefixPlain, prefixLength, prefered, valid);

    snprintf(buf, 127, "%s/%d", prefixPlain, prefixLength);
    argv[0] = buf;
    argv[1] = "dev";
    argv[2] = (char*)ifname;

    /* iproute2:
    ip route change uses iproute_modify(RTM_NEWROUTE, NLM_F_REPLACE, ...)
    ip route replace uses iproute_modify(RTM_NEWROUTE, NLM_F_CREATE|NLM_F_REPLACE, ...)
    */

    result = iproute_modify(RTM_NEWROUTE, NLM_F_CREATE|NLM_F_REPLACE, 3, argv);

    if (result == 0)
        return LOWLEVEL_NO_ERROR;
    else
        return LOWLEVEL_ERROR_UNSPEC;
}


int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength) {

    int result;
    char *argv[3];
    char buf[512];

    delete_radvd_conf(ifname, prefixPlain, prefixLength);

    snprintf(buf, 127, "%s/%d", prefixPlain, prefixLength);
    argv[0] = buf;
    argv[1] = "dev";
    argv[2] = (char*)ifname;
    
    result = iproute_modify(RTM_DELROUTE, 0, 3, argv);

    if (result == 0)
        return LOWLEVEL_NO_ERROR;
    else
        return LOWLEVEL_ERROR_UNSPEC;
}
