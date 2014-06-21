#include "../include/fs.h"

int fs_read( const char* path, unsigned char *output, int size )
{
	int file = open( path, O_RDONLY );
	int readed;

	if( file == -1 ) {
		perror("fs_read: ");
		return -1;
	}

	readed = read( file, output, size );

	if( readed == -1 ) {
		perror("fs_read: ");
		return -1;
	}

	close(file);

	return readed;
}

int fs_write(const char* path, unsigned char *input, int size)
{
	int file = open( path, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR );
	int writed;

	 if( file == -1 ) {
		perror("fs_write: ");
        return -1;
     }

	writed = write( file, input, size );

	if( writed == -1 ) {
		perror("fs_write: ");
		return -1;
	 }

	 close(file);
     return writed;
}
