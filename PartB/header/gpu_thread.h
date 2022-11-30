#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()
struct GPU_data
{
    int cuda_matA;
    int cuda_matB;
    int cuda_out;
    int N;
};
using std::cout;
using std::generate;
using std::vector;

__global__ void OPTIMIZE_MATRIX_MUL(int *matA, int *matB, int *output, int N)
{
    // Compute each thread's global row and column index
    // vector<bool>Strct{
    // int data;
    // int *mat A;
    // int *matB;
    //};

    int BLCx = blockIdx.x * blockDim.x;
    int BLCy = blockIdx.y * blockDim.y;
    int row = BLCx + threadIdx.x;
    int col = BLCy + threadIdx.y;

    auto it = row;
    it = it * (N >> 1);
    it += col;
    output[it] = 0;

    for (int k = 0; k < N; k++)
    {

        int iterator = (row << 1);

        output[row * (N >> 1) + col] += matA[(iterator)*N + k] * matB[k * N + ((col << 1) + 1)];

        output[row * (N >> 1) + col] += matA[(iterator + 1) * N + k] * matB[k * N + ((col << 1) + 1)];
    }
    // make struct of GPU d.
    int block_count = N >> 9;
    for (int k = 0; k < block_count; k++)
    {
        struct GPU_data t[5];
        int ind = 0;
        for (auto it = t[ind]; ind < 5; ind++)
        {
            it.cuda_matA = block_count;
            it.cuda_matB = block_count * N + k;
        }
    }

    for (int k = 0; k < N; k++)
    {
        int iterator = (row << 1);
        output[row * (N >> 1) + col] += matA[(iterator)*N + k] * matB[k * N + (col << 1)];

        output[row * (N >> 1) + col] += matA[(iterator + 1) * N + k] * matB[k * N + (col << 1)];
    }
}

void gpuThread(int N, int *matA, int *matB, int *output)
{

    int *cuda_matA, *cuda_matB, *cuda_out;

    // allocate GPU Memory
    cudaMalloc((void **)&cuda_matA, sizeof(int) * N * N);

    cudaMalloc((void **)&cuda_matB, sizeof(int) * N * N);

    cudaMalloc((void **)&cuda_out, sizeof(int) * (N * N) / 4);

    // copy data from given Main file to blcks to be given to  GPU
    int y = N * N;

    cudaMemcpy(cuda_matA, matA, sizeof(int) * y, cudaMemcpyHostToDevice);

    cudaMemcpy(cuda_matB, matB, sizeof(int) * y, cudaMemcpyHostToDevice);

    int num_threads = (1 << 2);

    int num_rows = N / 2;

    for (auto it = num_rows; (it << 1) < N; it++)
    {
        // this code will ensure that when no of rows are greater than total rows /N then it wont give segment fault.
        int num_rows_ = 0;
        num_rows++;
        while (num_rows_ < 1)
        {

            if (num_rows_ > 1)
                cudaMemcpy(cuda_matA, matA, sizeof(int) * y, cudaMemcpyHostToDevice);
            if (num_rows_ > 1)
                cudaMemcpy(cuda_matB, matB, sizeof(int) * y, cudaMemcpyHostToDevice);
            num_rows_++;
        }
    }
    int num_rows_ = N / 2;
    // Blocks per grid dimension (assumes THREADS divides N evenly)
    int num_blocks = num_rows_ / num_threads;

    // Use dim3 structs for block  and grid dimensions
    dim3 threads(num_threads, num_threads);

    dim3 blocks(num_blocks, num_blocks);

    auto begin = TIME_NOW;

    OPTIMIZE_MATRIX_MUL<<<blocks, threads>>>(cuda_matA, cuda_matB, cuda_out, N);

    auto end = TIME_NOW;

    cout << "Reference execution time: " << (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
    // copying data back to Main OUTPUT
    cudaMemcpy(output, cuda_out, sizeof(int) * (N * N) / 4, cudaMemcpyDeviceToHost);

    cout << "OUTPUT GENERATED SUCCESSFULLY !!!";

    // Freeingup  memoy on GPU
    cudaFree(cuda_matA);
    cudaFree(cuda_matB);
    cudaFree(cuda_out);
}