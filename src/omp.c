#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "../include/fs.h"
#include "../include/keygen.h"
#include "../include/encryptor.h"
#include "../include/commons.h"

#define NUM_THREADS 2

int main(int argc, char** argv)
{
	check_parameters( argc, argv );

	unsigned char encrypted_text[ BLOCK_SIZE ];
    unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};
    unsigned char **key = NULL;
    long cant_keys;
    long success_key = -1;
    int encryption_method = -1;
    int num_threads = NUM_THREADS;
    char * omp_num_threads = getenv("OMP_NUM_THREADS");
    int thread_id;
    time_t start_time, end_time;

    // get the time stamp
    start_time = time( NULL );

    read_parameters( argv, encrypted_text, &cant_keys );

	// get the number of threads from enviroment
	if( omp_num_threads != NULL ){
		num_threads = strtol( omp_num_threads, NULL, 0 );
		if( num_threads <= 0 )
			num_threads = NUM_THREADS;
	}

	omp_set_num_threads( num_threads );

	// create the blowfish encryptor
	Encryptor *bf = (Encryptor *) malloc( num_threads * sizeof( Encryptor ) );
	if( bf == NULL ){
		printf("\nerror on encryptor memory allocation\n");
		exit(-1);
	}

	// create the cast5 encryptor
    Encryptor *cast5;
    cast5 = (Encryptor *) malloc( num_threads *  sizeof( Encryptor ) );
    if( cast5 == NULL ){
        printf("error on encryptor memory allocation\n");
        exit(-1);
    }

	//for( int i = 0; i < num_threads; i++ ){
	//	init_decryptor( &bf[i], DECRYPT, BLOWFISH, iv, encrypted_text );
	//}

    	//for( int i = 0; i < num_threads; i++){
	//	init_decryptor( &cast5[i], DECRYPT, CAST5, iv, encrypted_text );
    	//}

	// begin the decryption
	key = ( unsigned char ** ) malloc( num_threads * sizeof( unsigned char * ) );

	if( key == NULL ) {
		printf("\nerror on key memory allocation\n");
		exit(-1);
	}

	for( int i = 0; i < num_threads; i++ ) {
		key[i] = ( unsigned char * ) malloc( KEY_LENGTH );
		if( key[i] == NULL ) {
			printf("\nerror on key memory allocation\n");
        	exit(-1);
		}
	}

	#pragma omp parallel for private( thread_id ) shared( success_key, encryption_method )
	for(long i = 0; i < cant_keys ; i++ ) {
		if( success_key == -1 ) {

			thread_id = omp_get_thread_num();
			keygen_itokey( key[ thread_id ], i );

			// BLOWFISH
			init_decryptor( &bf[thread_id], DECRYPT, BLOWFISH, iv, encrypted_text );
			encryptor_set_key( &bf[ thread_id ], key[ thread_id ] );
			encryptor_init( &bf[ thread_id ] );
			encryptor_update( &bf[ thread_id ] );
			encryptor_final( &bf[ thread_id ] );

			if( memcmp( (char *)bf[ thread_id ].output, "Frase", 5 ) == 0 ) {
				success_key = i;
				encryption_method = BLOWFISH;
			}
		}
	}

	#pragma omp parallel for private( thread_id ) shared( success_key, encryption_method )
	for(long i = 0; i < cant_keys ; i++ ) {
		if( success_key == -1 ) {

			thread_id = omp_get_thread_num();
			keygen_itokey( key[ thread_id ], i );

			//CAST5
			init_decryptor( &cast5[thread_id], DECRYPT, CAST5, iv, encrypted_text );
			encryptor_set_key( &cast5[ thread_id ], key[ thread_id ] );
            		encryptor_init( &cast5[ thread_id ] );
			encryptor_update( &cast5[ thread_id ] );
			encryptor_final( &cast5[ thread_id ] );

            if( memcmp( (char *)cast5[ thread_id ].output, "Frase", 5 ) == 0 ) {
               	success_key = i;
               	encryption_method = CAST5;
            }
		}
	}

	// take the timestamp
	end_time = time(NULL);
	print_result( success_key, encryption_method, difftime( end_time, start_time ) );

	exit(0);
}
