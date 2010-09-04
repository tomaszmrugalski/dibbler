/* $Id: interface.c,v 1.1 2009-03-24 23:18:15 thomson Exp $ */

/*
 * This file is part of ifplugd.
 *
 * ifplugd is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ifplugd is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ifplugd; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include <net/if.h>

#include "ethtool-local.h"
#include "interface.h"
#include <stdarg.h>

#include <types.h>

void daemon_log(int loglevel, const char *fmt,...)
{
    char buf[255];
    va_list args;
    va_start(args,fmt);
    vsnprintf(buf, 255, fmt, args);
    va_end(args);

    printf("%s\n", buf);
}

void interface_up(int fd, const char *iface) {
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);
    
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        if (interface_do_message)
            daemon_log(LOG_WARNING, "Warning: Could not get interface flags.");
        
        return;
    }

    if ((ifr.ifr_flags & IFF_UP) == IFF_UP)
        return;
    
    if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
        if (interface_do_message)
            daemon_log(LOG_WARNING, "Warning: Could not get interface address.");
    } else if (ifr.ifr_addr.sa_family != AF_INET) {
        if (interface_do_message)
            daemon_log(LOG_WARNING, "Warning: The interface is not IP-based.");
    } else {
        ((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr.s_addr = INADDR_ANY;
        if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
            if (interface_do_message)
                daemon_log(LOG_WARNING, "Warning: Could not set interface address.");
        }
    }

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        if (interface_do_message)
            daemon_log(LOG_WARNING, "Warning: Could not get interface flags.");
        
        return;
    }
    
    ifr.ifr_flags |= IFF_UP;
    
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
        if (interface_do_message)
            daemon_log(LOG_WARNING, "Warning: Could not set interface flags.");
}


interface_status_t interface_detect_beat_ethtool(int fd, const char *iface) {

    struct ifreq ifr;
    struct ethtool_value edata;

    if (interface_auto_up)
        interface_up(fd, iface);
    
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);

    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = /*(caddr_t)*/ &edata;

    if (ioctl(fd, SIOCETHTOOL, &ifr) == -1) {
        if (interface_do_message)
            daemon_log(LOG_ERR, "ETHTOOL_GLINK failed: %s", strerror(errno));
        
        return IFSTATUS_ERR;
    }

    return edata.data ? IFSTATUS_UP : IFSTATUS_DOWN;
}

interface_status_t interface_detect_beat_iff(int fd, const char *iface) {

    struct ifreq ifr;

    if (interface_auto_up)
        interface_up(fd, iface);

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        if (interface_do_message)
            daemon_log(LOG_ERR, "SIOCGIFFLAGS failed: %s", strerror(errno));

        return IFSTATUS_ERR;
    }

    return ifr.ifr_flags & IFF_RUNNING ? IFSTATUS_UP : IFSTATUS_DOWN;
}
