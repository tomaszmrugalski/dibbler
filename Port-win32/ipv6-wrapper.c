#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <Ws2tcpip.h.>
#include <Ws2spi.h>

#include <iphlpapi.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "Portable.h"

extern void displayError(int errCode);

void showIfaces(struct iface * list) {
    int j,k;
    char buf[512];
    while (list) {
        printf("\nIface: name=[%s],id=%d, flags=%d HWtype=%d\n",
               list->name, list->id, list->flags, list->hardwareType);
        
        printf("  maclen=%d, mac=[",list->maclen);               
        for (j=0;j< (list->maclen) ; j++) {
                printf("%02x",(unsigned char)list->mac[j]);
                if (j<list->maclen-1) printf("-");
        }
        printf("]\n");
        
        k=0;
        for (k=0; k< list->linkaddrcount; k++){
          inet_ntop6( (unsigned char *)(list->linkaddr+k*16),(unsigned char *)buf);
          printf("  LL-Addr: %s\n",buf);
        }
                
        list = list->next;
    }
}

time_t timer;
char ipv6Path[] = "d:\\winnt\\system32\\ipv6.exe";
char cmd[] = "d:\\winnt\\system32\\cmd.exe";
char tmpFile[] = "c:\\tmp\\ipv6.txt";

void if_list_release(struct iface * list) {
    struct iface * tmp;
    while (list) {
        tmp = list->next;
        free(list);
        list = tmp;
    }
}

struct iface* if_list_get() {
    struct iface * head = NULL;
    struct iface * tmp;
    int pos;
    char * bufPtr;
    char buf[512];
    char * name;
    char * tmpbuf;
    FILE *f;
    
    int result = spawnl(_P_WAIT,cmd,cmd,"/C",ipv6Path,"if",">",tmpFile,NULL);
    if (result<0) {    
        printf("ERROR: unable to run ipv6.exe\n");
        return NULL;
    }
    if ( ! (f=fopen(tmpFile,"r"))  ) {
        printf("ERROR: unable to open %s file\n",tmpFile);
        return NULL;
    }
    
    while ( !feof(f)) {
        fgets(buf,511,f);
        
        // Interface 1 (site 1): my network interface
        if (strstr(buf,"Interface ")) {
                tmp = (struct iface*)malloc(sizeof(struct iface));
                tmp->next = head;
                
                memset(tmp->mac,0,255);
                tmp->maclen = 0;
                tmp->linkaddrcount = 0;
                tmp->hardwareType = 1; // other (ipifcons.h)
                tmp->flags = 0;
                head = tmp;
                printf(">>>%s",(buf+10));
                tmp->id = atoi(buf+10);
                name = (strstr(buf+10,":")+2);
                name[ strlen(name)-1] = 0;
                sprintf(tmp->name,name);
        }
        // link-level address: 00-11-22-33-44-55
        if (strstr(buf,"link-level address:")) {
                printf("###(%d)%s",strlen(buf+22),buf+22);
                if (strlen(buf+22)==18) { // ethernet
                    tmp->hardwareType = 6; 
                    tmp->flags = IF_UP | IF_RUNNING | IF_MULTICAST;
                    sscanf( (buf+22) ,"%x-%x-%x-%x-%x-%x", tmp->mac, tmp->mac+1, tmp->mac+2,
                                                            tmp->mac+3, tmp->mac+4, tmp->mac+5);
                    tmp->maclen=6;
                } else 
                if (strlen(buf+22) > 7) { // tunnel 0.0.0.0
                    tmp->hardwareType = 131; // tunnel
                    sscanf( (buf+22), "%d.%d.%d.%d", tmp->mac, tmp->mac+1,
                                                     tmp->mac+2, tmp->mac+3);
                    tmp->maclen = 4;
                } else
                if (strlen(buf+22) < 2) { // loopback ""
                    tmp->hardwareType = 24; // loopback
                    tmp->flags = IF_UP | IF_RUNNING | IF_LOOPBACK | IF_MULTICAST;
                    tmp->maclen = 0;
                } else {
                    tmp->hardwareType = 1; // unknown
                    tmp->flags = IF_UP | IF_RUNNING | IF_MULTICAST;
                    // uncomment this if you like cute defaults
                    // tmp->hardwareType = 132; // coffee pot
                }
        }
        // link-local address 
        if (strstr(buf,"fe80::")) {
                bufPtr = strstr(buf,",");
                if (bufPtr) *bufPtr = 0;
                printf("ADDR=[%s]",strstr(buf,"fe80::")); fflush(stdout);
                pos = (tmp->linkaddrcount + 1)*16;
                printf("Alokuje %d pamieci.\n",pos);
                tmpbuf = (char*) malloc( pos );
                memcpy(tmpbuf,tmp->linkaddr, pos - 16);
                inet_pton6( strstr(buf,"fe80::"), tmpbuf + tmp->linkaddrcount*16);
                free(tmp->linkaddr);
                tmp->linkaddr = tmpbuf;
                tmp->linkaddrcount++;
        }
    }    
    fclose(f);
	return tmp;
}

int is_addr_tentative(char* ifacename, int iface, char* plainAddr)
{
    int result;
    char buf[512];
    FILE *f;

    sprintf(buf, "%d",iface);
    result = spawnl(_P_WAIT,cmd,cmd,"/C",ipv6Path,"if",buf,">",tmpFile,NULL);
    if (result<0) {    
        printf("ERROR: unable to run ipv6.exe\n");
        return -1;
    }
    if ( ! (f=fopen(tmpFile,"r"))  ) {
        printf("ERROR: unable to open %s file\n",tmpFile);
        return -1;
    }
    
    while ( !feof(f)) {
        fgets(buf,511,f);
        if (strstr(buf,plainAddr) && strstr(buf,"duplicate") ) {
                fclose(f);
                return 1; // duplicate
        }
    }
    fclose(f);
    return 0; // address ok
}

int ipaddr_add(char * ifacename, int ifaceid, char * addr, long pref, long valid)
{
    char addr2[128];
    char buf[64];
    int result;
    
    sprintf(buf,"%d/%d",valid,pref);
    sprintf(addr2,"%d/%s", ifaceid, addr);
    result = spawnl(_P_WAIT,cmd,cmd,"/C",ipv6Path,
                        "adu",addr2,"lifetime",buf,">",tmpFile,NULL);
    if (result<0) {
        return result;
    }
	return 0;
}

extern int ipaddr_del(char * ifacename, int ifaceid, char * addr)
{
	return ipaddr_add(ifacename, ifaceid, addr, 0, 0);
}

extern int dns_add(const char* ifname, int ifaceid, char* addrPlain) {
    return 0;
}
extern int dns_del(const char* ifname, int ifaceid, char* addrPlain) {
    return 0;
}

extern int domain_add(const char* ifname, int ifaceid, char* domain) {
    return 0;
}

extern int domain_del(const char* ifname, int ifaceid, char* domain) {
    return 0;
}
 
