// Optimize this function

typedef int vec __attribute__((vector_size(32)));

vec *alloc(int n)
{
    vec *ptr = (vec *)aligned_alloc(32, 32 * n);
    memset(ptr, 0, 32 * n);
    return ptr;
}
void singleThread(int n, int *_a, int *_b, int *c)
{
    int nB = (n + 7) / 8; // number of 8-element vectors in a row (rounded up)

    vec *a = alloc(n * nB);
    vec *b = alloc(n * nB);

    // move both matrices to the aligned region
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i * nB + j / 8][j % 8] = _a[i * n + j];
            b[i * nB + j / 8][j % 8] = _b[j * n + i]; // <- b is still transposed
        }
    }

    for (int i = 0; i < n; i += 2)
    {
        for (int j = 0; j < n; j += 2)
        {
            vec s; // initialize the accumulator with zeros
            for (int ll = 0; ll < 8; ll++)
            {
                s[ll] = 0;
            }
            // vertical summation
            for (int k = 0; k < nB; k++)
            {
                s += a[i * nB + k] * b[j * nB + k];
                s += a[(i + 1) * nB + k] * b[j * nB + k];
                s += a[i * nB + k] * b[(j + 1) * nB + k];
                s += a[(i + 1) * nB + k] * b[(j + 1) * nB + k];
            }
            // horizontal summation
            for (int k = 0; k < 8; k++)
                c[(i >> 1) * (n >> 1) + (j >> 1)] += s[k];
        }
    }

    std::free(a);
    std::free(b);
}

// helper functions

// typedef int vec __attribute__((vector_size(32)));

// vec *alloc(int n)
// { // allocates n vectors and initializes them with zeros
//     vec *ptr = (vec *)aligned_alloc(32, 32 * n);
//     memset(ptr, 0, (32 * n));
//     return ptr;
// }

// // main function performing single threading

// void singleThread(int n, int *matA, int *matB, int *output)
// {

//     // Taking transpose of matrix B

//     int *temp = (int *)malloc(n * n * sizeof(int));

//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             temp[(i * n) + j] = matB[(j * n) + i];
//         }
//     }

//     // Convert matrix A & B to (n*2) * n matrix by adding the consecutive two rows

//     int *tempA = (int *)malloc(n / 2 * n * sizeof(int));
//     int *tempB = (int *)malloc(n / 2 * n * sizeof(int));

//     int t = 0;
//     for (int i = 0; i < n / 2; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             tempA[(i * n) + j] = matA[(2 * i * n) + j] + matA[(2 * n * i) + n + j];
//             tempB[(i * n) + j] = temp[(2 * i * n) + j] + temp[(2 * n * i) + n + j];
//         }
//         t++;
//     }

//     // Using vectorization for RMM

//     int count = (n + 7) / 8; // it will give number of 8-element vectors in a row

//     vec *va = alloc(n / 2 * count);
//     vec *vb = alloc(n / 2 * count);

//     // both matrices A & B are moved to the aligned region
//     for (int i = 0; i < n / 2; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             va[(i * count) + j / 8][j % 8] = matA[(i * n) + j];
//             vb[(i * count) + j / 8][j % 8] = matB[(i * n) + j];
//         }
//     }

//     // matrix multiplication
//     for (int i = 0; i < n / 2; i++)
//     {
//         for (int j = 0; j < n / 2; j++)
//         {
//             vec v;

//             for (int k = 0; k < n / 2; k++)
//             {
//                 v[k] = 0;
//             }

//             for (int k = 0; k < count; k++)
//             {
//                 v += va[(i * count) + k] * vb[(j * count) + k];
//             }

//             for (int k = 0; k < 8; k++)
//                 output[(i * n / 2) + j] += v[k];
//         }
//     }

//     std::free(va);
//     std::free(vb);
// }
