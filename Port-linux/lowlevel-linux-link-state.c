/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: lowlevel-linux-link-state.c,v 1.3 2008-10-12 14:32:12 thomson Exp $
 */	

#include "Portable.h"

volatile struct link_state_notify_t * changed_links = 0;
volatile int * notifier = 0;

/** 
 * begin monitoring of those interfaces
 * 
 * @param changed_links 
 */
void link_state_change_init(volatile struct link_state_notify_t * monitored_links, volatile int * notify)
{

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
    
    
    /* FIXME: Add actual monitoring here (spawn extra thread or install some handlers, etc.)  */
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
	*notifier = 1; /* notify that change has occured */
    } else
    {
	printf("Error: link state change detected, but notification system not initiated properly\n");
	/* link_state_change_init() should have been called in advance */
    }

}

/** 
 * cleanup code for link state monitoring
 * 
 */
void link_state_change_cleanup()
{

}

