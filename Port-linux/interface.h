#ifndef INTERFACE_H
#define INTERFACE_H

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

#ifdef __cplusplus 
extern "C" {
#endif

int interface_auto_up;
int interface_do_message;

typedef enum { IFSTATUS_UP, IFSTATUS_DOWN, IFSTATUS_ERR } interface_status_t;

void interface_up(int fd, const char *iface);

/* interface_status_t interface_detect_beat_mii(int fd, char *iface); */
/* interface_status_t interface_detect_beat_priv(int fd, char *iface); */
interface_status_t interface_detect_beat_ethtool(int fd, const char *iface);
/* interface_status_t interface_detect_beat_wlan(int fd, char *iface); */
interface_status_t interface_detect_beat_iff(int fd, const char *iface);
#ifdef __cplusplus 
}
#endif
#endif
