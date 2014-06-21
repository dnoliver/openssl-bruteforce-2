#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fs.h"
#include "../include/encryptor.h"
#include "../include/keygen.h"

#define NUM_PARAMS 5

int main(int argc, char** argv)
{
	if( argc != NUM_PARAMS ) {
		printf("usage: %s [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE] \n", argv[0] );
		exit(-1);
	}

	Encryptor enc;
	unsigned char key[ KEY_LENGTH ];
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};

	long cant_keys;

	char *input_file = argv[1];
	long keycode = strtol( argv[2], NULL, 0 );
	char *method = argv[3];
	char *path = argv[4];
	int method_code;
	unsigned char input[100];

	int input_length = fs_read( input_file, input, 100 );

	if( input_length <= 0 ){
		printf("the [INPUT FILE] must have at least 1 character \n");
		exit(-1);
	}

	keygen_getenv( &cant_keys );

	if( keycode <= 0 || keycode >= cant_keys ){
		printf( "the [KEY CODE] must be in [1,%li] \n", cant_keys - 1 );
        exit(-1);
	}

	if( strcmp( method, "blowfish" ) != 0 && strcmp( method, "cast5" ) != 0 ){
		printf( "method must be blowfish or cast5 \n" );
		exit(4);
	}

	method_code = strcmp( method, "blowfish" ) == 0? BLOWFISH : CAST5;

	if( keygen_itokey( key, keycode ) != 0) {
		printf("error on generating key: %li \n", keycode);
        exit(-1);
	}

	if(
		encryptor_init_data( &enc, ENCRYPT, method_code, iv ) != 0 ||
	    encryptor_set_input( &enc, input, input_length ) != 0 ||
	    encryptor_set_key( &enc, key ) != 0 )
	{
		printf("error on encryption initialization\n");
		exit(-1);
	}

	if( encryptor_init( &enc ) != 0 ||
		encryptor_update( &enc ) != 0 ||
		encryptor_final( &enc ) != 0 )
	{
		printf("error on encryption execution\n");
        exit(-1);
	}

	if( fs_write( path, enc.output, enc.output_length + enc.padding_length ) == -1 ) {
		printf("error on file writting\n");
		exit(-1);
	}

	if( encryptor_clean_up( &enc ) != 0 ) {
		printf("error on encryption destruction\n");
		exit(-1);
	}

	exit(0);
}
