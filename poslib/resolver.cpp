/*
    Posadis - A DNS Server
    Dns Resolver API
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

#include "socket.h"
#include "exception.h"
#include "random.h"
#include "resolver.h"
#include "postime.h"

#include <stdio.h>

#ifndef _WIN32
#include <pthread.h>
#include <unistd.h>
#endif

WaitAnswerData::WaitAnswerData(u_int16 _r_id, _addr &_from) {
  r_id = _r_id;
  from = _from;
}

pos_resolver::pos_resolver() {
  /* default values */
  n_udp_tries = 1;
  udp_tries = (int *)malloc(3 * sizeof(int));
  udp_tries[0] = 1000;
  udp_tries[1] = 3000; // ignored for now
  udp_tries[2] = 6000; // ignored for now
  tcp_timeout = 1000;
}

pos_resolver::~pos_resolver() {
  free(udp_tries);
}

void pos_resolver::query(DnsMessage *q, DnsMessage*& a, _addr *server, int flags) { }
void pos_resolver::query(DnsMessage *q, DnsMessage*& a, stl_slist(_addr) &servers, int flags) { }
void pos_resolver::sendmessage(DnsMessage *msg, _addr *res, int sockid) { }
bool pos_resolver::waitanswer(DnsMessage*& ans, stl_slist(WaitAnswerData)& wait, int timeout, stl_slist(WaitAnswerData)::iterator& it, int sockid) { return false; }

/* tcp queries */

int pos_resolver::tcpconnect(_addr *res) {
  return tcpopen(res);
}

void pos_resolver::tcpdisconnect(int sockid) {
  tcpclose(sockid);
}

void pos_resolver::tcpquery(DnsMessage *q, DnsMessage*& a, int sockid) {
  q->ID = posrandom();
  tcpsendmessage(q, sockid);
  tcpwaitanswer(a, sockid);
  if (a->ID != q->ID) {
    delete a; a = NULL;
    throw PException("Answer ID does not match question ID!");
  }
}

void pos_resolver::tcpsendmessage(DnsMessage *msg, int sockid) {
  unsigned char len[2];
  message_buff buff = msg->compile(TCP_MSG_SIZE);
  if (buff.len > 65536) return;
  len[0] = buff.len / 256;
  len[1] = buff.len;
  tcpsendall(sockid, (char*)len, 2, tcp_timeout / 4);
  tcpsendall(sockid, (char*)buff.msg, buff.len, tcp_timeout / 4);
}

void pos_resolver::tcpwaitanswer(DnsMessage*& ans, int sockid) {
  unsigned char len_b[2];
  unsigned char *msg = NULL;
  int len;
  postime_t end = getcurtime() + tcp_timeout;

  try {
    ans = NULL;
    tcpreadall(sockid, (char*)len_b, 2, end.after(getcurtime()));
    len = len_b[0] * 256 + len_b[1];
    msg = (unsigned char *)malloc(len);
    tcpreadall(sockid, (char*)msg, len, end.after(getcurtime()));
    ans = new DnsMessage();
    ans->read_from_data(msg, len);
  } catch(PException p) {
    if (ans) { delete ans; ans = NULL; }
    if (msg) { delete msg; msg = NULL; }
    throw p;
  }
  if (msg) { delete msg; msg = NULL; }
}

/* stand-alone client resolver */

pos_cliresolver::pos_cliresolver() :
  pos_resolver() {
  sockid = -1;
  quit_flag = false;
#ifndef _WIN32
  pipe(clipipes);
#endif
}

pos_cliresolver::~pos_cliresolver() {
#ifndef _WIN32
    close(clipipes[0]);
    close(clipipes[1]);
#endif
}

void pos_cliresolver::stop() {
  quit_flag = true;
  if (sockid > 0) {
#ifdef _WIN32
    if (is_tcp) 
	tcpclose(sockid); 
    else 
	udpclose(sockid);
    sockid = -1;
#else
    write(clipipes[1], "x", 1);
#endif
  }
}

void pos_cliresolver::clrstop() {
  quit_flag = false;
#ifndef _WIN32
  char buff;
  smallset_t set;
  set.init(1);
  set.set(0, clipipes[0]);
  set.check();
  while (set.isdata(0)) {
    read(clipipes[0], &buff, 1);
    set.check();
  }
#endif
}

void pos_cliresolver::query(DnsMessage *q, DnsMessage*& a, _addr *server, int flags) {
  stl_slist(_addr) servers;
  servers.push_front(*server);
  query(q, a, servers, flags);
}


void pos_cliresolver::query(DnsMessage *q, DnsMessage*& a, stl_slist(_addr) &servers, int flags) {
  int x = -1;
  stl_slist(_addr)::iterator server, sbegin;
  stl_slist(WaitAnswerData) waitdata;
  stl_slist(WaitAnswerData)::iterator it;
  int ipv4sock = 0;
#ifdef HAVE_IPV6
  int ipv6sock = 0;
#endif
  unsigned char addr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  _addr tmp;

  clrstop();

  if (servers.empty()) throw PException("Empty servers list for query");
  
  int z = posrandom() % servers.size();
  sbegin = servers.begin();
  while (z) { z--; sbegin++; }

  while (++x < n_udp_tries) {
    server = sbegin;
    do {
      try {
        /* register and assign a new query ID */
        q->ID = posrandom();
#ifdef HAVE_IPV6
        if (sock_is_ipv6(&*server)) {
          if (!ipv6sock) {
            getaddress_ip6(&tmp, addr, 0);
            ipv6sock = udpcreateserver(&tmp);
          }
          sockid = ipv6sock;
        } else
#endif
          if (sock_is_ipv4(&*server)) {
          if (!ipv4sock) {
            getaddress_ip4(&tmp, addr, 0);
            ipv4sock = udpcreateserver(&tmp);
          }
          sockid = ipv4sock;
        } else throw PException("Unknown address family");
        sendmessage(q, &*server, sockid);
        waitdata.push_front(WaitAnswerData(q->ID, *server));
        if (waitanswer(a, waitdata, udp_tries[x], it, sockid)) {
          /* answer received */
          if (a->TC && flags == Q_DFL) {
            delete a;
            a = NULL;
            /* retry using TCP */
            sockid = 0;
            try {
              sockid = tcpconnect(&it->from);
              tcpquery(q, a, sockid);
            } catch (PException p) {
              tcpdisconnect(sockid);
              throw PException("Failed to retry using TCP: ", p);
            }
            tcpdisconnect(sockid);
          } else if (a->RCODE == RCODE_SRVFAIL ||
                     a->RCODE == RCODE_REFUSED ||
                     a->RCODE == RCODE_NOTIMP) {
            stl_slist(_addr)::iterator tmpit = server;
            tmpit++; if (tmpit == servers.end()) tmpit = servers.begin();
            if (tmpit != sbegin) throw PException("Answer has error RCODE");
          }
#ifdef HAVE_IPV6
          if (ipv6sock) udpclose(ipv6sock); ipv6sock = 0;
#endif
          if (ipv4sock) udpclose(ipv4sock); ipv4sock = 0;
          return;
        } else if (quit_flag) throw PException("Interrupted");
      } catch(PException p) {
#ifdef HAVE_IPV6
        if (ipv6sock) udpclose(ipv6sock); ipv6sock = 0;
#endif
        if (ipv4sock) udpclose(ipv4sock); ipv4sock = 0;
        stl_slist(_addr)::iterator s2 = server; s2++;
        if (s2 == servers.end()) s2 = servers.begin();
        if (s2 == sbegin) throw PException("Resolving failed: ", p);
      }
      server++;
      if (server == servers.end()) server = servers.begin();
    } while (server != sbegin);
  }
#ifdef HAVE_IPV6
  if (ipv6sock) udpclose(ipv6sock); ipv6sock = 0;
#endif
  if (ipv4sock) udpclose(ipv4sock); ipv4sock = 0;
  throw PException("No server could be reached!");
}

void pos_cliresolver::sendmessage(DnsMessage *msg, _addr *res, int sockid) {
  message_buff buff = msg->compile(UDP_MSG_SIZE);
  udpsend(sockid, (char*)buff.msg, buff.len, res);
}

bool pos_cliresolver::waitanswer(DnsMessage*& ans, stl_slist(WaitAnswerData)& wait, int timeout, stl_slist(WaitAnswerData)::iterator& wit, int sockid) {
  /* client implementation */
  _addr src;
  smallset_t set;
  postime_t end = getcurtime() + timeout;

  while (1) {
#ifdef _WIN32
    set.init(1);
    set.set(0, sockid);
#else
    set.init(2);
    set.set(0, sockid);
    set.set(1, clipipes[0]);
#endif
    set.wait(end.after(getcurtime()));
#ifndef _WIN32
    if (set.isdata(1)) {
      char data;
      read(clipipes[0], &data, 1);
    }
#endif

    if (!set.iserror(0) && !set.ishup(0) && set.isdata(0)) {
      unsigned char msg[UDP_MSG_SIZE];
      int len = udpread(sockid, (char*)msg, sizeof(msg), &src);

      wit = wait.begin();
      while (wit != wait.end()) {
        if (address_matches(&wit->from, &src)) {
          try {
            ans = new DnsMessage();
            ans->read_from_data(msg, len);
          } catch (PException p) {
            if (len >= 12 && (msg[2]&2)) {
              /* message was truncated */
              delete ans;
              ans = new DnsMessage();
              ans->TC = true;
              return true;
            }
            delete ans;
            ans = NULL;
            throw p;
          }
          return true;
        }
        wit++;
      }
      /* the answer was not from someone we wanted */
      throw PException("Got answer from unexpected server!");
    } else return false;
  }
//  return false;
}

