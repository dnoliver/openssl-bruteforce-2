#ifndef COMMONS_H
#define COMMONS_H

#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#include "encryptor.h"
#include "keygen.h"

/** number of parameters mandatory to execution */
#define NUM_PARAMS 2

/**
 * check the number of parameters
 * @param [argc]	number of parameters
 * @param [argv]	parameters array
 */
void check_parameters( int argc, char **argv );

/**
 * Get the parameters values
 * @param [argv]			parameters array
 * @param [encrypted_text]	buffer to save the input
 * @param [cant_keys]		where to save the number of keys to use
 */
void read_parameters( char **argv, unsigned char *encrypted_text, long *cant_keys );

/**
 * Initialiates a Encryptor
 * @param [decryptor]	a valid Encryptor pointer
 * @param [action]		DECRYPT || ENCRYPT
 * @param [type]		BLOWFISH || CAST5
 * @param [iv]			iv initialization array
 * @param [input]		input text for encryptor
 */
void init_decryptor( Encryptor *decryptor, int action, int type, unsigned char *iv, unsigned char *input );

/**
 * Print the result of the algoritm
 * @param [succes_key]			-1 || key that perform de decryption
 * @param [encryption_method]	-1 || cipher used in decryption
 * @param [time_spent]			aprox time spent by the execution
 */
void print_result( long success_key, int encryption_method, float time_spent );

/**
 * Generate a report file
 * @param [succes_key]          -1 || key that perform de decryption
 * @param [encryption_method]   -1 || cipher used in decryption
 * @param [time_spent]          aprox time spent by the execution
 */
void generate_report( long success_key, int encryption_method, float time_spent );

/**
 * Generate the key file
 * @param [succes_key]		succes decryption key
 */
void generate_key_file( long success_key );

#endif
