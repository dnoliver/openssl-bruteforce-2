#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main( int argc, char **argv )
{
	int thread_id;
	int num_threads;

	#pragma omp parallel private( num_threads )
	{
		thread_id = omp_get_thread_num();
		printf("\nHello from thread: %d", thread_id );
	}

	return 0;
}
