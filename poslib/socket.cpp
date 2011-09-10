/*
    Posadis - A DNS Server
    Socket functions
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

#include "syssocket.h"
#include "socket.h"
#include "sysstring.h"

#include "exception.h"
#include "lexfn.h"
#include "postime.h"
#include <errno.h>

#ifndef WIN32
#include <pthread.h>
#endif

bool posclient_quitflag = false;

int struct_pf(_addr *addr) {
    if (addr->s_family == AF_INET) 
	return PF_INET;
#ifdef HAVE_IPV6
    else  if (addr->s_family == AF_INET6) 
	return PF_INET6;
#endif
    return -1;
}

int struct_len(_addr *addr) {
    if (addr->s_family == AF_INET) 
	return sizeof(sockaddr_in);
#ifdef HAVE_IPV6
    else if (addr->s_family == AF_INET6) 
	return sizeof(sockaddr_in6);
#endif
    return -1;
}


#ifdef _WIN32

/* static socket library initialization */

class __init_socklib {
 public:
  __init_socklib() {
    WSADATA info;
#   ifdef HAVE_IPV6
      WSAStartup(MAKEWORD(2, 2), &info);
#   else
      WSAStartup(MAKEWORD(1, 1), &info);
#   endif
  }

  ~__init_socklib() {
    WSACleanup();
  }
} __static_init_socklib;

#endif

void setnonblock(int sockid) {
#ifdef _WIN32
    long int val = 1;
    u_long req = FIONBIO;
    ioctlsocket(sockid, val, &req);
#else
    if (fcntl(sockid, F_SETFL, O_NONBLOCK) < 0) { 
	closesocket(sockid); 
	throw PException("Could not set socket to non-blocking"); 
    }
#endif
}

int udpcreateserver(_addr* socketaddr) {
    int sockid;
    int one = 1;
    
    if ((sockid = socket(struct_pf(socketaddr), SOCK_DGRAM, IPPROTO_UDP)) < 0) 
	throw PException("Could not create UDP socket!");
    if (bind(sockid, (sockaddr *)(socketaddr), struct_len(socketaddr)) < 0) { 
	closesocket(sockid); 
	throw PException("Could not bind to socket!"); 
    }
    
    setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    setnonblock(sockid);
    
    return sockid;
}

void udpclose(int sockid) {
    closesocket(sockid);
}

int udpread(int sockid, const char *buff, int len, _addr *addr) {
    socklen_t addr_size = sizeof(_addr);
    int ret = recvfrom(sockid, (char*)buff, len, 0, (sockaddr *) addr, &addr_size);
    if (ret <= 0) 
	throw PException("Could not receive data from UDP socket");
    return ret;
}

void udpsend(int sockid, const char *buff, int len, _addr *addr) {
    if (sendto(sockid, (char*)buff, len, 0, (sockaddr *)addr, struct_len(addr)) < 0)
	throw PException(true, "Could not send UDP packet: sock %d, err %d", sockid, errno);
}

int tcpcreateserver(_addr *socketaddr) {
    int sockid;
    int one = 1;
    
    if ((sockid = socket(struct_pf(socketaddr), SOCK_STREAM, IPPROTO_TCP)) < 0) 
	throw PException("Could not create TCP socket");
    if (bind(sockid, (sockaddr *)socketaddr, struct_len(socketaddr)) < 0) { 
	closesocket(sockid); 
	throw PException("Could not bind TCP socket"); 
    }
    setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    setnonblock(sockid);
    if (listen(sockid, 5) < 0) { 
	closesocket(sockid); 
	throw PException("Could not listen to TCP socket"); 
    }

    return sockid;
}

int tcpopen(_addr *addr) {
    int sockid;
    if ((sockid = socket(struct_pf(addr), SOCK_STREAM, IPPROTO_TCP)) < 0)
	throw PException("Could not create TCP socket");
    if (connect(sockid, (sockaddr *)addr, struct_len(addr)) < 0) { 
	closesocket(sockid);
	string txt = addr_to_string(addr, false);
	throw PException(true, "Could not connect TCP socket to dst addr=%s", txt.c_str());
    }
    return sockid;
}

int tcpopen_from(_addr *to, _addr *source) {
    int sockid;
    if ((sockid = socket(struct_pf(to), SOCK_STREAM, IPPROTO_TCP)) < 0) 
	throw PException("Could not create TCP socket");
    if (bind(sockid, (sockaddr *)source, struct_len(source)) < 0) { 
	closesocket(sockid); 
	throw PException("Could not bind TCP socket"); 
    }
    if (connect(sockid, (sockaddr *)to, struct_len(to)) < 0) { 
	closesocket(sockid); 
	throw PException("Could not connect TCP socket"); 
    }
    return sockid;
}

void tcpclose(int socket) {
    closesocket(socket);
}

int tcpaccept(int socket, _addr *askaddr) {
  _addr querier;
  socklen_t len = sizeof(_addr);
  int ret = accept(socket, (sockaddr *)&querier, &len);
  if (ret < 0) throw PException("Could not accept TCP connection");
  if (askaddr) memcpy(askaddr, &querier, len);
  return ret;
}

int tcpsend(int socket, const char *buff, int bufflen) {
  int ret = send(socket, buff, bufflen, 0);
  if (ret < 0 && errno != EAGAIN) throw PException("Could not send TCP message");
  return ret;
}

int tcpread(int socket, const char *buff, int bufflen) {
  int ret = recv(socket, (char*)buff, bufflen, 0);
  if (ret < 0) {
    if (errno == EAGAIN) return 0;
    throw PException(true, "Could not read TCP message");
  }
  return ret;
}

void tcpsendall(int sockid, const char *buff, int len, int maxtime) {
  smallset_t set;
  postime_t end = getcurtime() + maxtime, cur;
  int ret, x;

  set.init(1);
  set.set(0, sockid);

  while (len > 0) {
    while ((cur = getcurtime()) <= end) {
      set.init(1);
      set.set(0, sockid);
      x = end.after(cur);
      if (x > 1000) x = 1000;
      set.waitwrite(x);
      if (set.canwrite(0) || posclient_quitflag) break;
    }
    if (!set.canwrite(0)) throw PException("Could not send buffer");
    ret = tcpsend(sockid, buff, len);

    buff += ret;
    len -= ret;
  }
}

void tcpreadall(int sockid, const char *buff, int len, int maxtime) {
  smallset_t set;
  postime_t end = getcurtime() + maxtime, cur;
  int ret, x;

  set.init(1);
  set.set(0, sockid);

  while (len > 0) {
    while ((cur = getcurtime()) <= end) {
      set.init(1);
      set.set(0, sockid);
      x = end.after(cur);
      if (x > 1000) x = 1000;
      set.wait(x);
      if (set.isdata(0) || posclient_quitflag) break;
    }
//    set.check();
    if (!set.isdata(0)) {
      throw PException("Could not read TCP message: no data");
    }
    ret = tcpread(sockid, buff, len);
    if (ret == 0) throw PException("TCP client hung up!");

    buff += ret;
    len -= ret;
  }
}

bool tcpisopen(int sockid) {
  smallset_t set;
  set.init(1);
  set.set(0, sockid);
  set.check();
  if (!set.isdata(0)) return true;
  char buff[1];
  if (recv(sockid, buff, 1, MSG_PEEK) <= 0) return false; else return true;
}

/* address functions */

void getaddress_ip4(_addr *res, const unsigned char *ipv4_data, int port) {
  memset(res, 0, sizeof(_addr));
#ifdef HAVE_SIN_LEN
  ((sockaddr_in *)res)->sin_len = sizeof(_addr);
#endif
  ((sockaddr_in *)res)->sin_family = AF_INET;
  ((sockaddr_in *)res)->sin_port = htons(port);
  memcpy(&((sockaddr_in *)res)->sin_addr.s_addr, ipv4_data, 4);
}

#ifdef HAVE_IPV6
void getaddress_ip6(_addr *res, const unsigned char *ipv6_data, int port) {
  memset(res, 0, sizeof(_addr));
#ifdef HAVE_SIN6_LEN
  ((sockaddr_in6 *)res)->sin6_len = sizeof(_addr);
#endif
  ((sockaddr_in6 *)res)->sin6_family = AF_INET6;
  ((sockaddr_in6 *)res)->sin6_port = htons(port);
  memcpy(&((sockaddr_in6 *)res)->sin6_addr, ipv6_data, 16);
}
#endif

void getaddress(_addr *res, const char *ip, int port) {
#ifdef HAVE_IPV6
  char *ptr = strchr((char*)ip, ':');

  if (ptr) {
    /* ipv6 */
    memset(res, 0, sizeof(sockaddr_in6));
#ifdef HAVE_SIN6_LEN
    ((sockaddr_in6 *)res)->sin6_len = sizeof(sockaddr_in6);
#endif
    ((sockaddr_in6 *)res)->sin6_family = AF_INET6;
    ((sockaddr_in6 *)res)->sin6_port = htons(port);
    txt_to_ipv6((unsigned char *)&((sockaddr_in6 *)res)->sin6_addr, ip);
    return;
  }
#endif
  /* ipv4 */
  memset(res, 0, sizeof(sockaddr_in));
#ifdef HAVE_SIN_LEN
  ((sockaddr_in *)res)->sin_len = sizeof(sockaddr_in);
#endif
  ((sockaddr_in *)res)->sin_family = AF_INET;
  ((sockaddr_in *)res)->sin_port = htons(port);
  txt_to_ip((unsigned char *)&((sockaddr_in *)res)->sin_addr, ip);
}

void addr_setport(_addr *addr, int port) {
  ((sockaddr_in *)addr)->sin_port = htons(port);
}
int addr_getport(_addr *addr) {
  return ntohs(((sockaddr_in *)addr)->sin_port);
}

bool address_lookup(_addr *res, const char *name, int port) {
  struct hostent *ent;

  if (strchr(name, ':')) {
    /* try ipv6 */
    getaddress(res, name, port);
    return true;
  }

  ent = gethostbyname(name);
  if (!ent) return false;

  memset(res, 0, sizeof(_addr));
#ifdef HAVE_SIN_LEN
  ((sockaddr_in *)res)->sin_len = sizeof(_addr);
#endif
  ((sockaddr_in *)res)->sin_family = ent->h_addrtype;
  ((sockaddr_in *)res)->sin_port = htons(port);
  memcpy(&((sockaddr_in *)res)->sin_addr, ent->h_addr, ent->h_length);
  return true;
}


bool address_matches(_addr *addr1, _addr *addr2) {
  if (addr1->s_family != addr2->s_family) return false;

  if (addr1->s_family == AF_INET)
    return (memcmp(&((sockaddr_in *)addr1)->sin_addr, &((sockaddr_in *)addr2)->sin_addr, 4) == 0);
#ifdef HAVE_IPV6
  else if (addr1->s_family == AF_INET6)
    return (memcmp(&((sockaddr_in6 *)addr1)->sin6_addr, &((sockaddr_in6 *)addr2)->sin6_addr, 16) == 0);
#endif
  return false;
}

bool addrport_matches(_addr *addr1, _addr *addr2) {
  if (address_matches(addr1, addr2)) {
    if (addr1->s_family == AF_INET)
      return ((sockaddr_in *)addr1)->sin_port == ((sockaddr_in *)addr2)->sin_port;
#ifdef HAVE_IPV6
    else if (addr1->s_family == AF_INET6)
      return ((sockaddr_in6 *)addr1)->sin6_port == ((sockaddr_in6 *)addr2)->sin6_port;
#endif
  }
  return false;
}

bool sock_is_ipv6(_addr *a) {
#ifdef HAVE_IPV6
  return (a->s_family == AF_INET6);
#else
  return false;
#endif
}

bool addr_is_ipv6(_addr *a) { return sock_is_ipv6(a); }

bool sock_is_ipv4(_addr *a) {
  return (a->s_family == AF_INET);
}

bool addr_is_ipv4(_addr *a) { return sock_is_ipv4(a); }

unsigned char *get_ipv4_ptr(_addr *a) {
  return (unsigned char *)&((sockaddr_in *)a)->sin_addr;
}

bool addr_is_any(_addr *addr) {
  unsigned char *ptr = get_ipv4_ptr(addr);
  return (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0);
}

bool addr_is_none(_addr *addr) {
  unsigned char *ptr = get_ipv4_ptr(addr);
  return (ptr[0] == 255 && ptr[1] == 255 && ptr[2] == 255 && ptr[3] == 255);
}

#ifdef HAVE_IPV6
unsigned char *get_ipv6_ptr(_addr *a) {
  return (unsigned char *)&((sockaddr_in6 *)a)->sin6_addr;
}
#endif


stl_string addr_to_string(_addr *addr, bool include_port) {
  char *caddr, msg[64];

  if (addr->s_family == AF_INET) {
    /* IPv4 */
    caddr = (char *)&((sockaddr_in *)addr)->sin_addr;
    sprintf(msg, "%d.%d.%d.%d", caddr[0], caddr[1], caddr[2], caddr[3]);
    if (include_port)
      sprintf(msg + strlen(msg),"#%d", ntohs(((sockaddr_in *)addr)->sin_port) & 32767);
    return stl_string(msg);
  }
#ifdef HAVE_IPV6
  if (addr->s_family == AF_INET6) {
    /* IPv6 */
    caddr = (char *)&((sockaddr_in6 *)addr)->sin6_addr;
    sprintf(msg, "%x:%x:%x:%x:%x:%x:%x:%x",
            caddr[0]*256+ caddr[1], caddr[2]*256+ caddr[3], caddr[4]*256+ caddr[5],
            caddr[6]*256+ caddr[7], caddr[8]*256+ caddr[9], caddr[10]*256+ caddr[11],
            caddr[12]*256+ caddr[13], caddr[14]*256+ caddr[15]);
    if (include_port)
      sprintf(msg + strlen(msg), "#%d",ntohs(((sockaddr_in6 *)addr)->sin6_port) & 32767);
    return stl_string(msg);
  }
#endif
  sprintf(msg, "<unknown socket family %d>", addr->s_family);
  return stl_string(msg);
}

smallset_t::smallset_t() {
  nitems = 0;
  items = NULL;
}

smallset_t::~smallset_t() {
  destroy();
}

void smallset_t::init(int size) {
  if (nitems) destroy();
  nitems = size;
  items = (pollfd *)malloc(size * sizeof(pollfd));
}

void smallset_t::set(int ix, int socket) {
  items[ix].fd = socket;
}

void smallset_t::destroy() {
  free(items);
  items = NULL;
  nitems = 0;
}

void smallset_t::check() {
  wait(0);
}

void smallset_t::runpoll(int msecs) {
  int ret;
  while (1) {
	  ret = poll(items, nitems, (msecs >= 1000) ? 1000 : msecs);
	  if (ret < 0 && errno != EINTR) {
      throw PException(true, "Error during poll: %d->%d", ret, errno);
    }
    if (ret > 0) return;
    if (posclient_quitflag) return;
    if (msecs <= 1000) return;
    msecs -= 1000;
//    if (msecs == 0) return;
  }
}

void smallset_t::wait(int msecs) {
  int x;
  if (msecs < 0) msecs = 0;
  for (x = 0; x < nitems; x++) {
    items[x].events = POLLIN;
    items[x].revents = 0;
  }
  runpoll(msecs);
}


void smallset_t::waitwrite(int msecs) {
  int x;
  if (msecs < 0) msecs = 0;
  for (x = 0; x < nitems; x++) {
    items[x].events = POLLOUT;
    items[x].revents = 0;
  }
  runpoll(msecs);
}


bool smallset_t::canwrite(int ix) {
  return (items[ix].revents & POLLOUT);
}

bool smallset_t::isdata(int ix) {
  return (items[ix].revents & POLLIN);
}

bool smallset_t::iserror(int ix) {
  return (items[ix].revents & POLLERR);
}

bool smallset_t::ishup(int ix) {
  return (items[ix].revents & POLLHUP);
}

int getprotocolbyname(const char *name) {
  struct protoent *protocol;
  try {
    int t = txt_to_int(name);
    return t;
  } catch (PException p) { }
  if ((protocol = getprotobyname(name)) == NULL) throw PException(true, "Unknown protocol %s", name);
  return protocol->p_proto;
}

int getserviceportbyname(const char *name) {
  struct servent *service;
  try {
    int t = txt_to_int(name);
    return t;
  } catch (PException p) { }
  if ((service = getservbyname(name, NULL)) == NULL) throw PException(true, "Unknown service %s", name);
  return ntohs(service->s_port);
}
