/*
    Posadis - A DNS Server
    Master file reading functionality
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

#include "masterfile.h"
#include "exception.h"
#include "rr.h"
#include "lexfn.h"
#include "sysstring.h"

#include <sys/stat.h>

#ifdef WIN32
const char pathdelim = '\\';
#define S_ISREG(x) 1 /* always return 1 */
#else
const char pathdelim = '/';
#endif

stl_string rrdata_convertdoms(rr_type *rr, domainname znroot, domainname origin, char *ptr) {
  stl_string ret, tmp;
  char *cptr = rr->properties;
  domainname tmpd;
  while (*cptr) {
    tmp = read_entry(ptr);
    if (*cptr == 'd' || *cptr == 'm') {
      /* domain name */
      if (!(*cptr == 'm' && strchr(tmp.c_str(), '@')) &&
          tmp[tmp.size() - 1] != '.') {
        /* domain name is relative to a non-root origin. convert it to a root origin */
        tmpd = domainname(tmp.c_str(), origin);
        if (ret.size()) ret += " ";
        ret += tmpd.torelstring(znroot);
        cptr++;
        continue;
      }
    }
    if (ret.size()) ret += " ";
    ret += tmp;
    cptr++;
  }
  return ret;
}


domainname guess_zone_name(const char *file) {
  const char *ptr = file + strlen(file) - 1;
  char tmp[256];
  int t;

  while (ptr >= file) {
    if (*ptr == pathdelim) { ptr++; break; }
    ptr--;
  }

  t = strlen(ptr);
  if (tolower(ptr[0]) == 'd' &&
      tolower(ptr[1]) == 'b' &&
      ptr[2] == '.') {
    return ptr + 3;
  } else if (t >= 4 && (strncmpi(ptr + t - 4, ".prm", 4) == 0 || strncmpi(ptr + t - 4, ".dns", 4) == 0)) {
    if (strlen(ptr) - 4 >= 256) throw PException("File name too long!");
    memcpy(tmp, ptr, t - 4);
    tmp[t - 4] = '\0';
    return domainname((char*)tmp);
  } else return ptr;
}

bool file_exists(const char *file) {
  struct stat st;
  if (stat(file, &st) == 0) return true; else return false;
}

FILE *try_fopen_r(const char *file) {
  struct stat st;

  if (stat(file, &st) != 0) return NULL;
  else if (!S_ISREG(st.st_mode)) return NULL;
  else return fopen(file, "r");
}

FILE *try_fopen(const char *file, const char *mode) {
  struct stat st;
  int ret = stat(file, &st);

  if (strcmpi(mode, "r") == 0 && ret != 0) return NULL;
  if (ret == 0 && !S_ISREG(st.st_mode)) return NULL;
  return fopen(file, mode);
}

void read_master_file(const char *file, domainname &znroot, void *userdat,
                      error_callback err, rr_callback rr_cb, rr_literal_callback rrl_cb,
                      comment_callback comm_cb, int flags) {
  bool has_znroot = !(flags&POSLIB_MF_AUTOPROBE), has_soa = false;
  char buff[1024];
  char *ptr, *p2;
  int c;
  bool ttl_given = false; stl_string ttl = "3600";
  domainname origin, nam;
  int linenum = 1, linenum2;
  stl_string str, tmp, t2, t3, nm;
  DnsRR rr;
  rr_type *type = NULL;
  
  FILE *f = try_fopen_r(file);
  if (!f) throw PException(true, "Could not open %s", file);
  
  c = fgetc(f);
  if (c != EOF) ungetc(c, f);
		
	if (has_znroot) origin = znroot;

  try {
  begin:
    while (!feof(f)) {
      /* since the poslib functions will filter out comments, let's do them ourselves */
      if (comm_cb) {
        c = fgetc(f);
        if (c == ';') {
            fgets(buff, 1024, f);
	    ptr = buff + strlen(buff) - 1;
	    while (ptr >= buff && (*ptr == '\n' || *ptr == '\r')) *ptr = '\0';
	    comm_cb(userdat, buff);
	    continue;
        }
        ungetc(c, f);
      }

      try {
        read_line(buff, f, &linenum, &linenum2);
      } catch (PException p) {
        break;
      }

      if (buff[0] == 0) goto begin;
      ptr = buff;
      while (*ptr == ' ' || *ptr == '\t') ptr++;
      if (*ptr == '\0') continue;

      if (buff[0] == '$') {
        if (strncmpi(buff, "$ttl ", 4) == 0) {
          ttl = buff + 5;
          ttl_given = true;
        } else if (strncmpi(buff, "$origin ", 8) == 0) {
          /* origin */
          origin = domainname(buff + 8, znroot);
          if (!has_znroot) {
            /* first entry. this is the zone root */
            znroot = origin;
            has_znroot = true;
          }
        } else err(userdat, file, linenum2,
                   PException(true, "Unknown directive %s", buff).message);
        continue;
      }

      if (!has_znroot) {
        znroot = guess_zone_name(file);
        origin =  znroot;
        has_znroot = true;
      }

      if (buff[0] != ' ' && buff[0] != '\t') {
        nam = domainname(read_entry(ptr).c_str(), origin);
      } else if (!has_soa) {
        err(userdat, file, linenum2, "First record in zone should have a domain name! Using origin instead.");
        nam = origin;
      }

      /* can be class or ttl or rrtype */
      while (1) {
        str = read_entry(ptr);
        if (str == "IN" || str == "HS" || str == "CH" || str == "HS" ||
            str == "in" || str == "hs" || str == "ch" || str == "hs") {
          /* class */
          continue;
        } else if ((type = rrtype_getinfo(str.c_str())) != NULL) {
          break;
        } else {
          try {
            txt_to_int(str.c_str());
            ttl_given = true;
          } catch (PException p) {
            err(userdat, file, linenum2, PException(true, "Invalid TTL/RR type %s!", str.c_str()).message);
            goto begin;
          }
          ttl = str.c_str();
        }
      }

      if (!has_soa) {
        if (type->type != DNS_TYPE_SOA) {
          if (!(flags & POSLIB_MF_NOSOA)) {
            err(userdat, file, linenum2, "First record was no SOA record; using default SOA record instead.");
            if (rr_cb) {
              rr.NAME = znroot;
              rr.TYPE = DNS_TYPE_SOA;
              rr.TTL = 3600;
              tmp = rr_fromstring(DNS_TYPE_SOA, "ns1 hostmaster 1 2h 1h 1d 2h", znroot);
              rr.RDLENGTH = tmp.size();
              rr.RDATA = (unsigned char *)memdup(tmp.c_str(), tmp.size());
              rr_cb(userdat, &rr);
            }
            if (rrl_cb)
              rrl_cb(userdat, "@", "1h", "SOA", "ns1 hostmaster 1 2h 1h 1d 2h", znroot);
          } else if (!ttl_given) {
            err(userdat, file, linenum2, "First record should have TTL value; using 1h for now.");
            ttl = "3600";
            ttl_given = true;
          }
        } else {
          if (!ttl_given) {
            p2 = ptr;
            read_entry(p2); read_entry(p2); read_entry(p2);
            read_entry(p2); read_entry(p2); read_entry(p2);
            t3 = read_entry(p2);
            if (strlen(t3.c_str()) < 32) ttl = t3;
          }
        }
        has_soa = true;
      } else if (type->type == DNS_TYPE_SOA && !(flags & POSLIB_MF_NOSOA)) {
        err(userdat, file, linenum2, "Non-initial SOA record ignored");
        continue;
      }

      if (!(nam >= znroot)) {
        err(userdat, file, linenum2, PException(true, "Ignoring domain name %s outside of zone %s!", nam.tocstr(), znroot.tocstr()).message);
        continue;
      }

      if (rr_cb) {
        try {
          rr.NAME = nam;
          rr.TYPE = type->type;
          rr.TTL = txt_to_int(ttl.c_str());
          tmp = rr_fromstring(type->type, ptr, origin);
          rr.RDLENGTH = tmp.size();
          rr.RDATA = (unsigned char *)memdup(tmp.c_str(), tmp.size());
          rr_cb(userdat, &rr);
        } catch (PException p) {
          err(userdat, file, linenum2, p.message);
        }
      }

      if (rrl_cb) {
        try {
          t2 = ptr;
          nm = nam.torelstring(znroot);
          tmp = rr_fromstring(type->type, ptr, origin);
          if (origin != znroot) t2 = rrdata_convertdoms(type, znroot, origin, ptr);
        } catch (PException p) {
          err(userdat, file, linenum2, p.message);
        }
        rrl_cb(userdat, nm.c_str(), ttl.c_str(), type->name, t2.c_str(), znroot);
      }
    }
  } catch (PException p) {
    fclose(f);
    throw p;
  }
  fclose(f);
}
