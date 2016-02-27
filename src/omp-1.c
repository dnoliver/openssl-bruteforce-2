#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>

#define ASCII_SPACE 32
#define ASCII_ZERO 48
#define BLOCK_SIZE 8
#define IV_LENGTH 8
#define KEY_LENGTH 16
#define CANT_KEYS 100000000
#define NUM_THREADS 2

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
	int output_length;
	int padding_length;
	int thread_id;
	int num_threads = NUM_THREADS;
	int success_key = -1;
	char * omp_num_threads = getenv("OMP_NUM_THREADS");
	
	if( omp_num_threads != NULL ){
		num_threads = strtol( omp_num_threads, NULL, 0 );
		if( num_threads <= 0 )
			num_threads = NUM_THREADS;
	}
	
	omp_set_num_threads( num_threads );
	
	EVP_CIPHER_CTX *ctx = (EVP_CIPHER_CTX*) malloc( num_threads * sizeof(EVP_CIPHER_CTX));
	unsigned char **key = ( unsigned char ** ) malloc( num_threads * sizeof( unsigned char * ) );
	unsigned char **output = ( unsigned char ** ) malloc( num_threads * sizeof( unsigned char * ) );
	
	for( int i = 0; i < num_threads; i++ ){
		EVP_CIPHER_CTX_init( &ctx[i] );
		EVP_CipherInit_ex( &ctx[i], cipher, 0, 0, iv, 0);
		EVP_CIPHER_CTX_set_key_length( &ctx[i], KEY_LENGTH );
		EVP_CIPHER_CTX_set_padding(&ctx[i], 0);
		key[i] = ( unsigned char * ) malloc( KEY_LENGTH );
		output[i] = ( unsigned char * ) malloc( BLOCK_SIZE );
	
		memset(key[i],ASCII_SPACE,KEY_LENGTH);
	}
	
	#pragma omp parallel for private( thread_id, output_length, padding_length ) shared( success_key )
	for(long i = 0; i < cant_keys ; i++ ) {
		if(success_key != -1) continue;
		
		thread_id = omp_get_thread_num();
		gen_key(&key[thread_id][KEY_LENGTH-1],i);
		EVP_CipherInit_ex( &ctx[thread_id], 0, 0, key[thread_id], 0, 0 );
		EVP_DecryptUpdate( &ctx[thread_id], output[thread_id], &output_length, encrypted_text, BLOCK_SIZE );
		EVP_DecryptFinal_ex( &ctx[thread_id], output[thread_id], &padding_length );
		
		if( memcmp( output[ thread_id ], "Frase", 5 ) == 0 ) {
			success_key = i;
		}
	}
	
	for( int i = 0; i < num_threads; i++ ){
		EVP_CIPHER_CTX_cleanup( &ctx[i] );
		free(key[i]);
		free(output[i]);
	}
	
	free(ctx);
	free(key);
	free(output);
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