#ifndef FS_H
#define FS_H

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * Read from file
 * @param [path]	path to file
 * @param [output]	buffer to save the file content
 * @param [size]	how many bytes want to read
 * @return number of bytes readed
 */
int fs_read( const char* path, unsigned char *output, int size );

/**
 * Write to file
 * @param [path]	path to file
 * @param [input]	buffer to save in file
 * @param [size]	how many bytes want to write
 * @return number of bytes writed
 */
int fs_write( const char* path, unsigned char *input, int size );

#endif
