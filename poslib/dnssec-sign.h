/*
    Posadis - A DNS Server
    Dns Message signing
    Copyright (C) 2005  Meilof Veeningen <meilof@users.sourceforge.net>

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

#ifndef __POSLIB_DNSSEC_SIGN_H
#define __POSLIB_DNSSEC_SIGN_H

/*! \file poslib/dnssec-sign.h
 * \brief DNS message signing
 *
 * This file contains code related to signing DNS messages and verifying signed
 * messages.
 */


#include "dnsmessage.h"

/*!
 * \brief Verify TSIG signature
 *
 * Using the original TSIG record and the sign key #DnsMessage::sign_key, verify whether
 * the DNS message is correctly signed; if not, an exception will be thrown
 * and the check_tsig's error will be set to indicate the error that occured
 * when checking the DNS message.
 *
 * If use_orig_mac is set, the MAC from the original message will be included
 * in the check MAC, as dictated by RFC 2845, section 4.2. The original TSIG
 * record's MAC will be updated to match the answer's MAC.
 * The message length this function takes, is the message length without
 * the TSIG record included in the message, as returned by #DnsMessage::read_from_data.
 *
 * \param check_tsig    TSIG record to check against
 * \param message_tsig  TSIG record from the message
 * \param key           Key to use
 * \param message       Binary representation of message to check
 */
void verify_signature (DnsRR *check_tsig, DnsRR *message_tsig, stl_string key, message_buff message);
  
stl_string calc_mac (DnsRR &tsig_rr, message_buff message, stl_string sign_key, message_buff *extra = NULL);
/**< Calculate message MAC */

stl_string base64_decode (const char *str); /**< Base64-decode strings */
    
/*!
 * \brief create sparse TSIG record
 *
 * Creates a TSIG RR with suitable values for use as #DnsMessage::tsig_rr in checking and
 * signing of DNS messages.
 *
 * \param   keyname         Name of the key
 * \param   fudge           Permitted time difference between signing and checking
 * \param   sign_algorithm  Algorithm used to sign the message (currently, only the default is supported)
 * \return  The TSIG record
 */
DnsRR *tsig_record (domainname keyname, uint16_t fudge = 600, domainname sign_algorithm = "HMAC-MD5.SIG-ALG.REG.INT");

/*!
 * \brief get sign parameters from key string
 *
 * Sets the TSIG signing parameters from a key string; see the other
 * #tsig_from_string for syntax.
 */
void tsig_from_string (DnsRR*& tsig_rr, stl_string& sign_key, const char* keystring);

/*!
 * \brief set message sign parameters from key string
 *
 * Sets the TSIG signing parameters for a DNS message from a key string, which
 * is a string with the format keyname:key[:fudge]. The key is in BASE64
 * encoded form; if no fudge is given, the default fudge value of #tsig_record
 * will be used.
 *
 * \param message The message to set TSIG parameters of
 * \param keystring The key string
 */
void tsig_from_string (DnsMessage *message, const char *keystring);

void print_buff (int size, const unsigned char* buff);

#endif /* __POSLIB_DNSSEC_SIGN_H */
