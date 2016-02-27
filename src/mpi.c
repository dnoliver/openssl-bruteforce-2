#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mpi.h"
#include "../include/fs.h"
#include "../include/keygen.h"
#include "../include/encryptor.h"
#include "../include/commons.h"

#define TAG 0
#define MASTER_PROC 0
#define NUM_PARAMS 2

int main( int argc, char* argv[] )
{
	check_parameters( argc, argv );

	int encryption_method = -1;
	long success_key = -1;
    int proc_id, num_procs;
	int chunk, first, last;
	long work_size;
	unsigned char encrypted_text[8];
	unsigned char iv[ IV_LENGTH ] = {1,2,3,4,5,6,7,8};
	unsigned char key[ KEY_LENGTH ];
	time_t start_time, end_time;

	// init mpi
    MPI_Init( &argc, &argv );

	// check this process id
    MPI_Comm_rank( MPI_COMM_WORLD, &proc_id );

	// check how many process are available
    MPI_Comm_size( MPI_COMM_WORLD, &num_procs );

	//take timestamp and alocate result array
	start_time = time(NULL);

	read_parameters( argv, encrypted_text, &work_size );

	Encryptor decryptor[2];
	//init_decryptor( &decryptor[0], DECRYPT, BLOWFISH, iv, encrypted_text );
	//init_decryptor( &decryptor[1], DECRYPT, CAST5, iv, encrypted_text );

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

	for( long i = first; i < last && success_key == - 1; i++ ) {

		keygen_itokey( key, i );
		init_decryptor( &decryptor[0], DECRYPT, BLOWFISH, iv, encrypted_text );
		encryptor_set_key( &decryptor[0], key );
        encryptor_init( &decryptor[0] );
        encryptor_update( &decryptor[0] );
        encryptor_final( &decryptor[0] );

        if( memcmp( (char *)decryptor[0].output, "Frase", 5 ) == 0 ) {
			success_key = i;
            encryption_method = BLOWFISH;
            break;
        }

	}

	for( long i = first; i < last && success_key == -1; i++ ) {

		keygen_itokey( key, i );
	init_decryptor( &decryptor[1], DECRYPT, CAST5, iv, encrypted_text );
        encryptor_set_key( &decryptor[1], key );
        encryptor_init( &decryptor[1] );
        encryptor_update( &decryptor[1] );
        encryptor_final( &decryptor[1] );

        if( memcmp( (char *)decryptor[1].output, "Frase", 5 ) == 0 ) {
			success_key = i;
            encryption_method = CAST5;
            break;
        }
	}

	//master wait to all proccess to finalize
	end_time = time( NULL );

	if( success_key != -1 ) {
		print_result( success_key, encryption_method, difftime( end_time, start_time ) );
	}

	// close mpi
    MPI_Finalize();

    return 0;
}

