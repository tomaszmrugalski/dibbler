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

#ifndef __POSLIB_SOCKET_H
#define __POSLIB_SOCKET_H

#include "syssocket.h"
#include "sysstl.h"

/*! \file poslib/socket.h
 * \brief system-indepent socket functions
 *
 * System-indepent socket functions.
 */

/*!
 * \brief address type
 *
 * This type offers a system-independent type representing an IPv4/IPv6
 * address. Internally, this is a typedef to sockaddr_storage.
 */
typedef sockaddr_storage _addr;

#define UDP_MSG_SIZE 512    /**< Maximum size of an UDP packet. */
#define TCP_MSG_SIZE 65536  /**< Maximum size of a TCP packet. */

#define DNS_PORT 53         /**< Default port for DNS. */

#define T_UDP 1             /**< Constant for UDP connections. */
#define T_TCP 2             /**< Constant for TCP connections. */

/** Set to true if you want to close down your application when sockets might
    still be active. */
extern bool posclient_quitflag;

/* udp socket functions */

/** Opens an UDP server at the specified address/port, returning the socket id. */
int udpcreateserver(_addr *a);
/** Closes an UDP connection, both server and client. */
void udpclose(int sockid);

/** Reads data from an UDP connection. The address of the sender is put in addr. */
int udpread(int sockid, const char *buff, int len, _addr *addr);

/** Sends data to the specified server through UDP. */
void udpsend(int sockid, const char *buff, int len, _addr *addr);

/* tcp socket functions */

/** Opens a TCP server at the specified address/port, returning the socket id. */
int tcpcreateserver(_addr *a);
/** Opens a TCP client connection to the specified server. */
int tcpopen(_addr *a);
/** Opens a TCP client coonection to the specified server from the specificed source address. */
int tcpopen_from(_addr *to, _addr *source);
/** Closes a TCP client/server connection. */
void tcpclose(int sockid);
/** Accepts a client connection on a TCP server connection. */
int tcpaccept(int sockid, _addr *addr);

/** Sends data through the TCP connection. Doesn't guarantee all data is sent, but returns immediately. */
int tcpsend(int sockid, const char *buff, int len);
/** Sends all data through the TCP connection. Take at most \p maxtime milliseconds. */
void tcpsendall(int sockid, const char *buff, int len, int maxtime);
/** Reads data from the TCP connection. Doesn't guarantee all data is read, but returns immediately. */
int tcpread(int sockid, const char *buff, int len);
/** Reads \p len bytes through the TCP connection. Take at most \p maxtime milliseconds. */
void tcpreadall(int sockid, const char *buff, int len, int maxtime);

/** Checks whether the TCP connection is still open. */
bool tcpisopen(int sockid);

/* address functions */

/** Converts an IPv4 binary address to an _addr structure. */
void getaddress_ip4(_addr *res, const unsigned char *ipv4_data, int port = 0);

#ifdef HAVE_IPV6
/** Converts an IPv6 binary address to an _addr structure. */
void getaddress_ip6(_addr *res, const unsigned char *ipv6_data, int port = 0);
#endif
/** Converts an textual address (either IPv4/IPv6) to an _addr structure. */
void getaddress(_addr *res, const char *data, int port = 0);
/** Looks up the specified domain name using the system resolver, and creates an _addr structure. */
bool address_lookup(_addr *res, const char *name, int port);

/** Sets the port of an _addr structure. */
void addr_setport(_addr *addr, int port);
/** Gets the port of an _addr structure. */
int addr_getport(_addr *addr);

/** Checks whether both _addr structures point to the same address. */
bool address_matches(_addr *a1, _addr *a2);
/** Checks whether both _addr structures point to the same address and port. */
bool addrport_matches(_addr *a1, _addr *a2);

/** Checks whether the address is an IPv4 address (obsolete, use #addr_is_ipv6). */
bool sock_is_ipv6(_addr *a);
/** Checks whether the address is an IPv4 address. */
bool addr_is_ipv6(_addr *a);
/** Checks whether the address is an IPv6 address (obsolete, use #addr_is_ipv4). */
bool sock_is_ipv4(_addr *a);
/** Checks whether the address is an IPv6 address. */
bool addr_is_ipv4(_addr *a);

/** Returns pointer to the four bytes of the IPv4 address. */
unsigned char *get_ipv4_ptr(_addr *a);

bool addr_is_any(_addr *addr);  /** Returns true if the given address is the IPv4 any address */
bool addr_is_none(_addr *addr); /** Returns true if the given address is the IPv4 none address */

/** Returns pointer to the sixteen bytes of the IPv6 address. */
#ifdef HAVE_IPV6
unsigned char* get_ipv6_ptr(_addr *a);
#endif

/** Converts the _addr structure to a human-readable string. */
stl_string addr_to_string(_addr *addr, bool include_port = true);

/* small watchset functions */

/*!
 * \brief checks whether data is avaiable on sockets
 *
 * This is a structure which is used to track whether data is avaible on a
 * series of sockets. As such, it is a replacement for, and actually a wrapper
 * for, the standard Unix poll() and select() functions.
 */
class smallset_t {
 public:
  smallset_t();                 /**< Constructor. */
  ~smallset_t();                /**< Destructor. */

  void init(int ix);            /**< Intiailizes the structure to hold \p ix sockets. */
  void set(int ix, int socket); /**< Adds the socket at the specified index. */

  void check();                 /**< Check the status of the sockets. */
  void waitwrite(int msecs);    /**< Wait for at most the specified time until we can write on a socket. */
  void wait(int msecs);         /**< Wait for at most the specified time until data is received. */

  bool canwrite(int ix);        /**< Returns true if you can write non-blockingly to the socket at \p ix. */
  bool isdata(int ix);          /**< Returns true if you can read non-blockingly from the socket at \p ix. */
  bool iserror(int ix);         /**< Returns true if an error occured on the socket at \p ix. */
  bool ishup(int ix);           /**< Returns true if the connection was hung up. */

 private:
  void runpoll(int msecs);
  void destroy();
  int nitems;    /**< Number of sockets. */
  pollfd *items; /**< Information for the sockets. */
};

/*!
 * \brief returns the internet protocol ID from name
 *
 * Uses the system's getprotobyname() function to look up the service ID for
 * a given protocol (usually, "udp" or "tcp"). Additionally, if the given
 * string is a number, it will return that number.
 *
 * \param name The protocol name
 * \return The internet protocol ID
 */
int getprotocolbyname(const char *name);

/*!
 * \brief returns the internet service port from name
 *
 * Uses the system's getservbyname() function to look up the port for
 * a given service (for example, "http" or "ftp"). Additionally, if the given
 * string is a number, it will return that number.
 *
 * \param name The service name
 * \return The service port
 */
int getserviceportbyname(const char *name);

#endif /* __POSLIB_SOCKET_H */
