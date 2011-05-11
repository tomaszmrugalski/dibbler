/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: lowlevel-linux-link-state.c,v 1.4 2009-03-24 23:17:18 thomson Exp $
 */

#define __USE_UNIX98

#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bits/sigthread.h>
#include "Portable.h"
#include "interface.h"

#define IF_RECONNECTED_DETECTED -1

int report_link_state(const char* iface);


volatile struct link_state_notify_t * changed_links = 0;
volatile int * notifier = 0;
int isDone = 0;
pthread_t parent_id;
pthread_t ntid;
pthread_mutex_t lock;

struct state {
    int id;
    int stat;
    struct state* next;
};

struct state* states = NULL;

int getStateById(int id){
    struct state* item = states;
    struct state* new_state;
    for(;item!=NULL;item = item->next){
	if(item->id==id)
	    return item->stat;
    }
    new_state = malloc(sizeof(struct state));
    new_state->id = id;
    new_state->stat = 1;
    new_state->next = states;
    states = new_state;
    return new_state->stat;
}

void setStateById(int id,int stat){
    struct state* item = states;
    struct state* new_state;
    for(;item!=NULL;item = item->next){
	if(item->id==id){
	    item->stat = stat;
	    return;
	}
    }
    new_state = malloc(sizeof(struct state));
    new_state->id = id;
    new_state->stat = stat;
    new_state->next = states;
    states = new_state;
}

/**
 * this function should be called when interface link state change is detected
 *
 * @param ifindex index of the changed interface
 */
void link_state_changed(int ifindex)
{
    if (changed_links)
    {
	if (changed_links->cnt<16)
	    changed_links->ifindex[changed_links->cnt++] = ifindex;
	pthread_mutex_lock(&lock);
	*notifier = 1; /* notify that change has occured */
	pthread_mutex_unlock(&lock);
	pthread_kill(parent_id,SIGUSR1);
    } else
    {
	printf("Error: link state change detected, but notification system not initiated properly\n");
	/* link_state_change_init() should have been called in advance */

    }

}

void * checkLinkState(void * ptr){
    struct iface * head = (struct iface *)ptr;
    while(!isDone){
	struct iface * item = head;
	for(;item!=NULL;item = item->next){
	    int old_r = item->link_state;
	    int new_r = report_link_state(item->name);
	    if (new_r - old_r ==  IF_RECONNECTED_DETECTED){
		link_state_changed(item->id);
	    }
	    item->link_state = new_r;
	}
	/* printf("."); */
	sleep(1);
    }
    /* if_list_release(head); */
    /* printf("Finishing link-state change\n"); */

    /* pthread_exit(NULL); */
    return NULL;
}


/**
 * removes intefaces that are not to be monitored from the list
 *
 * @param head
 * @param monitored_links
 *
 * @return
 */
struct iface * filter_if_list(struct iface *head, volatile struct link_state_notify_t * monitored_links)
{
    struct iface * out = NULL;
    struct iface * tmp = NULL;
    int i;

    while (head!=NULL) {
	int copy = 0;
	for (i=0; i<monitored_links->cnt; i++) {
	    if (head->id == monitored_links->ifindex[i]) {
		copy = 1;
	    }
	}
	if (copy) {
	    tmp = head;
	    head = head->next;

	    tmp->next = out;
	    out = tmp;
	} else {
	    tmp = head;
	    head = head->next;
	    tmp->next = 0;
	    if_list_release(tmp);
	}
    }

    return out;
}


/**
 * begin monitoring of those interfaces
 *
 * @param monitored_links head of the monitored links list
 * @param notify pointer to variable that is going to be modifed if change is detected
 */
void link_state_change_init(volatile struct link_state_notify_t * monitored_links, volatile int * notify)
{
    struct iface * ifacesLst;
    int err;

    /* the same structure will be used for notifying about link-state change */
    notifier = notify;
    changed_links = monitored_links;

    parent_id = pthread_self();
    /** @todo Add actual monitoring here (spawn extra thread or install some handlers, etc.)  */

    ifacesLst = if_list_get();

    ifacesLst = filter_if_list(ifacesLst, monitored_links);

    /* uncomment this section to get information regarding interfaces to be monitored */
    /*
    int i=0;
    printf("About to start monitoring %d interfaces:", monitored_links->cnt);
    for (i=0; i<monitored_links->cnt; i++)
	printf(" %d", monitored_links->ifindex[i]);
    printf("\n");
    */


    err = pthread_create(&ntid, NULL, checkLinkState, (void*)ifacesLst);

    if (err !=0 ){
	printf("process create fail in dibbler/Port-linux/lowlevel-linux-link-state.c.");
	return ;
    }
}

/**
 * cleanup code for link state monitoring
 *
 */
void link_state_change_cleanup()
{
    isDone = 1;
}

int report_link_state(const char* iface) {
    int fd, r = 0;
    interface_status_t s;

    if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
	printf("Socket create failed\n");
	return -1;
    }

    /* here just to make it more convenient for testing:if we define DEBUG,so a plugin-out and plugin-in event
     * will be detected if we do "ifconfig interface-name down" and ifconfig interface-name up".Otherwise,
     * if we do not define DEBUG, a plugin-in and plugin-out event can only be detected when the local link
     * plugin in and out. Skip this comment if you did not catch it.
     */

    /* #define DEBUG*/
    #if 0
    if ((s = interface_detect_beat_ethtool(fd, iface)) == IFSTATUS_ERR) {
	printf("    SIOCETHTOOL failed (%s)\n", strerror(errno));
	close(fd);
	return -1;
    }
    #else
    if ((s = interface_detect_beat_iff(fd,iface)) == IFSTATUS_ERR) {
	printf("   IFF_RUNNING failed (%s)\n", strerror(errno));
	close(fd);
	return -1;
    }
    #endif

    switch(s) {
	case IFSTATUS_UP:
	    /* link beat detected */
	    r = 1;
	    break;

	case IFSTATUS_DOWN:
	    /* unplugged */
	    r = 2;
	    break;

	default:
	    /* not supported (Retry as root?)*/
	    r = -1;
	    break;
    }

   close(fd);
   return r;
}
