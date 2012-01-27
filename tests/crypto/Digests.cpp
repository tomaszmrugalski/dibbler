#include <string.h>
#include "sha256.h"

void printHash(char* digest) {
    int x=0;
    for (x=0;x<32;x++) {
        printf("%02x", digest[x]);
    }
    printf("\n");
}

int main() {
    
    char * buf1 = "The quick brown fox jumps over the lazy dog";
    char digest[32];

    int len = strlen(buf1);
    void * result = sha256_buffer(buf1, len, (void*)digest);

    printf("str=[%s] len=%d result=%p digest=%p\n", buf1, strlen(buf1), result, digest);
    printHash(digest);

    printf("\n");

    char * buf2="The quick brown fox jumps over the lazy dogThe quick brown fox jumps over the lazy dog";
    len = strlen(buf2);
    result = sha256_buffer(buf2, len, (void*)digest);

    printf("str=[%s] len=%d result=%p digest=%p\n", buf2, strlen(buf2), result, digest);
    printHash(digest);
}
