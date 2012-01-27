/*
    Posadis - A DNS Server
    Include all poslib headers
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

#ifndef __POSLIB_POSLIB_H
#define __POSLIB_POSLIB_H

/*! \mainpage Poslib documentation
 *
 * \section intro Introduction
 * Poslib is a DNS client/server library written in C++, which is available for
 * many different platforms, including Linux, FreeBSD and other Unices, but
 * also the Windows family of operating systems. This documentation describes
 * the C++ interface of Poslib.
 *
 * Poslib consists of two parts: a client part, \p libposlib, and a server
 * part, \p libposserver. The client library offers functionality for
 * resolving (see pos_cliresolver), domain-name manupulation (the domainname
 * class), and DNS message creation (the DnsMessage class). Also, it provices
 * a system-independent abstaction layer for socket functionality (socket.h).
 * The functions of the client library can be accessed by
 * #including <poslib/poslib.h>.
 *
 * The server library, based on the client core, can be used to develop Domain
 * Name System servers. By implementing a query entry-point function using the
 * Poslib library of functions, you can easily create DNS servers, without
 * worrying about low-level details such as DNS message compilation,
 * domain-name compression and UDP/TCP transmission. It also contains a
 * generic logging mechanism, as well as a threading system based on pthreads.
 * The poslib server functionality is in <poslib/server/server.h>.
 *
 * See also:
 *   - \ref client,
 *   - \ref server.
 */
/*!
 * \page client Using the client library
 *
 * \subsection clientexceptions Exceptions
 *
 * The Poslib library is built using 100% pure C++. One of the implications of
 * this is, that Poslib uses \b exceptions all the way. This is a very
 * important thing, because when a poslib function fails to do its job, it will
 * throw an exception to notify you of its failure, instead of just returning
 * an error value the C way. For this exception handling, Poslib uses its own
 * exception class, PException, which has one member variable,
 * PException::message, which will contain a description of the error. For
 * example, you could call a Poslib function thus:
 *
 * \code
 * try {
 *   domainname x = "www"; // this might throw an exception
 *   x += "acdam.net";     // this might fail as well
 * } catch (PException p) {
 *   printf("Error during domain name construction: %s\n", p.message);
 * }
 * \endcode
 *
 * Another issue is the usage of the STL. Poslib uses the STL on several
 * occasions, including the nonstandard \p slist. Poslib refers to this
 * STL objects as \p stl_slist, \p stl_list and \p stl_string. This is because
 * for debug builds, we use the \p malloc_alloc allocator. These typedefs make
 * life with STL much easier. So, when you use Poslib STL objects, be sure to
 * refer to them using the \p stl_* typedefs.
 *
 * Either way, enough about this, let's start writing some code! Here's a
 * simple example, that will lookup an IP address for a given domain name:
 *
 * \include host.cpp
 *
 * This example uses the following Poslib functionality:
 *  - #txt_to_addr() to interpret the argument specifying the server address
 *  - #create_query() to create a query DnsMessage object
 *  - pos_cliresolver::query() to query the DNS server
 *  - #get_a_record() to extract the A record from the answer
 *
 * Also, note the use of the \p try...catch block with the PException object
 * we used here. Any error that might occur will be caught by this block and
 * reported to the user.
 *
 * On my Linux box, I compiled this code using the following command (this
 * should work on any Unix box, and possibly also under Mingw's MSys for
 * Windows):
 * \verbatim g++ `poslib-config --libs --cflags` host.cpp -o host \endverbatim
 *
 * You can read more information about the various pieces Poslib contains by
 * viewing the source file and class documentation.
 */

/*! \file poslib/poslib.h
 * \brief Poslib main include file
 *
 * This file can be used to include all Poslib client header files in one.
 * See \ref client.
 */

#include "bits.h"
#include "dnsdefs.h"
#include "dnsmessage.h"
#include "domainfn.h"
#include "exception.h"
#include "lexfn.h"
#include "masterfile.h"
#include "postime.h"
#include "random.h"
#include "resolver.h"
#include "rr.h"
#include "socket.h"
#include "syssocket.h"
#include "sysstl.h"
#include "sysstring.h"
#include "types.h"
#include "vsnprintf.h"

#endif /* __POSLIB_POSLIB_H */
