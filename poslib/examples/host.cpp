#include <poslib/poslib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  _addr addr;
  DnsMessage *q = NULL, *a = NULL;
  pos_cliresolver res;
  a_record ip;

  try {
    /* get command-line arguments */
    if (argc == 3 && argv[1][0] == '@') {
      txt_to_addr(&addr, argv[1] + 1);
      q = create_query(argv[2]);
    } else if (argc == 2) {
      txt_to_addr(&addr, "127.0.0.1");
      q = create_query(argv[1]);
    } else {
      printf("Usage: host [@if] host\n");
      return 1;
    }

    res.query(q, a, &addr);
    ip = get_a_record(a);
    printf("%s has address %d.%d.%d.%d\n", q->questions.begin()->QNAME.tocstr(), ip.address[0],
           ip.address[1], ip.address[2], ip.address[3]);

  } catch (PException p) {
    printf("Fatal exception: %s\n", p.message);
    return 1;
  }

  if (q) delete q;
  if (a) delete a;

  return 0;
}
