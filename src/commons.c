#include "../include/commons.h"

void check_parameters( int argc, char **argv )
{
    if( argc != NUM_PARAMS ){
        printf("usage: %s ENCRYPTED_FILE\n", argv[0] );
        exit(-1);
    }
}

void read_parameters( char **argv, unsigned char *encrypted_text, long *cant_keys )
{
    char *path = argv[1];

    if( fs_read( path, encrypted_text, BLOCK_SIZE ) != BLOCK_SIZE ){
        printf("\nerror opening the encrypted file\n");
        exit(-1);
    }

    if( keygen_getenv( cant_keys ) != 0 ){
        printf("\nerror reading enviroment\n");
        exit(-1);
    }
}

void init_decryptor( Encryptor *decryptor, int action, int type, unsigned char *iv, unsigned char *input )
{
    if(
        encryptor_init_data( decryptor, action, type, iv ) != 0 ||
        encryptor_set_input( decryptor, input, BLOCK_SIZE ) != 0
    ){
        printf("\nerror on blowfish encryptor initialization\n");
        exit(-1);
    }
}

void print_result( long success_key, int encryption_method, float time_spent )
{
	printf("done...");
    if( success_key != -1 ) {
		printf("success\n");
		generate_key_file( success_key );
		generate_report( success_key, encryption_method, time_spent );
    }
    else {
		printf("not found\n");
		exit(-1);
    }

}

void generate_report( long success_key, int encryption_method, float time_spent )
{
	char *path = "./report";
	char report[1000];
    int length;

    length = sprintf( report, "=== DECRYPTION REPORT === \n\tkey: %li \n\tcipher: %s \n\ttime spent: %f \n=========================\n",
		success_key,
		encryption_method == BLOWFISH? "blowfish" : "cast5",
		time_spent
	);

    if( fs_write( path, (unsigned char *) report, length ) != length ) {
        printf("\nerror writing report file\n");
        exit(-1);
    }
}

void generate_key_file( long success_key )
{
	char *path = "./key";
	char key[20];
	int length;

	length = sprintf( key, "%li", success_key );

	if( fs_write( path, (unsigned char *) key, length ) != length ) {
		printf("\nerror writing key file\n");
        exit(-1);
	}
}


