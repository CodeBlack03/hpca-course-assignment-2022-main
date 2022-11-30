// Referenced Matrix Multiplication using Single Thread Vectorization

typedef int vec __attribute__((vector_size(32)));

// allocate memory
vec *allocate_memory(int n)
{
    vec *ptr = (vec *)aligned_alloc(32, 32 * n);
    memset(ptr, 0, 32 * n);
    return ptr;
}
void singleThread(int n, int *input_matrixA, int *input_matrixB, int *output_matrix)
{
    int num_vectors_per_row = (n + 7) / 8; // number of 8-element vectors in a row (rounded up)

    vec *vector_A = allocate_memory(n * num_vectors_per_row);
    vec *vector_B = allocate_memory(n * num_vectors_per_row);

    // copy data of input arrays in the vector array with 8 elements in each vector
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            vector_A[i * num_vectors_per_row + j / 8][j % 8] = input_matrixA[i * n + j];
            vector_B[i * num_vectors_per_row + j / 8][j % 8] = input_matrixB[j * n + i]; // <- b is still transposed
        }
    }

    for (int i = 0; i < n; i += 2)
    {
        for (int j = 0; j < n; j += 2)
        {
            vec sum_accumulator; // vector that adds up the vector multiplication result
            for (int x = 0; x < 8; x++)
            {
                sum_accumulator[x] = 0; // Initialise vector by 0
            }
            // vector multiplication of row couple with column couple
            for (int k = 0; k < num_vectors_per_row; k++)
            {
                sum_accumulator += vector_A[i * num_vectors_per_row + k] * vector_B[j * num_vectors_per_row + k];
                sum_accumulator += vector_A[(i + 1) * num_vectors_per_row + k] * vector_B[j * num_vectors_per_row + k];
                sum_accumulator += vector_A[i * num_vectors_per_row + k] * vector_B[(j + 1) * num_vectors_per_row + k];
                sum_accumulator += vector_A[(i + 1) * num_vectors_per_row + k] * vector_B[(j + 1) * num_vectors_per_row + k];
            }
            // update the output with the result
            for (int k = 0; k < 8; k++)
                output_matrix[(i >> 1) * (n >> 1) + (j >> 1)] += sum_accumulator[k];
        }
    }

    std::free(vector_A);
    std::free(vector_B);
}
