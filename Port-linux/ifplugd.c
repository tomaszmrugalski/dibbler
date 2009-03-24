#include "ifplugd.h"

int handle(const char* iface) {
    int fd, r = 0;
    interface_status_t s;
    
    if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {	
        printf("Socket create failed\n");
	return -1;
    }   
    
    /* here just to make it more convenient for testing:if we define DEBUG,so a plugin-out and plugin-in event
     * will be detected if we do "ifconfig interface-name down" and ifconfig interface-name up".Otherwise,
     * if we do not define DEBUG, a plugin-in and plugin-out event can only be detected when the local link
     * plugin in and out. Skip this comment if you did not catch it.
     */
    
    /* #define DEBUG*/
    #ifndef DEBUG
    if ((s = interface_detect_beat_ethtool(fd, iface)) == IFSTATUS_ERR) {
        printf("    SIOCETHTOOL failed (%s)\n", strerror(errno));
	return -1;
    }
    #else
    if ((s = interface_detect_beat_iff(fd,iface)) == IFSTATUS_ERR) {
	printf("   IFF_RUNNING failed (%s)\n", strerror(errno));
	return -1;
    }
    #endif
 
    switch(s) {
        case IFSTATUS_UP:
            /* link beat detected */
            r = 1;
            break;
                
        case IFSTATUS_DOWN:
            /* unplugged */
            r = 2;
            break;

        default:
            /* not supported (Retry as root?)*/
            r = -1;
            break;
    }
            
   close(fd);
   return r;
}
