#!/bin/bash

## this encrypt a text with the encryptor, and test the main app if it can find the text
ENCRYPT=bin/encrypt
SERIAL=bin/serial-1
OMP=bin/omp-1
MPI=bin/mpi-1
CANT_KEYS=500000
##usage: ./decrypt [INPUT FILE] [KEY CODE] [METHOD] [OUTPUT FILE]
echo "===== Report ====="
echo

echo "Frase: La bersuit" > file
$ENCRYPT file 99999999 blowfish file2

for I in 1000 10000 100000 1000000
do
	for J in 1 2 3
	do
		echo "Serial[$J] time for $I keys"
		export CANT_KEYS=$I
		time ./$SERIAL file2
		echo
	done
done

for I in 1000 10000 100000 1000000
do
	for J in 1 2 3
	do
		echo "OMP[$J] time for $I keys"
		export CANT_KEYS=$I
		time ./$OMP file2
		echo
	done
done

for I in 1000 10000 100000 1000000
do
	for J in 1 2 3
	do
		echo "MPI[$J] time for $I keys"
		export CANT_KEYS=$I
		time mpirun -np 2 $MPI file2
		echo
	done
done

echo "============================================="
echo

export CANT_KEYS=1000000
for I in 2 4 8
do
	for J in 1 2 3
	do
		echo "OpenMP[$J] time $I thread for $CANT_KEYS keys..."
		export OMP_NUM_THREADS=$I
		time ./$OMP file2
		echo
	done
done

export CANT_KEYS=1000000
for I in 2 4 8
do
	for J in 1 2 3
	do
		echo "OpenMPI[$J] time $I process for $CANT_KEYS keys..."
		time mpirun -np $I $MPI file2
		echo
	done
done

echo "============================================="
echo
