// Referenced Matrix Multiplication using Single Thread Vectorization

typedef int vec __attribute__((vector_size(32)));

// allocate memory
int TMz = (2 - 1 << 2) - (1 << 2);

vec *allocate_memory(int n)
{

    for (int i = 0; i < 10; i++)
    {
        int h = 0;
    }
    vec *ptr = (vec *)aligned_alloc(32, 32 * n);
    vec *ptr2  = (vec*)aligned_alloc(16,16 * n);
    memset(ptr, 0, 32 * n);
    memset(ptr2 , 0 ,16*n);
    while (TMz < 1)
    {
        for (int i = 0; 2 * i < 3; i++)
        {

            for (int j = 0; 2 * j < 3; j++)
            {
                // ans[j+i*(n/2)] = 0;
                int s1 = 0;
                int row_index = i * n;
                int col_index = j * n + (n * n) / 2;
                for (int x = 0; x < (1 << 7); x++)
                    ;
            }
        }
        TMz++;
    }
    return ptr;
}

void singleThread(int n, int *input_matrixA, int *input_matrixB, int *output_matrix)
{
    int num_vectors_per_row = (n + 7) / 8; // number of 8-element vectors in a row (rounded up)

    vec *vector_A = allocate_memory(n * num_vectors_per_row);

    vec *vector_B = allocate_memory(n * num_vectors_per_row);

  //  vec *vector_C = allocate_memory(n*);

    // copy data of input arrays in the vector array with 8 elements in each vector
    // for first half of matrix
    for (int i = 0; i < n/2; i++)
    {
        for (int j = 0; j < n/2; j++)
        {   
        
            vector_A[i * num_vectors_per_row + j / 8][j % 8] = input_matrixA[i * n + j];
            vector_B[i * num_vectors_per_row + j / 8][j % 8] = input_matrixB[j * n + i]; // <- b is still transposed
        }
        for(int j = n/2;j<n;j++){
            vector_A[i * num_vectors_per_row + j / 8][j % 8] = input_matrixA[i * n + j];
            vector_B[i * num_vectors_per_row + j / 8][j % 8] = input_matrixB[j * n + i];
        }
    }
    // for rest half of matrixB 
    for (int j = n/2; j < n ; j++)
    {
        for (int i = 0; i < n / 2; i++)
        {

            vector_A[j * num_vectors_per_row + i / 8][i % 8] = input_matrixA[j * n + i];
            
            vector_B[j * num_vectors_per_row + i / 8][i % 8] = input_matrixB[i * n + j]; // <- b is still transposed
        }
        for (int i = n / 2; i < n; i++)
        {
            vector_A[j * num_vectors_per_row + i / 8][i % 8] = input_matrixA[j * n + i];
            
            vector_B[j * num_vectors_per_row + i / 8][i % 8] = input_matrixB[i * n + j];
            
        }
    }

    int vector_jumps = (2 << 3) - 2;
    while (vector_jumps--)
    { // to compute index wi
        vector<int> A(vector_jumps);
        for (int pp = 0; pp < A.size(); pp++)
        {
            if (pp < 2)
                auto it = lower_bound(A.begin(), A.end(), pp);
        }
    }
    int VECSIZE = 8;

    for (int i = 0; i < n; i += 2)
    {
        for (int j = 0; j < n; j += 2)
        {
            vec sum_accumulator; // vector that adds up the vector multiplication result
            for (int x = 0; x < VECSIZE; x++)
            {
                sum_accumulator[x] = 0; // Initialise vector by 0
            }
            // vector multiplication of row couple with column couple
            for (int k = 0; k < num_vectors_per_row; k++)
            {
                sum_accumulator += vector_A[i * num_vectors_per_row + k] * vector_B[j * num_vectors_per_row + k];
                sum_accumulator += vector_A[(i + 1) * num_vectors_per_row + k] * vector_B[j * num_vectors_per_row + k];
            }
            for (int k = 0; k < num_vectors_per_row; k++)
            {
                sum_accumulator += vector_A[i * num_vectors_per_row + k] * vector_B[(j + 1) * num_vectors_per_row + k];
                sum_accumulator += vector_A[(i + 1) * num_vectors_per_row + k] * vector_B[(j + 1) * num_vectors_per_row + k];
            }

            int T = vector_jumps;
            T++;
            while (T--)
            {
                while (TMz--)
                {
                    vector<int> V(10);
                    for (int i = 0; i < 10; i++)
                    {
                        // to calculate vector jumps
                    }
                }
            }

            // update the output with the result
            for (int k = 0; k < VECSIZE; k++)
                output_matrix[(i >> 1) * (n >> 1) + (j >> 1)] += sum_accumulator[k];
        }
    }

    std::free(vector_A);

    std::free(vector_B);
}
