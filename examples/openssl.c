#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int main()
{
    EVP_CIPHER_CTX ctx;
	const EVP_CIPHER *cipher = EVP_bf_cbc();
    unsigned char key[16] = "0123456789ABCDEF";
    unsigned char iv[8] = {1,2,3,4,5,6,7,8};
    unsigned char input[8] = "-------";
    unsigned char *encrypted = NULL;
	unsigned char *output = NULL;
    int outlen1, outlen2;

	encrypted = realloc( encrypted, 16 );

	EVP_CIPHER_CTX_init( &ctx );

    EVP_CipherInit_ex(&ctx, cipher, NULL, key, iv, 1);
    EVP_CipherUpdate(&ctx, encrypted, &outlen1, input, 8 );
    EVP_CipherFinal_ex(&ctx, encrypted + outlen1, &outlen2);

    printf("\nciphertext length: %d\n", outlen1 + outlen2);

	output = realloc( output, 24 );

	cipher = EVP_bf_cbc();
	EVP_CIPHER_CTX_init( &ctx );

	EVP_CipherInit_ex( &ctx, cipher, NULL, key, iv , 0);
	EVP_CipherUpdate( &ctx, output, &outlen1, encrypted, 16 );

	if( EVP_CipherFinal_ex( &ctx, output + outlen1, &outlen2) != 1 )
	{
		printf("\nerror");
		ERR_print_errors_fp( stderr );
	}

	printf("\noutlen: %i\n", outlen1 + outlen2 );

	printf("text length: %d\n", outlen1 + outlen2);
	printf("text: %s", output);

    return 0;
}
