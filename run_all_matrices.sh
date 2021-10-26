#!/bin/bash

RUNS = 3

echo "Starting script"

echo "Running Hamiltonian Matrices:"

echo "Computing hamsqr128.dat"
./main hamsqr128.dat 128 ${RUNS} > log/hamsqr128.log
cat log/hamsqr128.log

echo "-----------------------"

echo "Computing hamsqr256.dat"
./main hamsqr256.dat 256 ${RUNS} > log/hamsqr256.log
cat log/hamsqr256.log

echo "-----------------------"

echo "Computing hamsqr512.dat"
./main hamsqr512.dat 512 ${RUNS} > log/hamsqr512.log
cat log/hamsqr512.log

echo "-----------------------"

echo "Computing hamsqr1024.dat"
./main hamsqr1024.dat 1024 ${RUNS} > log/hamsqr1024.log
cat log/hamsqr1024.log

echo "-----------------------"

echo "Computing hamsqr2048.dat"
./main hamsqr2048.dat 2048 ${RUNS} > log/hamsqr2048.log
cat log/hamsqr2048.log

echo "-----------------------"

echo "Computing hamsqr4096.dat"
./main hamsqr4096.dat 4096 ${RUNS} > log/hamsqr4096.log
cat log/hamsqr4096.log

echo "-----------------------"

echo "Computing hamsqr8192.dat"
./main hamsqr8192.dat 8192 ${RUNS} > log/hamsqr8192.log
cat log/hamsqr8192.log

echo "-----------------------"

# echo "Running Overlap Matrices:"

# echo "Computing oversqr128.dat"
# ./main oversqr128.dat 128 > log/oversqr128.log
# cat log/oversqr128.log

# echo "-----------------------"

# echo "Computing oversqr256.dat"
# ./main oversqr256.dat 256 > log/oversqr256.log
# cat log/oversqr256.log

# echo "-----------------------"

# echo "Computing oversqr512.dat"
# ./main oversqr512.dat 512 > log/oversqr512.log
# cat log/oversqr512.log

# echo "-----------------------"

# echo "Computing oversqr1024.dat"
# ./main oversqr1024.dat 1024 > log/oversqr1024.log
# cat log/oversqr1024.log

# echo "-----------------------"

# echo "Computing oversqr2048.dat"
# ./main oversqr2048.dat 2048 > log/oversqr2048.log
# cat log/oversqr2048.log

# echo "-----------------------"

# echo "Computing oversqr4096.dat"
# ./main oversqr4096.dat 4096 > log/oversqr4096.log
# cat log/oversqr4096.log

# echo "-----------------------"

# echo "Computing oversqr8192.dat"
# ./main oversqr8192.dat 8192 > log/oversqr8192.log
# cat log/oversqr2048.log

echo "Finished all computations."
