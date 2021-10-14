# CUDA C++ code to compute Eigenvectors and Eigenvalues

Objective is to get benchmark timings for computation to compare to an FPGA implementation.
The main.cpp file is a modified version from https://docs.nvidia.com/cuda/cusolver/index.html#syevd-example1 
Uses the cusolver and cudart libaries for computation.
Data includes hamiltonian and overlap matrices of various sizes. 
Makefile points to cuda libary on https://hpcc.ucr.edu cluster
