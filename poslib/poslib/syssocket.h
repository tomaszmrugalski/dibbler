/*
    Posadis - A DNS Server
    Universal include file for sockets, since different OS'ses use different directories
    Copyright (C) 2001  Meilof Veeningen <meilof@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __POSLIB_SYSSOCKET_H
#define __POSLIB_SYSSOCKET_H

#include "config.h"

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/time.h>
#  ifdef HAVE_SYS_POLL_H
#    include <sys/poll.h>
#  else
#    ifdef HAVE_POLL_H
#      include <poll.h>
#    else
#      include "w32poll.h"
#    endif
#  endif
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <errno.h>
#  define closesocket close
#endif

#ifndef HAVE_SOCKLEN_T
#define socklen_t int
#endif

#ifdef HAVE_SOCKADDR_STORAGE
#  ifdef HAVE___SS_FAMILY
#    define s_family __ss_family
#  else
#    define s_family ss_family
#  endif
#  ifdef __BORLANDC__
#    define sockaddr_storage sockaddr_in6
#    undef s_family
#    define s_family sin6_family
#  endif
#else
#  define sockaddr_storage sockaddr
#  define s_family sa_family
#  ifdef HAVE_IPV6
#    warning sockaddr_storage not available: ipv6 support has been disabled!
#  endif
#  undef HAVE_IPV6
#endif

#ifndef HAVE_POLL
#  include "w32poll.h"
#endif

#endif /* __POSLIB_SYSSOCKET_H */

