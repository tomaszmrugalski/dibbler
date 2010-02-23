/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: lowlevel-linux-link-state.c,v 1.4 2009-03-24 23:17:18 thomson Exp $
 */	

#include "Portable.h"
#include "ifplugd.h"
#include <pthread.h>
#include <signal.h>
#define IF_RECONNECTED_DETECTED -1

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

void checkLinkState(){
    struct iface * head = if_list_get();
    while(!isDone){
	struct iface * item = head;
	for(;item!=NULL;item = item->next){
	    int old_r = getStateById(item->id);
	    int new_r=handle(item->name);
            if (new_r - old_r ==  IF_RECONNECTED_DETECTED){
		link_state_changed(item->id);	
    	    }
	    setStateById(item->id,new_r);
	}
	sleep(0.5);
    }
}


/** 
 * begin monitoring of those interfaces
 * 
 * @param monitored_links head of the monitored links list
 * @param notify pointer to variable that is going to be modifed if change is detected
 */
void link_state_change_init(volatile struct link_state_notify_t * monitored_links, volatile int * notify)
{
    int err;
    /* uncomment this section to get information regarding interfaces to be monitored */
    /*
    int i=0;
    printf("About to start monitoring %d interfaces:", monitored_links->cnt);
    for (i=0; i<monitored_links->cnt; i++)
	printf(" %d", monitored_links->ifindex[i]);
    printf("\n");
    */

    /* the same structure will be used for notifying about link-state change */
    notifier = notify;
    changed_links = monitored_links; 

    parent_id = pthread_self();
    /** @todo Add actual monitoring here (spawn extra thread or install some handlers, etc.)  */
    err = pthread_create(&ntid,NULL,checkLinkState,NULL);

    if (err !=0 ){
        //cout << "process create fail in dibbler/Port-linux/lowlevel-linux-link-state.c." << endl;
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

