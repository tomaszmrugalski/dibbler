
#include "DNSUpdate.h"

#include "base64.h"

int main()
{
    char *plain = "any carnal pleasure.";
    char encoded[128];
    char decoded[128];

    base64_encode(plain, strlen(plain), encoded, 128);
    
    printf("encoded [%s] => [%s]\n", plain, encoded);

    // decode
    struct base64_decode_context ctx;
    base64_decode_ctx_init(&ctx);

    size_t decodedLen;
    base64_decode(&ctx, encoded, strlen(encoded),
                  decoded, &decodedLen);

    printf("decoded [%s] => [%s]\n", encoded, decoded);


    return 0;
}
