/*
 * Dnstimeago - Check in which timespan a domain name was last accessed
 *
 * Copyright (C) Meilof Veeningen, 2003-2004
 */

#include <poslib/poslib.h>
#include <stdio.h>
#include <string.h>

class query_item {
 public:
  domainname qname;
  uint16_t qtype;
};

stl_list(query_item) query_item_list;
bool verbose = false;

void print_help() {
  printf(
      "dnstimeago - check timespan of last visit of a domain name\n"
      "\n"
      "Usage:\n"
      "  dnstimeago [@server] [options] querydom1 [qtype1] [querydom2 "
      "[qtype2]]...\n"
      "Options can be one or more of the following:\n"
      "     -h, --help                Show usage information\n"
      "     -r, --version             Show version info\n"
      "     -v, --verbose             Give more detailed info on what happens\n"
      "     -a, --alternate <server>  Use alternate server to get glue from\n"
      "\n"
      "Copyright (C) Meilof Veeningen, 2003-2004. Freely distributable under "
      "the terms\n"
      "and conditions of the GNU General Public License. Part of the Poslib "
      "DNS\n"
      "library: http://www.posadis.org/poslib/\n");
}

void print_version() {
  printf(
      "dnstimeago - check timespan of last visit of a domain name\n"
      "\n"
      "Version:    " __DATE__ "\n");
}

uint32_t soa_ttl(DnsMessage *a) {
  stl_list(DnsRR)::iterator it = a->authority.begin();
  while (it != a->authority.end()) {
    if (it->TYPE == DNS_TYPE_SOA) return it->TTL;
    it++;
  }
  return 0;
}

void find_servers(DnsMessage *a, domainname &dom, stl_slist(_addr) & lst) {
  _addr ad;
  stl_list(DnsRR)::iterator it = a->additional.begin();
  while (it != a->additional.end()) {
    if (it->NAME == dom && it->TYPE == DNS_TYPE_A) {
      getaddress_ip4(&ad, it->RDATA, 53);
      lst.push_front(ad);
#ifdef HAVE_IPV6
    } else if (it->NAME == dom && it->TYPE == DNS_TYPE_AAAA) {
      getaddress_ip6(&ad, it->RDATA, 53);
      lst.push_front(ad);
#endif
    }
    it++;
  }
}

void extract_servers(DnsMessage *a, stl_slist(domainname) & norrdoms, stl_slist(_addr) & lst) {
  stl_list(DnsRR)::iterator it = a->authority.begin();
  domainname dom;
  int t;
  while (it != a->authority.end()) {
    if (it->TYPE == DNS_TYPE_NS) {
      dom = domainname(true, it->RDATA);
      t = lst.size();
      find_servers(a, dom, lst);
      if (t == lst.size()) {
        norrdoms.push_front(dom);
      }
    }
    it++;
  }
}

void find_addresses(_addr &srv, domainname dom, stl_slist(_addr) & lst) {
  DnsMessage *q = NULL, *a = NULL;
  pos_cliresolver res;
  stl_list(a_record) aalist;
  stl_list(a_record)::iterator it;
  _addr ad;
  try {
    q = create_query(dom, DNS_TYPE_A);
    res.query(q, a, &srv);
    aalist = get_a_records(a);
    it = aalist.begin();
    if (it == aalist.end()) throw PException(true, "No addresses for dom %s", dom.tocstr());
    while (it != aalist.end()) {
      getaddress_ip4(&ad, it->address, 53);
      lst.push_front(ad);
      it++;
    }
  } catch (PException p) {
    if (q) delete q;
    if (a) delete a;
    throw p;
  }
  if (q) delete q;
  if (a) delete a;
}

int main(int argc, char **argv) {
  /* read command line */
  query_item item;
  rr_type *type;
  _addr server, alternate;
  bool alternate_given = false;
  txt_to_addr(&server, "127.0.0.1");
  for (int x = 1; x < argc; x++) {
    if (strcmpi(argv[x], "--help") == 0 || strcmpi(argv[x], "-h") == 0) {
      print_help();
      return 1;
    } else if (strcmpi(argv[x], "--version") == 0 || strcmpi(argv[x], "-r") == 0) {
      print_version();
      return 1;
    } else if (strcmpi(argv[x], "--verbose") == 0 || strcmpi(argv[x], "-v") == 0) {
      verbose = true;
    } else if (argv[x][0] == '@') {
      try {
        txt_to_addr(&server, argv[x] + 1);
      } catch (PException p) {
        printf("Error: incorrect server to query: %s\n", argv[x]);
        return 1;
      }
    } else if (strcmpi(argv[x], "--alternate") == 0 || strcmpi(argv[x], "-a") == 0) {
      if (x == argc - 1) {
        printf("Error: expected argument to --alternate!\n");
        return 1;
      }
      alternate_given = true;
      try {
        txt_to_addr(&alternate, argv[++x]);
      } catch (PException p) {
        printf("Error: incorrect server to query: %s\n", argv[x]);
        return 1;
      }
    } else {
      try {
        item.qname = argv[x];
      } catch (PException p) {
        printf("Error: incorrect query domain name: %s\n", argv[x]);
        return 1;
      }
      item.qtype = DNS_TYPE_A;

      if (x + 1 < argc) {
        type = rrtype_getinfo(argv[x + 1]);
        if (type) {
          item.qtype = type->type;
          x++;
        }
      }
      query_item_list.push_back(item);
      continue;
    }
  }
  /* do queries */
  stl_list(query_item)::iterator it = query_item_list.begin();
  pos_cliresolver res;
  DnsMessage *q = NULL, *a = NULL, *a2 = NULL;
  uint32_t rttl;
  stl_slist(_addr) servers;
  stl_slist(domainname) domz;
  while (it != query_item_list.end()) {
    try {
      /* send query */
      q = create_query(it->qname, it->qtype, false);
      printf("-> %s\n", it->qname.tocstr());
      if (verbose)
        printf("Sending query [%s,%s]\n", it->qname.tocstr(), str_qtype(it->qtype).c_str());
      res.query(q, a, &server);
      if (verbose) printf("Answer received.\n");
      /* find ttl of answer */
      if (a->RCODE == RCODE_NXDOMAIN) {
        printf("Server %30s said: NXDOMAIN, ", addr_to_string(&server).c_str());
        rttl = soa_ttl(a);
        if (rttl) {
          printf("ttl=%s\n", str_ttl(rttl).c_str());
        } else {
          printf("no ttl\n");
        }
      } else if (a->RCODE != RCODE_NOERROR) {
        printf("Server %30s said: query error %s\n", str_rcode(a->RCODE).c_str());
      } else if (a->answers.size() == 0) {
        rttl = soa_ttl(a);
        if (rttl) {
          printf("Server %-30s said: NODATA, ttl=%s\n", addr_to_string(&server).c_str(),
                 str_ttl(rttl).c_str());
        } else {
          printf("Server %-30s said: domain not in cache\n", addr_to_string(&server).c_str());
        }
      } else if (a->answers.begin()->TYPE == DNS_TYPE_CNAME) {
        printf("Server %-30s said: CNAME, points to %s\n", addr_to_string(&server).c_str(),
               domainname(true, a->answers.begin()->RDATA).tocstr());
      } else if (a->answers.begin()->TYPE != it->qtype || a->answers.begin()->NAME != it->qname) {
        throw PException("Answer didn't answer query!");
      } else {
        /* an answer! */
        rttl = a->answers.begin()->TTL;
        printf("Server %-30s answered: ttl=%s\n", addr_to_string(&server).c_str(),
               str_ttl(rttl).c_str());

        if (alternate_given) {
          if (verbose) printf("Querying alternate server for glue\n");
          q->RD = true;
          res.query(q, a2, &alternate);
          if (verbose) printf("Finished querying alternate server\n");
        } else
          a2 = a;

        servers.clear();
        extract_servers(a2, domz, servers);
        if (servers.begin() == servers.end()) {
          if (domz.begin() == domz.end()) throw PException("No NS list in answer!");
          find_addresses(server, *domz.begin(), servers);
        }
        if (servers.begin() == servers.end())
          throw PException("Couldn't find nameservers to query!");

        /* now, query real server */
        delete a;
        a = NULL;
        res.query(q, a, servers);
        if (a->RCODE == RCODE_NOERROR) {
          if (a->answers.size()) {
            if (a->answers.begin()->TYPE == DNS_TYPE_CNAME) {
              printf("Server %-30s answered: CNAME\n", addr_to_string(&*servers.begin()).c_str());
            } else {
              printf("Server %-30s answered: ttl=%s\n", addr_to_string(&*servers.begin()).c_str(),
                     str_ttl(a->answers.begin()->TTL).c_str());
              if (a->answers.begin()->TTL > rttl) {
                printf("The domain %s was probably visited less than %s ago.\n", it->qname.tocstr(),
                       str_ttl(a->answers.begin()->TTL - rttl).c_str());
              } else {
                printf("The %s server reduced their TTL value.\n", it->qname.tocstr());
              }
            }
          } else {
            printf("Server %-30s answered: NODATA\n", addr_to_string(&*servers.begin()).c_str());
          }
        } else {
          printf("Server %-30s answered: error %s\n", addr_to_string(&*servers.begin()).c_str(),
                 str_rcode(a->RCODE).c_str());
        }
      }
    } catch (PException p) {
      printf("*** query failed: %s\n", p.message);
    }
    if (q) {
      delete q;
      q = NULL;
    }
    if (a) delete a;
    if (a2 && a != a2) delete a2;
    a = a2 = NULL;
    printf("\n");
    it++;
  }
}
