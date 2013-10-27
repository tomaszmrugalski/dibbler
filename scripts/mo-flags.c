

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/queue.h>

#define IF_RA_MANAGED 0x40
#define IF_RA_OTHERCONF 0x80

int main() {
    int sd, status;
    int seq = time(NULL);
    struct sockaddr_nl nl_addr;
    struct nlmsghdr *nlm_hdr;
    struct rtgenmsg *rt_genmsg;
    char buf[NLMSG_ALIGN(sizeof(struct nlmsghdr)) + NLMSG_ALIGN(sizeof(struct rtgenmsg))];
    struct msghdr msgh;
    struct nlmsghdr *nlm;
    size_t newsize = 65536, size = 0;
    int msg_len;
    char *nbuf = NULL;
    int nlm_len;
    struct ifinfomsg *ifim;
    struct rtattr *rta, *rta1;
    size_t rtasize, rtapayload, rtasize1;
    void *rtadata;
    char if_name[10];

    sd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sd < 0) {
        perror("socket");
        return -1;
    }

    memset(&nl_addr, 0, sizeof(nl_addr));
    nl_addr.nl_family = AF_NETLINK;
    if (bind(sd, (struct sockaddr *)&nl_addr, sizeof(nl_addr)) < 0) {
        perror("bind");
        return -1;
    }

    memset(&buf, 0, sizeof(buf));

    nlm_hdr = (struct nlmsghdr *)buf;
    nlm_hdr->nlmsg_len = NLMSG_LENGTH(sizeof(*rt_genmsg));
    nlm_hdr->nlmsg_type = RTM_GETLINK;
    nlm_hdr->nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST;
    nlm_hdr->nlmsg_pid = getpid();
    nlm_hdr->nlmsg_seq = seq;

    memset(&nl_addr, 0, sizeof(nl_addr));
    nl_addr.nl_family = AF_NETLINK;

    rt_genmsg = (struct rtgenmsg *)NLMSG_DATA(nlm_hdr);
    rt_genmsg->rtgen_family = AF_INET6;
    status = sendto(sd, (void *)nlm_hdr, nlm_hdr->nlmsg_len, 0, (struct sockaddr *)&nl_addr, sizeof(nl_addr));
    if (status < 0) {
        perror("sendto");
        return -1;
    }

    for (;;) {
        void *newbuf = realloc(nbuf, newsize);
        if (newbuf == NULL) {
            perror("realloc");
            return -1;
        }

        nbuf = newbuf;

        /* If call intrupted do it again */
        do {
            struct iovec iov = {nbuf, newsize};
            memset(&msgh, 0, sizeof(msgh));
            msgh.msg_name = (void *)&nl_addr;
            msgh.msg_namelen = sizeof(nl_addr);
            msgh.msg_iov = &iov;
            msgh.msg_iovlen = 1;
            msg_len = recvmsg(sd, &msgh, 0);
        }while (msg_len < 0 && errno == EINTR);

        /* If msg truncated because of less size, call it with double the size */
        if (msg_len < 0 || msgh.msg_flags & MSG_TRUNC) {
            size = newsize;
            newsize *= 2;
            continue;
        } else if (msg_len == 0)
            break;

        for (nlm = (struct nlmsghdr *)nbuf; NLMSG_OK(nlm, msg_len);
             nlm = (struct nlmsghdr *)NLMSG_NEXT(nlm, msg_len)) {
            if (nlm->nlmsg_type == NLMSG_DONE) {
                printf("NLMSG_DONE\n");
                return 0;
            }
            else if (nlm->nlmsg_type == NLMSG_ERROR) {
                printf("NLMSG_ERROR\n");
                return -1;
            }

            if (nlm->nlmsg_pid != getpid() || nlm->nlmsg_seq != seq)
                continue;

            ifim = (struct ifinfomsg *)NLMSG_DATA(nlm);
            if (ifim->ifi_family != AF_INET6 || nlm->nlmsg_type != RTM_NEWLINK) {
                printf("Not AF_INET6 or RTM_NEWLINK request\n");
                return -1;
            }

            nlm_len = msg_len;
            rtasize = NLMSG_PAYLOAD(nlm, nlm_len) - NLMSG_ALIGN(sizeof(*ifim));
            for (rta = (struct rtattr *) (((char *) NLMSG_DATA(nlm)) +
                                          NLMSG_ALIGN(sizeof(*ifim))); RTA_OK(rta, rtasize);
                 rta = RTA_NEXT(rta, rtasize)) {
                rtadata = RTA_DATA(rta);
                rtapayload = RTA_PAYLOAD(rta);

                switch (rta->rta_type) {
                case IFLA_PROTINFO:
                    rtasize1 = rta->rta_len;
                    for (rta1 = (struct rtattr *)rtadata; RTA_OK(rta1, rtasize1);
                         rta1 = RTA_NEXT(rta1, rtasize1)) {
                        void *rtadata1 = RTA_DATA(rta1);
                        switch(rta1->rta_type) {
                        case IFLA_INET6_FLAGS:
                            if_indextoname(ifim->ifi_index, if_name);
                            printf("interface: %s\n", if_name);

                            if (*((u_int32_t *)rtadata1) & IF_RA_MANAGED)
                                printf("\tmanaged flag is set\n");
                            else
                                printf("\tmanaged flag is not set\n");
                            if (*((u_int32_t *)rtadata1) & IF_RA_OTHERCONF)
                                printf("\tother flag is set\n");
                            else
                                printf("\tother flag is not set\n");
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}
