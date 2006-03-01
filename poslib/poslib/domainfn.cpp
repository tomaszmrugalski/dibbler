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

#include "dnsmessage.h"
#include "domainfn.h"
#include "lexfn.h"
#include "exception.h"
#include "sysstl.h"
#include "sysstring.h"

void *memdup(const void *src, int len) {
  if (len == 0) return NULL;
  void *ret = malloc(len);
  memcpy(ret, src, len);
  return ret;
}

#define DOM_RECLEVEL 10
int dom_comprlen(message_buff &buff, int ix) {
  int len = 0;
  unsigned char x;
  unsigned char *ptr = buff.msg + ix,
                *end = buff.msg + buff.len;
  
  while (true) {
    if (ptr >= end) throw PException("Domain name exceeds message borders");
    
    if (*ptr == 0)
      /* we're at the end! */
      return len + 1;
    
    if ((*ptr & 192) == 192) {
      if (ptr + 1 >= end) throw PException("Compression offset exceeds message borders");
      return len + 2;
    }
    x = *ptr & 192;
    if (x != 0) throw PException("Unknown domain label type");
    len += *ptr + 1;
    ptr += *ptr + 1;
    if (len >= 255) throw PException("Domain name too long");
  }
}

_domain dom_uncompress(message_buff &buff, int ix) {
  int reclevel = 0, len = 0, val;
  unsigned char *ptr = buff.msg + ix, *end = buff.msg + buff.len;
  unsigned char dbuff[255];

  while (true) {
    if (ptr >= end) throw PException("Domain name exceeds message borders");

    if (*ptr == 0) {
      /* we're at the end! */
      dbuff[len] = '\0';
      return domdup(dbuff);
    }

    if ((*ptr & 192) == 192) {
      if (++reclevel >= DOM_RECLEVEL) throw PException("Max dom recursion level reached");
      if (ptr + 1 >= end) throw PException("Compression offset exceeds message borders");
      val = (ptr[0] & 63) * 256 + ptr[1];
      if (val >= (ptr - buff.msg)) throw PException("Bad compression offset");
      ptr = buff.msg + val;
      continue;
    }

    if ((*ptr & 192) != 0) throw PException("Unknown domain label type");
    if (len + *ptr + 1 >= 255) throw PException("Domain name too long");
    if (ptr + *ptr + 1 >= end) throw PException("Domain name exceeds message borders");
    memcpy(dbuff + len, ptr, *ptr + 1);
    len += *ptr + 1;
    ptr += *ptr + 1;
  }

//  return domdup(dbuff);
}

dom_compr_info::dom_compr_info(_cdomain _dom, int _ix, int _nl, int _nul) {
  dom = _dom;
  ix = _ix;
  nl = _nl;
  nul = _nul;
}

int dom_partiallength(_cdomain _dom, int n) {
  _domain dom = (_domain) _dom;
  int len = 0;
  while (--n >= 0) {
    len += *dom + 1;
    dom += *dom + 1;
  }
  return len;
}

void dom_write(stl_string &ret, _cdomain dom, stl_slist(dom_compr_info) *comprinfo) {
  if (!comprinfo) {
    ret.append((char *)dom, domlen(dom));
    return;
  }
  
  stl_slist(dom_compr_info)::iterator it = comprinfo->begin(), best = comprinfo->end();
  int nlabels = dom_nlabels(dom) - 1, x;
  int ix = ret.size();

  while (it != comprinfo->end()) {
    if (nlabels >= it->nl && (best == comprinfo->end() || best->nul < it->nul)) {
      if (domcmp(domfrom(dom, nlabels - it->nl), it->dom)) {
        /* the same! */
        best = it;
        if (nlabels == best->nl) break; /* perfect match */
      }
    }
    it++;
  }

  /* let's go! */
  if (best == comprinfo->end()) {
    ret.append((char *)dom, domlen(dom));
    x = nlabels; /* number of stored labels */
  } else {
    unsigned char val;
    ret.append((char *)dom, dom_partiallength(dom, nlabels - best->nl));
    val = (best->ix / 256) | 192; ret.append((char *)&val, 1);
    val = best->ix; ret.append((char*)&val, 1);
    x = nlabels - best->nl; /* number of stored labels */
  }

  /* add our information to the list */
  _cdomain pdom = dom;
  for (int z = 0; z < x; z++) {
    if (ix + (pdom - dom) >= 16384) break;
    comprinfo->push_front(dom_compr_info(pdom, ix + (pdom - dom), nlabels - z, x - z));
    pdom += *pdom + 1;
  }
}

_domain domfrom(_cdomain dom, int ix) {
  while (ix > 0) {
    if (*dom == 0) throw PException("Domain label index out of bounds");
    dom += *dom + 1;
    ix--;
  }
  return (_domain) dom;
}

bool domisparent(_cdomain parent, _cdomain child) {
  int x = domlen(parent), y = domlen(child);
  if (x > y) return false;
  return domcmp(parent, child + y - x);
}

int domlen(_cdomain dom) {
  int len = 1;
  while (*dom) {
    if (*dom > 63) throw PException(true, "Unknown domain nibble %d", *dom);
    len += *dom + 1;
    dom += *dom + 1;
    if (len > 255) throw PException("Length too long");
  }
  return len;
}

_domain domdup(_cdomain dom) {
  return (_domain) memdup(dom, domlen(dom));
}

bool domlcmp(_cdomain dom1, _cdomain dom2) {
  _domain a = (_domain) dom1;
  _domain b = (_domain) dom2;
  if (*a != *b) return false;
  for (int t = 1; t <= *a; t++)
    if (tolower(a[t]) != tolower(b[t])) return false;
  return true;
}

bool domcmp(_cdomain _dom1, _cdomain _dom2) {
  _domain dom1 = (_domain)_dom1, dom2 = (_domain)_dom2;
  if (*dom1 != *dom2) return false;

  int x = domlen(dom1), y = domlen(dom2);
  if (x != y) return false;

  while (*dom1) {
    if (*dom1 != *dom2) return false;
    for (int t = 1; t <= *dom1; t++)
      if (tolower(dom1[t]) != tolower(dom2[t])) return false;
    dom1 += *dom1 + 1;
    dom2 += *dom2 + 1;
  }

  return true;
}

domainname::domainname() {
  domain = (unsigned char *)strdup("");
}

domainname::domainname(const char *string, const domainname origin) {
  unsigned char tmp[DOM_LEN];

  txt_to_email(tmp, string, origin.domain);
  domain = domdup(tmp);
}

domainname::domainname(const char *string, _cdomain origin) {
  unsigned char tmp[DOM_LEN];

  txt_to_email(tmp, string, origin);
  domain = domdup(tmp);
}

domainname::domainname(message_buff &buff, int ix) {
  domain = dom_uncompress(buff, ix);
}


domainname::domainname(bool val, const unsigned char* dom) {
  domain = domdup(dom);
}

domainname::domainname(const domainname& nam) {
  domain = domdup(nam.domain);
}

domainname::~domainname() {
  free(domain);
}

domainname& domainname::operator=(const domainname& nam) {
  if (this != &nam) {
    if (domain) free(domain);
    domain = domdup(nam.domain);
  }
  return *this;
}

domainname& domainname::operator=(const char *buff) {
  unsigned char tmp[DOM_LEN];

  if (domain) {
    free(domain);
    domain = NULL;
  }
  
  txt_to_dname(tmp, buff, (unsigned char *)"");
  domain = domdup(tmp);
  return *this;
}


bool domainname::operator==(const domainname& nam) const {
  return domcmp(domain, nam.domain);
}

bool domainname::operator!=(const domainname& nam) const {
  return !domcmp(domain, nam.domain);
}

domainname& domainname::operator+=(const domainname& nam) {
  int lenres = domlen(domain),
      lensrc = domlen(nam.domain);

  if (lenres + lensrc - 1 > DOM_LEN) throw PException("Domain name too long");
  domain = (unsigned char *)realloc(domain, lenres + lensrc - 1);
  memcpy(domain + lenres - 1, nam.domain, lensrc);
  return *this;
}

domainname& domainname::operator+(const domainname& nam) {
  domainname *ret = new domainname(*this);
  ret->operator+=(nam);
  return *ret;
}

bool domainname::operator>=(const domainname& dom) const {
  return domisparent(dom.domain, domain);
}

bool domainname::operator>(const domainname& dom) const {
  return !domcmp(dom.domain, domain) && domisparent(dom.domain, domain);
}

_domain domainname::c_str() const {
  if (domain == NULL) throw PException("Domain name is empty");
  return domain;
}

int domainname::len() const {
  return domlen(domain);
}

stl_string domainname::tostring() const {
  return dom_tostring(domain);
}

int domainname::nlabels() const {
  return dom_nlabels(domain);
}

stl_string domainname::label(int ix) const {
  return dom_label(domain, ix);
}

domainname domainname::from(int ix) const {
  stl_string ret;
  unsigned char *dom = domain;
  while (ix > 0) {
    if (*dom == 0) throw PException("Domain label index out of bounds");
    dom += *dom + 1;
    ix--;
  }
  return domainname(true, dom);
}

domainname domainname::to(int labels) const {
  unsigned char ptr[DOM_LEN];
  domto(ptr, domain, labels);
  return domainname(true, ptr);
}

stl_string domainname::torelstring(const domainname &root) const {
  if (*this == root) {
    return "@";
  } else if (*this >= root) {
    stl_string str = to(nlabels() - root.nlabels()).tostring();
    str.resize(str.size() - 1);
    return str;
  } else return tostring();
}

int domainname::ncommon(const domainname &dom) const {
  return domncommon(domain, dom.domain);
}

void domcat(_domain res, _cdomain src) {
  int lenres = domlen(res),
      lensrc = domlen(src);
  if (lenres + lensrc - 1 > DOM_LEN) throw PException("Domain name too long");
  memcpy(res + lenres - 1, src, lensrc);
}

void domcpy(_domain res, _cdomain src) {
  memcpy(res, src, domlen(src));
}
 
void domfromlabel(_domain dom, const char *label, int len) {
  if (len == -1) len = strlen(label);
  if (len > DOMLABEL_LEN) throw PException(true, "Domain name label %s too long", label);
  dom[0] = len;
  memcpy(dom + 1, label, len);
  dom[len + 1] = '\0';
}

int dom_nlabels(_cdomain dom) {
  int n_labels = 1;
  while (*dom) {
    dom += *dom + 1;
    n_labels++;
  }
  return n_labels;
}

stl_string dom_label(_cdomain dom, int label) {
  stl_string ret;
  while (label > 0) {
    if (*dom == 0) return "";
    dom += *dom + 1;
    label--;
  }

  ret.append((char*)dom + 1, (int)*dom);
  return ret;
}

_domain dom_plabel(_cdomain dom, int label) {
  unsigned char *ret = (unsigned char *)dom;
  if (label < 0) throw PException("Negative label accessed");
  while (label--) {
    if (*ret == 0) throw PException("Label not in domain name");
    ret += *ret + 1;
  }
  return (_domain) ret;
}

stl_string dom_tostring(_cdomain dom) {
  if (*dom == '\0') return ".";

  stl_string x;

  while (*dom != '\0') {
    x.append((char*)dom + 1, (int)*dom);
    x.append(".");

    dom += *dom + 1;
  }

  return x;
}

int domncommon(_cdomain _dom1, _cdomain _dom2) {
  _domain dom1 = (_domain)_dom1,
          dom2 = (_domain)_dom2;

  int a = dom_nlabels(dom1), b = dom_nlabels(dom2);
  if (a > b)
    dom1 = dom_plabel(dom1, a - b);
  else
    dom2 = dom_plabel(dom2, b - a);

  int x = 0;
  while (*dom1) {
    if (domlcmp(dom1, dom2)) x++; else x = 0;
    dom1 += *dom1 + 1;
    dom2 += *dom2 + 1;
  }
  return x;
}

int domccmp(_cdomain _dom1, _cdomain _dom2) {
  _domain dom1 = (_domain)_dom1,
          dom2 = (_domain)_dom2;

  int x = domncommon(dom1, dom2), y = dom_nlabels(dom1), z = dom_nlabels(dom2);
  if (x == y - 1) { if (x == z - 1) return 0; else return -1; }
  else if (x == z - 1) { return 1; } else {
    /* check the last label */
    return strcmpi(dom_label(dom1, y - x - 2).c_str(), dom_label(dom2, z - x - 2).c_str());
  }
}

void domto(_domain ret, _cdomain src, int labels) {
  _domain ptr = dom_plabel(src, labels);
  memcpy(ret, src, ptr - src);
  ret[ptr - src] = '\0';
}
