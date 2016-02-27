#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <unistd.h>
#include "mpi.h"

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

void do_decrypt(long start_key, long end_key, unsigned char encrypted_text[ BLOCK_SIZE ], const EVP_CIPHER *cipher)
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
	
	for( ; start_key < end_key && memcmp( output, "Frase", 5 ) != 0; start_key++ ) {
		gen_key(&key[KEY_LENGTH-1],start_key);
		EVP_CipherInit_ex( &ctx, 0, 0, key, 0, 0 );
		EVP_DecryptUpdate( &ctx, output, &output_length, encrypted_text, BLOCK_SIZE );
		EVP_DecryptFinal_ex( &ctx, output, &padding_length );		
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
	int proc_id, num_procs;
	int chunk, first, last;
	long work_size;
	
	// init mpi
    MPI_Init( &argc, &argv );

	// check this process id
    MPI_Comm_rank( MPI_COMM_WORLD, &proc_id );

	// check how many process are available
    MPI_Comm_size( MPI_COMM_WORLD, &num_procs );
	
	fs_read(argv[1],encrypted_text,BLOCK_SIZE);	
	get_cant_keys(&work_size);
	
	chunk = work_size / num_procs;
	first = proc_id * chunk;
	
	// if i am the last process, take the last portion of work
	// is to prevent bad chunk calculation
	if( proc_id == num_procs - 1 ) {
		last = work_size;
	}
	else {
		last = first + chunk;
	}
	
	// begin the decryption
	do_decrypt(first,last,encrypted_text,EVP_bf_cbc());
	do_decrypt(first,last,encrypted_text,EVP_cast5_cbc());

	// close mpi
    MPI_Finalize();
	
	exit(0);
}
