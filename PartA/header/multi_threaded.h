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
struct thread_data
{
    int N;
    int start;
    int end;
    int *a;
    int *b;
    int *c;
};
void *referenced_multiplication(void *thread_data)
{
    struct thread_data *data = (struct thread_data *)thread_data;
    int start = data->start;
    int end = data->end;
    int i = 0;
    int j = start;
    int N = data->N;
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < N / 2; j++)
        {
            int s = 0;
            for (int k = 0; k < N / 2; k++)
            {
                s += data->a[i * N / 2 + k] * data->b[j * N / 2 + k + i];
            }
            data->c[i * N / 2 + j] += s;
        }
    }

    // for (int i = data->start; i < data->end; i++)
    // {
    //     for (int j = 0; j < data->N / 2; j++)
    //     {
    //         int s = 0;
    //         for (int k = 0; k < data->N; k++)
    //         {
    //             sum += data->a[i * data->N + k] * data->b[j * data->N + k + i];
    //         }
    //         data->c[i * (data->N / 2) + j] = s;
    //     }
    // }
    pthread_exit(NULL);
}
void *referenced_mul(void *thread_data)
{
    struct thread_data *data = (struct thread_data *)thread_data;
    for (int i = 0; i < data->N / 2; i++)
    {
        for (int j = data->start; j < data->end; j++)
        {
            int s = 0;
            s += data->a[i * data->N + j] * data->b[j * data->N + i];
        }
        data->c[i * (data->N / 2) + j] += s;
    }
}
void multiThread(int N, int *_a, int *_b, int *c)
{

    assert(N >= 4 and N == (N & ~(N - 1)));
    int *a = (int *)malloc((sizeof(int)) * N * N / 2);
    int *b = (int *)malloc((sizeof(int)) * N * N / 2);
    for (int i = 0; i < N; i += 2)
    {
        for (int j = 0; j < N; j++)
        {
            a[i * N + j] = _a[i * N + j] + _a[(i + 1) * N + j];
        }
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N / 2; j++)
        {
            b[j * N + i] = _b[j * N + i] + _b[(j + 1) * N + i];
        }
    }
    int num_threads = 4;
    pthread_t thread_process[num_threads];
    struct thread_data data[threads];

    for (int i = 0; i < num_threads; i++)
    {
        data[i].N = N;
        data[i].a = a;
        data[i].b = b;
        data[i].startInd = i * (N / (2 * num_threads));
        data[i].endInd = i * (N / (2 * num_threads)) + (N / (2 * num_threads));
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&thread_process[i], NULL, &referenced_multiplication, &data[i]);
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(thread_process[i], NULL);
    }
}
