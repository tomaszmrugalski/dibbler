/*
    Posadis - A DNS Server
    Domain functions
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

#ifndef __POSLIB_DOMAINFN_H
#define __POSLIB_DOMAINFN_H

class domainname;

#include "sysstl.h"
#include "dnsmessage.h"

/*! \file poslib/domainfn.h
 * \brief domain-name manipulation
 *
 * This source file offers various functions for domain-name manipulation.
 * Firstly, it offers the domainname class, which is a C++ class representing
 * a domain name. Secondly, it offers various functions for reading and
 * writing domain names from and to DNS messages, and thirdly, it offers
 * functions which can be used to manipulate domain names as they would
 * appear in DNS messages. These functions use the _domain typedef to
 * represent such a domain name. Usually you will not need to call these
 * yourself.
 */

/*!
 * \brief Maximum length of binary domain name
 *
 * This is the maximum length of a decompressed binary domain name.
 */
#define DOM_LEN 255

/*!
 * \brief Maximum length of domain name label
 *
 * This is the maximum length of a domain name label, not including the length
 * byte (in binary data), or the trailing \p '\0' label.
 */
#define DOMLABEL_LEN 63

/*!
 * \brief class representing a domain name
 *
 * This class represents a domain name. It offers functions to add domain
 * names together, retrieving labels from the domainname, and converting it to
 * human-readable strings and the binary format used in DNS messages.
 */
class domainname {
  public:
  /*!
   * \brief default constructor
   *
   * This constructor sets the domain name to ".", the root domain.
   */
  domainname();

  /*!
   * \brief constructor from human-readable text
   *
   * This constructor takes a domain name in human-readable notation, e.g.
   * "www.acdam.net", and an origin. If a relative domain name is given, it
   * will be considered relative to the specified origin.
   * \param text Human-readable domain name
   * \param origin Origin to which relative domain names are relative
   */
  domainname(const char *text, const domainname origin);

  /*!
   * \brief constructor from human-readable text
   *
   * This constructor takes a domain name in human-readable notation, e.g.
   * "www.acdam.net", and optionally an origin. The origin is in the binary
   * _domain format as found in DNS messages. In case of a relative domain
   * name, it is considered relative to this origin (or to the root domain, if
   * no origin is given).
   * \param text Human-readable domain name
   * \param origin Origin, in binary format, to which relative domain names are
   *               relative
   */
  domainname(const char *text, _cdomain origin = (unsigned char*)"");

  /*!
   * \brief constructor from data in a DNS message
   *
   * This constructor takes a DNS message, stored in a message_buff structor,
   * and an offset in this message where the domain name is to begin. This
   * function will decompress the domain name if nessecary.
   * \param buff A DNS message
   * \param ix Offset in the DNS message
   */
  domainname(message_buff &buff, int ix);

  /*!
   * \brief constructor from binary domain name
   *
   * This constructor takes a domain name in binary form. Since a domain name
   * in binary form is a char *, just like a human-readable domain name, this
   * contructor takes a boolean value as well to prevent it from being
   * ambiguous. The value of the boolean is silently ignored.
   * \param is_binary Ignored
   * \param  The binary domain name
   */
  domainname(bool is_binary, const unsigned char* dom);

  /*!
   * \brief copy constructor
   *
   * This constructor just copies the given domainname structore.
   * \param nam The domain name
   */
  domainname(const domainname& nam);

  /*!
   * \brief equality test
   *
   * Tests whether the two domain names are the same. Comparison is done in
   * a case-insensitive way.
   * \param nam Domain name to compare with
   * \return True if the domain names are the same
   */
  bool operator==(const domainname& nam) const;

  /*!
   * \brief negatice equality test
   *
   * Tests whether the two domain names are the same. Comparison is done in
   * a case-insensitive way.
   * \param nam Domain name to compare with
   * \return True if the domain names are not the same
   */
  bool operator!=(const domainname& nam) const;

  /*!
   * \brief assignment
   *
   * Assigns another domain name
   * \param nam The domain name to assign
   * \return The assigned domain name
   */
  domainname& operator=(const domainname& nam);

  /*!
   * \brief assignment from human-readable text
   *
   * Assigns another domain name, given in human-readable text. Relative
   * domain names are considered relative to the root domain
   * \param buff The domain name in human-readable text
   * \return The assigned domain name
   */
  domainname& operator=(const char *buff);

  /*!
   * \brief concatenation using +=
   *
   * Appends another domain name to the current domain name. The current
   * domain name becomes a child domain of the appended domain name, for
   * example, domainname("www") += domainname("acdam.net") would become
   * \p www.acdam.net.
   * \param dom Domain name to append
   * \return The resulting domain name
   * \sa #operator+
   */
  domainname& operator+=(const domainname& dom);
  /*!
   * \brief concatenation using +
   *
   * Appends two domain names, returning a third. The first domain name
   * becomes a child domain of the second one.
   * \param dom The domain name to append
   * \return The result of the concaternation.
   */
  domainname& operator+(const domainname& dom);

  /*!
   * \brief parent-child test
   *
   * Tests whether we are the child domain of the given domain name. This
   * function also returns true if the child and parent domains are the same.
   * \param dom Domain name to test
   * \return True if we are the parent
   * \sa operator>
   */
  bool operator>=(const domainname& dom) const;

  /*!
   * \brief parent-child test
   *
   * Tests whether we are the child domain of the given domain name. Returns
   * false if the child and parent domain names given are the same.
   * \param dom Domain name to test
   * \return True if we are the parent
   * \sa operator>=
   */
  bool operator>(const domainname& dom) const;

  /*!
   * \brief destructor
   *
   * Frees resources associated with the domain name
   */
  ~domainname();

  /*!
   * \brief binary representation of domain
   *
   * Returns the binary representation of the domain name as it would appear
   * in DNS messages (though in uncompressed form).
   * \return Binary representation for the domain name
   */
  _domain c_str() const;

  /*!
   * \brief length of binary representation
   *

   * Returns the length, in bytes, also counting the terminating \r \0
   * character, of the binary representation of the domain name.
   * \return Length of binary representation
   */
  int len() const;

  /*!
   * \brief convert to human-readable string
   *
   * Converts the domain name to a human-readable string. The string will
   * always have a trailing dot.
   * \return Human-readable domain name
   * \sa tocstr()
   */
  stl_string tostring() const;

  /*!
   * \brief convert to human-readable character array
   *
   * Converts the domain name to a human-readable character array. It will
   * also have a trailing dot. This is static data, so if you want a copy,
   * use a strdup().
   * \sa tostring()
   */
#define tocstr() tostring().c_str()

  /*!
   * \brief number of labels of the domain name
   *
   * Returns the number of labels in the domain name, also counting the root
   * \p '\0' label at the end,
   * \return Number of labels
   * \sa label()
   */
  int nlabels() const;

  /*!
   * \brief label in domain name
   *
   * Returns a label in the domain name. This is just plain human-readable
   * text. It does not contain dots.
   * \param Label index (0 <= ix < nlabels())
   * \return The label at the specified index
   * \sa nlabels()
   */
  stl_string label(int ix) const;

  /*!
   * \brief domain-name portion
   *
   * Returns the portion of the domain name from the label specified by ix.
   * \param ix Label index (inclusive)
   * \return The domain name portion
   * \sa nlabels(), to()
   */
  domainname from(int ix) const;

  /*!
   * \brief domain-name portion
   *
   * Returns a domain name consisting of the first \c label labels of the given
   * domain name.
   * \param labels Number of labels
   * \return The domain name portion
   * \sa from()
   */
  domainname to(int labels) const;

  /*!
   * \brief return relative representation
   *
   * Returns a string representation of the domain name, relative to the given
   * origin. If the domain is not a child of the given root, the complete,
   * absolte domain name is returned. If we are the domain name queried
   * itself, an "@" is returned.
   * \param Domain name this domain is relative to
   * \return Relative string representation
   * \sa tostring()
   */
  stl_string torelstring(const domainname &root) const;

  /*!
   * \brief check label match count
   *
   * Returns the number of labels the two domain names have in common at their
   * ends; for example this returns 2 for \c www.acdam.net and
   * \c www.foo.acdam.net .
   * \param dom The domain name to check with
   * \return Number of common labels
   * \sa nlabels()
   */
  int ncommon(const domainname &dom) const;
   
  private:
  unsigned char *domain;
};

/*!
 * \brief static binary domain name
 *
 * Use this typedef if you want to declare a static _domain variable.
 */
typedef unsigned char _sdomain[DOM_LEN];

/*!
 * \brief dump memory
 *
 * This is an alternative to the c strdup() function, but instead it can dump
 * any type of memory, as long as you give the right length.
 * \param src Source memory location
 * \param len Length of data
 * \return A newly-allocated copy of src.
 */
void *memdup(const void *src, int len);

/*!
 * \brief compressed length
 *
 * This function returns the compressed length - that is, the length the domain
 * takes up in the DNS message - of a domain name.
 * \param buff A DNS messagee
 * \param ix Index of the domain name
 * \return Length in bytes the domain name takes up.
 */
int dom_comprlen(message_buff &buff, int ix);

/*!
 * \brief uncompress domain name
 *
 * This function decompresses a domain name in a DNS message. It returns the
 * binary, decompressed data describing the domain name.
 * \param buff A DNS message
 * \param ix Index of the domain name
 * \return Uncompressed binary domain name (dynamically allocated)
 */
_domain dom_uncompress(message_buff &buff, int ix);

/*!
 * \brief internal domain name compression structure
 *
 * Internal structure for domain name compression
 */
struct dom_compr_info {
 public:
  dom_compr_info(_cdomain _dom, int _ix, int _nl, int _nul); /**< Constructor. */
  _cdomain dom;                     /**< Pointer to binary domain. */
  int ix;                                /**< Index in message. */
  int nl;                                /**< Total number of labels. */
  int nul;                               /**< Number of uncompressed labels. */
};

/*!
 * \brief compress domain name
 *
 * This function writes a domain name to the end of a DNS message, compressing
 * it if possible.
 * \param ret A (partial) DNS message
 * \param dom Domain name to write
 * \param compr List of earlier compressed domain names, or NULL if no compression
 */
void dom_write(stl_string &ret, _cdomain dom, stl_slist(dom_compr_info)* compr);

/* traditional domain-name functions */

/*!
 * \brief domain name portion pointer
 *
 * Returns a pointer to the portion of the domain name from the ix'th label.
 * \param dom Domain name
 * \param ix Index
 * \return Domain name portion
 */
_domain domfrom(_cdomain dom, int ix);

/*!
 * \brief test for parent<->child relationship
 *
 * Tests whether the first domain name is a parent of the second domain name.
 * \param parent Parent domain
 * \param child Child domain
 * \return True if \p parent is indeed a parent of \p child .
 */

bool domisparent(_cdomain parent, _cdomain child);

/*!
 * \brief length of binary domain name
 *

 * Returns the length, in bytes, including the trailing \p '\0' character, of
 * the domain name.
 * \param dom Domain name
 * \return Length of the domain name
 */
int domlen(_cdomain dom);

/*!
 * \brief dynamic copy of binary domain name
 *
 * Makes a dynamically allocated copy of a domain name.
 * \param dom Domain name
 * \return Copy of domain name
 * \sa domcpy()
 */
_domain domdup(_cdomain dom);

/*!
 * \brief compare binary domain labels
 *
 * Checks whether both binary domain name start with the same label.
 * \param dom1 First domain name
 * \param dom2 Second domain name
 * \return \p true if the domain names start with the same label
 */
bool domlcmp(_cdomain dom1, _cdomain dom2);

/*!
 * \brief compare binary domain names
 *
 * Checks whether both binary domain names are equal.
 * \param dom1 First domain name
 * \param dom2 Second domain name
 * \return \p true if the domain names are equal
 */
bool domcmp(_cdomain dom1, _cdomain dom2);

/*!
 * \brief domain name concatenation
 *
 * Appends \p src to \p target. Since it does not re-allocate memory, Make sure
 * that \p target can hold at least #DOM_MAX bytes.
 * \param target Target
 * \param src Source
 */
void domcat(_domain target, _cdomain src);

/*!
 * \brief static copy of binary domain name
 *
 * Makes a static copy of a domain name. Make sure that \p res can hold at least
 * #DOM_MAX bytes.
 * \param res Target
 * \param src Source
 * \sa domdup()
 */
void domcpy(_domain res, _cdomain src);

/*!
 * \brief create domain name from label
 *
 * Creates a domain name containing just one label: the string argument given.
 * If a length is given, only the first few characters of the string are used.
 * Make sure that \p dom can hold at least #DOM_MAX bytes.
 * \param dom Result
 * \param label String label
 * \param len If given, length of string label (default: strlen(label)).
 */
void domfromlabel(_domain dom, const char *label, int len = -1);

/*!
 * \brief to-string conversion
 *
 * Converts the domain name to a human-readable string. Contains the trailing
 * dot.
 * \param dom The domain name
 * \return Human-readable string.
 */
stl_string dom_tostring(_cdomain dom);

/*!
 * \brief number of labels
 *
 * Returns the number of labels, also counting the empty \p '\0' label, of the
 * domain name.
 * \param dom The domain name
 * \return Number of labels
 */
int dom_nlabels(_cdomain dom);

/*!
 * \brief label of domain name
 *
 * Returns a label of the domain name in human-readable form.
 * \param dom The domain name
 * \param label Label index (0 <= label < #dom_nlabels(dom))
 * \return The label
 * \sa dom_nlabels()
 */
stl_string dom_label(_cdomain dom, int label);

/*!
 * \brief label of domain name
 *
 * Returns a label of the domain name as a pointer to the position in the domain.
 * \param dom The domain name
 * \param label Label index (0 < label < #dom_nlabels(dom))
 * \return The label
 * \sa dom_nlabels()
 */
_domain dom_plabel(_cdomain dom, int label);

/*!
 * \brief check label match count
 *
 * Returns the number of labels the two domain names have in common at their
 * ends; for example this returns 2 for \c www.acdam.net and
 * \c www.foo.acdam.net .
 * \param dom The domain name to check with
 * \return Number of common labels
 * \sa nlabels()
 */
int domncommon(_cdomain dom1, _cdomain dom2);

/*!
 * \brief compare domain names
 *
 * This function offers a way to compare domain names the way the \c strcmp
 * do. It operates in such a way that child domains are greater than parent
 * domains, and for for other domains, the first non-matching domain label is
 * compared using strcmpi. For example, \c www.foo.acdam.net is greater than
 * \c bar.acdam.net because \c foo is greater than \c bar which is the first
 * non-matching label after the common \c acdam.net part of the domain names.
 *
 * \param dom1 First domain name
 * \param dom2 Second domain name
 * \return <0, 0 or >0 if the first domain name is smaller than, equal to, or
 * larger than the second one, respectively.
 */
int domccmp(_cdomain _dom1, _cdomain _dom2);

/*!
 * \brief return domain name portion
 *
 * Returns a domain name consisting of the first \c label labels of the given
 * domain name. The \c ret buffer should be large enough to hold the result
 * to prevent a buffer overflow.
 *
 * \param ret Result buffer
 * \param src Source domain
 * \param labels Number of labels to include
 */
void domto(_domain ret, _cdomain src, int labels);

#endif /* __POSLIB_DOMAINFN_H */
