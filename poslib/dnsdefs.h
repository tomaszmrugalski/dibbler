/*
    Posadis - A DNS Server
    DNS definitions
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

#ifndef __DNS_DNSDEFS_H
#define __DNS_DNSDEFS_H

/*! \file poslib/dnsdefs.h
 * \brief DNS definitions
 *
 * This file contains #defines for the various RR types, QTYPEs, RCODEs,
 * OPCODEs and such, as mostly defined in RFC 1035.
 */

/* RR types */

#define DNS_TYPE_A                   1  /**< IPv4 address RR type */
#define DNS_TYPE_NS                  2  /**< Nameserver RR type */
#define DNS_TYPE_MD                  3  /**< Mail Domain RR type (deprecated) */
#define DNS_TYPE_MF                  4  /**< Mail Forwarder RR type (deprecated) */
#define DNS_TYPE_CNAME               5  /**< Canonical Name RR type */
#define DNS_TYPE_SOA                 6  /**< Start of Authority RR type */
#define DNS_TYPE_MB                  7  /**< Mail Box RR type (experimental) */
#define DNS_TYPE_MG                  8  /**< Mail Group RR type (experimental) */
#define DNS_TYPE_MR                  9  /**< Mail Rename RR type (experimental) */
#define DNS_TYPE_NULL               10  /**< NULL RR type (experimental) */
#define DNS_TYPE_WKS                11  /**< Well-Known Services RR type */
#define DNS_TYPE_PTR                12  /**< Pointer RR type */
#define DNS_TYPE_HINFO              13  /**< Host Info RR type */
#define DNS_TYPE_MINFO              14  /**< Mailbox Info RR type */
#define DNS_TYPE_MX                 15  /**< Mail eXchanger RR type */
#define DNS_TYPE_TXT                16  /**< Text RR type */
#define DNS_TYPE_RP                 17  /**< Responsible Person RR type */
#define DNS_TYPE_AFSDB              18  /**< Andrew File System Database RR type */
#define DNS_TYPE_PX                 26  /**< DNS X.400 Mail Mapping Information RR type */
#define DNS_TYPE_AAAA               28  /**< IPv6 address RR type */
#define DNS_TYPE_LOC                29  /**< LOC (location) RR type */
#define DNS_TYPE_SRV                33  /**< Services RR type */
#define DNS_TYPE_NAPTR              35  /**< Naming Authority Pointer RR type */
#define DNS_TYPE_A6                 38  /**< Prefixed IPv6 address (experimental) */
#define DNS_TYPE_DNAME              39  /**< Sub-canonical Domain Name RR type (experimental) */

/* QTYPEs */

#define QTYPE_NONE                   0  /**< No RR (DNS update) */                                                                  
#define QTYPE_IXFR                 251  /**< Incremental Zone Transfer QTYPE */
#define QTYPE_AXFR                 252  /**< Complete Zone Transfer QTYPE */
#define QTYPE_MAILB                253  /**< Mailbox-related RRs QTYPE (experimental) */
#define QTYPE_MAILA                254  /**< Mail agent RRs QTYPE (deprecated) */
#define QTYPE_ALL                  255  /**< All RR types QTYPE */
#define QTYPE_ANY                  255  /**< All RR types QTYPE */

/* DNS classes */

#define CLASS_IN                     1  /**< Internet class */
#define CLASS_CS                     2  /**< CSNET class */
#define CLASS_CH                     3  /**< Chaos class */
#define CLASS_HS                     4  /**< Hesiod class */

/* QCLASSes */

#define QCLASS_NONE                254  /**< No class (for DNS update) */
#define QCLASS_ANY                 255  /**< Any class */
#define QCLASS_ALL                 255  /**< All classes */

/* RCODEs */

#define RCODE_NOERROR                0  /**< No error */
#define RCODE_QUERYERR               1  /**< Error in query */
#define RCODE_SERVFAIL               2  /**< Server failure */
#define RCODE_SRVFAIL                2  /**< Server failure */
#define RCODE_NXDOMAIN               3  /**< Domain name doesn't exist */
#define RCODE_NOTIMP                 4  /**< Feature not implemented */
#define RCODE_REFUSED                5  /**< Action refused */
#define RCODE_YXDOMAIN               6  /**< Domain name should'nt exist (DNS Update) */
#define RCODE_YXRRSET                7  /**< RRset shouldn't exist (DNS Update) */
#define RCODE_NXRRSET                8  /**< RRset doesn't exist (DNS Update) */
#define RCODE_NOTAUTH                9  /**< Not authoritative when required */
#define RCODE_NOTZONE               10  /**< Domain name not in zone */

/* OPCODEs */

#define OPCODE_QUERY                 0  /**< Normal query */
#define OPCODE_IQUERY                1  /**< Inverse query (deprecated) */
#define OPCODE_STATUS                2  /**< Status request */
#define OPCODE_COMPLETION            3  /**< Completion query (deprecated) */
#define OPCODE_NOTIFY                4  /**< Notification message */
#define OPCODE_UPDATE                5  /**< DNS update message */

#endif /* __DNS_DNSDEFS_H */

