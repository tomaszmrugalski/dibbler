/*
    Posadis - A DNS Server
    Resource Records
    Meilof Veeningen <meilof@users.sourceforge.net>

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

#ifndef __POSLIB_RR_H
#define __POSLIB_RR_H

#include "dnsmessage.h"

/*! \file poslib/rr.h
 * \brief Resource-record information database
 *
 * This file contains functions to deal with Resource Records, the information
 * chunks contained by DNS messages. It has functions for converting RRs to
 * text and back, and to extract information from them.
 */

/*!
 * \brief flags for Resource Record types
 *
 * These are flags for the rr_type::flags member for Resource Record
 * information.
 */
enum _rr_flags {
 R_NONE = 0,          /**< No RR flags */
 R_ASP = 1,           /**< Do additional section processing for domain names
                           in this Resource Record */
 R_COMPRESS = 2,      /**< Compress domain names in this RR in outgoing DNS
                           messages */
 R_ASPCOMPRESS = 3    /**< Do both additional section processing and
                           domain name compression */
};

/*!
 * \brief Resource Record information
 *
 * The rr_type structure contains information about the information a Resource
 * Record type contains, and how it needs to be handled. It is mainly of
 * interest for the internal RR functions declared in the rr.h header file.
 */
struct rr_type {
  /*!
   * \brief RR type name
   *
   * This is the name of the RR type. This information is case-insensitive,
   * though by convention all RR types have an uppercase name.
   */
  char name[9];

  /*!
   * \brief RR type code
   *
   * The 16-bit identifier code for the RR. This is the value that appears
   * directly in DNS messages to identify the RR type.
   */
  u_int16 type;
  
  /*!
   * \brief list of properties for the RR type
   *
   * This is a string, in which each character stands for a chunk of
   * information in the RR data. Each possible type has its own code:
   * \p d for a domain name,
   * \p i for a ip number
   * \p s for a short number (16-bit unsigned)
   * \p l for a long number (32-bit unsigned)
	 * \p t for a ttl value (32-bit unsigned; same as l, but different string
	 *      representation
   * \p c for a chararacter string
   * \p h for multiple character strings
   * \p n for a NULL RR
   * \p w for a well-known services list
   * \p 6 for an IPv6 address
   * \p 7 for an IPv6 prefix
   * \p m for an email-address.
   * \p o for LOC information
   */
  char properties[9];

  /*!
   * \brief flags for the RR
   *
   * These flags determine how the RR is treated when reading from or writing
   * to a DNS message. See the documentation for #_rr_flags for more
   * information.
   */
  _rr_flags flags;
};

/*!
 * \brief number of supported rr types
 *
 * This is the number of RR types supported by Poslib. Thus, it is also the
 * number of entries in the #rr_types array.
 * \sa #rr_types
 */
extern const int n_rr_types;

/*!
 * \brief array of supported rr types
 *
 * This array contains information for all RR types supported by Poslib. For
 * information about the meaning of the fields, see the rr_type documentation.
 * This array contains exactly #n_rr_types elements.
 * \sa #n_rr_types
 * \sa rr_type
 */
extern const rr_type rr_types[];
  
/* low-level RR information */

/*!
 * \brief retrieves RR information
 *
 * This function retrieves information about a Resource Record by its 16-bit
 * unique identifier as found in DNS messages. If no matching RR type is found,
 * this function returnes \p NULL.
 * \param type The RR type code
 * \return The RR type information, or \p NULL if none was found
 */
rr_type *rrtype_getinfo(u_int16 type);

/*!
 * \brief retrieves RR information by name
 *
 * This function retrieves information a Resource Record by its name. If no
 * matching RR type is found, this function returns \p NULL.
 * \param name The RR name (case insensitive)
 * \return The RR type information, or \p NULL if none was found
 */
rr_type *rrtype_getinfo(const char *name);

/*!
 * \brief retrieves RR name
 *
 * This function returns the name for a RR type by its 16-bit code. If no
 * matching RR type is found, this function returns \p NULL.
 * \param type The 16-bit RR code
 * \return Name of the RR, or \p NULL if none was found
 */
char *rrtype_getname(u_int16 type);

/*!
 * \brief retrieves qtype code
 *
 * This function tries to return the QTYPE code for a given string, that can
 * either be a RR, "any", "ixfr", "axfr", "maila" or "mailb", or a numeric
 * value.
 * \param type The query type
 * \param allow_qtype If set to false (default true), only allow common types
 * \return The 16-bit QTYPE value for the given string
 */
uint16_t qtype_getcode(const char *name, bool allow_qtype = true);

/*!
 * \brief check whether RR type answers QTYPE
 *
 * This function checks whether the given RR type provides an answer to the
 * given QTYPE. This is the case if qtype matches rrtype, or if qtype is
 * \p QTYPE_ANY , or if qtype is \p QTYPE_MAILB or \p QTYPE_MAILA and rrtype
 * is an appropriate RR type.
 * \param rrtype Resource Record type
 * \param qtype Question type
 * \return \p true if the rrtype provides an answer to the qtype.
 */
bool answers_qtype(uint16_t rrtype, uint16_t qtype);

/*!
 * \brief check whether the RR type is a common RR
 *
 * This function returns true if rrtype is a common RR type (that is, not a
 * query type like \p QTYPE_ANY or \p QTYPE_IXFR . Note that this does not
 * nessecarily mean Poslib supports the given RR type.
 * \param rrtype Resource Record type to check
 * \return \p true if the rrtype is a common RR type
 */
bool is_common_rr(uint16_t rrtype);

/*!
 * \brief read a Resource Record from a DNS message
 *
 * This function reads a Resource Record from a DNS message or other binary
 * data source. It will decompress compressed domain names in the RR on the
 * way, and return the data in the RDLEN and RDATA arguments.
 * \param RRTYPE The type of RR to read
 * \param RDATA This will hold the RR data if the function succeeds
 * \param RDLEN This will hold the length of the RR if the function succeeds
 * \param buff The message_buff the RR is stored in
 * \param ix Index in the buffer the RR begins
 * \param len Length the RR takes in the buffer
 */
void rr_read(u_int16 RRTYPE, unsigned char*& RDATA, uint16_t &RDLEN, message_buff &buff, int ix, int len);

/*!
 * \brief write a Resource Record to a DNS message
 *
 * This function writes a Resource Record to a DNS message. It will compress
 * domain names along the way.
 * \param RRTYPE The type of RR to write
 * \param RDATA Pointer to the RR data
 * \param RDLEN Length of the RR data
 * \param dnsmessage The message to write to
 * \param comprinfo List of compressed domain names in DNS message, or NULL for no compression
 */
void rr_write(u_int16 RRTYPE, unsigned char *RDATA, uint16_t RDLEN, stl_string &dnsmessage, stl_slist(dom_compr_info) *comprinfo);

/*!
 * \brief convert a binary RR to string
 *
 * This function converts the binary RR to a human-readable string in master
 * file format.
 * \param RRTYPE The RR type
 * \param RDATA The binary RR data
 * \param RDLENGTH Length of the binary RR data
 * \return A string describing the RR
 * \sa rr_fromstring()
 */
stl_string rr_tostring(u_int16 RRTYPE, const unsigned char *RDATA, int RDLENGTH);

/*!
 * \brief Convert RR property to strings
 * 
 * Converts a RR property as in #rr_type.property, to string.
 * \param type     Property type (see #rr_type.property)
 * \param RDATA    RR data
 * \param RDLENGTH Lenth of rest of RR
 * \param zone     Zone to make domains relative to
 */
stl_string rr_property_to_string(char type, const unsigned char*& RDATA, int& RDLENGTH, domainname& zone);

/*!
 * \brief convert a binary RR to string
 *
 * This function converts the binary RR to a human-readable string in master
 * file format.
 * \param RRTYPE The RR type
 * \param RDATA The binary RR data
 * \param RDLENGTH Length of the binary RR data
 * \param zone If given, domain names in the RR will be relative to that zone where possible.
 * \return A string describing the RR
 * \sa rr_fromstring()
 */
stl_string rr_torelstring(u_int16 RRTYPE, const unsigned char *RDATA, int RDLENGTH, domainname zone);

/*!
 * \brief convert a string to binary RR data
 *
 * This function converts a string describing a Resource Record to binary RR
 * data. The string should be in master file format - that is, if multiple
 * arguments are to be put in the RR data, they should be separated by any
 * number of spaces and tabs. For example, MX data might be "10 mail.yo.net.".
 * You can specify an origin to which domain names are considered relative by
 * means of the origin parameter.
 * \param RRTYTPE Type of the RR
 * \param data The text describing the RR
 * \param origin If given, the domain name relative domain names are considered
                 relative to. This should be a binary domain name, like the
                 domainname::domain field. If not given, domain names are
                 considered relative to the root domain.
 * \return Binary data describing the RR
 * \sa rr_tostring()
 */
stl_string rr_fromstring(u_int16 RRTYPE, const char *data, _domain origin = (unsigned char*)"");

/*!
 * \brief convert a string to binary RR data
 *
 * This function converts a string describing a Resource Record to binary RR
 * data. The string should be in master file format - that is, if multiple
 * arguments are to be put in the RR data, they should be separated by any
 * number of spaces and tabs. For example, MX data might be "10 mail.yo.net.".
 * You can specify an origin to which domain names are considered relative by
 * means of the origin parameter.
 * \param RRTYTPE Type of the RR
 * \param data The text describing the RR
 * \param origin If given, the domain name relative domain names are considered
                 relative to. This should be a binary domain name, like the
                 domainname::domain field. If not given, domain names are
                 considered relative to the root domain.
 * \return The domainname
 * \sa rr_tostring()
 */
stl_string rr_fromstring(u_int16 RRTYPE, const char *data, domainname origin);

/*!
 * \brief reads a domain name from RR data
 *
 * This function will read a domain name, in binary for, from RR data.
 * The _domain it returns is dynamically allocated. The index is the property
 * index in the RR data, beginning with 0, e.g. the domain name in the \p MX
 * RR is one.
 *
 * \param RDATA The RR data
 * \param RRTYPE Type of RR
 * \param ix If given, property index in the RR (defaults to zero)
 * \return Domain name at the specified position in the RR
 */
_domain rr_getbindomain(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief reads a domain name from RR data
 *
 * Variant of the rr_getbindomain() function returning a domainname structure.
 */
domainname rr_getdomain(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief reads an email address from RR data
 *
 * This is currently an alias for rr_getdomain().
 */
_domain rr_getbinmail(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief reads a domain name from RR data
 *
 * Variant of the rr_getbinmail() function returning a domainname structure.
 */
domainname rr_getmail(const char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief read a 16-bit value from RR data
 *
 * For details, see rr_getdomain().
 */
u_int16 rr_getshort(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief read a 32-bit value from RR data
 *
 * For details, see rr_getdomain().
 */
u_int32 rr_getlong(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief read an IP address from RR data
 *
 * For details, see rr_getdomain().  Data is dynamically allocated.
 */
unsigned char *rr_getip4(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief read an IPv6 addres from RR data
 *
 * For details, see rr_getdomain(). Data is dynamically allocated.
 */
unsigned char *rr_getip6(const unsigned char *RDATA, u_int16 RRTYPE, int ix = 0);

/*!
 * \brief read next item in a space-delimited string
 *
 * This function reads the next item from a space-delimited string. It will
 * raise an exception if none is found, and it will update the char pointer
 * on the way. Quotes, unless escaped by a backslash, will not be included
 * in the resulting string. Note that currently the maximum length of an
 * entry is 256 bytes.
 * 
 * \param data Pointer to string (gets updated)
 * \return The next entry in the string
 * \sa read_line
 */
stl_string read_entry(char*& data);

/*!
 * \brief read line from master/configuration file
 *
 * This function attempts to read a line from the file pointed to by f, placing
 * the results in the "buff" buffer of "buffsz" bytes. This function, which is
 * suitable for reading entries in a DNS master file, can automagically detect
 * escaping characters with special meaning, and it can read multi-line lines
 * with "(" and ")". The results that are placed in the buffer are suitable for
 * use in the read_entry function.
 *
 * One important thing in this context is the line number counter: as you can
 * see there are two line number counters, which is nessecary because the
 * line read function will always go to the beginning of the next source line
 * before exiting. Thus, the line number counter would point to the beginning
 * of the next (non-read) line. To prevent this, another pointer is given which
 * is at the beginning of the function set to the current, accurate, line
 * number. For this, a right value of the first pointer is required, though.
 * Note that linenum should be initially set to 1 before the first read_line call.
 *
 * \param buff Buffer to place the results in
 * \param f The file to read from
 * \param linenum Pointer to line number counter (may be NULL; points to next
 *                source line)
 * \param linenum2 Pointer to current line number.
 * \param buffsz Size in bytes of the buffer (defaults to 1024)
 * \sa read_entry
 */

void read_line(char *buff, FILE *f, int *linenum = NULL, int *linenum2 = NULL,
               int buffsz = 1024);

#endif /* __POSLIB_RR_H */
