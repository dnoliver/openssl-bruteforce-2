#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <unistd.h>

#define ASCII_SPACE 32
#define ASCII_ZERO 48
#define BLOCK_SIZE 8
#define IV_LENGTH 8
#define KEY_LENGTH 16
#define CANT_KEYS 100000000

static unsigned char iv[] = {1,2,3,4,5,6,7,8};

void gen_key(unsigned char *pointer, long i)
{
	for(*pointer = ASCII_ZERO; i > 0; i /= 10, --pointer){
		*pointer = ASCII_ZERO + i % 10;
	}
}

void get_cant_keys( long* cant_keys )
{
	char *env = getenv("CANT_KEYS");
	*cant_keys = env == NULL? CANT_KEYS : atol(env);
}

void do_decrypt(long cant_keys, unsigned char encrypted_text[ BLOCK_SIZE ], const EVP_CIPHER *cipher)
{
	unsigned char key[ KEY_LENGTH ];
	unsigned char output[ BLOCK_SIZE ];
	int output_length;
	int padding_length;
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init( &ctx );
	EVP_CipherInit_ex( &ctx, cipher, 0, 0, iv, 0);
	EVP_CIPHER_CTX_set_key_length( &ctx, KEY_LENGTH );
	EVP_CIPHER_CTX_set_padding(&ctx, 0);
	
	memset(key,ASCII_SPACE,KEY_LENGTH);
	memset(output,ASCII_SPACE,BLOCK_SIZE);
	
	while(cant_keys != 0 && memcmp( output, "Frase", 5 ) != 0){
		gen_key(&key[KEY_LENGTH-1],cant_keys);
		EVP_CipherInit_ex( &ctx, 0, 0, key, 0, 0 );
		EVP_DecryptUpdate( &ctx, output, &output_length, encrypted_text, BLOCK_SIZE );
		EVP_DecryptFinal_ex( &ctx, output, &padding_length );		
		--cant_keys;
	}
	
	EVP_CIPHER_CTX_cleanup( &ctx );
}


int fs_read( const char* path, unsigned char *output, int size )
{
	int file = open( path, O_RDONLY );
	int readed;

	if( file == -1 ) {
		return -1;
	}
	
	readed = read( file, output, size );
	close(file);

	return readed;
}

int main( int argc, char** argv )
{
	unsigned char encrypted_text[ BLOCK_SIZE ];
	long cant_keys;
	
	fs_read(argv[1],encrypted_text,BLOCK_SIZE);	
	get_cant_keys(&cant_keys);
	
	// begin the decryption
	do_decrypt(cant_keys,encrypted_text,EVP_bf_cbc());
	do_decrypt(cant_keys,encrypted_text,EVP_cast5_cbc());
	
	exit(0);
}
