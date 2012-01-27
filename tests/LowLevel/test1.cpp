
#include <wchar.h>
#include "Portable.h"


int main() {
    
    char dev[] = "eth0";
    char prefix[] = "2000::";
    int  len = 64;
    int result;

    if (prefix_forwarding_enabled()) {
	printf("IPv6 forward enabled\n");
	return 0;
    } else {
	printf("IPv6 forward disabled\n");
	return 0;
    }

    domain_add("eth0", 4, "example.com");
    domain_add("eth0", 4, "klub.com.pl");

    getwchar();

    domain_del("eth0", 4, "example.com");
    getwchar();
    domain_del("eth0", 4, "klub.com.pl");

#if 0
    lowlevelInit();

    printf("Adding %s/%d prefix to %s interface\n", prefix, len, dev);
    result = prefix_add(dev, 4, prefix, len);
    printf("RESULT=%d\n", result);

    getwchar();

    result = printf("Deleting the same prefix\n");
    prefix_del(dev, 4, prefix, len);
    printf("RESULT=%d\n", result);

    lowlevelExit();
#endif

    return 0;
}
