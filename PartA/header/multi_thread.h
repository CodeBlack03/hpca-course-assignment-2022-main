#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
using namespace std;

int Thread_Count = (1 << 2);
int gbN;
struct data
{
    int rowa;
    
    int colb;
    int n;
    int *mata;
    int *matb;
} dt1;

void *optimize_multiplication(void *tmp)
{
    int n = gbN;
    int ans_size = (n * n) / 4;
    int *ans = (int *)malloc(sizeof(int) * ans_size);

    for (int i = 0; 2 * i < n; i++)
    {
        for (int j = 0; 2 * j < n; j++)
        {
            // ans[j+i*(n/2)] = 0;
            int s1 = 0;
            int row_index = i * n;
            int col_index = j * n + (n * n) / 2;
            for (int k = 0; k < n; k++)
                s1 += ((int *)tmp)[row_index++] * ((int *)tmp)[col_index++];

            ans[j + i * (n / 2)] = s1;
        }
    }
    pthread_exit((void *)ans);
}

void multiThread(int N, int *matA, int *matB, int *output)
{
    gbN = N;
    assert(N >= 4 and N == (N & ~(N - 1)));
    pthread_t *T;
    // making array of threads
    auto temp = malloc(sizeof(pthread_t) * (1 << 2));

    int *part1 = NULL;
    int *part2 = NULL;
    int *part3 = NULL;
    int *part4 = NULL;
    int ind = 0;
    T = (pthread_t *)temp;
    for (int i = 0; i < (1 << 2); i++)
    {
        ind = 0;
        if (i == 0)
        {
            part1 = (int *)malloc(sizeof(int) * (N * N));

            // iterating over every other row
            for (int row = 0; row < N; row += 2)
                for (int iter = 0; iter < N; ++iter)
                    part1[ind++] = matA[row * N + iter];

            for (int col = 0; col < N; col += 2)
                for (int iter = 0; iter < N; iter++)
                    part1[ind++] = matB[iter * N + col];

            pthread_create(&T[i], NULL, optimize_multiplication, part1);
        }
        else if (i == 1)
        {
            int ind = 0;
            part2 = (int *)malloc(sizeof(int) * (N * N));
            for (int row = 1; row < N; row += 2)
                for (int iter = 0; iter < N; ++iter)
                    part2[ind++] = matA[row * N + iter];

            for (int col = 0; col < N; col += 2)
                for (int iter = 0; iter < N; iter++)
                    part2[ind++] = matB[iter * N + col];
            pthread_create(&T[i], NULL, optimize_multiplication, part2);
        }
        else if (i == 2)
        {
            part3 = (int *)malloc(sizeof(int) * (N * N));
            for (int row = 0; row < N; row += 2)
                for (int iter = 0; iter < N; ++iter)
                    part3[ind++] = matA[row * N + iter];

            for (int col = 1; col < N; col += 2)
                for (int iter = 0; iter < N; iter++)
                    part3[ind++] = matB[iter * N + col];
            pthread_create(&T[i], NULL, optimize_multiplication, part3);
        }
        else
        {
            part4 = (int *)malloc(sizeof(int) * (N * N));
            for (int row = 1; row < N; row += 2)
                for (int iter = 0; iter < N; ++iter)
                    part4[ind++] = matA[row * N + iter];

            for (int col = 1; col < N; col += 2)
                for (int iter = 0; iter < N; iter++)
                    part4[ind++] = matB[iter * N + col];
            pthread_create(&T[i], NULL, optimize_multiplication, part4);
        }
    }

    for (int i = 0; i < Thread_Count; i++)
    {
        int *ans;
        pthread_join(T[i], (void **)&ans);
      
        for (int j = 0; 4 * j < N * N; j++)
        {
            output[j] += ans[j];
        }
    }
}