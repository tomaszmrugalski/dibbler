/*
    Posadis - A DNS Server
    Win32 poll implementation
    Copyright (C) 2002  Meilof Veeningen <meilof@users.sourceforge.net>

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

#include "dibbler-config.h"
#include "syssocket.h"

#ifndef HAVE_POLL

#include "syssocket.h"
#include "sysstring.h"
#include "w32poll.h"

#ifdef _WIN32
#define COMPAT_ECONNRESET WSAECONNRESET
#define COMPAT_ECONNREFUSED WSAECONNREFUSED
#define COMPAT_ENETDOWN WSAENETDOWN
#define COMPAT_ECONNABORTED WSAECONNABORTED
#define COMPAT_EBADF WSAEBADF
#else
#define COMPAT_ECONNRESET ECONNRESET
#define COMPAT_ECONNREFUSED ECONNREFUSED
#define COMPAT_ENETDOWN ENETDOWN
#define COMPAT_ECONNABORTED ECONNABORTED
#define COMPAT_EBADF EBADF
#endif

#include <stdio.h>

fd_set set_in, set_out, set_err;

int poll(struct pollfd *ufds, unsigned int nfds, int timeout) {
  int ret = 0;
  unsigned int x;
  /* convert to msecs */
  struct timeval tv = {timeout / 1000, (timeout % 1000) * 1000};
  bool have_inerr = false;
  bool have_out = false;

  FD_ZERO(&set_in);
  FD_ZERO(&set_out);
  FD_ZERO(&set_err);

  // at first, initialize the sockets
  for (x = 0; x < nfds; x++) {
    if (ufds[x].events & POLLIN) {
      have_inerr = true;
      FD_SET((unsigned int)ufds[x].fd, &set_in);
    }
    if (ufds[x].events & POLLOUT) {
      FD_SET((unsigned int)ufds[x].fd, &set_out);
      have_out = true;
    }
    if (ufds[x].events & POLLERR) {
      FD_SET((unsigned int)ufds[x].fd, &set_err);
      have_inerr = true;
    }
  }

  if (have_inerr) {
    // call select
    ret = select(FD_SETSIZE - 1, &set_in, NULL, &set_err, &tv);
    if (ret < 0) {
      return -1;
    }
  }
  if (have_out || !have_inerr) {
    // ..twice
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(FD_SETSIZE - 1, NULL, &set_out, NULL, &tv);
  }

  // set right values
  for (x = 0; x < nfds; x++) {
    ufds[x].revents = 0;
    if (FD_ISSET(ufds[x].fd, &set_in)) ufds[x].revents |= POLLIN;
    if (FD_ISSET(ufds[x].fd, &set_out)) ufds[x].revents |= POLLOUT;
    if (FD_ISSET(ufds[x].fd, &set_err)) {
      ufds[x].revents |= POLLERR;
      /* check for hup */
      u_long arg = 0;
      socklen_t arglen = sizeof(arg);
      if (!getsockopt(ufds[x].fd, SOL_SOCKET, SO_ERROR, (char *)&arg, &arglen) && arg != 0) {
        switch (arg) {
          case COMPAT_ECONNRESET:
          case COMPAT_ECONNREFUSED:
          case COMPAT_ENETDOWN:
          case COMPAT_ECONNABORTED:
            ufds[x].revents |= POLLHUP;
            break;
          case COMPAT_EBADF:
            ufds[x].revents |= POLLERR;
            break;
        }
        ufds[x].revents |= POLLERR;
      }
    }
  }
  return ret;
}

#endif
