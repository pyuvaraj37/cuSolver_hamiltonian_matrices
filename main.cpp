
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>
#include <cusolverDn.h>

 void printMatrix(int m, int n, const double*A, int lda, const char* name)
 {
     for(int row = 0 ; row < m ; row++){
         for(int col = 0 ; col < n ; col++){
             double Areg = A[row + col*lda];
             printf("%s(%d,%d) = %f\n", name, row+1, col+1, Areg);
       }
    }
}

int main(int argc, char*argv[])
{
    int runs, n;
    float sum_DtoH_time = 0.0, sum_HtoD_time = 0.0, sum_K_time = 0.0, sum_error = 0.0; 
    FILE *fptr, *vftr;
    std::string full_file_path, full_verify_file_path, file;

    // Command line arguments        
    file = argv[1];
    n = atoi(argv[2]);
    runs = atoi(argv[3]);
    
    // Creating matrix and verification eigenvector with corresponding sizes
    double A[n * n];
    double lambda[n];  

    // Concat file locations for both data and verification vector
    full_verify_file_path += "verif/v";
    full_verify_file_path += file; 
    full_file_path += "data/";
    full_file_path += file;
    
    // Open files
    fptr = fopen(full_file_path.c_str(), "r");
    vftr = fopen(full_verify_file_path.c_str(), "r");

    // Copy file to verification vector
    for (size_t i = 0; i < n; i++) {
        fscanf(vftr, "%lf", &(lambda[i]));
    }

    // Copy file to matrix 
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            fscanf(fptr, "%lf", &(A[n * i + j]));
        }
    }
    
    //Close files
    fclose(fptr);
    fclose(vftr);

    
    for (int j = 0; j < runs; j++) {
        
        cudaSetDevice(0);   

        cusolverDnHandle_t cusolverH = NULL;
        cusolverStatus_t cusolver_status = CUSOLVER_STATUS_SUCCESS;
        cudaError_t cudaStat1 = cudaSuccess;
        cudaError_t cudaStat2 = cudaSuccess;
        cudaError_t cudaStat3 = cudaSuccess;
        const int m = n;
        const int lda = n;

        double V[lda*m]; // eigenvectors
        double W[m]; // eigenvalues

        double *d_A = NULL;
        double *d_W = NULL;
        int *devInfo = NULL;
        double *d_work = NULL;
        int  lwork = 0;

        int info_gpu = 0;

        //printf("A = (matlab base-1)\n");
        //printMatrix(m, m, A, lda, "A");
        //printf("=====\n");

    // timing variables

        float HtoDtime, Ktime, DtoHtime; 
        cudaEvent_t HtoDstart, HtoDstop, Kstart, Kstop, DtoHstart, DtoHstop;  

    
    // Create cusolver/cublas handle
        cusolver_status = cusolverDnCreate(&cusolverH);
        assert(CUSOLVER_STATUS_SUCCESS == cusolver_status);

    // Copy A and B to device
        cudaStat1 = cudaMalloc ((void**)&d_A, sizeof(double) * lda * m);
        cudaStat2 = cudaMalloc ((void**)&d_W, sizeof(double) * m);
        cudaStat3 = cudaMalloc ((void**)&devInfo, sizeof(int));
        assert(cudaSuccess == cudaStat1);
        assert(cudaSuccess == cudaStat2);
        assert(cudaSuccess == cudaStat3);
        
        cudaEventCreate(&HtoDstart);
        cudaEventCreate(&HtoDstop);
        cudaEventRecord(HtoDstart,0);


        cudaStat1 = cudaMemcpy(d_A, A, sizeof(double) * lda * m, cudaMemcpyHostToDevice);
        assert(cudaSuccess == cudaStat1);
        
        cudaEventRecord(HtoDstop, 0);
        cudaEventSynchronize(HtoDstop);
        cudaEventElapsedTime(&HtoDtime, HtoDstart, HtoDstop);

    // step 3: query working space of syevd
        cusolverEigMode_t jobz = CUSOLVER_EIG_MODE_VECTOR; // compute eigenvalues and eigenvectors.
        cublasFillMode_t uplo = CUBLAS_FILL_MODE_LOWER;
        cusolver_status = cusolverDnDsyevd_bufferSize(
            cusolverH,
            jobz,
            uplo,
            m,
            d_A,
            lda,
            d_W,
            &lwork);
        assert (cusolver_status == CUSOLVER_STATUS_SUCCESS);

        cudaStat1 = cudaMalloc((void**)&d_work, sizeof(double)*lwork);
        assert(cudaSuccess == cudaStat1);

    // step 4: compute spectrum
        
        cudaEventCreate(&Kstart);
        cudaEventCreate(&Kstop);
        cudaEventRecord(Kstart,0);

        cusolver_status = cusolverDnDsyevd(
            cusolverH,
            jobz,
            uplo,
            m,
            d_A,
            lda,
            d_W,
            d_work,
            lwork,
            devInfo);
        cudaStat1 = cudaDeviceSynchronize();
        assert(CUSOLVER_STATUS_SUCCESS == cusolver_status);
        assert(cudaSuccess == cudaStat1);


        cudaEventRecord(Kstop, 0);
        cudaEventSynchronize(Kstop);
        cudaEventElapsedTime(&Ktime, Kstart, Kstop);

        cudaEventCreate(&DtoHstart);
        cudaEventCreate(&DtoHstop);
        cudaEventRecord(DtoHstart,0);

        cudaStat1 = cudaMemcpy(W, d_W, sizeof(double)*m, cudaMemcpyDeviceToHost);
        cudaStat2 = cudaMemcpy(V, d_A, sizeof(double)*lda*m, cudaMemcpyDeviceToHost);
        cudaStat3 = cudaMemcpy(&info_gpu, devInfo, sizeof(int), cudaMemcpyDeviceToHost);
        assert(cudaSuccess == cudaStat1);
        assert(cudaSuccess == cudaStat2);
        assert(cudaSuccess == cudaStat3);

        cudaEventRecord(DtoHstop, 0);
        cudaEventSynchronize(DtoHstop);
        cudaEventElapsedTime(&DtoHtime, DtoHstart, DtoHstop);
    
        //printf("after syevd: info_gpu = %d\n", info_gpu);
        assert(0 == info_gpu);

        // printf("eigenvalue = (matlab base-1), ascending order\n");
        // for(int i = 0 ; i < m ; i++){
        //     printf("W[%d] = %E\n", i+1, W[i]);
        // }

        // printf("V = (matlab base-1)\n");
        // printMatrix(m, m, V, lda, "V");
        // printf("=====\n");

    //step 4: check eigenvalues
        double lambda_sup = 0;
        for(int i = 0 ; i < m ; i++){
            double error = fabs( lambda[i] - W[i]);
            lambda_sup = (lambda_sup > error)? lambda_sup : error;
        }
        printf("Run %d results: \n", j+1);
        printf("Error %E\n", lambda_sup);
        sum_error += lambda_sup;
        printf("HtoD memcpy %f ms \n", HtoDtime);
        sum_HtoD_time += HtoDtime;
        printf("Kernel %f ms \n", Ktime);
        sum_K_time += Ktime;
        printf("DtoH memcpy %f ms \n", DtoHtime);
        sum_DtoH_time += DtoHtime;
        
    // free resources
        if (d_A    ) cudaFree(d_A);
        if (d_W    ) cudaFree(d_W);
        if (devInfo) cudaFree(devInfo);
        if (d_work ) cudaFree(d_work);

        if (cusolverH) cusolverDnDestroy(cusolverH);

        cudaDeviceReset();
    }
    printf("Average results for %d runs: \n", runs);
    printf("Error %E\n", sum_error/runs);
    printf("HtoD memcpy %f ms \n", sum_HtoD_time/runs);
    printf("Kernel %f ms \n", sum_K_time/runs);
    printf("DtoH memcpy %f ms \n", sum_DtoH_time/runs);

    return 0;
}
