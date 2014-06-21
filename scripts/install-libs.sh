#!/bin/bash

cd libs

## openmpi-1.6.5
tar -zxvf openmpi-1.6.5.tar.gz
cd openmpi-1.6.5
./configure
sudo make all install
cd ..

## CUnit-2.1-2:
tar -zxvf CUnit-2.1-2.tar.gz
cd CUnit-2.1-2
./configure
make
make check
sudo make install
cd ..

## openssl-1.0.1e:
tar -zxvf openssl-1.0.1e.tar.gz
cd libs/openssl-1.0.1e
./config --prefix=/usr/local
make
make test
sudo make install

