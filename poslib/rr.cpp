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

#include "dnsmessage.h"
#include "domainfn.h"
#include "sysstring.h"
#include "exception.h"
#include "bits.h"
#include "lexfn.h"

#include "rr.h"
#include "domainfn.h"

/* the RR type database */

const rr_type rr_types[] = {
    { "A",      1,  "i",        R_NONE        },
    { "NS",     2,  "d",        R_ASPCOMPRESS },
    { "MD",     3,  "d",        R_ASPCOMPRESS },
    { "MF",     4,  "d",        R_ASPCOMPRESS },
    { "CNAME",  5,  "d",        R_COMPRESS    },
    { "SOA",    6,  "dmltttt",  R_COMPRESS    },
    { "NULL",   10, "n",        R_NONE        },
    { "WKS",    11, "iw",       R_NONE        },
    { "PTR",    12, "d",        R_COMPRESS    },
    { "HINFO",  13, "cc",       R_NONE        },
    { "MX",     15, "sd",       R_ASPCOMPRESS },
    { "TXT",    16, "h",        R_NONE        },
    { "RP",     17, "md",       R_NONE        },
    { "AFSDB",  18, "sd",       R_ASP,        },
    { "PX",     26, "sdd",      R_NONE        },
    { "AAAA",   28, "6",        R_NONE        },
    { "LOC",    29, "o",        R_NONE        },
    { "SRV",    33, "sssd",     R_ASP,        },
    { "NAPTR",  35, "sscccd",   R_NONE        },
    { "A6",     38, "7",        R_NONE        },
    { "DNAME",  39, "d",        R_ASP         },
};

const int n_rr_types = sizeof(rr_types) / sizeof(rr_type);

rr_type *rrtype_getinfo(u_int16 type) {
   int t;
   for (t = 0; t < n_rr_types; t++)
     if (rr_types[t].type == type) return (rr_type *)&rr_types[t];
     
   return NULL;
}

rr_type *rrtype_getinfo(const char *name) {
   int t;
   for (t = 0; t < n_rr_types; t++)
     if (strcmpi(rr_types[t].name, name) == 0) return (rr_type *)&rr_types[t];

     
   return NULL;
}

char *rrtype_getname(u_int16 type) {
  rr_type *info = rrtype_getinfo(type);
  if (info) return info->name; else return NULL;
}

uint16_t qtype_getcode(const char *name, bool allow_qtype) {
  try {
    uint16_t ret = txt_to_int(name);
    if (!is_common_rr(ret)) throw PException();
    return ret;
  } catch (PException p) {
    if (allow_qtype) {
      if (strcmpi(name, "maila") == 0) return QTYPE_MAILA;
      if (strcmpi(name, "mailb") == 0) return QTYPE_MAILB;
      if (strcmpi(name, "ixfr") == 0) return QTYPE_IXFR;
      if (strcmpi(name, "axfr") == 0) return QTYPE_AXFR;
      if (strcmpi(name, "any") == 0) return QTYPE_ANY;
      if (strcmpi(name, "all") == 0) return QTYPE_ANY;
    }  
    rr_type *t = rrtype_getinfo(name);
    if (!t) throw PException(true, "Qtype %s not supported", name);
    return t->type;
  }
}

bool answers_qtype(uint16_t rrtype, uint16_t qtype) {
  return (rrtype == qtype ||
          qtype == QTYPE_ANY ||
          (qtype == QTYPE_MAILA && (rrtype == DNS_TYPE_MF || rrtype == DNS_TYPE_MD)) ||
          (qtype == QTYPE_MAILB && (rrtype >= DNS_TYPE_MB && rrtype <= DNS_TYPE_MR)));
}

bool is_common_rr(uint16_t rrtype) {
  return (rrtype < QTYPE_IXFR);
}

/* this function complains when, to get to know the length, it has to exceed
   the buffer border. otherwise, it returns the length whether it's beyond the
   border or not. len is guaranteed to be at least one. */

int rr_len(char prop, message_buff &buff, int ix, int len) {
  unsigned char *ptr;
  int x;
  switch (prop) {
    case 'd': /* a domain name */
    case 'm': /* email address */
      return dom_comprlen(buff, ix);
    case 'i': /* ipv4 number */
    case 'l': /* 32-bit number */
		case 't':
      return 4;
    case 's': /* 16-bit number */
      return 2;
    case 'c': /* character string */
      return buff.msg[ix] + 1;
    case 'h': /* character strings */
      ptr = buff.msg + ix;
      while (ptr - buff.msg  - ix < len) ptr += *ptr + 1;
      if (ptr != buff.msg + ix + len) throw PException("Character strings too long for RR");
      return len;
    case 'n': /* NULL rdata */
      return len;
    case 'w': /* well-known services */
      if (len < 5) throw PException("WKS RR too long for RR");
      return len;
    case '6': /* ipv6 address */
      return 16;
    case '7': /* ipv6 address + prefix */
      x = ((135 - buff.msg[ix]) / 8); /* prefix length in bytes */
      if (ix + x + 1 >= len) throw PException("A6 too long for RR");
      if (buff.msg[ix] != 0)
        /* domain name nessecary */
        x += dom_comprlen(buff, ix + x + 1);
      return x + 1;
    case 'o': /* DNS LOC */
      if (buff.msg[ix] != 0) throw PException("Unsupported LOC version");
      return 16;
  }
  throw PException(true, "Unknown RR item type %c", prop);
}
      
void rr_read(u_int16 RRTYPE, unsigned char*& RDATA, uint16_t &RDLEN, message_buff &buff, int ix, int len) {
  rr_type *info = rrtype_getinfo(RRTYPE);
  char *ptr;
  stl_string res;
  int x;
  _domain dom;

  if (ix + len > buff.len) throw PException("RR doesn't fit in DNS message");

  if (info) {
    /* we support the RR type */
    try {
      ptr = info->properties;
      while (*ptr) {
        x = rr_len(*ptr, buff, ix, len);
        if (x > len) throw PException("RR item too long!");
        if (*ptr == 'd' || *ptr == 'm') {
          /* domain name: needs to be decompressed */
          dom = dom_uncompress(buff, ix);
          res.append((char*)dom, domlen(dom));
          free(dom);
        } else {
          res.append((char*)buff.msg + ix, x);
        }
        
        ix += x;
        len -= x;
        
        ptr++;
      }
      if (len != 0) throw PException("extra data in RR");
    } catch(PException p) {
      throw PException("Parsing RR failed: ", p);
    }
    if (len != 0) throw PException("RR length too long");
  } else {
    /* we do not support the RR type: just copy it altogether */
    res.append((char*)buff.msg + ix, len);
  }
  RDLEN = res.length();
  RDATA = (unsigned char *)memdup((void *)res.c_str(), res.length());
}

void rr_write(u_int16 RRTYPE, unsigned char *RDATA, uint16_t RDLEN, stl_string &dnsmessage, stl_slist(dom_compr_info) *comprinfo) {
  rr_type *info = rrtype_getinfo(RRTYPE);
  char *ptr;
  int len, ix = 0;
  message_buff rrbuff(RDATA, RDLEN);

  if (!info || !(info->flags & R_COMPRESS) || !comprinfo) {
    dnsmessage.append((char*)RDATA, RDLEN);
    return;
  }

  ptr = info->properties;
  while (*ptr) {
    len = rr_len(*ptr, rrbuff, ix, RDLEN - ix);
    if ((*ptr == 'd' || *ptr == 'm') && comprinfo) {
      /* compress dname */
      dom_write(dnsmessage, RDATA + ix, comprinfo);
    } else {
      dnsmessage.append((char*)RDATA + ix, len);
    }
    ix += len;
    ptr++;
  }
}

stl_string rr_tostring(u_int16 RRTYPE, const unsigned char *_RDATA, int RDLENGTH) {
  return rr_torelstring(RRTYPE, _RDATA, RDLENGTH, "");
}

stl_string rr_property_to_string(char type, const unsigned char*& RDATA, int RDLENGTH, domainname& zone) {
    char buff[128];
    const unsigned char *ptr;
    string ret;
    domainname dom;
    int x, y;
    message_buff msgbuff = message_buff((unsigned char*)RDATA, RDLENGTH);

    switch (type) {
      case 'd': // domain name
      case 'm': // email address
        dom = domainname(true, RDATA);
        RDATA += domlen((_domain)RDATA);

        if (zone == "")
          return dom.tostring();
        else          
          return dom.torelstring(zone);
      case 'i': // ipv4 address
        sprintf(buff, "%d.%d.%d.%d", RDATA[0], RDATA[1], RDATA[2], RDATA[3]);
        RDATA += 4;
        return buff;
      case 's': // 16-bit value
        sprintf(buff, "%d", RDATA[0] * 256 + RDATA[1]);
        RDATA += 2;
        return buff;
      case 'l': // 32-bit value
        sprintf(buff, "%d", RDATA[0] * 16777216 + RDATA[1] * 65536 + RDATA[2] * 256 + RDATA[3]);
        RDATA += 4;
        return buff;
      case 't': 
        ret = str_ttl(RDATA[0] * 16777216 + RDATA[1] * 65536 + RDATA[2] * 256 + RDATA[3]);
        RDATA += 4;
        return ret;
      case 'c': // character string
        ret.append("\"");
        ret.append((char*)RDATA + 1, (int)*RDATA);
        ret.append("\"");
        RDATA += *RDATA + 1;
        return ret;
      case 'h': // character strings
        ptr = RDATA + RDLENGTH;
        while (RDATA < ptr) {
          ret.append("\"");
          ret.append((char*)RDATA + 1, (int)*RDATA);
          ret.append("\" ");
          RDATA += *RDATA + 1;
        }
        return ret;
      case '6':
        sprintf(buff, "%x:%x:%x:%x:%x:%x:%x:%x",
          RDATA[0]*256 + RDATA[1], RDATA[2]*256 + RDATA[3], RDATA[4]*256 + RDATA[5],
          RDATA[6]*256 + RDATA[7], RDATA[8]*256 + RDATA[9], RDATA[10]*256 + RDATA[11],
          RDATA[12]*256 + RDATA[13], RDATA[14]*256 + RDATA[15]);
        RDATA += 16;
        return buff;
      case 'w': // well-known services
        sprintf(buff, "%d", *(RDATA++));
        ret.append(buff);
        y = 0;
        ptr = RDATA + RDLENGTH;
        while (RDATA < ptr) {
          for (x = 0; x < 8; x++) {
            if (bitisset(RDATA, x)) {
              sprintf(buff, " %d", y + x);
              ret.append((char*)buff);
            }
          }
          y += 8;
          RDATA++;
        }
        return ret;
      case 'o': // RFC1876 location information
        return str_loc(RDATA);
      default:
        return "?";
    }  
}  

stl_string rr_torelstring(u_int16 RRTYPE, const unsigned char *_RDATA, int RDLENGTH, domainname zone) {
  rr_type *info = rrtype_getinfo(RRTYPE);
  char *ptr;
  const unsigned char *RDATA = _RDATA;
  stl_string ret;

  if (!info) return "<unknown rr type>";

  ptr = info->properties;
  while (*ptr) {
    if (ret != "") ret.append(" ");
    ret.append (rr_property_to_string (*ptr, RDATA, RDLENGTH - (int)(RDATA - _RDATA), zone));
    ptr++;
  }
  return ret;
}

void read_line(char *buff, FILE *f, int *linenum, int *linenum_old, int buffsz) {
  bool inb = false;
  int c;
  int pos = 0;
  if (linenum && linenum_old) *linenum_old = *linenum;
  while (!feof(f)) {
    /* line reading magic */
    c = fgetc(f);
    if (c == EOF) break;

    switch (c) {
      case '\n':
      case '\r': /* line end */
        if (c == '\n' && linenum) (*linenum)++;
        while (!feof(f)) {
          c = fgetc(f);
          if (c == '\n' && linenum) (*linenum)++;
          if (c != '\n' && c != '\r') {
            ungetc(c, f);
            break;
          }
        }
        if (feof(f) || !inb) goto rln_finished;
        continue;
      case '(':
        inb = true;
        continue;
      case ')':
        inb = false;
        continue;
      case '"':
        if (pos > buffsz - 2) throw PException("Line too long");
        buff[pos++] = '"';
        do {
          c = fgetc(f);
          if (c == '"') break;
          if (c == '\n' && linenum) (*linenum)++;
          if (pos > buffsz - 3) throw PException("Line too long");
          if (feof(f)) throw PException("EOF in quotes");
          buff[pos++] = c;
        } while (1);
        buff[pos++] = '"';
        continue;
      case ';':
        /* find end of line */
        while (!feof(f) && c != '\n' && c != '\r') c = fgetc(f);
        if (!feof(f)) ungetc(c,f);
        continue;
      case '\\':
        c = fgetc(f);
        if (c == '\n') {
          if (*linenum) (*linenum)++;
          c = fgetc(f);
          if (c != '\r') ungetc(c, f);
          continue;
        }
        if (c != '\n' && c != '\r' && c != '(' && c != ')' && c != '"' && c != '\\') {
          ungetc(c, f);
          c = '\\';
        }
      default:
        if (pos > buffsz - 2) throw PException("Line too long");
        buff[pos++] = c;
    }
  }
rln_finished:
  buff[pos] = 0;
}


stl_string read_entry(char*& data) {
  char buff[256];
  unsigned int len = 0;

  bool in_quote = false;
  char *tmp = data;

  if (*tmp == 0) throw PException("Unexpected end-of-line");

  while (*tmp != '\0' && (in_quote || (*tmp != ' ' && *tmp != '\t'))) {
    if (*tmp == '\\' &&
        (*(tmp+1) == '\\' || *(tmp+1) == ' ' || *(tmp+1) == '\t')) tmp++;
    else if (*tmp == '"') { in_quote = !in_quote; tmp++; continue; }
    if (len >= sizeof(buff) - 1) throw PException("Data too long!");
    buff[len++] = *tmp;
    tmp++;
  }

  /* find beginning of next entry */
  while (*tmp == ' ' || *tmp == '\t') tmp++;
  data = tmp;

  buff[len] = 0;

  return stl_string(buff);
}

stl_string rr_fromstring(u_int16 RRTYPE, const char *_data, _domain origin) {
  stl_string ret;

  stl_string tmp;
  char buff[256];
  int val, x;
  domainname tdom;
  char *data = (char*)_data;

  rr_type *info = rrtype_getinfo(RRTYPE);
  char *ptr;

  if (!info) throw PException("Unknown RR type");
  ptr = info->properties;

  while (*ptr) {
    switch (*ptr) {
      case 'd':
      case 'm':
        tmp = read_entry(data);
        tdom = domainname(tmp.c_str(), origin);
        ret.append((char*)tdom.c_str(), tdom.len());
        break;
      case 'i':
        tmp = read_entry(data);
        txt_to_ip((unsigned char*)buff, (char *)tmp.c_str());
        ret.append(buff, 4);
        break;
      case 's':
        tmp = read_entry(data);
        val = txt_to_int((char *)tmp.c_str());
        ret.append((char*)uint16_buff(val), 2);
        break;
      case 'l':
      case 't':
        tmp = read_entry(data);
        val = txt_to_int((char *)tmp.c_str());
        ret.append((char*)uint32_buff(val), 4);
        break;
      case '6':
        tmp = read_entry(data);
        txt_to_ipv6((unsigned char*)buff, (char *)tmp.c_str());
        ret.append(buff, 16);
        break;
      case 'c':
        tmp = read_entry(data);
        if (tmp.size() > 63) throw PException("Character string too long");
        buff[0] = strlen(tmp.c_str());
        ret.append(buff, 1);
        ret.append(tmp.c_str(), strlen(tmp.c_str()));
        break;
      case 'h':
        tmp = read_entry(data);
        while (1) {
          if (tmp.size() > 63) throw PException("Character string too long");
          buff[0] = tmp.size();
          ret.append(buff, 1);
          ret.append(tmp.c_str(), strlen(tmp.c_str()));
          if (*data == 0) break;
          tmp = read_entry(data);
        }
        break;
      case 'w':
        tmp = read_entry(data);
        buff[0] = getprotocolbyname(tmp.c_str());
        ret.append(buff, 1);
        memset(buff, 0, sizeof(buff));
        x = 0; /* highest port */
        while (*data) {
          val = getserviceportbyname(read_entry(data).c_str());
          if (val >= (signed)sizeof(buff) * 8) throw PException(true, "Port number %d too large", val);
          buff[val / 8] |= (1 << (val%8));
        }
        ret.append(buff, (x / 8) + 1);
        break;
      case 'o':
        txt_to_loc((unsigned char *)buff, data);
        ret.append(buff, 16);
        break;
      default:
        throw PException("Unknown RR property type");
    }
    ptr++;
  }
  if (*data) throw PException("Extra data on RR line");
  return ret;
}

stl_string rr_fromstring(u_int16 RRTYPE, const char *data, domainname origin) {
  return rr_fromstring(RRTYPE, data, origin.c_str());
}

void rr_goto(unsigned char*& RDATA, u_int16 RRTYPE, int ix) {
  rr_type *info = rrtype_getinfo(RRTYPE);
  if (!info) throw PException("Unknown RR type");
  int len;

  char *ptr = info->properties;
  message_buff buff;
  
  for (int x = 0; x < ix; x++) {
    if (ptr[x] == '\0') throw PException("RR does not contain that property");
    buff = message_buff(RDATA, 65535);
    len = rr_len(ptr[x], buff, 0, 65536);
    RDATA += len;
  }
}

_domain rr_getbindomain(const unsigned char *_RDATA, u_int16 RRTYPE, int ix) {
  unsigned char *RDATA = (unsigned char*)_RDATA;
  rr_goto(RDATA, RRTYPE, ix);
  return domdup(RDATA);
}

domainname rr_getdomain(const unsigned char *RDATA, u_int16 RRTYPE, int ix) {
  domainname dom;
  _domain ptr = rr_getbindomain(RDATA, RRTYPE, ix);
  dom = domainname(true, ptr);
  free(ptr);
  return dom;
}

_domain rr_getbinmail(const unsigned char *RDATA, u_int16 RRTYPE, int ix) {
  return rr_getbindomain(RDATA, RRTYPE, ix);
}

domainname rr_getmail(const unsigned char *RDATA, u_int16 RRTYPE, int ix) {
  return rr_getdomain(RDATA, RRTYPE, ix);
}

u_int16 rr_getshort(const unsigned char *_RDATA, u_int16 RRTYPE, int ix) {
  unsigned char *RDATA = (unsigned char*)_RDATA;
  rr_goto(RDATA, RRTYPE, ix);
  return RDATA[0] * 256 + RDATA[1];
}  
  
u_int32 rr_getlong(const unsigned char *_RDATA, u_int16 RRTYPE, int ix) {
  unsigned char *RDATA = (unsigned char*)_RDATA;
  rr_goto(RDATA, RRTYPE, ix);
  return RDATA[0] * 16777216 + RDATA[1] * 65536 + RDATA[2] * 256 + RDATA[3];
}

unsigned char *rr_getip4(const unsigned char *_RDATA, u_int16 RRTYPE, int ix) {
  unsigned char *RDATA = (unsigned char*)_RDATA;
  rr_goto(RDATA, RRTYPE, ix);
  unsigned char *ret = (unsigned char *)malloc(4);
  memcpy(ret, RDATA, 4);
  return ret;
}

unsigned char *rr_getip6(const unsigned char *_RDATA, int RRTYPE, int ix) {
  unsigned char *RDATA = (unsigned char*)_RDATA;
  rr_goto(RDATA, RRTYPE, ix);
  unsigned char *ret = (unsigned char *)malloc(16);
  memcpy(ret, RDATA, 16);
  return ret;
}
