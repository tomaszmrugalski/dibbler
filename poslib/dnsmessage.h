/*
    Posadis - A DNS Server
    Dns Message handling
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

#ifndef __POSLIB_DNSMESSAGE_H
#define __POSLIB_DNSMESSAGE_H

class message_buff;

#include "dnsdefs.h"
#include "types.h"
#include "sysstl.h"
#include "domainfn.h"

/* when constructed with the message and length, the data is regarded static
   and not cleaned up on destruction */

/*! \file poslib/dnsmessage.h
 * \brief DNS message functions
 *
 * This file contains functions and classes for dealing with DNS messages. The
 * DnsMessage class is used to represent a DNS message, and some functions for
 * reading numbers from binary DNS data are also included.
 */

/*!
 * \brief message buffer
 *
 * This is a structure that can hold a piece of binary data and its length.
 * Depending on the way it is constructed, or whether #is_static is set, it
 * will choose whether or not to free the resources associated with it upon
 * destruction.
 */
class message_buff {
 public:
  /*!
   * \brief static constructor
   *
   * This constructor will just set the #msg pointer to the address pointed to
   * by \p _msg, and copy the length. It will not copy the data, and, when the
   * structure is destroyed, the data will not be freed (e.g., it sets
   * #is_static to \p true), unless you set is_dynamic to true.
   * \param _msg Source data
   * \param _len Source length
   * \param is_dynamic Whether the data is dynamic.
   */
  message_buff(unsigned char *_msg, int _len, bool is_dynamic = false);
  /*!
   * \brief copy constructor
   *
   * This constructor will copy the specified message buffer. If the source
   * message buffer is dynamic (e.g. \p buff.is_static is false), the data
   * will be dynamically copied.
   * \param buff The buffer to copy
   */
  message_buff(const message_buff& buff);
  /*!
   * \brief dynamic constructor
   *
   * This constructor will set #msg to NULL, #len to 0 and #is_static to false.
   * Thus, it will creatr a dynamic message buffer whose #msg will be destroyed
   * by the destructor.
   */
  message_buff();
  /*!
   * \brief destructor
   *
   * If #is_static is set to false, this destructor will free the memory
   * associated with #msg.
   */
  ~message_buff();
  /*!
   * \brief assignment operator
   *
   * This operator will copy the given message buffer. It works the same as
   * #message_buff(const messagebuf &buff).
   * \param buff Message buffer to copy
   */
  message_buff& operator=(const message_buff& buff);
  /*!
   * \brief whether the message is static
   *
   * This determines whether the #msg field is static - that is, whether it
   * should be freed by the message_buffer ( \p false ) or not. The default value
   * of this depends on the used constructor.
   */
  bool is_static;
  /*!
   * \brief buffer length
   *
   * The length, in bytes, of the data pointed to by #msg.
   */
  int len;
  /*!
   * \brief buffer
   *
   * The data the message_buff holds.
   */
  unsigned char *msg;
};

/*!
 * \brief DNS question object
 *
 * This object holds a DNS question object - an entry in the question section
 * of a DNS message.
 */
class DnsQuestion {
 public:
  /*!
   * \brief constructor
   *
   * This is the default constructor.
   */
  DnsQuestion();
  /*!
   * \brief copy constructor
   *
   * This constructor copies the given question.
   * \param q Question to copy
   */
  DnsQuestion(const DnsQuestion& q);
  /*!
   * \brief assignment
   *
   * This is the assignment operator. It works in the same way as the copy
   * constructor.
   */
  DnsQuestion& operator=(const DnsQuestion& q);

  /*!
   * \brief constructor
   *
   * This constructor takes values for the various fields of the question
   * structure.
   * \param QNAME Domain name to query
   * \param QTYPE Type to query
   * \param QCLASS Class the data is in (or #CLASS_IN if none specified)
   */
  DnsQuestion(domainname QNAME, u_int16 QTYPE, u_int16 QCLASS = CLASS_IN);

  /*!
   * \brief destructor
   *
   * Frees data associated with the object.
   */
  ~DnsQuestion();

  /*!
   * \brief query domain name
   *
   * Domain name to query for.
   */
  domainname QNAME;

  /*!
   * \brief query type
   *
   * The type of data to query for. This can be a RR type constant (for
   * example, #DNS_TYPE_A), or a QTYPE_* constant (for example, #QTYPE_ANY).
   * These constants can be found in the dnsdefs.h header.
   */
  u_int16 QTYPE;
  /*!
   * \brief query class
   *
   * The class to query. This is mainly historical, the only class that is used
   * nowadays is #CLASS_IN (the internet). Other values as #CLASS_HS for Hesiod,
   * #CLASS_CH for chaos, and #CLASS_CS for CSNET. The QCLASS_ANY constant means
   * you don't care what class, but this is usually not supported. The list of
   * constants can be found in the dnsdefs.h header.
   */
  u_int16 QCLASS;
};

/*!
 * \brief resource record
 *
 * This class represents a resource record (RR), the fundamental building block
 * of the DNS. A RR contains type-dependent information in its #RDATA field, as
 * well as some general information on the RR itself, such as its #TTL.
 */
class DnsRR {
 public:
  /*!
   * \brief constructor
   *
   * This is the default constructor.
   */
  DnsRR();

  /*!
   * \brief comparator
   */
  bool operator==(const DnsRR& other);

  /*!
   * \brief sort comparison function
   *
   * This function defines an order on DnsRRs, returning -1 if the current item
   * is smaller, 0 if two DnsRRs are equal, and 1 otherwise. The items are
   * sorted, in order, by their #CLASS, #NAME, #TYPE (SOA going before other
   * types), #TTL and #RDATA fields.
   * \param other Other RR
   * \return Result of the comparison
   */
  int compareTo(const DnsRR& other) const;
  bool operator<(const DnsRR& rr) const;
  bool operator<=(const DnsRR& rr) const;
  bool operator>(const DnsRR& rr) const;
  bool operator>=(const DnsRR& rr) const;

  /*!
   * \brief constructor taking some fields
   *
   * This constructor sets the values of many of the class members, but not for
   * the RR data.
   * \param NAME The domain name
   * \param TYPE Resource Record type
   * \param CLASS Class the RR is in
   * \param TTL Time To Live for the RR
   */
  DnsRR(domainname NAME, u_int16 TYPE, u_int16 CLASS, u_int32 TTL);

  /*!
   * \brief constructor taking some fields
   *
   * This constructor sets the values of all class members, including
   * the RR data.
   * \param NAME The domain name
   * \param TYPE Resource Record type
   * \param CLASS Class the RR is in
   * \param TTL Time To Live for the RR
   * \param RDLENGTH Length of RR data
   * \param RDATA RR data (this is copied into the RR)
   */
  DnsRR(domainname NAME, u_int16 TYPE, u_int16 CLASS, u_int32 TTL, uint16_t RDLENGTH, const unsigned char *RDATA);

  /*!
   * \brief copy constructor
   *
   * This constructor copies the given RR. It dynamically allocates its own
   * copy of the RRs #RDATA field.
   * \param rr Resource Record to copy
   */
  DnsRR(const DnsRR& rr);

  /*!
   * \brief assignment operator
   *
   * The assignment operator works the same as the copy constructor.
   * \param rr Resource Record to copy
   */
  DnsRR& operator=(const DnsRR& rr);

  /*!
   * \brief destructor
   *
   * Frees all memory associated with the RR, including the #RDATA.
   */
  ~DnsRR();

  /*!
   * \brief domain name
   *
   * The domain name the Resource Record is bound to. Defaults to the root
   * domain.
   */
  domainname NAME;

  /*!
   * \brief RR type
   *
   * The type of the RR. One of the constants defined in dnsdefs.h, for example
   * #DNS_TYPE_A or #DNS_TYPE_MX (or the value of the rr_type.type field).
   */
  u_int16 TYPE;
  
  /*!
   * \brief class
   *
   * The DNS class the RR is in. See DnsQuestion::QCLASS for a list, except that
   * it can not be #QCLASS_ALL in this case. Defaults to #CLASS_IN.
   */
  u_int16 CLASS;

  /*!
   * \brief time to live
   *
   * The time to live for the RR - that is, the time in seconds it may be
   * stored in cache.
   */
  u_int32 TTL;

  /*!
   * \brief length of RR data
   *
   * The length in bytes of the data pointed to by #RDATA.
   */
  u_int16 RDLENGTH;

  /*!
   * \brief RR data
   *
   * Binary data for the RR. Use the RR functions from rr.h to interpret this
   * field. Automatically freed upon destruction.
   * \sa rr_tostring(), rr_getdomain(), rr_getmail(), ...
   */
  unsigned char *RDATA;
};

/*!
 * \brief DNS message
 *
 * This structure holds a DNS message, the message type with which DNS servers
 * talk to each other. It has member functions for reading data from binary DNS
 * messages, and to create a binary DNS message from the structure.
 */
class DnsMessage {
 public:
  /*!
   * \brief constructor
   *
   * This constructs an empty DNS message, with all fields set to defaults.
   */
  DnsMessage();

  /*!
   * \brief destructor
   *
   * This destroys the DNS message.
   */
  ~DnsMessage();

  /*!
   * \brief message ID
   *
   * This is the message ID field of the DNS message. This number is set by
   * client software, and is copied into the response by the server in order
   * for clients to be able to track queries. Note that the Posadis resolver
   * sets this value for you, so there's no need to do that yourself in client
   * applications.
   */
  u_int16 ID;

  /*!
   * \brief query bit
   *
   * This bit is set to \p false for queries, and \p true for answers.
   */
  bool QR;

  /*!
   * \brief operation
   *
   * This is the type of operation the query is. The most common are
   * OPCODE_QUERY for queries, OPCODE_UPDATE for dynamic updates, and
   * OPCODE_NOTIFY for DNS notifications. Possible values are in dnsdefs.h.
   */
  u_int4 OPCODE;

  /*!
   * \brief authoritative answer
   *
   * This is set to \p true by the server if it was authoritative for the
   * zone the query was in. Note that, if the answer contains CNAMEs, this does
   * not nessecarily mean the server was also authoritative for the domain the
   * CNAME pointed to.
   */
  bool AA;

  /*!
   * \brief truncated
   *
   * Set to \p true by the server if the answer was cut off because it didn't
   * fit in a UDP packet. Unless you instruct it not to, the Posadis resolver
   * will automatically retry using TCP to get the complete answer.
   */
  bool TC;

  /*!
   * \brief recursion desired
   *
   * Set this to \p true to instruct the server to do recursive operation
   * (e.g. consult other nameservers to find the right answer). Note that
   * servers may refuse to do this, and will set the #RA field accordingly.
   */
  bool RD;

  /*!
   * \brief recursion available
   *
   * Set to the server indicating whether it is willing to provide recursive
   * service. Note that, even if recursion was not desired (see #RD), this
   * value might still be set.
   */
  bool RA;

  /*!
   * \brief reserved bits
   *
   * This the value of three currently reserved bits in the DNS message.
   * Though these bits currently have no meaning and servers might require
   * them to be zero, Poslib is able to read and write them.
   */
  u_int3 Z;

  /*!
   * \brief return code
   *
   * Code indicating whether the query was succesful. Some famous RCODEs,
   * which are #defined in dnsdefs.h, are: RCODE_NOERROR to indicate success,
   * RCODE_NXDOMAIN if the domain name queried for didn't exist, or
   * RCODE_SERVFAIL in case of a server failure.
   */
  u_int4 RCODE;

  /*!
   * \brief question section
   *
   * This section should contain exactly one DnsQuestion object for common DNS
   * queries. This query is usually copied into the response by the server.
   */
  stl_list(DnsQuestion) questions;

  /*!
   * \brief answer section
   *
   * This section, filled by the server, contains the Resource Records that
   * form a direct answer to the query.
   */
  stl_list(DnsRR) answers;

  /*!
   * \brief authority section
   *
   * This section contains pointers to authoritative sources for the information.
   * Most nameservers put the nameserver list for the domain names in the
   * section here.
   */
  stl_list(DnsRR) authority;

  /*!
   * \brief additional section
   *
   * This section contains additional information that might be interesting for
   * the client, for example addresses for NS or MX records in the answer or
   * authority sections.
   */
  stl_list(DnsRR) additional;

  /*!
   * \brief read DNS message
   *
   * This function will read DNS message information from the binary DNS
   * message pointed to by data.
   * \param data Binary DNS message
   * \param len Length of message
   */
  void read_from_data(unsigned char *data, int len);

  /*!
   * \brief compile DNS message
   *
   * This function will compile the DNS message into the binary format sent
   * over UDP or TCP connections.
   * \param maxlen Maximum length. If the message exceeds this limit, it will
                                   be cut off and the #TC bit will be set.
                                   This should be \p 65535 for TCP messages and
                                   \p 512 for UDP messages.
   * \return The compiled DNS message
   */
  message_buff compile(int maxlen);

  static void write_rr(DnsRR &rr, stl_string &message, stl_slist(dom_compr_info) *comprinfo,
  int flags = 0);
  void write_section(stl_list(DnsRR)& section, int lenpos, stl_string& message, stl_slist(dom_compr_info) &comprinfo, int maxlen, bool is_additional = false);
  void read_section(stl_list(DnsRR)& section, int count, message_buff &buff, int &pos);
  static DnsRR read_rr(message_buff &buff, int &pos, int flags = 0);
};

u_int16 uint16_value(const unsigned char *buff);
u_int32 uint32_value(const unsigned char *buff);

unsigned char *uint16_buff(uint16_t val);
unsigned char *uint32_buff(uint32_t val);

/*!
 * \brief create a query message
 *
 * Creates a Dns question message which can be used to query a DNS server.
 * This message is dynamically allocated, so you'll have to delete it yourself.
 * \param QNAME Domain name to query. See DnsQuestion::QNAME.
 * \param QTYPE Type of RR to query. See DnsQuestion::QTYPE for more information.
 *              Defaults to \p DNS_TYPE_A .
 * \param RD whether we want the server to do recursion. See DnsMessage::RD.
 *           Defaults to \p true .
 * \param QCLASS The class to query in. See DnsQuestion::QCLASS. Defaults to
 *               \p CLASS_IN .
 * \return DNS message containing the query.
 */
DnsMessage *create_query(domainname QNAME, uint16_t QTYPE = DNS_TYPE_A, bool RD = true, uint16_t QCLASS = CLASS_IN);


/*!
 * \brief ipv4 address
 *
 * Class representing an IPv4 address.
 */
class a_record {
 public:
  char address[4];    /**< The four-byte IPv4 address. */
};

//! Gets the first address in the answer to an address query.
a_record get_a_record(DnsMessage *a);
//! Gets a list of addresses in the answer to an address query.
stl_list(a_record) get_a_records(DnsMessage *a, bool fail_if_none = false);

/*!
 * \brief ipv6 address
 *
 * Class representing an IPv6 address.
 */
class aaaa_record {
 public:
  char address[16];   /**< The sixteen-byte IPv6 address. */
};

//! Gets the first IPv6 address in the answer to an IPv6 address query.
aaaa_record get_aaaa_record(DnsMessage *a);
//! Gets a list of addresses in the answer to an IPv6 address query.
stl_list(aaaa_record) get_aaaa_records(DnsMessage *a, bool fail_if_none = false);

/*!
 * \brief mx record
 *
 * Class representing an Mail eXchanger (MX) record.
 */
class mx_record {
 public:
  uint16_t preference; /**< Prefence value: the lower, the better. */
  domainname server;   /**< Domain name for the mail server. */
};

//! Gets the first MX record in te answer to a MX query.
mx_record get_mx_record(DnsMessage *a);
//! Gets a list of MX records in te answer to a MX query.
stl_list(mx_record) get_mx_records(DnsMessage *a, bool fail_if_none = false);

//! Gets the first NS record in te answer to a NS query.
domainname get_ns_record(DnsMessage *a);
//! Gets the list of NS records in te answer to a NS query.
stl_list(domainname) get_ns_records(DnsMessage *a, bool fail_if_none = false);

//! Gets the first PTR record in te answer to a PTR query.
domainname get_ptr_record(DnsMessage *a);
//* Gets the list of PTR records in te answer to a PTR query.
stl_list(domainname) get_ptr_records(DnsMessage *a, bool fail_if_none = false);

//* Structure for RR data returned by get_records
class rrdat {
 public:
  rrdat(uint16_t, uint16_t, unsigned char *); //! constructor
  uint16_t type;                              //! rr type
  uint16_t len;                               //! rdata length
  unsigned char *msg;                         //! rdata
};

//! Gets a list of all RRs in the answer section answering the DNS query. May follow CNAMEs. */
stl_list(rrdat) get_records(DnsMessage *a, bool fail_if_none = false, bool follow_cname = true, stl_list(domainname) *followed_cnames = NULL);

//! Enumeration of different possible answer types
enum  _answer_type {
  A_ERROR,      //! Error
  A_CNAME,      //! Alias
  A_NXDOMAIN,   //! Domain doesn't exist
  A_ANSWER,     //! Answer
  A_REFERRAL,   //! Referral to other server
  A_NODATA      //! No such data
};

//! Returns the answer type of an answer message for a given query
_answer_type check_answer_type(DnsMessage *msg, domainname &qname, uint16_t qtype);

//! Returns true if the given RRset is present in the DNS message section
bool has_rrset(stl_list(DnsRR) &rrlist, domainname &QNAME, uint16_t QTYPE = QTYPE_ANY);

#endif /* __POSLIB_DNSMESSAGE_H */

