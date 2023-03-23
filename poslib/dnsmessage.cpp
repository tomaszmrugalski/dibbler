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

#include "sysstl.h"

#include "dnsmessage.h"
#include "dnssec-sign.h"
#include "domainfn.h"
#include "exception.h"
#include "lexfn.h"
#include "rr.h"

message_buff::message_buff() {
  is_static = false;
  msg = NULL;
  len = 0;
}

message_buff::message_buff(unsigned char *_msg, int _len, bool _is_dynamic) {
  is_static = !_is_dynamic;
  msg = _msg;
  len = _len;
}

message_buff::message_buff(const message_buff &buff) {
  if (buff.is_static) {
    msg = buff.msg;
    len = buff.len;
    is_static = true;
  } else {
    if (buff.msg)
      msg = (unsigned char *)memdup(buff.msg, buff.len);
    else
      msg = NULL;
    len = buff.len;
    is_static = false;
  }
}

message_buff::~message_buff() {
  if (!is_static) free(msg);
}

message_buff &message_buff::operator=(const message_buff &buff) {
  if (this != &buff) {
    if (buff.is_static) {
      msg = buff.msg;
      len = buff.len;
      is_static = true;
    } else {
      if (buff.msg)
        msg = (unsigned char *)memdup(buff.msg, buff.len);
      else
        msg = NULL;
      len = buff.len;
      is_static = false;
    }
  }
  return *this;
}

DnsQuestion::DnsQuestion() : QNAME(""), QTYPE(0), QCLASS(CLASS_IN) {}

DnsQuestion::DnsQuestion(const DnsQuestion &q) : QNAME(q.QNAME), QTYPE(q.QTYPE), QCLASS(q.QCLASS) {}

DnsQuestion &DnsQuestion::operator=(const DnsQuestion &q) {
  if (this != &q) {
    QNAME = q.QNAME;
    QTYPE = q.QTYPE;
    QCLASS = q.QCLASS;
  }
  return *this;
}

DnsQuestion::DnsQuestion(domainname _QNAME, u_int16 _QTYPE, u_int16 _QCLASS) {
  QNAME = _QNAME;
  QTYPE = _QTYPE;
  QCLASS = _QCLASS;
}

DnsQuestion::~DnsQuestion() {}

DnsRR::DnsRR()
    : NAME(""),
      TYPE(0),
      CLASS(CLASS_IN),
      TTL(0),
      RDLENGTH(0),
      RDATA(NULL),
      presign_RDLENGTH(0),
      presign_RDATA(NULL) {}

DnsRR::DnsRR(domainname name, u_int16 type, u_int16 rrClass, u_int32 ttl)
    : NAME(name),
      TYPE(type),
      CLASS(rrClass),
      TTL(ttl),
      RDLENGTH(0),
      RDATA(NULL),
      presign_RDLENGTH(0),
      presign_RDATA(NULL) {}

DnsRR::DnsRR(domainname name, u_int16 type, u_int16 rrClass, u_int32 ttl, uint16_t _RDLENGTH,
             const unsigned char *_RDATA)
    : NAME(name), TYPE(type), CLASS(rrClass), TTL(ttl) {
  RDLENGTH = _RDLENGTH;
  RDATA = (unsigned char *)memdup(_RDATA, _RDLENGTH);
  presign_RDATA = NULL;
  presign_RDLENGTH = 0;
}

DnsRR::DnsRR(const DnsRR &rr) : NAME(rr.NAME), TYPE(rr.TYPE), CLASS(rr.CLASS), TTL(rr.TTL) {
  RDATA = (unsigned char *)memdup(rr.RDATA, rr.RDLENGTH);
  RDLENGTH = rr.RDLENGTH;
  presign_RDATA = NULL;
  presign_RDLENGTH = 0;
  if (rr.presign_RDATA) {
    presign_RDATA = (unsigned char *)memdup(rr.presign_RDATA, rr.presign_RDLENGTH);
    presign_RDLENGTH = rr.presign_RDLENGTH;
  }
}

DnsRR::~DnsRR() {
  if (RDATA) free(RDATA);
  if (presign_RDATA) free(presign_RDATA);
}

DnsRR &DnsRR::operator=(const DnsRR &rr) {
  if (this != &rr) {
    free(RDATA);
    NAME = rr.NAME;
    TYPE = rr.TYPE;
    CLASS = rr.CLASS;
    TTL = rr.TTL;
    RDATA = (unsigned char *)memdup(rr.RDATA, rr.RDLENGTH);
    RDLENGTH = rr.RDLENGTH;
    presign_RDATA = (unsigned char *)memdup(rr.presign_RDATA, rr.presign_RDLENGTH);
    presign_RDLENGTH = rr.presign_RDLENGTH;
  }
  return *this;
}

bool DnsRR::operator==(const DnsRR &other) {
  return (TYPE == other.TYPE && TTL == other.TTL && RDLENGTH == other.RDLENGTH &&
          CLASS == other.CLASS && NAME == other.NAME && memcmp(RDATA, other.RDATA, RDLENGTH) == 0);
}

#define INTCMP(x, y) ((x) < (y) ? -1 : ((x) == (y) ? 0 : -1))
#define COMPARE(expr)                    \
  {                                      \
    int tmp;                             \
    if ((tmp = (expr)) != 0) return tmp; \
  }
#define SPECIFIC(x, y, expr)               \
  if ((x) == (expr) && (y) != (expr))      \
    return -1;                             \
  else if ((y) == (expr) && (x) != (expr)) \
  return 1
int DnsRR::compareTo(const DnsRR &other) const {
  COMPARE(INTCMP(CLASS, other.CLASS));
  COMPARE(domccmp(NAME.c_str(), other.NAME.c_str()));
  SPECIFIC(TYPE, other.TYPE, DNS_TYPE_SOA);
  COMPARE(INTCMP(TYPE, other.TYPE));
  COMPARE(INTCMP(TTL, other.TTL));
  COMPARE(INTCMP(rr_tostring(TYPE, RDATA, RDLENGTH).c_str(),
                 rr_tostring(other.TYPE, other.RDATA, other.RDLENGTH)));
  return 0;
}

bool DnsRR::operator<(const DnsRR &rr) const { return compareTo(rr) == -1; }
bool DnsRR::operator<=(const DnsRR &rr) const { return compareTo(rr) <= 0; }
bool DnsRR::operator>(const DnsRR &rr) const { return compareTo(rr) == 1; }
bool DnsRR::operator>=(const DnsRR &rr) const { return compareTo(rr) >= 0; }

DnsMessage::DnsMessage() {
  ID = 0;
  QR = false;
  OPCODE = 0;
  AA = false;
  TC = false;
  RD = false;
  RA = false;
  Z = 0;
  RCODE = 0;
  tsig_rr = NULL;
  tsig_rr_signtime = 0;  // pick current time during message sending
}

DnsMessage::~DnsMessage() {
  if (tsig_rr) delete tsig_rr;
}

DnsMessage *DnsMessage::initialize_answer() {
  DnsMessage *a = new DnsMessage();
  if (tsig_rr) a->tsig_rr = new DnsRR(*tsig_rr);
  a->sign_key = sign_key;
  return a;
}

/* flags != 0 currently means we accept zero-length (UPDATE) rrs */
DnsRR DnsMessage::read_rr(message_buff &buff, int &pos, int flags) {
  DnsRR rr;
  int x;
  domainname dom;

  if (pos >= buff.len) throw PException("Message too small for RR");

  x = dom_comprlen(buff, pos);
  if (pos + x + 10 > buff.len) throw PException("Message too small for RR");

  rr.NAME = domainname(buff, pos);
  rr.TYPE = uint16_value(buff.msg + pos + x);
  rr.CLASS = uint16_value(buff.msg + pos + x + 2);
  rr.TTL = uint32_value(buff.msg + pos + x + 4);

  pos += x + 10;
  x = uint16_value(buff.msg + pos - 2);
  if (x != 0 || !flags) rr_read(rr.TYPE, rr.RDATA, rr.RDLENGTH, buff, pos, x);
  pos += x;

  return rr;
}

void DnsMessage::read_section(stl_list(DnsRR) & section, int count, message_buff &buff, int &pos,
                              unsigned int *tsig_pos) {
  while (--count >= 0) {
    int origpos = pos;
    DnsRR rr = read_rr(buff, pos, (OPCODE == OPCODE_UPDATE) ? 1 : 0);
    if (rr.TYPE == DNS_TYPE_TSIG && tsig_pos) *tsig_pos = origpos;
    section.push_back(rr);
  }
}

int DnsMessage::read_from_data(unsigned char *data, int len) {
  message_buff buff(data, len);
  int qdc, adc, nsc, arc, t, x, pos = 12;

  if (len < 12) throw PException("Corrupted DNS packet: too small for header");

  ID = uint16_value(data);
  QR = data[2] & 128;
  OPCODE = (data[2] & 120) >> 3;
  AA = data[2] & 4;
  TC = data[2] & 2;
  RD = data[2] & 1;
  RA = data[3] & 128;

  Z = (data[3] & 112) >> 3;
  RCODE = data[3] & 15;

  qdc = uint16_value(data + 4);
  adc = uint16_value(data + 6);
  nsc = uint16_value(data + 8);
  arc = uint16_value(data + 10);

  /* read question section */

  for (t = 0; t < qdc; t++) {
    if (pos >= len) throw PException("Message too small for question item!");
    x = dom_comprlen(buff, pos);
    if (pos + x + 4 > len) throw PException("Message too small for question item !");
    questions.push_back(DnsQuestion(domainname(buff, pos), uint16_value(data + pos + x),
                                    uint16_value(data + pos + x + 2)));
    pos += x;
    pos += 4;
  }

  /* read other sections */
  read_section(answers, adc, buff, pos);
  read_section(authority, nsc, buff, pos);
  unsigned int tsig_loc = 0;
  read_section(additional, arc, buff, pos, &tsig_loc);

  if (tsig_loc == 0) {
    /* unsigned message */
    if (tsig_rr != NULL)
      throw PException(true, "Unsigned answer to signed message (key=%s)", tsig_rr->NAME.tocstr());
    return 0;
  }

  DnsRR message_tsig = additional.back();
  additional.pop_back();

  if (tsig_rr == NULL) {
    /* no automatic checking; set tsig_rr to found RR */
    tsig_rr = new DnsRR(message_tsig);
  } else {
    verify_signature(tsig_rr, &message_tsig, sign_key, message_buff(data, tsig_loc));
  }

  if (tsig_loc) {
    return tsig_loc;
  } else {
    return pos;
  }
}

class PTruncatedException {
 public:
  PTruncatedException() {}
};

void DnsMessage::write_rr(DnsRR &rr, stl_string &message, stl_slist(dom_compr_info) * comprinfo,
                          int flags) {
  dom_write(message, rr.NAME.c_str(), comprinfo);
  message.append((char *)uint16_buff(rr.TYPE), 2);
  message.append((char *)uint16_buff(rr.CLASS), 2);
  message.append((char *)uint32_buff(rr.TTL), 4);
  message.append((char *)uint16_buff(rr.RDLENGTH), 2);
  int p = message.size();
  if (rr.RDLENGTH != 0 || !flags) rr_write(rr.TYPE, rr.RDATA, rr.RDLENGTH, message, comprinfo);
  int x = message.size();
  message[p - 2] = (x - p) / 256;
  message[p - 1] = (x - p);
}

void DnsMessage::write_section(stl_list(DnsRR) & section, int lenpos, stl_string &message,
                               stl_slist(dom_compr_info) & comprinfo, int maxlen,
                               bool is_additional) {
  stl_list(DnsRR)::iterator it = section.begin();

  int n = 0, x;

  x = message.size();

  while (it != section.end()) {
    write_rr(*it, message, &comprinfo, (OPCODE == OPCODE_UPDATE) ? 1 : 0);
    if (maxlen != -1 && message.size() > (unsigned)maxlen) {
      /* truncate it here */
      message.resize(x);
      if (!is_additional) message[2] |= 2;
      message[lenpos] = n / 256;
      message[lenpos + 1] = n;
      throw PTruncatedException();
    }
    x = message.size();
    ++it;
    n++;
  }

  /* write number of written items */
  message[lenpos] = n / 256;
  message[lenpos + 1] = n;
}

message_buff DnsMessage::compile(int maxlen) {
  stl_string msg;
  unsigned char ch;
  stl_slist(dom_compr_info) comprinfo;

  try {
    msg.append((char *)uint16_buff(ID), 2);
    if (QR)
      ch = 128;
    else
      ch = 0;
    ch += OPCODE << 3;
    if (AA) ch += 4;
    if (TC) ch += 2;
    if (RD) ch++;
    msg.append((char *)&ch, 1);
    if (RA)
      ch = 128;
    else
      ch = 0;
    ch += Z << 4;
    ch += RCODE;
    msg.append((char *)&ch, 1);
    msg.append((char *)uint16_buff(0), 2);
    msg.append((char *)uint16_buff(0), 2);
    msg.append((char *)uint16_buff(0), 2);
    msg.append((char *)uint16_buff(0), 2);

    /* write questions */
    stl_list(DnsQuestion)::iterator it = questions.begin();
    int x, n = 0;

    while (it != questions.end()) {
      x = msg.size();
      dom_write(msg, it->QNAME.c_str(), &comprinfo);
      msg.append((char *)uint16_buff(it->QTYPE), 2);
      msg.append((char *)uint16_buff(it->QCLASS), 2);
      if (msg.size() > (unsigned)maxlen) {
        /* truncate it here */
        msg.resize(x);
        msg[2] |= 2;
        msg[4] = n / 256;  /// @todo: is this safe if chars are unsigned?
        msg[5] = n;
        throw PTruncatedException();
      }
      ++it;
      n++;
    }
    /* write number of written items */
    msg[4] = n / 256;
    msg[5] = n;

    /* other sections */
    write_section(answers, 6, msg, comprinfo, maxlen);
    write_section(authority, 8, msg, comprinfo, maxlen);
    write_section(additional, 10, msg, comprinfo, maxlen, true);
  } catch (const PTruncatedException &p) {

  } catch (const PException &p) {
    throw PException("Dns Message creation failed: ", p);
  }

  /* DNS message signing */
  if (tsig_rr != NULL) {
    /* increase answer count */
    int n = uint16_value((unsigned char *)msg.c_str() + 10) + 1;
    msg[10] = n / 256;
    msg[11] = n % 256;

    /* set ID, time signed of TSIG RR */
    memcpy(rr_getdata(tsig_rr->RDATA, DNS_TYPE_TSIG, 4), uint16_buff(ID), 2);

    // thomson: Useful for testing it is possible to force specific signtime
    if (!tsig_rr_signtime) tsig_rr_signtime = time(NULL);
    memcpy(rr_getdata(tsig_rr->RDATA, DNS_TYPE_TSIG, 1), uint48_buff(tsig_rr_signtime), 6);

    message_buff extra;
    unsigned char *ptr = rr_getdata(tsig_rr->RDATA, DNS_TYPE_TSIG, 3);
    int ptrlen = uint16_value(ptr);
    if (ptrlen) extra = message_buff(ptr, ptrlen + 2);

    stl_string key = calc_mac(*tsig_rr, message_buff((unsigned char *)msg.c_str(), msg.size()),
                              sign_key, &extra);

    /* store digest in tsig RR */
    // thomson: to be able to sign the message multiple times, uncomment:
    if (tsig_rr->presign_RDLENGTH == 0) {
      tsig_rr->presign_RDLENGTH = tsig_rr->RDLENGTH;
      tsig_rr->presign_RDATA = (unsigned char *)memdup(tsig_rr->RDATA, tsig_rr->RDLENGTH);
    }

    stl_string newdata;
    int digestpos = rr_getdata(tsig_rr->RDATA, DNS_TYPE_TSIG, 3) - tsig_rr->RDATA,
        digestlen = uint16_value(tsig_rr->RDATA + digestpos);
    newdata.append((char *)tsig_rr->RDATA, digestpos);
    newdata.append((char *)uint16_buff(key.size()), 2);
    newdata.append(key);
    newdata.append((char *)tsig_rr->RDATA + digestpos + 2 + digestlen,
                   tsig_rr->RDLENGTH - (digestpos + digestlen + 2));
    free(tsig_rr->RDATA);
    tsig_rr->RDATA = (unsigned char *)memdup(newdata.c_str(), newdata.size());
    tsig_rr->RDLENGTH = newdata.size();

    /* TODO: if things don't fit, remove the rest of the message and set the TC
     * bit */
    write_rr(*tsig_rr, msg, &comprinfo, 0);

    // thomson: to be able to sign the message multiple times, uncomment:
    // free(tsig_rr->RDATA);
    // tsig_rr->RDATA = old_RDATA;
    // tsig_rr->RDLENGTH = old_RDLENGTH;
  }

  int len = msg.size();
  return message_buff((unsigned char *)memdup((char *)msg.c_str(), len), len, true);
}

unsigned char _tmp[6];

unsigned char *uint16_buff(uint16_t val) {
  _tmp[0] = val / 256;
  _tmp[1] = val & 255;
  return _tmp;
}

unsigned char *uint32_buff(uint32_t val) {
  _tmp[0] = val / (1 << 24);
  _tmp[1] = val / (1 << 16);
  _tmp[2] = val / (1 << 8);
  _tmp[3] = val;
  return _tmp;
}

unsigned char *uint48_buff(u_int48 val) {
  _tmp[0] = val / ((u_int48)1 << 40);
  _tmp[1] = val / ((u_int48)1 << 32);
  _tmp[2] = val / ((u_int48)1 << 24);
  _tmp[3] = val / ((u_int48)1 << 16);
  _tmp[4] = val / ((u_int48)1 << 8);
  _tmp[5] = val;
  return _tmp;
}

u_int16 uint16_value(const unsigned char *buff) { return buff[0] * 256 + buff[1]; }

u_int32 uint32_value(const unsigned char *buff) {
  return uint16_value(buff) * 65536 + uint16_value(buff + 2);
}

u_int48 uint48_value(const unsigned char *buff) {
  return uint32_value(buff) * 65536 + uint16_value(buff + 4);
}

DnsMessage *create_query(domainname qname, uint16_t qtype, bool rd, uint16_t qclass) {
  DnsMessage *ret = new DnsMessage();
  ret->RD = rd;
  ret->questions.push_front(DnsQuestion(qname, qtype, qclass));
  return ret;
}

a_record get_a_record(DnsMessage *a) { return *get_a_records(a, true).begin(); }

stl_list(a_record) get_a_records(DnsMessage *a, bool fail_if_none) {
  stl_list(a_record) ret;
  a_record rec;
  stl_list(rrdat) res = get_records(a, fail_if_none);
  stl_list(rrdat)::iterator it = res.begin();
  while (it != res.end()) {
    memcpy(rec.address, it->msg, 4);
    ret.push_back(rec);
    ++it;
  }
  return ret;
}

aaaa_record get_aaaa_record(DnsMessage *a) { return *get_aaaa_records(a, true).begin(); }

stl_list(aaaa_record) get_aaaa_records(DnsMessage *a, bool fail_if_none) {
  stl_list(aaaa_record) ret;
  aaaa_record rec;
  stl_list(rrdat) res = get_records(a, fail_if_none, DNS_TYPE_AAAA);
  stl_list(rrdat)::iterator it = res.begin();
  while (it != res.end()) {
    memcpy(rec.address, it->msg, 16);
    ret.push_back(rec);
    ++it;
  }
  return ret;
}

mx_record get_mx_record(DnsMessage *a) { return *get_mx_records(a, true).begin(); }
stl_list(mx_record) get_mx_records(DnsMessage *a, bool fail_if_none) {
  stl_list(mx_record) ret;
  mx_record rec;
  stl_list(rrdat) res = get_records(a, fail_if_none);
  stl_list(rrdat)::iterator it = res.begin();
  while (it != res.end()) {
    rec.preference = rr_getshort(it->msg, DNS_TYPE_MX, 0);
    rec.server = rr_getdomain(it->msg, DNS_TYPE_MX, 1);
    ret.push_back(rec);
    ++it;
  }
  return ret;
}

domainname get_ns_record(DnsMessage *a) { return *get_ns_records(a, true).begin(); }
stl_list(domainname) get_ns_records(DnsMessage *a, bool fail_if_none) {
  stl_list(domainname) ret;
  stl_list(rrdat) res = get_records(a, fail_if_none);
  stl_list(rrdat)::iterator it = res.begin();
  while (it != res.end()) {
    ret.push_back(rr_getdomain(it->msg, DNS_TYPE_NS, 0));
    ++it;
  }
  return ret;
}

domainname get_ptr_record(DnsMessage *a) { return *get_ptr_records(a, true).begin(); }
stl_list(domainname) get_ptr_records(DnsMessage *a, bool fail_if_none) {
  stl_list(domainname) ret;
  stl_list(rrdat) res = get_records(a, fail_if_none);
  stl_list(rrdat)::iterator it = res.begin();
  while (it != res.end()) {
    ret.push_back(rr_getdomain(it->msg, DNS_TYPE_PTR, 0));
    ++it;
  }
  return ret;
}

rrdat::rrdat(uint16_t _type, uint16_t _len, unsigned char *_msg) {
  type = _type;
  len = _len;
  msg = _msg;
}

stl_list(rrdat) i_get_records(DnsMessage *a, bool fail_if_none, bool follow_cname, int reclevel,
                              domainname &dname, uint16_t qtype, stl_list(domainname) * fcn) {
  stl_list(rrdat) ret;
  domainname dm;
  if (reclevel < 0) throw PException("CNAME recursion level reached");
  /* look for records */
  stl_list(DnsRR)::iterator it = a->answers.begin();
  while (it != a->answers.end()) {
    if (it->NAME == dname) {
      if (it->TYPE == DNS_TYPE_CNAME && follow_cname && qtype != DNS_TYPE_CNAME) {
        dm = domainname(true, it->RDATA);
        if (fcn) fcn->push_back(dm);
        return i_get_records(a, fail_if_none, true, --reclevel, dm, qtype, fcn);
      } else if (it->TYPE == qtype || qtype == QTYPE_ALL)
        ret.push_back(rrdat(it->TYPE, it->RDLENGTH, it->RDATA));
    }
    ++it;
  }
  if (fail_if_none && ret.begin() == ret.end()) throw PException("No such data available");
  return ret;
}

stl_list(rrdat)
    get_records(DnsMessage *a, bool fail_if_none, bool follow_cname, stl_list(domainname) * fcn) {
  if (a->RCODE != RCODE_NOERROR)
    throw PException(true, "Query returned error: %s\n", str_rcode(a->RCODE).c_str());
  if (a->questions.begin() == a->questions.end()) throw PException("No question item in message");
  return i_get_records(a, fail_if_none, follow_cname, 10, a->questions.begin()->QNAME,
                       a->questions.begin()->QTYPE, fcn);
}

bool has_rrset(stl_list(DnsRR) & rrlist, domainname &name, uint16_t type) {
  std::list<DnsRR>::iterator it = rrlist.begin();

  while (it != rrlist.end()) {
    if (it->NAME == name && answers_qtype(it->TYPE, type)) return true;
    ++it;
  }
  return false;
}

bool has_parental_rrset(stl_list(DnsRR) & section, domainname &qname, uint16_t type) {
  stl_list(DnsRR)::iterator it = section.begin();
  while (it != section.end()) {
    if (it->TYPE == type && qname >= it->NAME) return true;
    ++it;
  }
  return false;
}

_answer_type check_answer_type(DnsMessage *msg, domainname &qname, uint16_t qtype) {
  if (msg->RCODE != RCODE_NOERROR && msg->RCODE != RCODE_NXDOMAIN) return A_ERROR;
  if (qtype != DNS_TYPE_CNAME && has_rrset(msg->answers, qname, DNS_TYPE_CNAME)) return A_CNAME;
  if (msg->RCODE == RCODE_NXDOMAIN) return A_NXDOMAIN;
  if (has_rrset(msg->answers, qname, qtype)) return A_ANSWER;
  if (has_parental_rrset(msg->authority, qname, DNS_TYPE_NS) &&
      !has_parental_rrset(msg->authority, qname, DNS_TYPE_SOA))
    return A_REFERRAL;
  return A_NODATA;
}
