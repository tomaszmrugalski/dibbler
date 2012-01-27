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

#include <ctype.h>

#include "dnsmessage.h"
#include "domainfn.h"
#include "sysstring.h"
#include "rr.h"
#include "exception.h"

#include "lexfn.h"

bool txt_to_bool(const char *buff) {
  if (strcmpi(buff, "yes") == 0) return true;
  if (strcmpi(buff, "true") == 0) return true;
  if (strcmpi(buff, "1") == 0) return true;
  if (strcmpi(buff, "on") == 0) return true;
  if (strcmpi(buff, "yo") == 0) return true;
  if (strcmpi(buff, "absolutely") == 0) return true;
  if (strcmpi(buff, "y") == 0) return true;
  if (strcmpi(buff, "j") == 0) return true; /* for dutch people out there ;) */
  if (strcmpi(buff, "no") == 0) return false;
  if (strcmpi(buff, "false") == 0) return false;
  if (strcmpi(buff, "0") == 0) return false;
  if (strcmpi(buff, "off") == 0) return false;
  if (strcmpi(buff, "nope") == 0) return false;
  if (strcmpi(buff, "never") == 0) return false;
  if (strcmpi(buff, "n") == 0) return false;
  throw PException(true, "Unknown boolean value %s", buff);
}

/* converts a buffer to a long numeric value, with postfix (e.g. 68K->68*1024) support */
int txt_to_int_internal(const char *_buff, bool support_negative) {
  char *buff = (char *)_buff;
  int val = 0, tmpval = 0;
  bool neg = false;
  bool have_digit = false;
  if (*buff == '-') {
    if (!support_negative) throw PException(true, "Negative number not supported: %s", _buff);
    neg = true; buff++;
  }
  while (1) {
    if (*buff >= '0' && *buff <= '9') {
      tmpval *= 10; tmpval += *buff - '0';
      have_digit = true;
    } else {
      if (*buff == '\0') {
        val += tmpval;
        if (!have_digit) throw PException(true, "Incorrect numeric value %s", _buff);
        return neg ? -val : val;
      }
      if (*buff == 'K') tmpval *= 1024;
      else if (*buff == 'M') tmpval *= 1048576;
      else if (*buff == 'G') tmpval *= 1073741824;
      else if (*buff == 's');
      else if (*buff == 'm') tmpval *= 60;
      else if (*buff == 'h') tmpval *= 3600;
      else if (*buff == 'd') tmpval *= 86400;
      else if (*buff == 'w') tmpval *= 604800;
      else if (*buff == 'y') tmpval *= 31536000;
      else throw PException(true, "Incorrect numeric value %s", _buff);

      val += tmpval;
      tmpval = 0;
    }
    buff++;
  }
}

int txt_to_negint(const char *buff) {
  return txt_to_int_internal(buff, true);
}

int txt_to_int(const char *buff) {
  return txt_to_int_internal(buff, false);
}

/* converts a buffer to an 32-bit ip number */
int txt_to_ip(unsigned char ip[4], const char *_buff, bool do_portion) {
  char *buff = (char *)_buff;
  int p = 0, tmp = 0, node = 0;
  if (strcmpi(buff, "any") == 0) {
    ip[0] = 0; ip[1] = 0; ip[2] = 0; ip[3]=  0;
    return 4;
  } else if (strcmpi(buff, "local") == 0) {
    ip[0] = 127; ip[1] = 0; ip[2] = 0; ip[3] = 1;
    return 4;
  } else if (strcmpi(buff, "none") == 0) {
    ip[0] = 255; ip[1] = 255; ip[2] = 255; ip[3] = 255;
  }
  ip[0] = 0; ip[1] = 0; ip[2] = 0; ip[3] = 0;
  while (buff[p] != '\0') {
    if (isdigit(buff[p])) {
      node *= 10;
      node += buff[p] - '0';
      if (node > 255) throw PException("IP node value exceeds 255");
    } else if (buff[p] == '*') {
      if (do_portion) {
        return tmp;
      } else {
        return 4;
      }
    } else {
      if (buff[p] == '.') {
        if (buff[p + 1] == '.') {
          throw PException("Expecting some value after dot");
        } else if (buff[p + 1] == '\0') break;
        if (tmp >= 3) throw PException("More than three dots in IP number");
        ip[tmp++] = (char)node;
        node = 0;
      } else throw PException("Unknown character in IP number");
    }
    p++;
  }
  ip[tmp++] = (unsigned char)node;
  if (tmp != 4 && !do_portion) throw PException("Not enough nodes in IP number");
  return tmp;
}

char incr_mask[8] = { 0, 128, 192, 224, 240, 248, 252, 254 };

void txt_to_iprange(unsigned char *iprange, const char *val) {
  char buff[128];
  char *ptr;
  int x, z;
  if (strcmpi(val, "any") == 0) {
    memset(iprange, 0, 8);
    return;
  }
  if (strcmpi(val, "none") == 0) {
    memset(iprange, 255, 4);
    memset(iprange + 4, 0, 4);
    return;
  }
  if ((ptr = strchr((char*)val, '/')) != NULL) {
    if (strchr(ptr, '.')) {
      /* complete IP number */
      txt_to_ip(iprange, ptr + 1);
    } else {
      memset(iprange, 0, 4);
      x = txt_to_int(ptr + 1);
      if (x > 128) throw PException("IPv6 mask value too long");
      for (z = 0; x >= 8; x -= 8) iprange[z++] = 255;
      iprange[z] = incr_mask[x];
    }
    if ((ptr - val) >= (signed)sizeof(buff)) throw PException("Ip number too long");
    memcpy(buff, val, ptr - val);
    buff[ptr - val] = '\0';
    txt_to_ip(iprange + 4, buff);
  } else {
    memset(iprange, 0, 4);
    for (x = txt_to_ip(iprange + 4, val, true) - 1; x >= 0; x--) iprange[x] = 255;
  }
}

bool iprange_matches(const unsigned char *iprange, const unsigned char *ip) {
  for (int x = 0; x < 4; x++) if ((ip[x] ^ iprange[x+4]) & iprange[x]) return false;
  return true;
}

int hextoint(char val) {
  if (val >= '0' && val <= '9') {
    return  val - '0';
  } else if (val >= 'a' && val <= 'f') {
    return val - 'a' + 10;
  } else if (val >= 'A' && val <= 'F') {
    return val - 'A' + 10;
  } else return -1;
}

/* converts a buffer to a 128-bit ipv6 number */
int txt_to_ipv6(unsigned char ipv6[16], const char *buff, bool do_portion) {
  int multigroup_pos = -1;
  int pos = -1;
  int node = 0; /* the pair we're working on */
  int nodeval = -1; /* node value */
  int nodestart = 0; /* start pos of node */
  int chval;
  int x;

  memset(ipv6, 0, 16);
  if (strcmpi(buff, ":any") == 0) return 16;
  if (strcmpi(buff, ":local") == 0) { ipv6[15] = 1; return 16; }

  while (buff[++pos] != '\0') {
    if (buff[pos] == '.') {
      /* an imbedded ipv4 */
      if (node + 2 > 8) throw PException("No room for embedded IPv4 in IPv6 address");
      try { txt_to_ip(&ipv6[node*2], &buff[nodestart]); }
      catch(PException p) { throw PException("Error in embedded IPv4 number: ", p); }
      node++;
      if (node == 8) break;
      nodeval = -1;
      break;
    }
    if (buff[pos] == ':') {
      /* write the previous node */
      if (pos) {
        if (nodeval == -1) throw PException("IPv6 address has empty node value");
        ipv6[node*2] = nodeval / 256;
        ipv6[node*2 + 1] = nodeval;
      } else {
        if (buff[pos + 1] != ':') throw PException("IPv6 address should have ::");
        node--;
      }
      if (buff[pos + 1] == ':') {
        /* we're having a multigroup indicator here */
         multigroup_pos = node + 1;
         pos++;
      }
      node++;
      if (node > 7) throw PException("IPv6 address has too much nodes");
      nodeval = -1;
      nodestart = pos + 1;
    } else if (buff[pos] == '*') {
      return node * 2;
    } else {
      chval = hextoint(buff[pos]);
      if (chval < 0) throw PException("Incorrect hex in IPv6 address");
      if (nodeval == -1)
        nodeval = chval;
      else
        nodeval = (nodeval * 16) + chval;
      if (nodeval > 65535) throw PException("IPv6 node val too large");
    }
  }

  if (nodeval != -1) {
    ipv6[node*2] = nodeval / 256;
    ipv6[node*2 + 1] = nodeval;
  } else {
    if (buff[pos - 1] == ':' && buff[pos - 2] != ':') throw PException("Expected :: in IPv6 address");
  }

  if (multigroup_pos != -1) {
    for (x = 15; x >= 14 - 2 * (node - multigroup_pos); x--)
      ipv6[x] = ipv6[x - 2 * (7 - node)];
    memset(&ipv6[multigroup_pos * 2], 0, 14 - (2*node));
  } else {
    if (node < 7) throw PException("Too less nodes in IPv6 address");
  }

  return 16;
}

void txt_to_ip6range(unsigned char *iprange, const char *val) {
  char buff[128];
  char *ptr;
  int x, z;
  if (strcmpi(val, "any") == 0) {
    memset(iprange, 0, 32);
    return;
  }
  if (strcmpi(val, "none") == 0) {
    memset(iprange, 255, 16);
    memset(iprange + 16, 0, 16);
    return;
  }
  if ((ptr = (char*)strchr(val, '/')) != NULL) {
    if (strchr(ptr, ':')) {
      /* complete IPv6 number */
      txt_to_ipv6(iprange, ptr + 1);
    } else {
      memset(iprange, 0, 16);
      x = txt_to_int(ptr + 1);
      if (x > 128) throw PException("IPv6 mask value too long");
      for (z = 0; x >= 8; x -= 8) iprange[z++] = 255;
      iprange[z] = incr_mask[x];
    }
    if ((ptr - val) >= (signed)sizeof(buff)) throw PException("Ip number too long");
    memcpy(buff, val, ptr - val);
    buff[ptr - val] = '\0';
    txt_to_ipv6(iprange + 16, buff);
  } else {
    memset(iprange, 0, 16);
    for (x = txt_to_ipv6(iprange + 16, val, true) - 1; x >= 0; x--) iprange[x] = 255;
  }
}

bool ip6range_matches(const unsigned char *iprange, const unsigned char *ip) {
  for (int x = 0; x < 16; x++) if ((ip[x] ^ iprange[x+16]) & iprange[x]) return false;
  return true;
}

#define R_IP4 0
#ifdef HAVE_IPV6
#define R_IP6 1
#endif
#define R_NONE 2
#define R_ANY 3

bool in_addrrange_list(stl_list(addrrange) &lst, _addr *a) {
  stl_list(addrrange)::iterator it = lst.begin();
  while (it != lst.end()) {
    if (addrrange_matches(it->range, a)) return true;
    it++;
  }
  return false;
}

#ifdef HAVE_SLIST
bool in_addrrange_list(stl_slist(addrrange) &lst, _addr *a) {
  stl_slist(addrrange)::iterator it = lst.begin();
  while (it != lst.end()) {
    if (addrrange_matches(it->range, a)) return true;
    it++;
  }
  return false;
}
#endif

#ifdef HAVE_SLIST
bool in_addr_list(stl_slist(_addr) &lst, _addr *a, bool match_port) {
  stl_slist(_addr)::iterator it = lst.begin();
  while (it != lst.end()) {
    if (match_port) {
      if (addrport_matches(&*it, a)) return true;
    } else {
      if (address_matches(&*it, a)) return true;
    }
    it++;
  }
  return false;
}
#endif

bool in_addr_list(stl_list(_addr) &lst, _addr *a, bool match_port) {
  stl_list(_addr)::iterator it = lst.begin();
  while (it != lst.end()) {
    if (match_port) {
      if (addrport_matches(&*it, a)) return true;
    } else {
      if (address_matches(&*it, a)) return true;
    }
    it++;
  }
  return false;
}

void txt_to_addrrange(unsigned char *iprange, const char *val) {
  if (strcmpi(val, "any") == 0) { iprange[0] = R_ANY; return; }
  if (strcmpi(val, "none") == 0) { iprange[0] = R_NONE; return; }
  if (!strchr(val, ':')) {
    iprange[0] = R_IP4;
    txt_to_iprange(iprange + 1, val);
#ifdef HAVE_IPV6
  } else {
    iprange[0] = R_IP6;
    txt_to_ip6range(iprange + 1, val);
#endif
  }
}

bool addrrange_matches(const unsigned char *iprange, _addr *a) {
  switch (iprange[0]) {
    case R_NONE: return false;
    case R_ANY: return true;
    case R_IP4: return iprange_matches(iprange + 1, get_ipv4_ptr(a));
#ifdef HAVE_IPV6
    case R_IP6: return ip6range_matches(iprange + 1, get_ipv6_ptr(a));
#endif
  }
  return false;
}

/* converts an email adress to a domain name
   (may be a <domain-name> or a true email address) */

void txt_to_email(_domain target, const char *src, _cdomain origin) {
  unsigned char dom[DOM_LEN];
  char *cptr;

  if ((cptr = (char *)strchr(src, '@')) != NULL && !(cptr[0] == '@' && cptr[1] == 0)) {
    /* contains a '@', so assume it's an email address */
    if (src[0] == '@') throw PException("Incorrect email adress/domain name: begins with @");
    domfromlabel(target, src, cptr - src);
    txt_to_dname(dom, cptr + 1);
    domcat(target, dom);
  } else {
    /* common domain name */
    txt_to_dname(target, src, origin);
  }
}

/* converts a textual representation for a domain name to an rfc <domain-name> */

#define hexfromint(hex) (((hex) < 10) ? (hex) + '0' : ((hex) - 10) + 'a')

void txt_to_dname(_domain target, const char *src, _cdomain origin) {
  char *ptr;
  unsigned char label[DOM_LEN];
  unsigned char tmp[16];
  char hex;
  int ttmp, ret;
  
  if (src[0] == '@' && src[1] == '\0') {
    /* nothing but the origin */
    if (!origin)
      target[0] = '\0';
    else
      memcpy(target, origin, domlen((_domain)origin));
    return;
  }

  target[0] = '\0';

  if (src[0] == '.' && src[1] == '\0') return;

  while (*src) {
    if (src[0] == '.' && src[1] != '\0') {
      /* our extension: allow .192.168.* and .dead:beef:* */
      if (strchr(src + 1, ':')) {
        if (domlen(target) + 42 >= DOM_LEN) throw PException("IPv6 domainname doesn't fit");
        ret = txt_to_ipv6(tmp, src + 1, true);
        for (ttmp = ret - 1; ttmp >= 0; ttmp--) {
          hex = hexfromint(tmp[ttmp] & 15);
          domfromlabel(target + domlen(target) - 1, &hex, 1);
          hex = hexfromint(tmp[ttmp] / 16);
          domfromlabel(target + domlen(target) - 1, &hex, 1);
        }
        domcat(target, (_domain) "\3ip6\3int");
        return;
      } else {
        /* ipv4 */
        if (domlen(target) + 14 >= DOM_LEN) throw PException("IPv6 domainname doesn't fit");
        ret = txt_to_ip(tmp, src + 1, true);
        for (ttmp = ret - 1; ttmp >= 0; ttmp--) {
          sprintf((char*)tmp + 4, "%d", tmp[ttmp]);
          domfromlabel(target + domlen(target) - 1, (char*)tmp + 4);
        }
        domcat(target, (_domain) "\7in-addr\4arpa");
        return;
      }    
    }
    
    ptr = (char *)strchr(src, '.');
    if (ptr) {
      if (ptr == src) throw PException("Zero length label");
      domfromlabel(label, src, ptr - src);
      domcat(target, label);
      src = ptr + 1;
    } else {
      /* end of relative domain name */
      domfromlabel(label, src);
      domcat(target, label);
      if (origin) domcat(target, (_domain)origin);
      return;
    }
  }
}

void txt_to_addr(_addr *ret, const char *addr, int default_port, bool client) {
  char taddr[128];
  char *ptr = strchr((char *)addr, '#');
  int x;
  if (ptr) {
    if ((unsigned)(ptr - addr) > (unsigned)sizeof(taddr)) throw PException("Address too long");
    memcpy(taddr, addr, (unsigned)(ptr - addr));
    taddr[ptr-addr] = '\0';
    txt_to_addr(ret, taddr, default_port, client);
    addr_setport(ret, txt_to_int(ptr + 1));
  } else {
    try {
      x = txt_to_int(addr);
      if (client)
        getaddress(ret, "127.0.0.1", x);
      else
        getaddress(ret, "0.0.0.0", x);
    } catch (PException p) {
      getaddress(ret, addr, default_port);
    }
  }
}

u_int32 poslib_degstr(char *&src, char pre, char post) {
  int deg, min = 0, sec = 0, msec = 0;
  u_int32 ret;
  string tmp;

  deg = txt_to_int(read_entry(src).c_str());
  tmp = read_entry(src);
  if (isdigit(tmp[0])) {
    min = txt_to_int(tmp.c_str());
    tmp = read_entry(src);
    if (isdigit(tmp[0])) {
      if (strchr(tmp.c_str(), '.')) {
        if (sscanf(tmp.c_str(), "%d.%d", &sec, &msec) != 2)
          throw PException(true, "Malformed LOC RR: invalid angle seconds %s", tmp.c_str());
      } else sec = txt_to_int(tmp.c_str());
      tmp = read_entry(src);
    }
  }
  ret = msec + sec * 1000 + min * 60000 + deg * 3600000;

  if (toupper(tmp[0]) == post)
    ret = 2147483648u + ret;
  else if (toupper(tmp[0]) == pre)
    ret = 2147483648u - ret;
  else throw PException(true, "Malformed LOC RR: expected '%c' or '%c', got %s", pre, post, tmp.c_str());

  return ret;
}

unsigned char poslib_loc_precision(const char *str) {
  int x, y = 0;
  int n = 0;
  if (sscanf(str, "%d.%dm", &x, &y) < 1) throw PException(true, "Invalid precision: %s", str);
  x = x*100+y;  
  while (x >= 10) {
    x/=10; n++;
  }
  return (x << 4) + n;
}

void txt_to_loc(unsigned char *res, char *&src) {
  stl_string tmp;
  u_int32 t;
  int x,y;

  res[0] = 0; /* version */

  /* read longitude, latitude */
  t = poslib_degstr(src, 'S', 'N');
  memcpy(res + 4, uint32_buff(t), 4);
  t = poslib_degstr(src, 'W', 'E');
  memcpy(res + 8, uint32_buff(t), 4);

  /* read altitude */
  x = y = 0;
  tmp = read_entry(src);
  if (sscanf(tmp.c_str(), "%d.%dm", &x, &y) <= 0) throw PException("Invalid altitude");
  memcpy(res + 12, uint32_buff((x*100+y)+10000000), 4);

  if (src[0]) /* size */
    res[1] = poslib_loc_precision(read_entry(src).c_str());
  else
    res[1] = 0x12;

  if (src[0]) /* hor */
    res[2] = poslib_loc_precision(read_entry(src).c_str());
  else
    res[2] = 0x16;

   if (src[0]) /* ver */
    res[3] = poslib_loc_precision(read_entry(src).c_str());
  else
    res[3] = 0x13;
}

int power10ed(unsigned char val) {
  int exp = val%15;
  int n = 1;
  while (--exp) n *= 10;
  return n * (val >> 4);
}

stl_string pos_degtostring(uint32_t val, char plus, char min) {
  char buff[32];
  char mod;
  if (val >= 2147483648u) { mod = plus; val -= 2147483648u; }
  else { mod = min; val = 2147483648u - val; }
  sprintf(buff, "%d %d %.3f %c", val / 3600000, (val % 3600000) / 60000,
                               (float)((val % 60000) / 1000), mod);
  return buff;
}

stl_string str_degrees(uint32_t value, char pos, char neg) {
  char post;
  char buff[32];
  if (value > 2147483648u) { post = pos; value -= 2147483648u; }
  else { post = neg; value = 2147483648u - value; }
  sprintf(buff, "%d %d %d.%2d %c", value/360000,(value%360000)/6000,(value%6000)/100,value%100, post);
  return buff;
}


stl_string str_loc(const unsigned char *locrr) {
  stl_string ret;
  char locbuff[96];
  uint32_t size = power10ed(locrr[1]),
           horpre = power10ed(locrr[2]),
           verpre = power10ed(locrr[3]),
           lati = uint32_value(locrr+4),
           longi = uint32_value(locrr+8),
           alti = uint32_value(locrr+12);

  sprintf(locbuff, "%.2fm %.2fm %.2fm %.2fm", ((float)(alti-10000000))/100, (float)size / 100,
                                           (float)horpre / 100, (float)verpre / 100);

  ret = pos_degtostring(lati, 'N', 'S') + " " + pos_degtostring(longi, 'E', 'W') + " " + locbuff;
  printf("Ret: %s\n", ret.c_str());
  return ret;
}

uint16_t txt_to_qclass(const char *str, bool allow_q) {
  if (strcmpi(str, "IN") == 0) return CLASS_IN;
  if (strcmpi(str, "CS") == 0) return CLASS_CS;
  if (strcmpi(str, "CH") == 0) return CLASS_CH;
  if (strcmpi(str, "HS") == 0) return CLASS_HS;
  if (allow_q) {
    if (strcmpi(str, "ANY") == 0) return QCLASS_ANY;
    if (strcmpi(str, "NONE") == 0) return QCLASS_NONE;
  }  
  throw PException(true, "Unknown class type %s", str);
}  

stl_string intstring(u_int16 x) {
  char tmp[16];
  snprintf(tmp, 15, "%d", x);
  return stl_string(tmp);
}

stl_string str_type(u_int16 type) {
  rr_type *rrtype = rrtype_getinfo(type);
  if (rrtype) return rrtype->name;
  return intstring(type);
}

stl_string str_qtype(u_int16 qtype) {
  if (qtype == QTYPE_AXFR) return "AXFR";
  if (qtype == QTYPE_IXFR) return "IXFR";
  if (qtype == QTYPE_MAILB) return "MAILB";
  if (qtype == QTYPE_MAILA) return "MAILA";
  if (qtype == QTYPE_ALL) return "ANY";
  if (qtype == QTYPE_NONE) return "NONE";
  return str_type(qtype);
}

stl_string str_class(u_int16 ctype) {
  if (ctype == CLASS_IN) return "IN";
  if (ctype == CLASS_CS) return "CS";
  if (ctype == CLASS_CH) return "CH";
  if (ctype == CLASS_HS) return "HS";
  return intstring(ctype);
}

stl_string str_qclass(u_int16 qctype) {
  if (qctype == QCLASS_ALL) return "ANY";
  if (qctype == QCLASS_NONE) return "NONE";
  return str_class(qctype);
}

/* OPCODEs */
stl_string str_opcode(u_int16 opcode) {
  if (opcode == OPCODE_QUERY) return "QUERY";
  if (opcode == OPCODE_IQUERY) return "IQUERY";
  if (opcode == OPCODE_STATUS) return "STATUS";
  if (opcode == OPCODE_COMPLETION) return "COMPL";
  if (opcode == OPCODE_NOTIFY) return "NOTIFY";
  if (opcode == OPCODE_UPDATE) return "UPDATE";
  return intstring(opcode);
}

stl_string str_rcode(int rcode) {
  if (rcode == RCODE_NOERROR) return "NOERROR";
  if (rcode == RCODE_QUERYERR) return "QUERYERR";
  if (rcode == RCODE_SRVFAIL) return "SRVFAIL";
  if (rcode == RCODE_NXDOMAIN) return "NXDOMAIN";
  if (rcode == RCODE_NOTIMP) return "NOTIMP";
  if (rcode == RCODE_REFUSED) return "REFUSED";
  if (rcode == RCODE_YXDOMAIN) return "YXDOMAIN";
  if (rcode == RCODE_YXRRSET) return "YXRRSET";
  if (rcode == RCODE_NXRRSET) return "NXRRSET";
  if (rcode == RCODE_NOTAUTH) return "NOTAUTH";
  if (rcode == RCODE_NOTZONE) return "NOTZONE";
  return intstring(rcode);
}

stl_string str_ttl(uint32_t ttl) {
  char val[16];
  stl_string res;
  struct _factor {
    char prefix;
    uint32_t factor;
  } factors[] = { {'y',31536000}, {'w',604800}, {'d',86400}, {'h',3600}, {'m',60}, {'s',1} };
  _factor *f = &factors[0];
  int x;
  if (ttl == 0) return "0";
  while (f->factor != 1) {
    if (ttl >= f->factor) {
      x = ttl / f->factor;
      sprintf(val, "%d%c", x, f->prefix);
      res += val;
      ttl -= x * f->factor;
    }
    if (ttl == 0) return res;
    f++;
  }
  sprintf(val, "%d", ttl);
  res += val;
  return res;
}
