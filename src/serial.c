#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/fs.h"
#include "../include/keygen.h"
#include "../include/encryptor.h"
#include "../include/commons.h"

int main( int argc, char** argv )
{
	check_parameters( argc, argv );

	unsigned char encrypted_text[ BLOCK_SIZE ];
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};
	unsigned char key[ KEY_LENGTH ];
	long cant_keys;
	long success_key = -1;
	int encryption_method = -1;
	time_t start_time, end_time;

	// get the time stamp
	start_time = time( NULL );

	read_parameters( argv, encrypted_text, &cant_keys );

	// create and init the encryptor
	Encryptor decryptor[2];
	init_decryptor( &decryptor[0], DECRYPT, BLOWFISH, iv, encrypted_text );
	init_decryptor( &decryptor[1], DECRYPT, CAST5, iv, encrypted_text );

	// begin the decryption
	for( long i = 0; i < cant_keys && success_key == -1; i++ ) {
		keygen_itokey( key, i );

		encryptor_set_key( &decryptor[0], key );
		encryptor_init( &decryptor[0] );
		encryptor_update( &decryptor[0] );
		encryptor_final( &decryptor[0] );

		if( memcmp( (char *)decryptor[0].output, "Frase", 5 ) == 0 ) {
			success_key = i;
			encryption_method = decryptor[0].type;
		}
	}

	for( long i = 0; i < cant_keys && success_key == -1; i++ ) {
		keygen_itokey( key, i );

		encryptor_set_key( &decryptor[1], key );
		encryptor_init( &decryptor[1] );
		encryptor_update( &decryptor[1] );
		encryptor_final( &decryptor[1] );

		if( memcmp( (char *)decryptor[1].output, "Frase", 5 ) == 0 ) {
			success_key = i;
			encryption_method = decryptor[1].type;
		}
	}

	end_time = time( NULL );
	print_result( success_key, encryption_method, difftime( end_time, start_time ) );

	exit(0);
}
