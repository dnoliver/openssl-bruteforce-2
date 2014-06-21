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
	unsigned char buffer[ 100 ];
	int readed;
	unsigned char key[ KEY_LENGTH ];
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};

	char *input_file = (char *)argv[1];
	keygen_itokey( key, strtol(argv[2],NULL,0) );
	char *method = argv[3];
	char *output_file = argv[4];
	int method_code;

	readed =  fs_read( input_file, buffer, 100 );

	if( readed == -1 )
	{
		printf("error on file reading\n");
		exit(-1);
	}

	if( strcmp(method,"blowfish") != 0 && strcmp(method,"cast5") != 0 ) {
		printf("method must be blowfish or cast5\n");
		exit(-1);
	}

	method_code = strcmp(method,"blowfish") == 0 ? BLOWFISH : CAST5;

	if( encryptor_init_data( &enc,DECRYPT, method_code, iv ) != 0 ||
    	encryptor_set_input( &enc, buffer, readed ) != 0 ||
    	encryptor_set_key( &enc,key ) != 0 )
	{
		printf("error on encryptor initialization\n");
		exit(-1);
	}

	if(	encryptor_init( &enc ) != 0 ||
		encryptor_update( &enc ) != 0 ||
		encryptor_final( &enc ) != 0 )
	{
		printf("error on encryptor execution\n");
		exit(-1);
	}

	if( fs_write( output_file, enc.output , enc.output_length + enc.padding_length ) == -1 ) {
		printf("error on file writting\n");
		exit(-1);
	}

	if( encryptor_clean_up( &enc ) != 0 ) {
		printf("error on encryptor destruction\n");
		exit(-1);
	}

	exit(0);
}
