CC=gcc
LDFLAGS=-std=c99
MPICC=mpicc
MPIRUN=mpirun

all: openssl openmpi openmp
	./openssl
	$(MPIRUN) -np 4 openmpi
	./openmp

openssl: openssl.c
	$(CC) $(LDFLAGS) -o $@ $^ -lcrypto
	
openmpi: openmpi.c
	$(MPICC) $^ -o $@

openmp: openmp.c
	$(CC) $(LDFLAGS) $^ -o $@ -fopenmp

clean:
	find -executable -delete
