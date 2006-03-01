/*
    Posadis - A DNS Server
    Lexical functions
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

#ifndef __POSLIB_LEXFN_H
#define __POSLIB_LEXFN_H

/*! \file poslib/lexfn.h
 * \brief lexical functions
 *
 * Functions for converting free-form text to various types of data.
 */

#include "types.h"
#include "socket.h"
#include "sysstl.h"
#include "dnsdefs.h"

/*!
 * \brief convert text to boolean
 *
 * Converts text to a boolean value. The following positive values are
 * supported: \p yes \p true \p 1 \p on \p yo \p absolutely . The following
 * negative values are supported: \p no \p false \p 0 \p off \p nope \p never .
 * \param buff Buffer containing boolean
 * \return Either true or false.
 */
bool txt_to_bool(const char *buff);

/*!
 * \brief convert text to numbers
 *
 * Converts text to a number. This function uses postfix operators, and can
 * also handle negative amounts.
 * \param buff Buffer containing text
 * \return Numeric value of the buffer
 * \sa #txt_to_int
 */
int txt_to_negint(const char *buff);

/*!
 * \brief convert text to numbers
 *
 * Converts text to a number. This function uses postfix operators. It doesn't
 * support negative numbers.
 * \param buff Buffer containing text
 * \return Numeric value of the buffer
 * \sa #txt_to_negint
 */
int txt_to_int(const char *buff);

/*!
 * \brief convert text to ipv4
 *
 * Converts the text to an IPv4 address. As an extension, Poslib also supports
 * the literal \p any value, which maps to \p 0.0.0.0 and the \p local value,
 * which maps to \p 127.0.0.1 .
 * \param ip Target
 * \param buff Source
 * \param do_portion If true, also accept portions such as 192.*.
 * \return The number of nodes of the address (4 for a complete one)
 */
int txt_to_ip(unsigned char ip[4], const char *buff, bool do_portion = false);

/*!
 * \brief Size of an IP range buffer
 *
 * The size, in characters, of an IP range buffer.
 */
#define sz_iprange 8

/*!
 * \brief Convert text to an IP range
 *
 * Converts the buffer to an IPv4 IP range, in the form of ip[/nsig].
 * \param iprange Result buffer (should be of size #sz_iprange).
 * \param val String value describing the range
 */
void txt_to_iprange(unsigned char *iprange, const char *val);

/*!
 * \brief Checks for IPv4 ranges
 *
 * Returns \p true if the given IP number is in the given IP range.
 * \param iprange The IP range
 * \param ip The IPv4 number
 * \return \p true if the ip is within the range
 * \sa #txt_to_ip, #txt_to_iprange
 */
bool iprange_matches(const unsigned char *iprange, const unsigned char *ip);

/*!
 * \brief convert text to ipv6
 *
 * Converts the text to an IPv6 address. As an extension, Poslib also supports
 * the literal \p :any value, mapping to \p ::0 and the \p :local value,
 * mapping to \p ::1 .
 *
 * \param ipv6 Target
 * \param buff Source
 * \param do_portion If true, also accepts portions such as dead:beef:*
 * \return The number of nodes of the address (16 for a complete one)
 */
int txt_to_ipv6(unsigned char ipv6[16], const char *buff, bool do_portion = false);


/*!
 * \brief Size of an IPv6 range buffer
 *
 * The size, in characters, of an IPv6 range buffer.
 */
#define sz_ip6range 32

/*!
 * \brief Convert text to an IPv6 range
 *
 * Converts the buffer to an IPv6 IP range, in the form of ip[/nsig].
 * \param iprange Result buffer (should be of size #sz_ip6range).
 * \param val String value describing the range
 */
void txt_to_ip6range(const char *iprange, const char *val);

/*!
 * \brief Checks for IPv6 ranges
 *
 * Returns \p true if the given IPv6 number is in the given IP range.
 * \param iprange The IPv6 range
 * \param ip The IPv6 number
 * \return \p true if the ip is within the range
 * \sa #txt_to_ipv6, #txt_to_ip6range
 */
bool ip6range_matches(const unsigned char *iprange, const unsigned char *ip);

/*!
 * \brief Size of an generic address range buffer
 *
 * The size, in characters, of an generic address range buffer.
 */
#define sz_addrrange 33

/*!
 * \brief Convert text to an address range
 *
 * Converts the buffer to an address IP range, in the form of ip[/nsig].
 * \param iprange Result buffer (should be of size #sz_addrrange).
 * \param val String value describing the range
 */
void txt_to_addrrange(unsigned char *iprange, const char *val);

/*!
 * \brief Checks for range matches
 *
 * Returns \p true if the given address number is in the given IP range.
 * \param iprange The address range
 * \param a The address
 * \return \p true if the ip is within the range
 * \sa #txt_to_addrrange
 */
bool addrrange_matches(const unsigned char *iprange, _addr *a);

/*!
 * \brief Address range class
 */
class addrrange { public: unsigned char range[sz_addrrange]; };

/*!
 * \brief looks item up in match list
 *
 * This function returns true if the given address matches one of the address
 * match items of the address match list.
 * \param lst Address range list
 * \param a Address to check
 * \return true if the address matches one of the items in the list
 */
bool in_addrrange_list(stl_list(addrrange) &lst, _addr *a);

#ifdef HAVE_SLIST
/*!
 * \brief looks item up in match list
 *
 * This function returns true if the given address matches one of the address
 * match items of the address match list.
 * \param lst Address range list
 * \param a Address to check
 * \return true if the address matches one of the items in the list
 */
bool in_addrrange_list(stl_slist(addrrange) &lst, _addr *a);
#endif

/*!
 * \brief looks item up in address list
 *
 * This function returns true if the given address is one of the addresses
 * in the address match list.
 * \param lst Address list
 * \param a Address to check
 * \param match_port Whether the port should also match
 * \return true if the address matches one of the items in the list
 */
bool in_addr_list(stl_list(_addr) &lst, _addr *a, bool match_port = false);

#ifdef HAVE_SLIST
/*!
 * \brief looks item up in address list
 *
 * This function returns true if the given address is one of the addresses
 * in the address match list.
 * \param lst Address list
 * \param a Address to check
 * \param match_port Whether the port should also match
 * \return true if the address matches one of the items in the list
 */
bool in_addr_list(stl_slist(_addr) &lst, _addr *a, bool match_port = false);
#endif

/*!
 * \brief convert e-mail address to binary domain name
 *
 * Converts the domain name or email address in src to a binary domain name.
 * \param target Target
 * \param src Source
 * \param origin Origin for relative domain names
 */
void txt_to_email(_domain target, const char *src, _cdomain origin = NULL);

/*!
 * \brief convert text domain name to binary domain name
 *
 * Converts the domain name or in src to a binary domain name.
 * \param target Target
 * \param src Source
 * \param origin Origin for relative domain names
 */
void txt_to_dname(_domain target, const char *src, _cdomain origin = NULL);

/*!
 * \brief convert text to address
 *
 * Converts the text pointed to by addr to an _addr address structure. If
 * the \p client parameter is set to true, the default IP is \p 127.0.0.1, else
 * it is \p 0.0.0.0 . Addresses can be given by only an address, only a port,
 * or a combination separated by a \p # . Being based on the #txt_to_ip and
 * #txt_to_ipv6 functions, this function also supports the literval values
 * \c any , \c local, \c :any and \c :local .
 * \param ret Memory to store result in
 * \param addr Text describing the address
 * \param default_port Default port if none is given
 * \param client Influences default address
 */
void txt_to_addr(_addr *ret, const char *addr, int default_port = DNS_PORT, bool is_client = true);

/*!
 * \brief convert text to LOC RR
 *
 * Converts the text pointed to by rr to LOC information as described in
 * RFC 1876.
 * \param ret Memory to store result in (should be >= 16 bytes)
 * \param rr String containing the data (after processing, this will point to the
 *           end of the RR)
 */
void txt_to_loc(unsigned char *res, char *&src);

/*!
 * \brief convert text to DNS class
 *
 * Converts text to one of the class constants supported by DNS (#CLASS_IN,
 * #CLASS_HS, #CLASS_CH, #CLASS_CS), or, if #allow_q is set, possibly to a
 * supported QCLASS (#CLASS_NONE or #CLASS_ANY). Only symbolic constants are
 * allowed; numeric values result in an exception.
 * \param str String to convert
 * \param allow_q Whether to allow QCLASSes (defaults to true)
 * \return A DNS CLASS or QCLASS constant
 */
uint16_t txt_to_qclass(const char *str, bool allow_q = true);

stl_string str_type(u_int16 type); /**< Returns string representation for the RR type. */
stl_string str_qtype(u_int16 qtype); /**< Returns string representation for the QTYPE. */
stl_string str_class(u_int16 ctype); /**< Returns string representation for the RR CLASS. */
stl_string str_qclass(u_int16 qctype); /**< Returns string representation for the QCLASS. */
stl_string str_opcode(u_int16 opcode); /**< Returns string representation for the OPCODE. */
stl_string str_rcode(int rcode); /**< Returns string representation for the RCODE. */
stl_string str_ttl(uint32_t ttl); /**< Returns string representation for the ttl (e.g. 2h1m) */
stl_string str_loc(const unsigned char *rr); /**< Returns string representation for a LOC RR */
stl_string intstring(u_int16 x); /**< Converts int to string */

#endif /* __POSLIB_LEXFN_H */
