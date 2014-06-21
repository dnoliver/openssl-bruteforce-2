#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
SERIAL=bin/serial
OMP=bin/omp
MPI=bin/mpi
FILE=input/Navarro1
CANT_KEYS=100000000

##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]

echo
echo "Serial bruteforce decryption test..."
## execute the serial app
## usage: ./serial [ENCRYPTED_FILE_PATH]
export CANT_KEYS=$CANT_KEYS
./$SERIAL $FILE && echo "passed" || echo "fail"
cat report > report.serial
cat key > key.serial
echo

echo "OpenMP bruteforce decryption test..."
export OMP_NUM_THREADS=2
export CANT_KEYS=$CANT_KEYS
./$OMP $FILE && echo "passed" || echo "fail"
cat report > report.omp
cat key > key.omp
echo

echo "MPI bruteforce decryption test..."
export CANT_KEYS=$CANT_KEYS
mpirun -np 2 $MPI $FILE && echo "passed" || echo "fail"
cat report > report.mpi
cat key > key.mpi
echo
