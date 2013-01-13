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


#include "poslib.h"
#include "dnssec-sign.h"

extern "C" {
#include "nettle/hmac.h"
#include "nettle/base64.h"
#include "nettle/md5.h"
}

/// @brief verifies TSIG of received response
///
/// Make sure that message_buff is trimmed down to not include TSIG
/// record, as it must be passed in message_tsig.
///
/// @param check_tsig TSIG RR from the original message
/// @param message_tsig TSIR RR from the response message that we are validating
/// @param key the key used for signing
/// @param message received message (without TSIG record)
void verify_signature (DnsRR *check_tsig, DnsRR *message_tsig,
                       stl_string key, message_buff message) {
  if (!message_tsig)
    throw PException (true, "Unsigned answer to a signed message (key %s)", check_tsig->NAME.tocstr ());

  unsigned char *errorptr = rr_getdata (check_tsig->RDATA, DNS_TYPE_TSIG, 5);

  domainname alg_name = rr_getdomain (check_tsig->RDATA, DNS_TYPE_TSIG),
             rr_alg_name = rr_getdomain (message_tsig->RDATA, DNS_TYPE_TSIG);
  if (rr_alg_name != alg_name || check_tsig->NAME != message_tsig->NAME) {
    memcpy (errorptr, uint16_buff (RCODE_BADKEY), 2);
    throw PException (true, "Key name/algorithm does not match: question signed with %s, answer "
                      "signed with %s", check_tsig->NAME.tocstr(), message_tsig->NAME.tocstr());
  }
  
  unsigned char *rr_macpos = rr_getdata (message_tsig->RDATA, DNS_TYPE_TSIG, 3),
                *macpos    = rr_getdata (check_tsig->RDATA, DNS_TYPE_TSIG, 3);
  uint16_t rr_maclen = uint16_value (rr_macpos),
           maclen    = uint16_value (macpos);
  
  if ((message.msg[3] & 15) == RCODE_NOTAUTH) {
    /* error! */
    uint16_t err = rr_getshort (message_tsig->RDATA, DNS_TYPE_TSIG, 5);
    if (err == RCODE_BADSIG)
      throw PException (true, "Question signed with %s had bad signature", check_tsig->NAME.tocstr());
    else if (err == RCODE_BADKEY)
      throw PException (true, "Question was signed with bad key (%s)", check_tsig->NAME.tocstr());
    else if (err == RCODE_BADTIME)
      throw PException (true, "Question sign time invalid (query time %d, answer time %d)",
                        uint48_value (rr_getdata (check_tsig->RDATA, DNS_TYPE_TSIG, 1)),
                        uint48_value (rr_getdata (message_tsig->RDATA, DNS_TYPE_TSIG, 1)));
    else throw PException (true, "Unknown sign error: %d", err);
  }
  
  time_t clienttime = time (NULL);
  u_int48 servertime = uint48_value (rr_getdata (check_tsig->RDATA, DNS_TYPE_TSIG, 1));
  uint16_t fudge = rr_getshort (check_tsig->RDATA, DNS_TYPE_TSIG, 2);
  
  if ( (clienttime > servertime && clienttime - servertime > fudge) ||
       (servertime > clienttime && servertime - clienttime > fudge) ) {
      throw PException (true, "Answer sign time invalid (answer time %d, real time %d)",
                        uint48_value (rr_getdata (message_tsig->RDATA, DNS_TYPE_TSIG, 1)),
                        clienttime);
    // TODO: set TSIG error of check_tsig for servers
  }

  if (rr_maclen != MD5_DIGEST_SIZE) {
    memcpy (errorptr, uint16_buff (RCODE_BADSIG), 2);
    throw PException (true, "Incorrect MAC size: %d", rr_maclen);
  }
  
  /* calculate MAC */
  message_buff ext; /* TODO: document: if this is from a message, it actually
                       means that the message was unsigned, but we should still
                       include the two length */
  if (maclen)
    ext = message_buff (macpos, maclen + 2);
  stl_string mac = calc_mac (*message_tsig, message, key, &ext);
  
  if (memcmp (mac.c_str(), rr_macpos + 2, MD5_DIGEST_SIZE) != 0) {
    memcpy (errorptr, uint16_buff (RCODE_BADSIG), 2);
    throw PException (true, "Message signed with key %s has bad signature",
                      message_tsig->NAME.tocstr ());
  }
}

//#undef hmac_md5_update
//void hmac_md5_update (struct hmac_md5_ctx *md5, int len, const unsigned char* ptr) {
//  print_buff (len, ptr);
//  nettle_hmac_md5_update (md5, len, ptr);
//}

stl_string calc_mac (DnsRR &tsig_rr, message_buff msg, stl_string sign_key, message_buff *extra) {
  struct hmac_md5_ctx md5;
  unsigned char md5res [MD5_DIGEST_SIZE];
  memset (&md5, 0, sizeof (hmac_md5_ctx));
    
  unsigned char *digestpos = rr_getdata (tsig_rr.RDATA, DNS_TYPE_TSIG, 3);
  uint16_t digestlen = uint16_value (digestpos);
  
//  print_buff (sign_key.size(), (unsigned char*)sign_key.c_str());
  hmac_md5_set_key(&md5, sign_key.size(), (uint8_t *)sign_key.c_str());
  
//  printf ("Begin MAC calculation\n");
  
  /* original MAC */
  if (extra && extra->len) hmac_md5_update(&md5, extra->len, extra->msg);
  
  /* message */
  hmac_md5_update(&md5, 10, msg.msg);
  hmac_md5_update(&md5, 2, uint16_buff (uint16_value (msg.msg + 10) - 1));
  hmac_md5_update(&md5, msg.len - 12, msg.msg + 12);
  
  /* tsig rr */
  stl_string canname = tsig_rr.NAME.canonical();
  hmac_md5_update(&md5, canname.size(), (const uint8_t *)canname.c_str());
  hmac_md5_update(&md5, 2, uint16_buff (QCLASS_ANY));
  hmac_md5_update(&md5, 4, uint32_buff (0));
  
  /* start of TSIG rrdata */
  domainname dom = domainname (true, tsig_rr.RDATA);
  canname = dom.canonical();
  hmac_md5_update (&md5, canname.size(), (const uint8_t*)canname.c_str());
  hmac_md5_update(&md5, 8, digestpos - 8);
  
  /* rest, excluding original ID */
  hmac_md5_update(&md5, tsig_rr.RDLENGTH - (digestpos - tsig_rr.RDATA) - digestlen - 4,
                  digestpos + digestlen + 4);
  hmac_md5_digest(&md5, MD5_DIGEST_SIZE, md5res);
  
//  printf ("End MAC calculation\n");
  
//  print_buff (MD5_DIGEST_SIZE, md5res);
  
  stl_string ret;
  ret.append ((char*)md5res, MD5_DIGEST_SIZE);
  return ret;
}


stl_string base64_decode (const char *line) {
  stl_string ret;
  
  unsigned len = strlen (line);
  char *res = (char *)malloc (len);
    
  struct base64_decode_ctx str;
  base64_decode_init (&str);
  base64_decode_update (&str, &len, (uint8_t*)res, len, (uint8_t*)line);
  base64_decode_final (&str);
  ret.append (res, len);
  free (res);
  
  return ret;
}

stl_string base64_encode (const char *buff, int bufflen) {
  stl_string ret;

  unsigned len = strlen (buff);
  char *res = (char *) malloc (BASE64_ENCODE_LENGTH (bufflen));

  struct base64_encode_ctx str;
  base64_encode_init (&str);
  len = base64_encode_update (&str, (uint8_t *) res, len, (uint8_t*) buff); // TODO: args?
  base64_encode_final (&str, (uint8_t*) res);
  ret.append (res, len);
  free (res);

  return ret;
}
   
DnsRR *tsig_record (domainname keyname, uint16_t fudge, domainname sign_algorithm) {
  DnsRR *tsig_rr = new DnsRR (keyname, DNS_TYPE_TSIG, QCLASS_ANY, 0);
  
  stl_string tsig_rrdata;
    
  tsig_rrdata.append ((char *)sign_algorithm.c_str (), sign_algorithm.len ());
  tsig_rrdata.append (6, '\0'); /* empty signtime */
  tsig_rrdata.append ((char *)uint16_buff (fudge), 2);
  tsig_rrdata.append (8, '\0'); /* empty MAC; original ID; error; other length */
    
  tsig_rr->RDLENGTH = tsig_rrdata.size ();
  tsig_rr->RDATA = (unsigned char*) memdup (tsig_rrdata.c_str(), tsig_rr->RDLENGTH);
  
  return tsig_rr;
}

void tsig_from_string (DnsRR*& tsig_rr, stl_string& sign_key, const char* keystring) {
  stl_string keyname, key;
  const char *ptr;
  
  ptr = strchr (keystring, ':');
  if (!ptr) throw PException (true, "%s is not a valid key string (should be keyname:key)", keystring);
  keyname.append (keystring, (int)(ptr - keystring));
  
  keystring = ptr + 1;
  
  ptr = strchr (keystring, ':');
  key.append (keystring, ptr ? (int)(ptr - keystring) : strlen (keystring));
  
  if (tsig_rr) {
    delete tsig_rr;
    tsig_rr = NULL;
  }
  
  if (ptr) {
    int fudge = txt_to_int (ptr + 1);
    tsig_rr = tsig_record (keyname.c_str (), fudge);
  } else {
    tsig_rr = tsig_record (keyname.c_str ());
  }
  
  sign_key = base64_decode (key.c_str ());
}

void tsig_from_string (DnsMessage *message, const char *keystring) {
	tsig_from_string (message->tsig_rr, message->sign_key, keystring);
}
