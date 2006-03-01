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

#ifndef __POSLIB_RESOLVER_H
#define __POSLIB_RESOLVER_H

#include "sysstl.h"

#include "socket.h"
#include "dnsmessage.h"

/*! \file poslib/resolver.h
 * \brief Posadis resolver functionality
 *
 * This file contains the Posadis resolver implementation. It contains the base
 * pos_resolver class, which does not contain the implementation for the
 * resolver. There are two resolver implementations: pos_cliresolver for client
 * programs and pos_srvresolver for server programs.
 */

/*!
 * \brief default flags for the pos_resolver::query function
 *
 * This define contains the default flags for the 'flag' parameter of the
 * pos_resolver::query function. This means trying UDP at first, and if the
 * answer is truncated, try TCP next.
 */
#define Q_DFL 0

/*!
 * \brief 'no-tcp' flag for pos_resolver::query
 *
 * This flag instructs the pos_resolver::query function not to retry the query
 * using TCP if the UDP answer is truncated. This means the answer returned by
 * the function might be truncated, which can be checked by looking at the
 * DnsMessage::TC flag.
 */
#define Q_NOTCP 1

class WaitAnswerData {
 public:
  WaitAnswerData(u_int16 _r_id, _addr& _from);
  u_int16 r_id;
  _addr from;
};

/*!
 * \brief Posadis abstract resolver class
 *
 * This is the abstract base class for the Posadis resolver functionality. The
 * pos_resolver class offers functions for both UDP and TCP resolving. For UDP
 * resolving, use the high-level query() functions. sendmessage() and
 * waitanswer() are low-level functions that applications would not normally
 * want to call. The same goes for TCP: tcpconnect() tcpdisconnect() and
 * tcpquery() are the functions it's all about.
 *
 * You can modify some parameters by setting member functions of the class, and
 * by specifying custom flags for the query functions.
 *
 * Like we mentioned, this is an abstract class that should _not_ be used
 * directly. For client applications, use the pos_cliresolver class, and for
 * server applications, use pos_srvresolver.
 *
 * Note that pos_resolver actually _does_ contain a complete TCP
 * implementation, so, theoretically it could be instantiated directly for use
 * as a TCP resolver. This is however not encouraged.
 */
class pos_resolver {
  public:
  /*!
   * \brief default constructor
   *
   * This constructor intializes the resolver, and sets the ::udp_tries,
   * ::n_udp_tries and ::tcp_timeout values to their defaults.
   */
  pos_resolver();

  /*!
   * \brief destructor
   *
   * This function clears all memory associated with the resolver object.
   */
  virtual ~pos_resolver();

  /*!
   * \brief number of UDP attempts
   *
   * Sets the number of times Posadis attempts to retry querying using UDP if
   * a previous attempt did not return an answer. If you change this value,
   * you should also change #udp_tries to have at least as many items as this
   * value.
   */
  int n_udp_tries;

  /*!
   * \brief UDP timeout values
   *
   * This is an array of UDP retry values, in milliseconds. This array can be
   * changed by hand. If you want to change the dimensions, a mere realloc()
   * is enough. Just make sure it has at least as many items as the
   * #n_udp_tries value, otherwise your application might crash :(
   */
  int *udp_tries;

  /*!
   * \brief TCP timeout value
   *
   * This is the time, in milliseconds, we're willing to wait for TCP
   * transmission. Because there are four operations involved in a TCP query
   * (two send operations and two read operations), each operation gets 25%
   * of this timeout to complete. Since TCP is a connection-based protocol,
   * we're not responsible for retransmission, thus we only attempt queries
   * once.
   */
  int tcp_timeout;

  /*!
   * \brief high-level query function
   *
   * This function will query the given DNS server for the information
   * identified by the DNS query message q. If it succeeds, it will return and
   * put the answer from the server in a, which need not be initialized
   * previously (in fact, this will result in a memory leak). If not, it will
   * raise a PException.
   *
   * If the query() function does not receive an answer in time, it will retry
   * for #n_udp_tries times, using the timeout values from the #udp_tries
   * array. If the answer it receives is truncated, it will retry using TCP,
   * unless instructed not to by the flags parameter.
   *
   * The behavior of the query function can be changed by the flags parameter.
   * Currently, this can only be Q_DFL (default flags) or Q_NOTCP (do not retry
   * using UDP).
   * \param q The DNS query message
   * \param a Variable to put the answer in
   * \param server The server to query
   * \param flags Flags controlling query behavior.
   */
  virtual void query(DnsMessage *q, DnsMessage*& a, _addr *server, int flags = Q_DFL);

  /*!
   * \brief high-level query function using multiple servers
   *
   * This function generally behaves the same as the query() function, except
   * it takes a list of servers instead of one. The query algorithm differs in
   * that for each timeout value from #udp_tries, all servers will be queried.
   * Also, if the answer is truncated, _only_ the server that returned the
   * truncated answer will be tried using TCP. This function will start
   * querying at a random place in the servers list; after that, it will run
   * through all servers listed in the order in which you specify them.
   * \param servers List of servers to query
   * \sa query()
   */
  virtual void query(DnsMessage *q, DnsMessage*& a, stl_slist(_addr) &servers, int flags = Q_DFL);

  /*!
   * \brief low-level resolver function for sending a message
   *
   * This function sends a DNS message to a specified server using UDP.
   * \param msg The DNS message to send
   * \param res The host to send the message to
   * \param sockid Implementation-dependent argument.
   */
  virtual void sendmessage(DnsMessage *msg, _addr *res, int sockid = -1);

  /*!
   * \brief low-level resolver function for waiting for an answer
   *
   * This function waits for at most the amount of milliseconds specified
   * by timeout until an answer to our query arrives. Since multiple
   * messages for the same query might have been sent out, it asks for a list
   * of sent queries.
   *
   * If no answer is received in time, this function will raise an exception.
   * \param ans If an answer is received, it will be put in this variable
   * \param wait List of sent queries we might get an answer to
   * \param timeout Number of milliseconds to wait at most
   * \param it If an answer is received, this iterator will point to the
   *           message this was an answer to.
   * \param sockid Implementation-dependent argument.
   */
  virtual bool waitanswer(DnsMessage*& ans, stl_slist(WaitAnswerData)& wait, int timeout, stl_slist(WaitAnswerData)::iterator& it, int sockid = -1);

  /*!
   * \brief establishes a TCP connection to a DNS server
   *
   * This function will try to connect to the given address using TCP.
   * \param res The server to connect to
   * \return A socket identifier to use for other tcp resolver functions
   * \sa tcpdisconnect()
   */
  virtual int tcpconnect(_addr *res);

  /*!
   * \brief disconnects from a DNS server
   *
   * This function will disconnect from a server we connected to earlier using
   * tcpconnect().
   * \sa tcpconnect()
   */
  virtual void tcpdisconnect(int sockid);

  /*!
   * \brief TCP query function
   *
   * This is the high-level TCP query function. It will query the server we
   * connected to, and put the answer to the query q in a. It will wait at
   * most #tcp_timeout milliseconds before it receives an answer.
   *
   * If the function receives a message which does not answer our query, it
   * will stop waiting and raise an exception.
   * \param q The DNS query message
   * \param a Variable to put the answer in
   * \param sockid The TCP connection (as returned by tcpconnect())
   */
  virtual void tcpquery(DnsMessage *q, DnsMessage*& a, int sockid);

  /*!
   * \brief TCP low-level function for sending a message
   *
   * This function sends a DNS message over a TCP connection.
   * \param msg The message to send
   * \param sockid The TCP connection (as returned by tcpconnect())
   */
  virtual void tcpsendmessage(DnsMessage *msg, int sockid);

  /*!
   * \brief TCP low-level function for waiting for an answer
   *
   * This function waits for at most #tcp_timeout milliseconds until it
   * receives an answer from the server on the other end of the line. Unlike
   * its UDP conterpart, this function does not check whether the server
   * actually answered our query.
   * \param ans Variable to put the received message in
   * \param sockid The TCP connection (as returned by tcpconnect())
   */
  virtual void tcpwaitanswer(DnsMessage*& ans, int sockid);
};

/*!
 * \brief resolver for client applications
 *
 * This is an implementation of the pos_resolver class meant for client
 * applications. It does not maintain a centralized query database like
 * pos_srvresolver. Instead, it will open up a new socket for each query it
 * attempts. The advantage is that it does not require the multi-thread
 * architecture pos_srvresolver depends on.
 *
 * pos_cliresolver implements the pos_resolver query(), sendmessage() and
 * waitanswer() functions.
 */

class pos_cliresolver : public pos_resolver {
  public:
  /*!
   * \brief resolver constructor
   *
   * Resolver for the client resolver.
   */
  pos_cliresolver();

  /*!
   * \brief destructor
   *
   * Destructor for the client resolver
   */
  virtual ~pos_cliresolver();
  
  void query(DnsMessage *q, DnsMessage*& a, _addr *server, int flags = Q_DFL);
  void query(DnsMessage *q, DnsMessage*& a, stl_slist(_addr) &servers, int flags = Q_DFL);
  /*!
   * \brief low-level resolver function for sending a message
   *
   * This function sends a DNS message to a specified server using UDP.
   * \param msg The DNS message to send
   * \param res The host to send the message to
   * \param sockid The socket to use
   */
  void sendmessage(DnsMessage *msg, _addr *res, int sockid = -1);
  /*!
   * \brief low-level resolver function for waiting for an answer
   *
   * This function waits for at most the amount of milliseconds specified
   * by timeout until an answer to our query arrives. Since multiple
   * messages for the same query might have been sent out, it asks for a list
   * of sent queries.
   *
   * If no answer is received in time, this function will raise an exception.
   * \param ans If an answer is received, it will be put in this variable
   * \param wait List of sent queries we might get an answer to
   * \param timeout Number of milliseconds to wait at most
   * \param it If an answer is received, this iterator will point to the
   *           message this was an answer to.
   * \param sockid The socket id the answers will come from.
   */
  bool waitanswer(DnsMessage*& ans, stl_slist(WaitAnswerData)& wait, int timeout, stl_slist(WaitAnswerData)::iterator& it, int sockid = -1);

  /*!
   * \brief stops the resolving process asap
   *
   * This function will try to stop the resolving process as soon as possible.
   * Thus, it will need to be called asynchronously (since the query functions
   * block), either from another thread or from a signal handler. It will tease
   * the query functions a bit by closing their sockets, and urge them to
   * quit.
   */
  void stop();
 private:
  void clrstop();
  int sockid;
  bool quit_flag;
  bool is_tcp;
#ifndef _WIN32
  int clipipes[2];
#endif
};

#endif /* __POSLIB_RESOLVER_H */
